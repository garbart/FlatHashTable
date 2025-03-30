## FlatHashTable
This repository contains implementation of a flat hash map using open addressing with linear probing, inspired by the paper "[Optimal Bounds for Open Addressing Without Reordering](https://arxiv.org/pdf/2501.02305)" by Martín Farach-Colton, Andrew Krapivin, and William Kuszmaul (2025).

The implementation itself is contained in the FunnelFlatHashTable file.hpp, you can download it and use it in your projects. However, I do not recommend doing this because the implementation is very crude.

## Benchamrk results
10M elemnts:
```
absl::flat_hash_map benchmark:
  Put time: 26947 ms
  Get time: 8824 ms
  Iterate time: 1522 ms
  Remove time: 13798 ms
std::unordered_map benchmark:
  Put time: 10444 ms
  Get time: 1674 ms
  Iterate time: 713 ms
  Remove time: 4140 ms
FunnelFlatHashTable benchmark:
  Put time: 3416 ms
  Get time: 1844 ms
  Iterate time: 435 ms
  Remove time: 2182 ms 
```
## Plans
&#x2610; STL-style inerface

&#x2610; Full iterator support

&#x2610; Error and corner cases handling

&#x2610; A more accurate benchmark

&#x2610; etc

## Discuss
If you have any questions, suggestions, or just have something to say, you can write to my Telegram channel: https://t.me/partypooper_cpp. Also, feel free to make pool requests if you want to help with development.

## Links
* https://github.com/ascv0228/elastic-funnel-hashing
* https://github.com/sternma/optopenhash
* https://github.com/royvanrijn/optimalopen
