#ifndef _SDL_RESIZE_H
#define _SDL_RESIZE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <SDL.h>

#include "resize.h"

int SDLSurfaceResize(SDL_Surface *src,
                     SDL_Surface *dst,
                     const FilterTypes filter,
                     const double blur);

SDL_Surface *SDLSurfaceResizeWrap(SDL_Surface *src,
                                  Uint64 w,
                                  Uint64 h,
                                  const FilterTypes filter,
                                  const double blur);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
