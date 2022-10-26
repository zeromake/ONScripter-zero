#include <SDL.h>
#include <SDL_ttf.h>
#include <cstdio>

// macosx https://discourse.libsdl.org/t/high-dpi-mode/34411
// https://github.com/libsdl-org/SDL/blob/main/docs/README-ios.md#notes----retina--high-dpi-and-window-sizes

SDL_Surface* _DrawText(TTF_Font* font, const char* text) {
  static SDL_Color fcol={0xff, 0x00, 0x00}, bcol={0xff, 0xff, 0xff};
  SDL_Surface *text_surface;
  text_surface = TTF_RenderUTF8_Blended(font, text, fcol);
  return text_surface;
}
#ifdef __APPLE__
#define DEFAUTL_TTF "/System/Library/Fonts/PingFang.ttc"
#elif defined(_WIN32)
#include <windows.h>
#define DEFAUTL_TTF "C:\\Windows\\Fonts\\msyh.ttc"
#endif


#undef main
int main()
{
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    bool run = true;

    TTF_Init();

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    int windowScale = 1;
    int textScale = 1;
#ifdef __APPLE__
    int rw = 0, rh = 0;
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
    if(rw != windowWidth) {
        float widthScale = (float)rw / (float) windowWidth;
        float heightScale = (float)rh / (float) windowHeight;
        if(widthScale != heightScale) {
            fprintf(stderr, "WARNING: width scale != height scale\n");
        }
        // SDL_RenderSetScale(renderer, widthScale, heightScale);
        textScale = (int)widthScale;
        windowScale = textScale
    }
#elif defined(_WIN32)
    textScale = GetDpiForSystem() / 96;
#endif
    SDL_Rect viewport_rect = {0, 0, 400*windowScale, 300*windowScale};
    TTF_Font *font = TTF_OpenFontDPI(DEFAUTL_TTF, 14, 72*textScale, 72*textScale);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    SDL_Surface* screen = (_DrawText)(font, "测试文章");
    auto texture = SDL_CreateTextureFromSurface(renderer, screen);
    int textureW = screen->w;
    int textureH = screen->h;
    // 获取贴图的宽和高
    SDL_FreeSurface(screen);

    SDL_Rect dstRect{ 130, 50, textureW, textureH };
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
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}