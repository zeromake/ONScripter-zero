#include <SDL.h>
#include <SDL_ttf.h>

#include <cstdio>
#include <infra/filesystem.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

// macosx https://discourse.libsdl.org/t/high-dpi-mode/34411
// https://github.com/libsdl-org/SDL/blob/main/docs/README-ios.md#notes----retina--high-dpi-and-window-sizes

SDL_Surface* _DrawText(TTF_Font* font, const char* text) {
    static SDL_Color fcol = {0x00, 0x00, 0x00, 0xff},
                     bcol = {0xff, 0xff, 0xff, 0xff};
    SDL_Surface* text_surface;
    text_surface = TTF_RenderUTF8_LCD(font, text, fcol, bcol);
    return text_surface;
}
#define DEFAUTL_TTF "JetBrainsMono-Regular.ttf"

#undef main
int main() {
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
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_ALLOW_HIGHDPI, &window,
                                &renderer);
    int textScale = 1;
#ifdef __APPLE__
    int rw = 0, rh = 0;
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
    if (rw != windowWidth) {
        float widthScale = (float)rw / (float)windowWidth;
        float heightScale = (float)rh / (float)windowHeight;
        if (widthScale != heightScale) {
            fprintf(stderr, "WARNING: width scale != height scale\n");
        }
        // SDL_RenderSetScale(renderer, widthScale, heightScale);
        textScale = (int)widthScale;
    }
#elif defined(_WIN32)
    textScale = GetDpiForSystem() / 96;
#endif
    auto ttfPath =
        std::fs::absolute(std::fs::current_path().parent_path() / ".." / ".." /
                          ".." / ".." / "fonts" / DEFAUTL_TTF);
    std::string ttfPathStr =
        ttfPath
            .string();  //"/System/Library/Fonts/Helvetica.ttc";////"C:\\Windows\\Fonts\\msyh.ttc";//
    printf("load: %s\n", ttfPathStr.c_str());
    TTF_Font* font = TTF_OpenFontIndex(ttfPathStr.c_str(), 16 * textScale, 0);
    TTF_SetFontHinting(font, TTF_HINTING_NONE);
    SDL_Surface* screen =
        _DrawText(font, u8"中文测试 \"License\" shall SDL Tutorial");
    auto texture = SDL_CreateTextureFromSurface(renderer, screen);
    int textureW = screen->w;
    int textureH = screen->h;
    // 获取贴图的宽和高
    SDL_FreeSurface(screen);

    SDL_Rect dstRect{130, 50, textureW, textureH};
    while (!exit) {
        while (SDL_PollEvent(&event)) switch (event.type) {
                case SDL_QUIT:
                    exit = SDL_TRUE;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_f) run = !run;
                    break;
            }
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}