#include <stdint.h>
#include <benchmark/benchmark.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STBIR_SIMD
#define STBIR_SSE2
#define STBIR_AVX2
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include <stb/stb_image_resize2.h>
#include <resize_image.h>
#include <resize/resize.h>

#define RESIZE_COUNT 16

typedef struct _Image {
    unsigned char* pixels;
    int w;
    int h;
} Image;

static Image* LoadImage(std::string filename) {
    int w, h, n;
    stbi_set_unpremultiply_on_load(1);
	stbi_convert_iphone_png_to_rgb(1);
    Image* img = new Image();
	img->pixels = stbi_load(filename.c_str(), &w, &h, &n, 4);
    img->w = w;
    img->h = h;
    return img;
}

static void DestroyImage(Image* img) {
    if (img->pixels) {
        delete []img->pixels;
    }
    delete img;
}

static Image* AllocateImage(int w, int h) {
    Image* img = new Image();
    img->pixels = new unsigned char[w*h*4];
    img->w = w;
    img->h = h;
    return img;
}

static void BM_ImageResize0(benchmark::State& state) {
    auto src = LoadImage("demo/resize.png");
    auto dst = AllocateImage(src->w*RESIZE_COUNT, src->h*RESIZE_COUNT);
    size_t len = src->w * (src->h + 1) * 4 + 4;
    auto resize_buffer = new unsigned char[len];
    for (auto _ : state) {
        resizeImage(
                dst->pixels,
                dst->w,
                dst->h,
                dst->w * 4,
                src->pixels,
                src->w,
                src->h,
                src->w * 4,
                4,
                resize_buffer,
                src->w * 4,
                false);
    }
    delete []resize_buffer;
    DestroyImage(src);
    DestroyImage(dst);
}


static void BM_ImageResize2(benchmark::State& state) {
    auto src = LoadImage("demo/resize.png");
    auto dst = AllocateImage(src->w*RESIZE_COUNT, src->h*RESIZE_COUNT);
    MagickImage source = {(MagickPixelPacket4*)src->pixels, {0, 1, 2, 3}, (uint64_t)src->w, (uint64_t)src->h};
    MagickImage destination = {(MagickPixelPacket4*)dst->pixels, {0, 1, 2, 3}, (uint64_t)dst->w, (uint64_t)dst->h};
    for (auto _ : state) {
        ResizeImage(&source, &destination, PointFilter, 1.0);
    }
    DestroyImage(src);
    DestroyImage(dst);
}

static void BM_ImageResizeSTB_LINEAR(benchmark::State& state) {
    auto src = LoadImage("demo/resize.png");
    auto dst = AllocateImage(src->w*RESIZE_COUNT, src->h*RESIZE_COUNT);
    for (auto _ : state) {
        stbir_resize_uint8_linear(
            src->pixels,
            src->w,
            src->h,
            0,
            dst->pixels,
            dst->w,
            dst->h,
            0,
            STBIR_RGBA
        );
    }
    DestroyImage(src);
    DestroyImage(dst);
}

static void BM_ImageResizeSTB_SRGB(benchmark::State& state) {
    auto src = LoadImage("demo/resize.png");
    auto dst = AllocateImage(src->w*RESIZE_COUNT, src->h*RESIZE_COUNT);
    for (auto _ : state) {
        stbir_resize_uint8_srgb(
            src->pixels,
            src->w,
            src->h,
            0,
            dst->pixels,
            dst->w,
            dst->h,
            0,
            STBIR_RGBA
        );
    }
    DestroyImage(src);
    DestroyImage(dst);
}


BENCHMARK(BM_ImageResize0);
BENCHMARK(BM_ImageResize2);
BENCHMARK(BM_ImageResizeSTB_LINEAR);
BENCHMARK(BM_ImageResizeSTB_SRGB);
BENCHMARK_MAIN();
