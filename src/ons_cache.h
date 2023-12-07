#ifndef __ONS_CACHE_H__
#define __ONS_CACHE_H__
#include <SDL.h>

#include <infra/Config.hpp>
#include <infra/cache.hpp>
#include <infra/lru_cache_policy.hpp>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace onscache {
typedef caches::fixed_sized_cache<
    onscripter::String,
    onscripter::SharedPtr<onscripter::Vector<uint8_t>>,
    caches::LRUCachePolicy>
    ImageBufferCache;
}  // namespace onscache

#endif
