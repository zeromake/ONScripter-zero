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

namespace onscache {
    typedef struct SurfaceNode {
        public:
            SurfaceNode(SDL_Surface *surface): value(surface) {}
            ~SurfaceNode() {
                if (value) {
                    SDL_FreeSurface(value);
                    value = nullptr;
                }
            }
            SDL_Surface* Get(){
                return value;
            }
        private:
            SDL_Surface *value;
    } SurfaceNode;
    typedef caches::fixed_sized_cache<std::string, std::shared_ptr<onscache::SurfaceNode>, caches::LRUCachePolicy> SurfaceCache;
}  // namespace onscache

#endif
