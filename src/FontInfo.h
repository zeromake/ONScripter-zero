/* -*- C++ -*-
 *
 *  FontInfo.h - Font information storage class of ONScripter
 *
 *  Copyright (c) 2001-2016 Ogapee. All rights reserved.
 *            (C) 2016 jh10001 <jh10001@live.cn>
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __FONT_INFO_H__
#define __FONT_INFO_H__

#include <SDL.h>

#include "BaseReader.h"
#include "FontConfig.h"
#include "resize/scale_manager.hpp"

typedef onscripter::Function<int(const char *, char *, bool)>
    generate_path_function;

typedef unsigned char uchar3[3];

struct Position {
    int xy[2];
    struct Position *next;
};

typedef struct {
    int width;
    int all_width;
    int height;
    int all_height;
} PositionOffset;

class _FontInfo {
   private:
    int xy[2];      // Current position
    int old_xy[2];  // Prev position
    struct Position *stash;
    PositionOffset positionOffset;  // maxWidth, allWidth,maxHeight,allHeight
    void resetPosition();

   public:
    enum Direction { YOKO_MODE = 0, TATE_MODE = 1 };
    static char *cache_font_file;
    static void *font_cache;
    static int font_cache_size;
    void *ttf_font[2];  // 0...normal rendering, 1...outline rendering
    uchar3 color;
    uchar3 on_color, off_color, nofile_color;
    int font_size_xy[2];
    int top_xy[2];    // Top left origin
    int num_xy[2];    // Row and column of the text windows
    int pitch_xy[2];  // Width and height of a character
    int wait_time;
    bool is_bold;
    bool is_shadow;
    bool is_transparent;
    bool is_newline_accepted;
    uchar3 window_color;

    int line_offset_xy[2];  // ruby offset for each line
    bool rubyon_flag;
    int tateyoko_mode;
    ons_font::FONT_TYPE types;

    _FontInfo();
    void reset();
    int getToPrev(int index = 0);
    int getSavePoint(int index);
    void saveToPrev(bool use_ruby_offset = true);
    void savePoint();
    void rollback(int mode = 3);
    void copyPosition(_FontInfo *font);
    void *openFont(
        char *font_file,
        const onscripter::SharedPtr<onscripter::ScaleManager> &screen_scale,
        generate_path_function f = nullptr,
        const ons_font::FontConfig *fontConfig = nullptr);
    void setTateyokoMode(int tateyoko_mode);
    int getTateyokoMode();
    int getRemainingLine();

    int x(bool use_ruby_offset = true);
    int y(bool use_ruby_offset = true);
    void setXY(int x = -1, int y = -1);
    void clear();
    void newLine();
    void setLineArea(int num);

    bool isEndOfLine(int margin = 0);
    int endStatus(int x, int y, bool useAutoOffset = true);
    bool isLineEmpty();
    void advanceCharInHankaku(int offest, int width = 0, int heigth = 0);
    void addLineOffset(int margin);
    void setRubyOnFlag(bool flag);

    SDL_Rect calcUpdatedArea(
        int start_xy[2],
        const onscripter::SharedPtr<onscripter::ScaleManager> &screen_scale);
    void addShadeArea(SDL_Rect &rect, int dx, int dy, int dw, int dh);
    int initRuby(_FontInfo &body_info, int body_count, int ruby_count);
};

#endif  // __FONT_INFO_H__
