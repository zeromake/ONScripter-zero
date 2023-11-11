#include "resize/SDL_resize.h"


#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

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
    ret = SDL_CreateWindowAndRenderer(windowWidth,
                                      windowHeight,
                                      SDL_WINDOW_ALLOW_HIGHDPI,
                                      &window,
                                      &renderer);
    CHECK_SDL_ERROR(ret);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    SDL_Surface* src_img = IMG_Load(argv[1]);
    if (src_img == NULL) {
        printf("load image err: %s\n", SDL_GetError());
        return 1;
    }
    auto fmt = src_img->format;
    SDL_Surface* img = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                            src_img->w * 2,
                                            src_img->h * 2,
                                            fmt->BitsPerPixel,
                                            fmt->Rmask,
                                            fmt->Gmask,
                                            fmt->Bmask,
                                            fmt->Amask);
    printf("src_img: %d x %d\n", src_img->w, src_img->h);
    printf("img: %d x %d\n", img->w, img->h);
    SDLSurfaceResize(src_img, img, UndefinedFilter, 1.0);
    // SDL_UpperBlitScaled(src_img, NULL, img, NULL);
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, src_img);
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, img);
    while (!exit) {
        while (SDL_PollEvent(&event)) switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
            }

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

        SDL_Rect imgRectSrc1 = {0, 0, src_img->w, src_img->h};
        SDL_Rect imgRect1 = {0, 0, src_img->w, src_img->h};
        SDL_Rect imgRectSrc2 = {0, 0, img->w, img->h};
        SDL_Rect imgRect2 = {img->w, 0, img->w, img->h};

        SDL_RenderCopy(renderer, texture1, &imgRectSrc1, &imgRect1);
        SDL_RenderCopy(renderer, texture2, &imgRectSrc2, &imgRect2);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
