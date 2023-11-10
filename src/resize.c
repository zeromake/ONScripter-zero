#include "resize.h"

#include <SDL.h>
#include <stdio.h>

typedef struct _ContributionInfo {
    double weight;
    long pixel;
} ContributionInfo;

typedef struct _FilterInfo {
    double (*function)(const double, const double), support;
} FilterInfo;

typedef struct _DoublePixelPacket {
    double red, green, blue, opacity;
} DoublePixelPacket;

typedef unsigned char Quantum;

typedef struct _RGBAPixelPacket {
    Quantum red, green, blue, opacity;
} RGBAPixelPacket;

typedef struct _ARGBPixelPacket {
    Quantum opacity, red, green, blue;
} ARGBPixelPacket;

typedef struct _BGRAPixelPacket {
    Quantum blue, green, red, opacity;
} BGRAPixelPacket;

static double J1(double x) {
    double p, q;

    register long i;

    static const double Pone[] = {0.581199354001606143928050809e+21,
                                  -0.6672106568924916298020941484e+20,
                                  0.2316433580634002297931815435e+19,
                                  -0.3588817569910106050743641413e+17,
                                  0.2908795263834775409737601689e+15,
                                  -0.1322983480332126453125473247e+13,
                                  0.3413234182301700539091292655e+10,
                                  -0.4695753530642995859767162166e+7,
                                  0.270112271089232341485679099e+4},
                        Qone[] = {0.11623987080032122878585294e+22,
                                  0.1185770712190320999837113348e+20,
                                  0.6092061398917521746105196863e+17,
                                  0.2081661221307607351240184229e+15,
                                  0.5243710262167649715406728642e+12,
                                  0.1013863514358673989967045588e+10,
                                  0.1501793594998585505921097578e+7,
                                  0.1606931573481487801970916749e+4,
                                  0.1e+1};

    p = Pone[8];
    q = Qone[8];
    for (i = 7; i >= 0; i--) {
        p = p * x * x + Pone[i];
        q = q * x * x + Qone[i];
    }
    return (p / q);
}

static double P1(double x) {
    double p, q;

    register long i;

    static const double Pone[] = {0.352246649133679798341724373e+5,
                                  0.62758845247161281269005675e+5,
                                  0.313539631109159574238669888e+5,
                                  0.49854832060594338434500455e+4,
                                  0.2111529182853962382105718e+3,
                                  0.12571716929145341558495e+1},
                        Qone[] = {0.352246649133679798068390431e+5,
                                  0.626943469593560511888833731e+5,
                                  0.312404063819041039923015703e+5,
                                  0.4930396490181088979386097e+4,
                                  0.2030775189134759322293574e+3,
                                  0.1e+1};

    p = Pone[5];
    q = Qone[5];
    for (i = 4; i >= 0; i--) {
        p = p * (8.0 / x) * (8.0 / x) + Pone[i];
        q = q * (8.0 / x) * (8.0 / x) + Qone[i];
    }
    return (p / q);
}

static double Q1(double x) {
    double p, q;
    register long i;
    static const double Pone[] = {0.3511751914303552822533318e+3,
                                  0.7210391804904475039280863e+3,
                                  0.4259873011654442389886993e+3,
                                  0.831898957673850827325226e+2,
                                  0.45681716295512267064405e+1,
                                  0.3532840052740123642735e-1},
                        Qone[] = {0.74917374171809127714519505e+4,
                                  0.154141773392650970499848051e+5,
                                  0.91522317015169922705904727e+4,
                                  0.18111867005523513506724158e+4,
                                  0.1038187585462133728776636e+3,
                                  0.1e+1};

    p = Pone[5];
    q = Qone[5];
    for (i = 4; i >= 0; i--) {
        p = p * (8.0 / x) * (8.0 / x) + Pone[i];
        q = q * (8.0 / x) * (8.0 / x) + Qone[i];
    }
    return (p / q);
}

static double BesselOrderOne(double x) {
    double p, q;
    if (x == 0.0) return (0.0);
    p = x;
    if (x < 0.0) x = (-x);
    if (x < 8.0) return (p * J1(x));
    q = sqrt(2.0 / (MagickPI * x)) *
        (P1(x) * (1.0 / sqrt(2.0) * (sin(x) - cos(x))) -
         8.0 / x * Q1(x) * (-1.0 / sqrt(2.0) * (sin(x) + cos(x))));
    if (p < 0.0) q = (-q);
    return (q);
}

static double Bessel(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x == 0.0) return (MagickPI / 4.0);
    return (BesselOrderOne(MagickPI * x) / (2.0 * x));
}

static double Sinc(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x == 0.0) return (1.0);
    return (sin(MagickPI * x) / (MagickPI * x));
}

static double Blackman(const double x, const double support) {
    ARG_NOT_USED(support);
    return (0.42 + 0.5 * cos(MagickPI * x) + 0.08 * cos(2 * MagickPI * x));
}

static double BlackmanBessel(const double x, const double support) {
    return (Blackman(x / support, support) * Bessel(x, support));
}

static double BlackmanSinc(const double x, const double support) {
    return (Blackman(x / support, support) * Sinc(x, support));
}

static double Box(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x < -0.5) return (0.0);
    if (x < 0.5) return (1.0);
    return (0.0);
}

static double Catrom(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x < -2.0) return (0.0);
    if (x < -1.0) return (0.5 * (4.0 + x * (8.0 + x * (5.0 + x))));
    if (x < 0.0) return (0.5 * (2.0 + x * x * (-5.0 - 3.0 * x)));
    if (x < 1.0) return (0.5 * (2.0 + x * x * (-5.0 + 3.0 * x)));
    if (x < 2.0) return (0.5 * (4.0 + x * (-8.0 + x * (5.0 - x))));
    return (0.0);
}

static double Cubic(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x < -2.0) return (0.0);
    if (x < -1.0) return ((2.0 + x) * (2.0 + x) * (2.0 + x) / 6.0);
    if (x < 0.0) return ((4.0 + x * x * (-6.0 - 3.0 * x)) / 6.0);
    if (x < 1.0) return ((4.0 + x * x * (-6.0 + 3.0 * x)) / 6.0);
    if (x < 2.0) return ((2.0 - x) * (2.0 - x) * (2.0 - x) / 6.0);
    return (0.0);
}

static double Gaussian(const double x, const double support) {
    ARG_NOT_USED(support);
    return (exp(-2.0 * x * x) * sqrt(2.0 / MagickPI));
}

static double Hanning(const double x, const double support) {
    ARG_NOT_USED(support);
    return (0.5 + 0.5 * cos(MagickPI * x));
}

static double Hamming(const double x, const double support) {
    ARG_NOT_USED(support);
    return (0.54 + 0.46 * cos(MagickPI * x));
}

static double Hermite(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x < -1.0) return (0.0);
    if (x < 0.0) return ((2.0 * (-x) - 3.0) * (-x) * (-x) + 1.0);
    if (x < 1.0) return ((2.0 * x - 3.0) * x * x + 1.0);
    return (0.0);
}

static double Lanczos(const double x, const double support) {
    if (x < -3.0) return (0.0);
    if (x < 0.0) return (Sinc(-x, support) * Sinc(-x / 3.0, support));
    if (x < 3.0) return (Sinc(x, support) * Sinc(x / 3.0, support));
    return (0.0);
}

static double Mitchell(const double x, const double support) {
#define B (1.0 / 3.0)
#define C (1.0 / 3.0)
#define P0 ((6.0 - 2.0 * B) / 6.0)
#define P2 ((-18.0 + 12.0 * B + 6.0 * C) / 6.0)
#define P3 ((12.0 - 9.0 * B - 6.0 * C) / 6.0)
#define Q0 ((8.0 * B + 24.0 * C) / 6.0)
#define Q1 ((-12.0 * B - 48.0 * C) / 6.0)
#define Q2 ((6.0 * B + 30.0 * C) / 6.0)
#define Q3 ((-1.0 * B - 6.0 * C) / 6.0)

    ARG_NOT_USED(support);
    if (x < -2.0) return (0.0);
    if (x < -1.0) return (Q0 - x * (Q1 - x * (Q2 - x * Q3)));
    if (x < 0.0) return (P0 + x * x * (P2 - x * P3));
    if (x < 1.0) return (P0 + x * x * (P2 + x * P3));
    if (x < 2.0) return (Q0 + x * (Q1 + x * (Q2 + x * Q3)));
    return (0.0);
}

static double Quadratic(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x < -1.5) return (0.0);
    if (x < -0.5) return (0.5 * (x + 1.5) * (x + 1.5));
    if (x < 0.5) return (0.75 - x * x);
    if (x < 1.5) return (0.5 * (x - 1.5) * (x - 1.5));
    return (0.0);
}

static double Triangle(const double x, const double support) {
    ARG_NOT_USED(support);
    if (x < -1.0) return (0.0);
    if (x < 0.0) return (1.0 + x);
    if (x < 1.0) return (1.0 - x);
    return (0.0);
}

static const FilterInfo filters[SincFilter + 1] = {{Box, 0.0},
                                                   {Box, 0.0},
                                                   {Box, 0.5},
                                                   {Triangle, 1.0},
                                                   {Hermite, 1.0},
                                                   {Hanning, 1.0},
                                                   {Hamming, 1.0},
                                                   {Blackman, 1.0},
                                                   {Gaussian, 1.25},
                                                   {Quadratic, 1.5},
                                                   {Cubic, 2.0},
                                                   {Catrom, 2.0},
                                                   {Mitchell, 2.0},
                                                   {Lanczos, 3.0},
                                                   {BlackmanBessel, 3.2383},
                                                   {BlackmanSinc, 4.0}};

static MagickPassFail HorizontalFilter(
    const SDL_Surface *restrict source,
    SDL_Surface *restrict destination,
    const double x_factor,
    const FilterInfo *restrict filter_info,
    const double blur,
    ContributionInfo *restrict view_data_set) {
    double scale, support;
    DoublePixelPacket zero;
    long x;
    const SDL_bool matte = SDL_ISPIXELFORMAT_ALPHA(destination->format->format);
    MagickPassFail status = MagickPass;
    scale = blur * Max(1.0 / x_factor, 1.0);
    support = scale * filter_info->support;
    if (support <= 0.5) {
        support = 0.5 + MagickEpsilon;
        scale = 1.0;
    }
    scale = 1.0 / scale;
    (void)memset(&zero, 0, sizeof(DoublePixelPacket));
    ContributionInfo *restrict contribution = view_data_set;
    ARGBPixelPacket *source_pixels = (ARGBPixelPacket *)source->pixels;
    ARGBPixelPacket *destination_pixels =
        (ARGBPixelPacket *)destination->pixels;
    printf("HorizontalFilter source: %d x %d %d\n",
           source->w,
           source->h,
           source->w * source->h);
    printf("HorizontalFilter destination: %d x %d %d\n",
           destination->w,
           destination->h,
           destination->w * destination->h);

    for (x = 0; x < (long)destination->w; x++) {
        double center, density;
        long n, start, stop, y;

        MagickPassFail thread_status;
        thread_status = status;
        if (thread_status == MagickFail) continue;
        center = (double)(x + 0.5) / x_factor;
        start = (long)Max(center - support + 0.5, 0);
        stop = (long)Min(center + support + 0.5, source->w);
        density = 0.0;
        for (n = 0; n < (stop - start); n++) {
            contribution[n].pixel = start + n;
            contribution[n].weight = filter_info->function(
                scale * ((double)start + n - center + 0.5),
                filter_info->support);
            density += contribution[n].weight;
        }
        if ((density != 0.0) && (density != 1.0)) {
            /*
                Normalize.
            */
            long i;

            density = 1.0 / density;
            for (i = 0; i < n; i++) contribution[i].weight *= density;
        }
        size_t p_offset = contribution[0].pixel;
        ARGBPixelPacket *p = (source_pixels + p_offset);
        size_t q_offset = x;
        ARGBPixelPacket *q = (destination_pixels + q_offset);

        if (thread_status != MagickFail) {
            if (matte) {
                for (long y = 0; y < (long)destination->h; y++) {
                    double transparency_coeff, normalize, weight;
                    DoublePixelPacket pixel;
                    long j;
                    long i;
                    pixel = zero;
                    normalize = 0.0;
                    long yy = (y / destination->h) * destination->h +
                              (y % destination->h);
                    for (i = 0; i < n; i++) {
                        j = y * (contribution[n - 1].pixel -
                                 contribution[0].pixel + 1) +
                            (contribution[i].pixel - contribution[0].pixel);
                        weight = contribution[i].weight;
                        long jj = (j / source->h) * source->h + (j % source->h);
                        // printf("jj: [%d, %d] %d %d\n", x, y, j, jj +
                        // p_offset); printf("HorizontalFilter: [%d, %d] [%d,
                        // %d] [%d, %d] [%d, %d]\n", x, y, i, j, p_offset,
                        // q_offset, jj, yy); fflush(stdout);
                        transparency_coeff =
                            weight *
                            (1 - ((double)p[jj].opacity / TransparentOpacity));
                        pixel.red += transparency_coeff * p[jj].red;
                        pixel.green += transparency_coeff * p[jj].green;
                        pixel.blue += transparency_coeff * p[jj].blue;
                        pixel.opacity += weight * p[jj].opacity;
                        normalize += transparency_coeff;
                    }
                    normalize = 1.0 / (AbsoluteValue(normalize) <= MagickEpsilon
                                           ? 1.0
                                           : normalize);
                    pixel.red *= normalize;
                    pixel.green *= normalize;
                    pixel.blue *= normalize;
                    q[yy].red = RoundDoubleToQuantum(pixel.red);
                    q[yy].green = RoundDoubleToQuantum(pixel.green);
                    q[yy].blue = RoundDoubleToQuantum(pixel.blue);
                    q[yy].opacity = RoundDoubleToQuantum(pixel.opacity);
                }
            }
        }
    }
    return status;
}
static MagickPassFail VerticalFilter(const SDL_Surface *restrict source,
                                     SDL_Surface *restrict destination,
                                     const double y_factor,
                                     const FilterInfo *restrict filter_info,
                                     const double blur,
                                     ContributionInfo *restrict view_data_set) {
    const SDL_bool matte = SDL_ISPIXELFORMAT_ALPHA(destination->format->format);
    MagickPassFail status = MagickPass;
    double scale = blur * Max(1.0 / y_factor, 1.0);
    double support = scale * filter_info->support;
    if (support <= 0.5) {
        support = 0.5 + MagickEpsilon;
        scale = 1.0;
    }
    scale = 1.0 / scale;
    DoublePixelPacket zero;
    (void)memset(&zero, 0, sizeof(DoublePixelPacket));
    ContributionInfo *restrict contribution = view_data_set;
    ARGBPixelPacket *source_pixels = (ARGBPixelPacket *)source->pixels;
    ARGBPixelPacket *destination_pixels =
        (ARGBPixelPacket *)destination->pixels;
    printf("VerticalFilter source: %d x %d %d\n",
           source->w,
           source->h,
           source->w * source->h);
    printf("VerticalFilter destination: %d x %d %d\n",
           destination->w,
           destination->h,
           destination->w * destination->h);

    for (int y = 0; y < destination->h; y++) {
        MagickPassFail thread_status;
        thread_status = status;
        if (thread_status == MagickFail) continue;
        double center = (double)(y + 0.5) / y_factor;
        long start = (long)Max(center - support + 0.5, 0);
        long stop = (long)Min(center + support + 0.5, source->h);
        double density = 0.0;
        long n = 0;
        for (n = 0; n < (stop - start); n++) {
            contribution[n].pixel = start + n;
            contribution[n].weight = filter_info->function(
                scale * ((double)start + n - center + 0.5),
                filter_info->support);
            density += contribution[n].weight;
        }
        if (density != 0.0 && density != 1.0) {
            long i;
            density = 1.0 / density;
            for (i = 0; i < n; i++) contribution[i].weight *= density;
        }
        size_t p_offset = source->w * contribution[0].pixel;
        ARGBPixelPacket *p = (source_pixels + p_offset);
        size_t q_offset = destination->w * y;
        ARGBPixelPacket *q = (destination_pixels + q_offset);

        if (thread_status != MagickFail) {
            if (matte) {
                for (long x = 0; x < (long)destination->w; x++) {
                    double transparency_coeff, normalize, weight;
                    DoublePixelPacket pixel = zero;
                    long j;
                    long i;
                    normalize = 0.0;
                    for (i = 0; i < n; i++) {
                        j = (long)((contribution[i].pixel -
                                    contribution[0].pixel) *
                                       source->w +
                                   x);
                        weight = contribution[i].weight;
                        // printf("VerticalFilter: [%d, %d] [%d, %d]\n", x, y,
                        // i, j);
                        transparency_coeff =
                            weight *
                            (1 - ((double)p[j].opacity / TransparentOpacity));
                        pixel.red += transparency_coeff * p[j].red;
                        pixel.green += transparency_coeff * p[j].green;
                        pixel.blue += transparency_coeff * p[j].blue;
                        pixel.opacity += weight * p[j].opacity;
                        normalize += transparency_coeff;
                    }
                    normalize = 1.0 / (AbsoluteValue(normalize) <= MagickEpsilon
                                           ? 1.0
                                           : normalize);
                    pixel.red *= normalize;
                    pixel.green *= normalize;
                    pixel.blue *= normalize;
                    q[x].red = RoundDoubleToQuantum(pixel.red);
                    q[x].green = RoundDoubleToQuantum(pixel.green);
                    q[x].blue = RoundDoubleToQuantum(pixel.blue);
                    q[x].opacity = RoundDoubleToQuantum(pixel.opacity);
                }
            }
        }
    }
    return status;
}

int ResizeImage(const SDL_Surface *src,
                SDL_Surface *dst,
                const FilterTypes filter,
                const double blur) {
    int columns = dst->w;
    int rows = dst->h;
    double support, x_factor, x_support, y_factor, y_support;
    int i = 0;
    size_t span;
    MagickPassFail status;
    Uint64 quantum;
    SDL_bool order;
    SDL_assert(((int)filter >= 0) && ((int)filter <= SincFilter));

    if (src->w == 0 || src->h == 0 || columns == 0 || rows == 0) {
        return 1;
    }
    printf("src fotmat: %d\n", src->format->format);
    if (columns == src->w && rows == src->h && blur == 1.0) {
        SDL_BlitSurface(src, NULL, dst, NULL);
        return 0;
    }

    order = (((double)columns * ((size_t)src->h + rows)) >
             ((double)rows * ((size_t)src->w + columns)));
    SDL_Surface *source_image = NULL;
    SDL_PixelFormat *fmt = src->format;
    if (order) {
        source_image = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                            columns,
                                            src->h,
                                            fmt->BitsPerPixel,
                                            fmt->Rmask,
                                            fmt->Gmask,
                                            fmt->Bmask,
                                            fmt->Amask);
    } else {
        source_image = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                            src->w,
                                            rows,
                                            fmt->BitsPerPixel,
                                            fmt->Rmask,
                                            fmt->Gmask,
                                            fmt->Bmask,
                                            fmt->Amask);
    }

    x_factor = (double)columns / src->w;
    y_factor = (double)rows / src->h;
    i = DefaultResizeFilter;
    if (filter != UndefinedFilter) {
        i = filter;
    } else if ((x_factor * y_factor) > 1.0 ||
               SDL_ISPIXELFORMAT_ALPHA(fmt->format)) {
        i = MitchellFilter;
    }
    x_support = blur * Max(1.0 / x_factor, 1.0) * filters[i].support;
    y_support = blur * Max(1.0 / y_factor, 1.0) * filters[i].support;
    support = Max(x_support, y_support);
    if (support < filters[i].support) support = filters[i].support;
    ContributionInfo *view_data_set = (ContributionInfo *)malloc(
        sizeof(ContributionInfo) * (size_t)(2.0 * Max(support, 0.5) + 3));
    if (view_data_set == NULL) {
        return 2;
    }
    status = MagickPass;
    quantum = 0;

    SDL_LockSurface(dst);
    SDL_LockSurface(src);
    SDL_LockSurface(source_image);

    if (order) {
        status = HorizontalFilter(
            src, source_image, x_factor, &filters[i], blur, view_data_set);
        if (status != MagickFail) {
            status = VerticalFilter(
                source_image, dst, y_factor, &filters[i], blur, view_data_set);
        }
    } else {
        status = VerticalFilter(
            src, source_image, y_factor, &filters[i], blur, view_data_set);
        if (status != MagickFail)
            status = HorizontalFilter(
                source_image, dst, x_factor, &filters[i], blur, view_data_set);
    }

    // free
    SDL_UnlockSurface(dst);
    SDL_UnlockSurface(src);
    SDL_UnlockSurface(source_image);
    free(view_data_set);
    SDL_FreeSurface(source_image);
    if (status == MagickFail) {
        return 3;
    }
    return 0;
}