#include <SDL.h>
#include <SDL_main.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    SDL_bool run = SDL_TRUE;
    SDL_Rect mRect = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight};
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow("SDL Tutorial", mRect.x, mRect.y, mRect.w, mRect.h,
                                SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int windowScale = 1;
    SDL_Rect viewport_rect = {0, 0, 400*windowScale, 300*windowScale};
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    while (!exit)
    {
        while (SDL_PollEvent(&event))
            switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_f)
                        run = run == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
                    break;
            }
        if (run) {
            viewport_rect.x += 1;
            if (viewport_rect.x >= 400) {
                viewport_rect.x = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x7f, 0x00, 0xff);
        SDL_RenderFillRect(renderer, &viewport_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}