#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

#include <absl/container/flat_hash_map.h>

#include "FunnelFlatHashTable.hpp"


int main() {
    std::vector<int> keys(1'000'000);
    std::iota(keys.begin(), keys.end(), 1);
    std::ranges::shuffle(keys.begin(), keys.end(), std::mt19937{ std::random_device{}() });

    {
        std::cout << "absl::flat_hash_map benchmark:" << std::endl;
        absl::flat_hash_map<int, int> _map;
        // PUT
        auto start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            _map.insert({ key, key * 2 });
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "  Put time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // GET
        start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            volatile int val = _map.at(key);
            (void)val;
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Get time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // ITERATE
        start = std::chrono::high_resolution_clock::now();
        for (const auto& [k, v] : _map) {
            (void)k;
            (void)v;
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Iterate time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // REMOVE
        start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {  
            _map.erase(key);
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Remove time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }

    {
        std::cout << "std::unordered_map benchmark:" << std::endl;
        std::unordered_map<int, int> _map;
        // PUT
        auto start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            _map.insert({ key, key * 2 });
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "  Put time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // GET
        start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            volatile int val = _map.at(key);
            (void)val;
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Get time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // ITERATE
        start = std::chrono::high_resolution_clock::now();
        for (const auto& [k, v] : _map) {
            (void)k;
            (void)v;
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Iterate time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // REMOVE
        start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            _map.erase(key);
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Remove time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }

    {
        std::cout << "FunnelFlatHashTable benchmark:" << std::endl;
        FunnelFlatHashTable<int, int> _map;
        // PUT
        auto start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            _map.Put(key, key * 2);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "  Put time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // GET
        start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            volatile int val = _map.Get(key);
            (void)val;
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Get time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // ITERATE
        start = std::chrono::high_resolution_clock::now();
        for (const auto& [k, v] : _map) {
            (void)k;
            (void)v;
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Iterate time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

        // REMOVE
        start = std::chrono::high_resolution_clock::now();
        for (int key : keys) {
            _map.Remove(key);
        }
        end = std::chrono::high_resolution_clock::now();
        std::cout << "  Remove time: " << duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }

    return 0;
}