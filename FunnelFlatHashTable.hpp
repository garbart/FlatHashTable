#pragma once

#include <cassert>
#include <functional>


template <typename K, typename V>
class FunnelFlatHashTable
{
public:
	static constexpr float DEFAULT_LOAD_FACTOR = 0.9f;
	static constexpr uint64_t DEFAULT_INITIAL_CAPACITY = 16;
	static constexpr uint64_t MAXIMUM_CAPACITY = 1 << 30;

private:
	struct Entry
	{
		K key;
		V value;
		int64_t hash;

		Entry() : key(), value(), hash(-1)
		{
		}

		Entry(const K& key, const V& value, int64_t hash) : key(key), value(value), hash(hash)
		{
		}

		bool IsEmpty() const
		{
			return hash == -1;
		}

		bool IsTombstone() const
		{
			return hash == -2;
		}

		void SetTombstone()
		{
			hash = -2;
		}
	};

	Entry* table;
	uint64_t size = 0;
	uint64_t capacity = DEFAULT_INITIAL_CAPACITY;
	float loadFactor = DEFAULT_LOAD_FACTOR;
	uint64_t tombstones = 0;

	int Hash(const K& key) const
	{
		std::hash<K> hasher;
		return hasher(key) ^ (hasher(key) >> 16);
	}

	uint64_t FunnelProbe(const K& key, int hash) const
	{
		uint64_t levelWidth = capacity >> 1;
		uint64_t offset = 0;

		do
		{
			uint64_t levelIndex = hash & (levelWidth - 1);
			uint64_t tableIndex = offset + levelIndex;

			const Entry& entry = table[tableIndex];
			if (entry.IsEmpty() || (!entry.IsTombstone() && entry.hash == hash && entry.key == key))
			{
				return tableIndex;
			}

			offset |= levelWidth;
			levelWidth >>= 1;
		}
		while (levelWidth > 0);

		return -1;
	}

	void Resize()
	{
		assert(capacity < MAXIMUM_CAPACITY);
		uint64_t newCapacity = std::min(capacity << 1, MAXIMUM_CAPACITY);

		Entry* oldTable = table;
		table = new Entry[newCapacity]();
		uint64_t oldCapacity = capacity;
		capacity = newCapacity;
		size = 0;
		tombstones = 0;

		for (uint64_t i = 0; i < oldCapacity; ++i)
		{
			const Entry& entry = oldTable[i];
			if (!entry.IsEmpty() && !entry.IsTombstone())
			{
				uint64_t idx = FunnelProbe(entry.key, entry.hash);
				table[idx] = entry;
				size++;
			}
		}
		delete[] oldTable;
	}

	void Cleanup()
	{
		Entry* oldTable = table;
		table = new Entry[capacity]();
		uint64_t newSize = 0;
		for (uint64_t i = 0; i < capacity; ++i)
		{
			const Entry& entry = oldTable[i];
			if (!entry.IsEmpty() && !entry.IsTombstone())
			{
				uint64_t idx = FunnelProbe(entry.key, entry.hash);
				if (table[idx].IsEmpty())
				{
					table[idx] = entry;
					newSize++;
				}
			}
		}
		size = newSize;
		tombstones = 0;
		delete[] oldTable;
	}

public:
	FunnelFlatHashTable() : table(new Entry[DEFAULT_INITIAL_CAPACITY]())
	{
	}

	~FunnelFlatHashTable()
	{
		delete[] table;
	}

	class Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::pair<const K&, V&>;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(Entry* table, uint64_t capacity, uint64_t index)
			: table(table), capacity(capacity), index(index)
		{
			AdvanceToValidEntry();
		}

		std::pair<const K&, V&> operator*() const
		{
			return {table[index].key, table[index].value};
		}

		Iterator& operator++()
		{
			++index;
			AdvanceToValidEntry();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const Iterator& other) const
		{
			return index == other.index;
		}

		bool operator!=(const Iterator& other) const
		{
			return !(*this == other);
		}

	private:
		void AdvanceToValidEntry()
		{
			while (index < capacity && (table[index].IsEmpty() || table[index].IsTombstone()))
			{
				++index;
			}
		}

		Entry* table;
		uint64_t capacity;
		uint64_t index;
	};

	Iterator begin()
	{
		return Iterator(table, capacity, 0);
	}

	Iterator end()
	{
		return Iterator(table, capacity, capacity);
	}

	V Get(const K& key) const
	{
		int64_t hash = this->Hash(key);

		uint64_t levelWidth = capacity >> 1;
		uint64_t offset = 0;

		do
		{
			int64_t levelIndex = hash & (levelWidth - 1);
			uint64_t tableIndex = offset + levelIndex;

			const Entry& entry = table[tableIndex];
			if (entry.IsEmpty())
			{
				return V();
			}
			if (!entry.IsTombstone() && entry.hash == hash && entry.key == key)
			{
				return entry.value;
			}

			offset |= levelWidth;
			levelWidth >>= 1;
		}
		while (levelWidth > 0);

		return V();
	}

	V Put(const K& key, const V& value)
	{
		if ((size + tombstones + 1.0) / capacity > loadFactor)
		{
			assert(capacity != MAXIMUM_CAPACITY);
			Resize();
		}
		int64_t hash = this->Hash(key);

		int64_t tombstoneIndex = -1;

		uint64_t levelWidth = capacity >> 1;
		uint64_t offset = 0;

		do
		{
			int64_t levelIndex = hash & (levelWidth - 1);
			uint64_t tableIndex = offset + levelIndex;

			Entry& entry = table[tableIndex];
			if (entry.IsEmpty() || (!entry.IsTombstone() && entry.hash == hash && entry.key == key))
			{
				if (entry.IsEmpty())
				{
					if (tombstoneIndex != -1)
					{
						tableIndex = tombstoneIndex;
						tombstones--;
					}
					table[tableIndex] = Entry(key, value, hash);
					size++;
					return V();
				}
				else
				{
					V oldVal = entry.value;
					entry.value = value;
					return oldVal;
				}
			}
			else if (entry.IsTombstone() && tombstoneIndex == -1)
			{
				tombstoneIndex = tableIndex;
			}

			offset |= levelWidth;
			levelWidth >>= 1;
		}
		while (levelWidth > 0);

		Resize();
		return Put(key, value);
	}

	V Remove(const K& key)
	{
		int64_t hash = this->Hash(key);

		uint64_t levelWidth = capacity >> 1;
		uint64_t offset = 0;

		do
		{
			int64_t levelIndex = hash & (levelWidth - 1);
			uint64_t tableIndex = offset + levelIndex;

			Entry& entry = table[tableIndex];
			if (entry.IsEmpty())
			{
				return V();
			}
			if (!entry.IsTombstone() && entry.hash == hash && entry.key == key)
			{
				V oldVal = entry.value;
				entry.SetTombstone();
				size--;
				tombstones++;
				if (tombstones > capacity >> 2)
				{
					Cleanup();
				}
				return oldVal;
			}

			offset |= levelWidth;
			levelWidth >>= 1;
		}
		while (levelWidth > 0);

		return V();
	}

	void Clear()
	{
		delete[] table;
		table = new Entry[DEFAULT_INITIAL_CAPACITY]();
		size = 0;
		capacity = DEFAULT_INITIAL_CAPACITY;
		tombstones = 0;
	}

	int GetSize() const { return size; }

	bool IsEmpty() const { return size == 0; }
};
