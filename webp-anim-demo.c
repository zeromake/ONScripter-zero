#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#define CHECK_SDL_ERROR(ret) if (ret != 0) {\
    printf("sdl error: %s\n", SDL_GetError());\
    return 2;\
}



void blendOver(png_bytep* dst, png_bytep* src, Uint32 x, Uint32 y, Uint32 w, Uint32 h) {
    Uint32 i, j;
    int u, v, al;
    for (j = 0; j < h; j++)
    {
        png_bytep sp = src[j];
        png_bytep dp = dst[j + y] + x * 4;

        for (i = 0; i < w; i++, sp += 4, dp += 4)
        {
            if (sp[3] == 255)
                memcpy(dp, sp, 4);
            else if (sp[3] != 0)
            {
                if (dp[3] != 0)
                {
                    u = sp[3] * 255;
                    v = (255 - sp[3]) * dp[3];
                    al = u + v;
                    dp[0] = (sp[0] * u + dp[0] * v) / al;
                    dp[1] = (sp[1] * u + dp[1] * v) / al;
                    dp[2] = (sp[2] * u + dp[2] * v) / al;
                    dp[3] = al / 255;
                }
                else
                    memcpy(dp, sp, 4);
            }
        }
    }
}

IMG_Animation* IMG_LoadPNGAnimation_RW(SDL_RWops *src) {
    const char *error;
    png_structp png_ptr;
    png_infop info_ptr;
    IMG_Animation* anim;
    Uint32 frameCount;
    Uint32 playCount;
    int width, height, bit_depth, color_type, interlace_type;
    Uint32 Rmask, Gmask, Bmask, Amask, ckey = -1;
    png_byte num_channels;
    png_bytep* first_row_pointers;
    png_bytep* row_pointers;
    SDL_Color *colors;
    png_color_16 *transv;

    if ( !src ) {
        /* The error message has been set in SDL_RWFromFile */
        return NULL;
    }

    if ( (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0 ) {
        return NULL;
    }
    error = NULL;
    png_ptr = NULL; info_ptr = NULL;
    /* Create the PNG loading context structure */
    png_ptr = lib.png_create_read_struct(PNG_LIBPNG_VER_STRING,
                      NULL,NULL,NULL);
    if (png_ptr == NULL){
        error = "Couldn't allocate memory for PNG file or incompatible PNG dll";
        goto done;
    }

     /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = lib.png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        error = "Couldn't create image information for PNG file";
        goto done;
    }
    #ifdef PNG_SETJMP_SUPPORTED
#ifdef _MSC_VER
#pragma warning(disable:4611)   /* warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable */
#endif
#ifndef LIBPNG_VERSION_12
    if ( setjmp(*lib.png_set_longjmp_fn(png_ptr, longjmp, sizeof (jmp_buf))) )
#else
    if ( setjmp(png_ptr->jmpbuf) )
#endif
    {
        error = "Error reading the PNG file.";
        goto done;
    }
#endif
    /* Set up the input control */
    lib.png_set_read_fn(png_ptr, src, png_read_data);


    lib.png_read_info(png_ptr, info_ptr);

    if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL)) {
        error = "Not a apng image.";
        goto done;
    }

    lib.png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
            &color_type, &interlace_type, NULL, NULL);


    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
    lib.png_set_strip_16(png_ptr);

    /* tell libpng to de-interlace (if the image is interlaced) */
    lib.png_set_interlace_handling(png_ptr);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
     * byte into separate bytes (useful for paletted and grayscale images).
     */
    lib.png_set_packing(png_ptr);

    /* scale greyscale values to the range 0..255 */
    if (color_type == PNG_COLOR_TYPE_GRAY)
        lib.png_set_expand(png_ptr);
    if ( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
        lib.png_set_gray_to_rgb(png_ptr);


    Rmask = Gmask = Bmask = Amask = 0 ;
    num_channels = lib.png_get_channels(png_ptr, info_ptr);
    if ( num_channels >= 3 ) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        Rmask = 0x000000FF;
        Gmask = 0x0000FF00;
        Bmask = 0x00FF0000;
        Amask = (num_channels == 4) ? 0xFF000000 : 0;
#else
        int s = (num_channels == 4) ? 0 : 8;
        Rmask = 0xFF000000 >> s;
        Gmask = 0x00FF0000 >> s;
        Bmask = 0x0000FF00 >> s;
        Amask = 0x000000FF >> s;
#endif
    }

    frameCount = 1;
    png_get_acTL(png_ptr, info_ptr, &frameCount, &playCount);
    if (frameCount <= 1) {
        error = "apng image not has frame.";
        goto done;
    }

    anim = (IMG_Animation *)SDL_malloc(sizeof(IMG_Animation));
    anim->w = width;
    anim->h = height;
    anim->count = frameCount;
    anim->frames = (SDL_Surface **)SDL_calloc(anim->count, sizeof(*anim->frames));
    anim->delays = (int *)SDL_calloc(anim->count, sizeof(*anim->delays));
    row_pointers = (png_bytep*) SDL_malloc(sizeof(png_bytep)*height);
    if (!row_pointers) {
        error = "Out of memory";
        goto done;
    }
    first_row_pointers = (png_bytep*) SDL_malloc(sizeof(png_bytep)*height);
    if (!first_row_pointers) {
        error = "Out of memory";
        goto done;
    }
    png_uint_32 w0;
    png_uint_32 h0;
    png_uint_32 x0;
    png_uint_32 y0;
    unsigned short delay_num = 1;
    unsigned short delay_den = 10;
    png_byte dop = 0;
    png_byte bop = 0;
    SDL_bool firstFrameIsHidden = png_get_first_frame_is_hidden(png_ptr, info_ptr) != 0;
    png_uint_32 first = firstFrameIsHidden ? 1:0;
    int png_num_palette;
    png_colorp png_palette;
    lib.png_get_PLTE(png_ptr, info_ptr, &png_palette, &png_num_palette);
    int paletteCount;
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        paletteCount = 256;
        colors = (SDL_Color*)SDL_malloc(sizeof(SDL_Color) * paletteCount);
        if (!colors) {
            error = "Colors Out of memory";
            goto done;
        }
        for (int i = 0; i < 256; i++) {
            colors[i].r = (Uint8)i;
            colors[i].g = (Uint8)i;
            colors[i].b = (Uint8)i;
        }
    } else {
        paletteCount = png_num_palette;
        colors = (SDL_Color*)SDL_malloc(sizeof(SDL_Color) * paletteCount);
        if (!colors) {
            error = "Colors Out of memory";
            goto done;
        }
        for (int i=0; i<paletteCount; i++) {
            colors[i].b = png_palette[i].blue;
            colors[i].g = png_palette[i].green;
            colors[i].r = png_palette[i].red;
        }
    }
    for(int frame_idx = 0; frame_idx < (anim->count); frame_idx++) {
        SDL_Surface *surface = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            width,
            height,
            bit_depth*num_channels,
            Rmask,Gmask,Bmask,Amask);
        if (surface == NULL) {
            error = SDL_GetError();
            goto done;
        }
        if (ckey != -1) {
            if (color_type != PNG_COLOR_TYPE_PALETTE) {
                /* FIXME: Should these be truncated or shifted down? */
                ckey = SDL_MapRGB(surface->format,
                            (Uint8)transv->red,
                            (Uint8)transv->green,
                            (Uint8)transv->blue);
            }
            SDL_SetColorKey(surface, SDL_TRUE, ckey);
        }
        anim->frames[frame_idx] = surface;
        for (int row = 0; row < (int)height; row++) {
            row_pointers[row] = (png_bytep)(Uint8 *)surface->pixels + row*surface->pitch;
        }
        png_read_frame_head(png_ptr, info_ptr);
        png_get_next_frame_fcTL(png_ptr, info_ptr, &w0, &h0, &x0, &y0, &delay_num, &delay_den, &dop, &bop);
        if (frame_idx == first) {
            bop = PNG_BLEND_OP_SOURCE;
            for (int row = 0; row < (int)height; row++) {
                first_row_pointers[row] = (png_bytep)(Uint8*)surface->pixels + row*surface->pitch;
            }
        }
        png_read_image(png_ptr, &row_pointers);
        if (bop == PNG_BLEND_OP_OVER) {
            blendOver(row_pointers, first_row_pointers, x0, y0, w0, h0);
        }
        anim->delays[frame_idx] = delay_num * 1000 / delay_den;
        SDL_Palette *palette = surface->format->palette;
        if (palette) {
            palette->ncolors = paletteCount;
            for (int i = 0; i < paletteCount; i++) {
                palette->colors[i].r = colors[i].r;
                palette->colors[i].g = colors[i].g;
                palette->colors[i].b = colors[i].b;
            }
        }
    }
done:
    if (png_ptr) {
        lib.png_destroy_read_struct(&png_ptr,
                                info_ptr ? &info_ptr : (png_infopp)0,
                                (png_infopp)0);
    }
    if (row_pointers) {
        SDL_free(row_pointers);
    }
    if (first_row_pointers) {
        SDL_free(first_row_pointers);
    }
    if (colors) {
        SDL_free(colors);
    }
    if (error) {
        IMG_SetError("%s", error);
        if (anim) {
            IMG_FreeAnimation(anim);
            anim = NULL;
        }
    }
    return(anim);
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
