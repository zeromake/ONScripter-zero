#ifndef __ONS_CACHE_H__
#define __ONS_CACHE_H__
#include <SDL.h>

#include <infra/cache.hpp>
#include <infra/lru_cache_policy.hpp>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#define USE_IMAGE_CACHE

namespace onscache {
typedef caches::fixed_sized_cache<std::string,
                                  std::shared_ptr<std::vector<uint8_t>>,
                                  caches::LRUCachePolicy>
    ImageBufferCache;
}  // namespace onscache

#endif
