#ifndef _MAGICK_RESIZE_H
#define _MAGICK_RESIZE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#include <stdint.h>
#include <stdbool.h>

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

typedef uint8_t MagickQuantum;
typedef MagickQuantum MagickPixelPacket4[4];
typedef struct _MagickPixelOrder {
    int red, green, blue, opacity;
} MagickPixelOrder;

typedef struct _MagickImage {
  MagickPixelPacket4* pixels; // image pixel ptr
  MagickPixelOrder order; // 4byte is rgba or brga
  uint64_t columns; // image pixel width
  uint64_t rows; // image pixel heigth
} MagickImage;

int ResizeImage(
  const MagickImage *src,
  const MagickImage *dst,
  const FilterTypes filter,
  const double blur
);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif