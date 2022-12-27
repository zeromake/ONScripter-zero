#include <SDL.h>
#include <SDL_main.h>
#include <SDL_ttf.h>
#include <freetype/freetype.h>

#ifdef __APPLE__
#define DEFAUTL_TTF "/System/Library/Fonts/PingFang.ttc"
#elif defined(_WIN32)
#include <windows.h>
#define DEFAUTL_TTF "C:\\Windows\\Fonts\\msyh.ttc"
// #define DEFAUTL_TTF "C:\\Users\\admin\\Desktop\\FiraSans-Medium.ttf"
#endif


// 67,425
// 15,40
// 80,425
// 21,40
// 94,425
SDL_Surface* _DrawText(TTF_Font* font, const char* text) {
  static SDL_Color fcol={0xff, 0x00, 0x00}, bcol={0xff, 0xff, 0xff};
  SDL_Surface *text_surface;
  text_surface = TTF_RenderUTF8_Blended(font, text, fcol);
  return text_surface;
}

#undef main
int main() {
    int windowScale = 1;
    int textScale = 1;
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Rect mRect = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight};
    window = SDL_CreateWindow(
        "SDL Tutorial",
        mRect.x,
        mRect.y,
        mRect.w,
        mRect.h,
        SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
        windowScale = textScale;
    }
#elif defined(_WIN32)
    textScale = GetDpiForSystem() / 96;
#endif
    TTF_Init();
    TTF_Font *font = TTF_OpenFontDPI(DEFAUTL_TTF, 16, 72*textScale, 72*textScale);
    TTF_SetFontHinting(font, FT_LOAD_NO_HINTING);
    SDL_Surface* screen = (_DrawText)(font, u8"测试中文，heavy!");
    auto texture = SDL_CreateTextureFromSurface(renderer, screen);
    int textureW = screen->w;
    int textureH = screen->h;
    SDL_Rect srcRect{0, 0,textureW, textureH };
    SDL_Rect dstRect{130, 50, textureW, textureH};
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
        SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
