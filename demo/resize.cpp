#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "resize.h"


#define CHECK_SDL_ERROR(ret)                       \
    if (ret != 0) {                                \
        printf("sdl error: %s\n", SDL_GetError()); \
        return 2;                                  \
    }

#undef main
int main(int argc, char* argv[]) {
    int windowWidth = 800;
    int windowHeight = 600;
    int ret = 0;
    ret = SDL_Init(SDL_INIT_VIDEO);
    CHECK_SDL_ERROR(ret);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    ret = SDL_CreateWindowAndRenderer(windowWidth, windowHeight,
                                      SDL_WINDOW_ALLOW_HIGHDPI, &window,
                                      &renderer);
    CHECK_SDL_ERROR(ret);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    SDL_Surface *src_img = IMG_Load(argv[1]);
    if (src_img == NULL) {
        printf("load image err: %s\n", SDL_GetError());
        return 1;
    }
    auto fmt = src_img->format;
    SDL_Surface *img = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        src_img->w * 2,
        src_img->h * 2,
        fmt->BitsPerPixel,
        fmt->Rmask,
        fmt->Gmask,
        fmt->Bmask,
        fmt->Amask
    );
    printf("src_img: %d x %d\n", src_img->w, src_img->h);
    printf("img: %d x %d\n", img->w, img->h);
    ResizeImage(src_img, img, UndefinedFilter, 1.0);
    // SDL_UpperBlitScaled(src_img, NULL, img, NULL);
    SDL_Texture* texture = 
        SDL_CreateTextureFromSurface(renderer, img);
    while (!exit) {
        while (SDL_PollEvent(&event)) switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
            }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(renderer);

        SDL_Rect imgRect = {0, 0, img->w, img->h};

        SDL_UpdateTexture(texture, &imgRect,
                          (unsigned char*)img->pixels,
                          img->pitch);

        SDL_RenderCopy(renderer, texture, &imgRect, &imgRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
