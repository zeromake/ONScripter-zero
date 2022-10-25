#include <SDL.h>
#include <SDL_ttf.h>

// macosx https://discourse.libsdl.org/t/high-dpi-mode/34411
// https://github.com/libsdl-org/SDL/blob/main/docs/README-ios.md#notes----retina--high-dpi-and-window-sizes

SDL_Surface* DrawText(TTF_Font* font, const char* text) {
  static SDL_Color fcol={0xff, 0x00, 0x00}, bcol={0xff, 0xff, 0xff};
  SDL_Surface *text_surface;
  text_surface = TTF_RenderUTF8_Shaded(font, text, fcol, bcol);
  return text_surface;
}

int main()
{
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect viewport_rect = {0, 0, 400, 300};
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    bool run = true;

    TTF_Init();
    TTF_Font *font = TTF_OpenFontDPI("/Users/zero/Documents/project/ONScripter-Jh/build/macosx/x86_64/release/default.ttf", 24, 72 * 2, 72 * 2);

    // SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    int rw = 0, rh = 0;
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
    if(rw != windowWidth) {
        float widthScale = (float)rw / (float) windowWidth;
        float heightScale = (float)rh / (float) windowHeight;
        if(widthScale != heightScale) {
            fprintf(stderr, "WARNING: width scale != height scale\n");
        }
        SDL_RenderSetScale(renderer, widthScale, heightScale);
    }
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    SDL_Surface* screen = DrawText(font, "测试文章");
    auto texture = SDL_CreateTextureFromSurface(renderer, screen);
    SDL_FreeSurface(screen);
    int textureW;
    int textureH;
    // 获取贴图的宽和高
    SDL_QueryTexture(texture, nullptr, nullptr, &textureW, &textureH);
    SDL_Rect imageRect{ 0, 0, textureW, textureH };
    SDL_Rect dstRect{ 130, 50, textureW/2, textureH/2 };
    // SDL_RenderSetViewport(renderer, &viewport_rect);
    while (!exit)
    {
        while (SDL_PollEvent(&event))
            switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_f)
                        run = !run;
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
        SDL_RenderCopy(renderer, texture, &imageRect, &dstRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}