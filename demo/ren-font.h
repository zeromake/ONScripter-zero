#pragma once

#ifndef __REN_FONT_H
#define __REN_FONT_H

#include <SDL.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftlcdfil.h>
#ifdef _WIN32
#include <windows.h>
#endif


#define FONT_FALLBACK_MAX 10
#define MAX_GLYPHSET 256
#define MAX_LOADABLE_GLYPHSETS 1024
#define SUBPIXEL_BITMAPS_CACHED 3

typedef enum { FONT_HINTING_NONE, FONT_HINTING_SLIGHT, FONT_HINTING_FULL } ERenFontHinting;
typedef enum { FONT_ANTIALIASING_NONE, FONT_ANTIALIASING_GRAYSCALE, FONT_ANTIALIASING_SUBPIXEL } ERenFontAntialiasing;
typedef enum { FONT_STYLE_BOLD = 1, FONT_STYLE_ITALIC = 2, FONT_STYLE_UNDERLINE = 4, FONT_STYLE_SMOOTH = 8, FONT_STYLE_STRIKETHROUGH = 16 } ERenFontStyle;
typedef struct { uint8_t b, g, r, a; } RenColor;

typedef struct {
  unsigned short x0, x1, y0, y1, loaded;
  short bitmap_left, bitmap_top;
  float xadvance;
} GlyphMetric;

typedef struct {
  SDL_Surface* surface;
  GlyphMetric metrics[MAX_GLYPHSET];
} GlyphSet;

typedef struct RenFont {
  FT_Face face;
  GlyphSet* sets[SUBPIXEL_BITMAPS_CACHED][MAX_LOADABLE_GLYPHSETS];
  float size, space_advance, tab_advance;
  unsigned short max_height, baseline, height;
  ERenFontAntialiasing antialiasing;
  ERenFontHinting hinting;
  unsigned char style;
  unsigned short underline_thickness;
#ifdef _WIN32
  unsigned char *file;
  HANDLE file_handle;
#endif
  char path[];
} RenFont;

typedef struct
{
   RenFont *fonts[FONT_FALLBACK_MAX];
} Command;



typedef struct { int x, y, width, height; } RenRect;

struct RenWindow {
  SDL_Window *window;
  RenRect clip;
  SDL_Surface* windowSurface;
};

static FT_Library library;
static RenWindow window_renderer;
static SDL_Surface *draw_rect_surface;

void ren_init(SDL_Window *win, SDL_Surface* windowSurface);
float ren_draw_text(RenFont **fonts, const char *text, size_t len, float x, int y, RenColor color);
RenFont* ren_font_load(const char* path, float size, ERenFontAntialiasing antialiasing, ERenFontHinting hinting, unsigned char style);
void ren_draw_rect(RenRect rect, RenColor color);

#endif