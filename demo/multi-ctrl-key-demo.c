#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int windowWidth = 800;
    int windowHeight = 600;
    int rect_width = 400;
    int rect_height = 300;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    SDL_bool run = SDL_FALSE;
    SDL_Rect mRect = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                      windowWidth, windowHeight};
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow("SDL Tutorial", mRect.x, mRect.y, mRect.w,
                              mRect.h, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int windowScale = 1;
    SDL_Rect viewport_rect = {0, 0, rect_width * windowScale,
                              rect_height * windowScale};
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    int up_count = 0;
    int down_count = 0;
    while (!exit) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LCTRL ||
                        event.key.keysym.sym == SDLK_RCTRL) {
                        run = SDL_FALSE;
                        printf("key up: %d\n", up_count++);
                        fflush(stdout);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_LCTRL ||
                        event.key.keysym.sym == SDLK_RCTRL) {
                        run = SDL_TRUE;
                        printf("key down: %d\n", down_count++);
                        fflush(stdout);
                    }
                    break;
            }
        }
        if (run) {
            viewport_rect.x += 1;
            if (viewport_rect.x >= windowWidth - rect_width) {
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
