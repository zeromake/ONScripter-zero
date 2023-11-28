#include "SDL_resize.h"

static SDL_bool InitMagickImage(MagickImage *out, SDL_Surface *img) {
    out->pixels = (MagickPixelPacket4 *)img->pixels;
    switch (img->format->format) {
        case SDL_PIXELFORMAT_RGBA32:
            out->order = (MagickPixelOrder){0, 1, 2, 3};
            break;
        case SDL_PIXELFORMAT_ARGB32:
            out->order = (MagickPixelOrder){1, 2, 3, 0};
            break;
        case SDL_PIXELFORMAT_BGRA32:
            out->order = (MagickPixelOrder){2, 1, 0, 3};
            break;
        case SDL_PIXELFORMAT_ABGR32:
            out->order = (MagickPixelOrder){3, 2, 1, 0};
            break;
        default:
            return SDL_FALSE;
    }
    out->columns = img->w;
    out->rows = img->h;
    return SDL_TRUE;
}

int SDLSurfaceResize(SDL_Surface *src,
                     SDL_Surface *dst,
                     const FilterTypes filter,
                     const double blur) {
    SDL_assert(src != NULL);
    SDL_assert(dst != NULL);
    SDL_LockSurface(src);
    SDL_LockSurface(dst);
    int ret = 0;
    MagickImage source;
    MagickImage destination;
    if (!InitMagickImage(&source, src)) {
        ret = -1;
        goto done;
    }
    if (!InitMagickImage(&destination, dst)) {
        ret = -2;
        goto done;
    }
    ret = ResizeImage(&source, &destination, filter, blur);
done:
    SDL_UnlockSurface(src);
    SDL_UnlockSurface(dst);
    return ret;
}

SDL_Surface *SDLSurfaceResizeWrap(SDL_Surface *src,
                                  Uint64 w,
                                  Uint64 h,
                                  const FilterTypes filter,
                                  const double blur) {
    SDL_PixelFormat *fmt = src->format;
    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(SDL_SWSURFACE,
                                            w,
                                            h,
                                            fmt->BitsPerPixel,
                                            fmt->format);
    if (dst == NULL) {
        return NULL;
    }
    int ret = SDLSurfaceResize(src, dst, filter, blur);
    if (ret != 0) {
        SDL_FreeSurface(dst);
        return NULL;
    }
    return dst;
}