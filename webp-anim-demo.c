#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#define CHECK_SDL_ERROR(ret) if (ret != 0) {\
    printf("sdl error: %s\n", SDL_GetError());\
    return 2;\
}

#undef main
int main(int argc, char* argv[])
{
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
    ret = SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    CHECK_SDL_ERROR(ret);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);

    IMG_Animation *anim = IMG_LoadAnimation(argv[1]);
    if (anim == NULL) {
        printf("load image err: %s\n", SDL_GetError());
        return 1;
    }
    int frameIndex = 0;
    SDL_Surface* firstImageSurface = anim->frames[0];
    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        renderer,
        firstImageSurface);
    firstImageSurface = NULL;
    while (!exit)
    {
        while (SDL_PollEvent(&event))
            switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
            }
        

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

        SDL_Surface* imageSurface = anim->frames[frameIndex];
        SDL_Rect imgRect = {0,0, imageSurface->w, imageSurface->h};

        SDL_UpdateTexture(texture, &imgRect, (unsigned char*)imageSurface->pixels, imageSurface->pitch);

        SDL_RenderCopy(renderer, texture, &imgRect, &imgRect);
        SDL_RenderPresent(renderer);

        SDL_Delay(anim->delays[frameIndex]);
        frameIndex++;
        if (frameIndex >= anim->count) {
            frameIndex = 0;
        }
    }
    IMG_FreeAnimation(anim);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}