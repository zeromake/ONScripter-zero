#include <SDL.h>
#include <SDL_main.h>
#include <stdio.h>
#include "ren-font.h"
#include <vector>
#include <infra/filesystem.hpp>
#include <chrono>

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

long long unix_now() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

#undef main
int main(int argc, char *argv[])
{
    auto rootNow = unix_now();
    int windowWidth = 800 * 2;
    int windowHeight = 600 * 2;
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
#ifdef __APPLE__
    mRect.w /= 2;
    mRect.h /= 2;
#endif
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow("SDL Tutorial", mRect.x, mRect.y, mRect.w, mRect.h, SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int windowScale = 1;
    int textScale = 1;
#ifdef __APPLE__
    textScale = 2;
    windowScale = 2;
#elif defined(_WIN32)
    textScale = GetDpiForSystem() / 96;
    windowScale = textScale;
#endif
    int renderer_w;
    int renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);
    // SDL_Rect viewport_rect = {0, 0, rect_width*windowScale, rect_height*windowScale};
    // SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    // SDL_RenderClear(renderer);
    // auto windowSurface = SDL_CreateRGBSurface(0, 800, 600, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);;
    renderer_w *= windowScale;
    renderer_h *= windowScale;
    auto windowSurface = SDL_CreateRGBSurfaceWithFormat(0, renderer_w, renderer_h, 32, SDL_PIXELFORMAT_BGRA32);
    ren_init(window, windowSurface, windowScale);

    printf("windowSurface: %ld\n", unix_now() - rootNow);
    SDL_Rect dstRect{ 0, 0, renderer_w, renderer_h };
    Command cmd;
    auto ttfPath = std::fs::absolute(std::fs::current_path().parent_path() / ".." / ".." / ".." / ".." / "fonts" / DEFAUTL_TTF);
    auto ttfPathStr = ttfPath.string();
    printf("load: %s\n", ttfPathStr.c_str());
    cmd.fonts[0] = ren_font_load(ttfPathStr.c_str(), 16, FONT_ANTIALIASING_SUBPIXEL, FONT_HINTING_SLIGHT, FONT_STYLE_UNDERLINE);
    if (!cmd.fonts[0]) {
        printf("load font error!");
        return -1;
    }
    printf("root font: %ld\n", unix_now() - rootNow);
    size_t fontOffset = 1;
    for (auto it: fonts) {
        cmd.fonts[fontOffset] = ren_font_load(it.c_str(), 16, FONT_ANTIALIASING_SUBPIXEL, FONT_HINTING_SLIGHT, FONT_STYLE_UNDERLINE);
        if (!cmd.fonts[fontOffset]) {
            printf("load font error!");
            return -1;
        }
        fontOffset++;
    }
    printf("all font: %ld\n", unix_now() - rootNow);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, windowWidth*windowScale, windowHeight*windowScale);
    ren_draw_rect(*(RenRect*)&dstRect, RenColor{0xff, 0xff, 0xff, 0xff});
    printf("draw rect: %ld\n", unix_now() - rootNow);
    ren_draw_text(cmd.fonts, u8"中文测试 \"License\" shall SDL Tutorial", 42, 130, 50, RenColor{0x00, 0x00, 0x00, 0xff});
    // ren_draw_text(cmd.fonts, u8"中文测试 \"License\" shall", 29, 130, 100 , RenColor{0x00, 0x00, 0x00, 0xff});

    printf("draw text: %ld\n", unix_now() - rootNow);
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
        update_rect(texture, windowSurface, (RenRect*)&dstRect);
        SDL_RenderCopy(renderer, texture, &dstRect, &dstRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}