#ifndef __ONS_CACHE_H__
#define __ONS_CACHE_H__
#include <SDL.h>

#include <config.hpp>
#include <infra/cache.hpp>
#include <infra/lru_cache_policy.hpp>

namespace onscache {
class SurfaceNode
{
private:
    onscripter::Vector<uint8_t> _pixels;
    int _width;
    int _height;
    int _depth;
    Uint32 _format;
public:
    SurfaceNode(SDL_Surface* surface) {
        auto size = surface->w * surface->h * 4;
        _pixels.resize(size);
        _width = surface->w;
        _height = surface->h;
        _depth = surface->format->BitsPerPixel;
        _format = surface->format->format;
        SDL_LockSurface(surface);
        memcpy(_pixels.data(), surface->pixels, size);
        SDL_UnlockSurface(surface);
    };
    SDL_Surface* CreateSurface() {
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(
            SDL_SWSURFACE,
            _width,
            _height,
            _depth,
            _format
        );
        auto size = surface->w * surface->h * 4;
        SDL_LockSurface(surface);
        memcpy(surface->pixels, _pixels.data(), size);
        SDL_UnlockSurface(surface);
        return surface;
    };
};

typedef caches::fixed_sized_cache<
    onscripter::String,
    onscripter::SharedPtr<onscripter::Vector<uint8_t>>,
    caches::LRUCachePolicy>
    ImageBufferCache;
typedef caches::fixed_sized_cache<
    onscripter::String,
    onscripter::SharedPtr<SurfaceNode>,
    caches::LRUCachePolicy>
    ImageSurfaceCache;
}  // namespace onscache

#endif
