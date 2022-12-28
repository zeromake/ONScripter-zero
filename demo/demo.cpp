#include <SDL.h>
#include <SDL_main.h>
#include <stdio.h>
#include "ren-font.h"
#include <vector>
#include <infra/filesystem.hpp>

#ifdef ANDROID
#include <android/log.h>
#endif
#define DEFAUTL_TTF "JetBrainsMono-Regular.ttf"

#ifdef __APPLE__
// #define DEFAUTL_TTF "/System/Library/Fonts/PingFang.ttc"
#elif defined(_WIN32)
#include <windows.h>
// #define DEFAUTL_TTF "C:\\Users\\admin\\Desktop\\FiraSans-Medium.ttf"
#endif
void update_rect(SDL_Texture* texture, SDL_Surface* surface, const RenRect *r) {
    int scale = 1;
    const int x = scale * r->x, y = scale * r->y;
    const int w = scale * r->width, h = scale * r->height;
    const SDL_Rect sr = {x, y, w, h};
    int32_t *pixels = ((int32_t *) surface->pixels) + x + surface->w * y;
    SDL_UpdateTexture(texture, &sr, pixels, surface->w * 4);
}
#undef main
int main(int argc, char *argv[])
{
    int windowWidth = 800;
    int windowHeight = 600;
    int rect_width = 400;
    int rect_height = 300;
    SDL_Init(SDL_INIT_VIDEO);
    std::vector<std::string> fonts;
#ifdef __APPLE__
    fonts.push_back("/System/Library/Fonts/PingFang.ttc");
#else
    fonts.push_back("C:\\Windows\\Fonts\\msyh.ttc");
#endif

#ifdef ANDROID
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);
    windowWidth = mode.w;
    windowHeight = mode.h;
    __android_log_print(ANDROID_LOG_INFO, "MAIN", "%d * %d", windowWidth, windowHeight);
#endif
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_bool exit = SDL_FALSE;
    SDL_bool run = SDL_TRUE;
    SDL_Rect mRect = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight};
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow("SDL Tutorial", mRect.x, mRect.y, mRect.w, mRect.h,
                                SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int windowScale = 1;
    int textScale = 1;
#ifdef __APPLE__
    int rw = 0, rh = 0;
    SDL_GetRendererOutputSize(renderer,&rw, &rh);
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
    // SDL_Rect viewport_rect = {0, 0, rect_width*windowScale, rect_height*windowScale};
    // SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    // SDL_RenderClear(renderer);
    // auto windowSurface = SDL_CreateRGBSurface(0, 800, 600, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);;
    auto windowSurface = SDL_CreateRGBSurfaceWithFormat(0, windowWidth*textScale,windowHeight*textScale, 30, SDL_PIXELFORMAT_BGRA32);
    ren_init(window, windowSurface);
    SDL_Rect dstRect{ 0, 0, windowWidth, windowHeight };
    RenRect windowRect{ 0, 0, windowWidth*textScale, windowHeight*textScale };
    Command cmd;
    auto ttfPath = std::filesystem::canonical(std::fs::current_path().parent_path() / "..\\..\\..\\..\\fonts" / DEFAUTL_TTF);
    auto ttfPathStr = ttfPath.string();
    printf("load: %s\n", ttfPathStr.c_str());
    cmd.fonts[0] = ren_font_load(ttfPathStr.c_str(), 32, FONT_ANTIALIASING_SUBPIXEL, FONT_HINTING_SLIGHT, 0);
    size_t fontOffset = 1;
    for (auto it: fonts) {
        cmd.fonts[fontOffset] = ren_font_load(it.c_str(), 32, FONT_ANTIALIASING_SUBPIXEL, FONT_HINTING_SLIGHT, 0);
        fontOffset++;
    }
    
    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, 800*2, 600*2);
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
        // if (run) {
        //     viewport_rect.x += 1;
        //     if (viewport_rect.x >= windowWidth - rect_width) {
        //         viewport_rect.x = 0;
        //     }
        // }
        // #242424
        ren_draw_rect(windowRect, RenColor{0x24, 0x24, 0x24, 0xff});
        // #D2DED7
        ren_draw_text(cmd.fonts, u8"中文测试 \"License\" shall", 29, 130, 50, RenColor{0xdf, 0xdf, 0xdf, 0xff});
        // SDL_UpdateWindowSurface(window);
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);
        update_rect(texture, windowSurface, &windowRect);
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        // SDL_SetRenderDrawColor(renderer, 0x00, 0x7f, 0x00, 0xff);
        // SDL_RenderFillRect(renderer, &viewport_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    // SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}