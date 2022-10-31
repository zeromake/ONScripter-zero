#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
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

// bool windowColorKey(SDL_Window *window, COLORREF colorKey) {
//     SDL_SysWMinfo wmInfo;
//     SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
//     SDL_GetWindowWMInfo(window, &wmInfo);
//     HWND hWnd = wmInfo.info.win.window;

//     // Change window type to layered
//     SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

//     // Set transparency color
//     return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
// }


// IMG_Animation *IMG_LoadWEBPAnimation_RW(SDL_RWops *src)
// {
//     Sint64 start;
//     const char *error = NULL;
//     SDL_Surface *surface = NULL;
//     Uint32 Rmask;
//     Uint32 Gmask;
//     Uint32 Bmask;
//     Uint32 Amask;
//     WebPBitstreamFeatures features;
//     int raw_data_size;
//     uint8_t *raw_data = NULL;
//     int r;
//     uint8_t *ret;
//     struct WebPDemuxer* dmuxer = NULL;
//     IMG_Animation *anim = NULL;

//     if ( !src ) {
//         /* The error message has been set in SDL_RWFromFile */
//         return NULL;
//     }

//     start = SDL_RWtell(src);

//     if ( (IMG_Init(IMG_INIT_WEBP) & IMG_INIT_WEBP) == 0 ) {
//         goto error;
//     }

//     raw_data_size = -1;
//     if ( !webp_getinfo( src, &raw_data_size ) ) {
//         error = "Invalid WEBP";
//         goto error;
//     }

//     raw_data = (uint8_t*) SDL_malloc( raw_data_size );
//     if ( raw_data == NULL ) {
//         error = "Failed to allocate enough buffer for WEBP";
//         goto error;
//     }

//     r = (int)SDL_RWread(src, raw_data, 1, raw_data_size );
//     if ( r != raw_data_size ) {
//         error = "Failed to read WEBP";
//         goto error;
//     }

// #if 0
//     // extract size of picture, not interesting since we don't know about alpha channel
//     int width = -1, height = -1;
//     if ( !WebPGetInfo( raw_data, raw_data_size, &width, &height ) ) {
//         printf("WebPGetInfo has failed\n" );
//         return NULL;
//     }
// #endif

//     if ( lib.WebPGetFeaturesInternal( raw_data, raw_data_size, &features, WEBP_DECODER_ABI_VERSION ) != VP8_STATUS_OK ) {
//         error = "WebPGetFeatures has failed";
//         goto error;
//     }

//     /* Check if it's ok !*/
// #if SDL_BYTEORDER == SDL_LIL_ENDIAN
//     Rmask = 0x000000FF;
//     Gmask = 0x0000FF00;
//     Bmask = 0x00FF0000;
//     Amask = (features.has_alpha) ? 0xFF000000 : 0;
// #else
//     {
//         int s = (features.has_alpha) ? 0 : 8;
//         Rmask = 0xFF000000 >> s;
//         Gmask = 0x00FF0000 >> s;
//         Bmask = 0x0000FF00 >> s;
//         Amask = 0x000000FF >> s;
//     }
// #endif

//     WebPData wd = { raw_data , raw_data_size};
//     dmuxer = lib.WebPDemux(&wd);
//     ;
//     WebPIterator iter;
//     anim = (IMG_Animation *)SDL_malloc(sizeof(IMG_Animation));
//     anim->w = features.width;
//     anim->h = features.height;
//     anim->count = lib.WebPDemuxGetI(dmuxer, WEBP_FF_FRAME_COUNT);
//     anim->frames = (SDL_Surface **)SDL_calloc(anim->count, sizeof(*anim->frames));
//     anim->delays = (int *)SDL_calloc(anim->count, sizeof(*anim->delays));
//     for (int frame_idx = 0; frame_idx < (anim->count); frame_idx++) {
//         if (lib.WebPDemuxGetFrame(dmuxer, frame_idx, &iter) == 0) {
//             break;
//         }
//         SDL_Surface* curr = SDL_CreateRGBSurface(SDL_SWSURFACE,
//             features.width, features.height,
//             features.has_alpha?32:24, Rmask,Gmask,Bmask,Amask);
//         if ( curr == NULL ) {
//             error = "Failed to allocate SDL_Surface";
//             goto error;
//         }

//         if ( features.has_alpha ) {
//             ret = lib.WebPDecodeRGBAInto(
//                 iter.fragment.bytes,
//                 iter.fragment.size,
//                 (uint8_t *)surface->pixels,
//                 surface->pitch * surface->h,
//                 surface->pitch);
//         } else {
//             ret = lib.WebPDecodeRGBInto(
//                 iter.fragment.bytes, iter.fragment.size,
//                 (uint8_t *)surface->pixels,
//                 surface->pitch * surface->h,
//                 surface->pitch);
//         }
//         if (ret == NULL) {
//             SDL_FreeSurface(curr);
//             break;
//         }
//         anim->frames[frame_idx] = curr;
//         anim->delays[frame_idx] = iter.duration;
//     }
//     if (dmuxer) {
//         WebPDemuxDelete(dmuxer);
//     }

//     if ( raw_data ) {
//         SDL_free( raw_data );
//     }
//     return anim;
// error:
//     if (anim) {
//         IMG_FreeAnimation(anim);
//     }
//     if (dmuxer) {
//         WebPDemuxDelete(dmuxer);
//     }
//     if ( raw_data ) {
//         SDL_free( raw_data );
//     }

//     if ( surface ) {
//         SDL_FreeSurface( surface );
//     }

//     if ( error ) {
//         IMG_SetError( "%s", error );
//     }
//     SDL_RWseek(src, start, RW_SEEK_SET);
//     return(NULL);
// }


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
    SDL_Rect mRect = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight};

    TTF_Init();

    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow("SDL Tutorial", mRect.x, mRect.y, mRect.w, mRect.h,
                                SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
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
        windowScale = textScale;
    }
#elif defined(_WIN32)
    textScale = GetDpiForSystem() / 96;
#endif
    SDL_Rect viewport_rect = {0, 0, 400*windowScale, 300*windowScale};
    // TTF_Font *font = TTF_OpenFontDPI(DEFAUTL_TTF, 14*2, 72*textScale, 72*textScale);
    // IMG_Animation *pAnim = IMG_LoadAnimation("C:\\Users\\admin\\Desktop\\demo01.webp");
    // if (pAnim == NULL) {
    //     printf("%s\n", SDL_GetError());
    //     return 1;
    // }
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    // auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, /**SDL_TEXTUREACCESS_TARGET |*/
    //                                     SDL_TEXTUREACCESS_STREAMING,
    //                                     windowWidth, windowHeight);
    // SDL_Surface* screen = (_DrawText)(font, "测试文章");
    // auto texture = SDL_CreateTextureFromSurface(renderer, screen);
    // int textureW = screen->w;
    // int textureH = screen->h;
    // 获取贴图的宽和高
    // SDL_FreeSurface(screen);
    // windowColorKey(window, RGB(0, 0, 0));
    // SDL_Rect dstRect{ 130, 50, textureW, textureH };
    int frameIndex = 0;
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
        
        // auto pImageSurface = pAnim->frames[frameIndex];
        // SDL_Rect imgRect {0,0, pImageSurface->w, pImageSurface->h};
        // SDL_Rect targetRect {0,0, pImageSurface->w*2, pImageSurface->h*2};
        // SDL_UpdateTexture(texture, &imgRect, (unsigned char*)pImageSurface->pixels, pImageSurface->pitch);
        // SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        // SDL_RenderClear(renderer);
        // SDL_BlitSurface(pImageSurface, nullptr, pWindowSurface, &imgRect);
        // SDL_UpdateWindowSurface(window);
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x7f, 0x00, 0xff);
        SDL_RenderFillRect(renderer, &viewport_rect);
        // SDL_RenderCopy(renderer, texture, &imgRect, &targetRect);
        SDL_RenderPresent(renderer);
        // SDL_Delay(pAnim->delays[frameIndex]);
        // frameIndex++;
        // if (frameIndex >= pAnim->count) {
        //     frameIndex = 0;
        // }
        SDL_Delay(16);
    }
    // IMG_FreeAnimation(pAnim);
    // SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}