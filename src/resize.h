#ifndef _MAGICK_RESIZE_H
#define _MAGICK_RESIZE_H

#include <SDL.h>

#define ARG_NOT_USED(arg) (void) arg
#define MaxRGB  255U
#define MagickEpsilon  1.0e-12
#define MagickPI  3.14159265358979323846264338327950288419716939937510
#define OpaqueOpacity  0UL
#define TransparentOpacity  MaxRGB
#define MaxRGBFloat 255.0f
#define MaxRGBDouble 255.0
#define RoundDoubleToQuantum(value) ((Quantum) (value < 0.0 ? 0U : \
  (value > MaxRGBDouble) ? MaxRGB : value + 0.5))

#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))

#define DefaultResizeFilter LanczosFilter
#define DefaultThumbnailFilter BoxFilter
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define MagickPassFail unsigned int
#define MagickPass     1
#define MagickFail     0

typedef enum
{
  UndefinedFilter,
  PointFilter,
  BoxFilter,
  TriangleFilter,
  HermiteFilter,
  HanningFilter,
  HammingFilter,
  BlackmanFilter,
  GaussianFilter,
  QuadraticFilter,
  CubicFilter,
  CatromFilter,
  MitchellFilter,
  LanczosFilter,
  BesselFilter,
  SincFilter
} FilterTypes;


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern int ResizeImage(const SDL_Surface *src, SDL_Surface *dst, const FilterTypes filter, const double blur);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif