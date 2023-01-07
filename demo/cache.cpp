#include <string>
#include "cache/cache.hpp"
#include "cache/lru_cache_policy.hpp"
#include <iostream>

// alias for easy class typing
template <typename Key, typename Value>
using lru_cache_t = typename caches::fixed_sized_cache<Key, Value, caches::LRUCachePolicy>;

void foo() {
  constexpr std::size_t CACHE_SIZE = 100;
  lru_cache_t<uint32_t, uintptr_t> cache(CACHE_SIZE);

  cache.Put(1, 1);
  cache.Put(2, 2);

  std::cout << cache.Get(1) << cache.Get(2) << std::endl;
}

int main() {
    foo();
    return 0;
}