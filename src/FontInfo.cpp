/* -*- C++ -*-
 *
 *  _FontInfo.cpp - Font information storage class of ONScripter
 *
 *  Copyright (c) 2001-2013 Ogapee. All rights reserved.
 *            (C) 2014 jh10001 <jh10001@live.cn>
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

#include "FontInfo.h"

#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#include "private/utils.h"

#if defined(PSP)
#include <stdlib.h>
#include <string.h>
extern int psp_power_resume_number;
#endif

static struct FontContainer {
    FontContainer *next;
    int size;
    TTF_Font *font[2];
#if defined(PSP)
    SDL_RWops *rw_ops;
    int power_resume_number;
    char name[256];
#endif

    FontContainer() {
        size = 0;
        next = NULL;
        font[0] = font[1] = NULL;
#if defined(PSP)
        rw_ops = NULL;
        power_resume_number = 0;
#endif
    };
} root_font_container;

char *_FontInfo::cache_font_file = NULL;
void *_FontInfo::font_cache = NULL;
int _FontInfo::font_cache_size = 0;

_FontInfo::_FontInfo() {
    ttf_font[0] = ttf_font[1] = NULL;

    color[0] = color[1] = color[2] = 0xff;
    on_color[0] = on_color[1] = on_color[2] = 0xff;
    off_color[0] = off_color[1] = off_color[2] = 0xaa;
    nofile_color[0] = 0x55;
    nofile_color[1] = 0x55;
    nofile_color[2] = 0x99;
    rubyon_flag = false;
    memset(&positionOffset, 0, sizeof(PositionOffset));
    stash = NULL;
    reset();
}

void _FontInfo::reset() {
    tateyoko_mode = YOKO_MODE;
    clear();

    is_bold = true;
    is_shadow = true;
    is_transparent = true;
    is_newline_accepted = false;
    while (stash) {
        auto cur = stash;
        stash = cur->next;
        cur->next = NULL;
        delete cur;
    }
}

void _FontInfo::copyPosition(_FontInfo *font) {
    xy[0] = font->xy[0];
    xy[1] = font->xy[1];
}

void _FontInfo::saveToPrev(bool use_ruby_offset) {
    old_xy[0] = x(use_ruby_offset);
    old_xy[1] = y(use_ruby_offset);
}

int _FontInfo::getToPrev(int index) { return old_xy[index]; }

void *_FontInfo::openFont(
    char *_font_file,
    const std::shared_ptr<onscripter::ScaleManager> &screen_scale,
    generate_path_function f,
    const ons_font::FontConfig *fontConfig) {
    char font_file[4096] = {0};
    if (f != nullptr) {
        f(_font_file, font_file, false);
    } else {
        strcpy(font_file, _font_file);
    }
    int font_size;
    if (font_size_xy[0] < font_size_xy[1])
        font_size = font_size_xy[0];
    else
        font_size = font_size_xy[1];

    FontContainer *fc = &root_font_container;
    while (fc->next) {
        if (fc->next->size == font_size) break;
        fc = fc->next;
    }
    if (!fc->next) {
        fc->next = new FontContainer();
        fc->next->size = font_size;
        SDL_RWops *fp;
        bool useFile = (cache_font_file == NULL ||
                        strcmp(cache_font_file, font_file) != 0);
        if (useFile) {
            fp = SDL_RWFromFile(font_file, "rb");
        } else {
            fp = SDL_RWFromConstMem(font_cache, font_cache_size);
        }
        if (fp == NULL) return NULL;
        fp->close(fp);
        int scale_font_size = screen_scale->Scale(font_size);
#if defined(PSP)
        fc->next->rw_ops = SDL_RWFromFile(font_file, "r");
        fc->next->font[0] =
            TTF_OpenFontRW(fc->next->rw_ops, SDL_TRUE, scale_font_size);
#if (SDL_TTF_MAJOR_VERSION >= 2) && (SDL_TTF_MINOR_VERSION >= 0) && \
    (SDL_TTF_PATCHLEVEL >= 10)
        fc->next->font[1] =
            TTF_OpenFontRW(fc->next->rw_ops, SDL_TRUE, scale_font_size);
        TTF_SetFontOutline(fc->next->font[1], fontConfig->outline_size);
#endif
        fc->next->power_resume_number = psp_power_resume_number;
        strcpy(fc->next->name, font_file);
#else
        if (useFile)
            fc->next->font[0] = TTF_OpenFont(font_file, scale_font_size);
        else
            fc->next->font[0] =
                TTF_OpenFontRW(SDL_RWFromConstMem(font_cache, font_cache_size),
                               1,
                               scale_font_size);
        if (useFile)
            fc->next->font[1] = TTF_OpenFont(font_file, scale_font_size);
        else
            fc->next->font[1] =
                TTF_OpenFontRW(SDL_RWFromConstMem(font_cache, font_cache_size),
                               1,
                               scale_font_size);
        if (fc->next->font[1] == nullptr) {
            utils::printError("Open font failed: %s\n", TTF_GetError());
        }
        TTF_SetFontHinting(fc->next->font[0], TTF_HINTING_NONE);
        TTF_SetFontHinting(fc->next->font[1], TTF_HINTING_NONE);
        TTF_SetFontOutline(fc->next->font[1], fontConfig->outline_size);

#endif
    }
#if defined(PSP)
    else if (fc->next->power_resume_number != psp_power_resume_number) {
        FILE *fp = fopen(fc->next->name, "r");
        fc->next->rw_ops->hidden.stdio.fp = fp;
        fc->next->power_resume_number = psp_power_resume_number;
    }
#endif

    ttf_font[0] = (void *)fc->next->font[0];
    ttf_font[1] = (void *)fc->next->font[1];
    return fc->next->font;
}

void _FontInfo::setTateyokoMode(int tateyoko_mode) {
    this->tateyoko_mode = tateyoko_mode;
    clear();
}

int _FontInfo::getTateyokoMode() { return tateyoko_mode; }

int _FontInfo::getRemainingLine() {
    if (tateyoko_mode == YOKO_MODE)
        return num_xy[1] - xy[1] / 2;
    else
        return num_xy[1] - num_xy[0] + xy[0] / 2 + 1;
}

int _FontInfo::x(bool use_ruby_offset) {
    int pitch_count = xy[0];
    int x = pitch_count * pitch_xy[0] / 2 + top_xy[0] + line_offset_xy[0];
    if (use_ruby_offset && rubyon_flag && tateyoko_mode == TATE_MODE)
        x += font_size_xy[0] - pitch_xy[0];
    if (tateyoko_mode == YOKO_MODE) {
        x += positionOffset.all_width;
    } else if (tateyoko_mode == TATE_MODE) {
        x += positionOffset.width * pitch_count;
    }
    return x;
}

void _FontInfo::resetPosition() {
    positionOffset.all_width = 0;
    positionOffset.all_height = 0;
}

int _FontInfo::y(bool use_ruby_offset) {
    int pitch_count = xy[1];
    int y = pitch_count * pitch_xy[1] / 2 + top_xy[1] + line_offset_xy[1];
    if (use_ruby_offset && rubyon_flag && tateyoko_mode == YOKO_MODE)
        y += pitch_xy[1] - font_size_xy[1];
    if (tateyoko_mode == YOKO_MODE) {
        y += positionOffset.height * pitch_count;
    } else if (tateyoko_mode == TATE_MODE) {
        y += positionOffset.all_width;
    }
    return y;
}

void _FontInfo::setXY(int x, int y) {
    if (x != -1) xy[0] = x * 2;
    if (y != -1) xy[1] = y * 2;
    resetPosition();
}

void _FontInfo::clear() {
    if (tateyoko_mode == YOKO_MODE)
        setXY(0, 0);
    else
        setXY(num_xy[0] - 1, 0);
    line_offset_xy[0] = line_offset_xy[1] = 0;
}

void _FontInfo::newLine() {
    if (tateyoko_mode == YOKO_MODE) {
        xy[0] = 0;
        xy[1] += 2;
    } else {
        xy[0] -= 2;
        xy[1] = 0;
    }
    line_offset_xy[0] = line_offset_xy[1] = 0;
    resetPosition();
}

void _FontInfo::setLineArea(int num) {
    num_xy[tateyoko_mode] = num;
    num_xy[1 - tateyoko_mode] = 1;
}

int _FontInfo::endStatus(int x, int y, bool useAutoOffset) {
    int result = 0;
    int pitch_width = pitch_xy[0] + positionOffset.width * 2;
    int pitch_height = pitch_xy[1] + positionOffset.height * 2;
    if (useAutoOffset) {
        x -= pitch_width;
        y -= pitch_height;
    }
    bool horizontal_over = this->x() + pitch_width >= x;
    bool vertical_over = this->y() + pitch_height >= y;
    bool endOfLine = isEndOfLine();
    if (tateyoko_mode == YOKO_MODE) {
        // 横向无法放下一个字符，标记为需要换行
        if (horizontal_over) {
            result |= 1;
        }
        if (vertical_over) {
            result |= 4;
        }
        // 横向无法放下一个字符，标记为需要点击切换
        if (vertical_over && (endOfLine || horizontal_over)) {
            result |= 2;
        }
    } else if (tateyoko_mode == TATE_MODE) {
        if (vertical_over) {
            result |= 1;
        }
        if (horizontal_over) {
            result |= 4;
        }
        if (horizontal_over && (endOfLine || vertical_over)) {
            result |= 2;
        }
    }
    return result;
}

bool _FontInfo::isEndOfLine(int margin) {
    if (xy[tateyoko_mode] + margin >= num_xy[tateyoko_mode] * 2) {
        return true;
    }
    return false;
}

bool _FontInfo::isLineEmpty() {
    if (xy[tateyoko_mode] == 0) return true;

    return false;
}

void _FontInfo::advanceCharInHankaku(int offset, int width, int height) {
    if (width > 0) {
        int offsetWidth = width - (offset * pitch_xy[0] / 2);
        positionOffset.width = std::max(offsetWidth, positionOffset.width);
        if (tateyoko_mode == YOKO_MODE) {
            positionOffset.all_width += offsetWidth;
        }
    }
    if (height > 0) {
        int offsetHeight = height - pitch_xy[1];
        positionOffset.height = std::max(offsetHeight, positionOffset.height);
        if (tateyoko_mode == TATE_MODE) {
            positionOffset.all_height += offsetHeight;
        }
    }
    // utils::printInfo("%d %d:%d %d:%d\n", offset, width, positionOffset.width,
    // height, positionOffset.height);
    xy[tateyoko_mode] += offset;
}

void _FontInfo::addLineOffset(int offset) {
    line_offset_xy[tateyoko_mode] += offset;
}

int _FontInfo::getSavePoint(int index) {
    if (stash) {
        return stash->xy[index];
    }
    return xy[index];
}

void _FontInfo::savePoint() {
    auto cur = new struct Position;
    cur->next = NULL;
    cur->xy[0] = xy[0];
    cur->xy[1] = xy[1];
    cur->next = stash;
    stash = cur;
}

void _FontInfo::rollback(int mode) {
    auto cur = stash;
    if (cur) {
        stash = cur->next;
        cur->next = NULL;
        if (mode & 1) {
            xy[0] = cur->xy[0];
        }
        if (mode & 2) {
            xy[1] = cur->xy[1];
        }
        delete cur;
    }
}

SDL_Rect _FontInfo::calcUpdatedArea(
    int start_xy[2],
    const std::shared_ptr<onscripter::ScaleManager> &screen_scale) {
    SDL_Rect rect;
    if (tateyoko_mode == YOKO_MODE) {
        if (start_xy[1] == xy[1]) {
            rect.x = top_xy[0] + pitch_xy[0] * start_xy[0] / 2;
            rect.w = pitch_xy[0] * (xy[0] - start_xy[0]) / 2 + 1;
        } else {
            rect.x = top_xy[0];
            rect.w = pitch_xy[0] * num_xy[0];
        }
        rect.y = top_xy[1] + start_xy[1] * pitch_xy[1] / 2;
        rect.h = font_size_xy[1] + pitch_xy[1] * (xy[1] - start_xy[1]) / 2;
        if (rubyon_flag) rect.h += pitch_xy[1] - font_size_xy[1];
    } else {
        rect.x = top_xy[0] + pitch_xy[0] * xy[0] / 2;
        rect.w = font_size_xy[0] + pitch_xy[0] * (start_xy[0] - xy[0]) / 2;
        if (rubyon_flag) rect.w += font_size_xy[0] - pitch_xy[0];
        if (start_xy[0] == xy[0]) {
            rect.y = top_xy[1] + pitch_xy[1] * start_xy[1] / 2;
            rect.h = pitch_xy[1] * (xy[1] - start_xy[1]) / 2 + 1;
        } else {
            rect.y = top_xy[1];
            rect.h = pitch_xy[1] * num_xy[1];
        }
        num_xy[0] = (xy[0] - start_xy[0]) / 2 + 1;
    }
    if (tateyoko_mode == YOKO_MODE) {
        rect.w += positionOffset.all_width;
        rect.h += positionOffset.height * num_xy[1];
    } else if (tateyoko_mode == TATE_MODE) {
        rect.w += positionOffset.width * num_xy[1];
        rect.h += positionOffset.all_height;
    }
    return rect;
}

void _FontInfo::addShadeArea(SDL_Rect &rect, int dx, int dy, int dw, int dh) {
    rect.x += dx;
    rect.y += dy;
    rect.w += dw;
    rect.h += dh;
}

int _FontInfo::initRuby(_FontInfo &body_info, int body_count, int ruby_count) {
    if ((tateyoko_mode == YOKO_MODE &&
         body_count + body_info.xy[0] / 2 >= body_info.num_xy[0] - 1) ||
        (tateyoko_mode == TATE_MODE &&
         body_count + body_info.xy[1] / 2 > body_info.num_xy[1]))
        body_info.newLine();

    top_xy[0] = body_info.x();
    top_xy[1] = body_info.y();
    pitch_xy[0] = font_size_xy[0];
    pitch_xy[1] = font_size_xy[1];

    int margin = 0;

    if (tateyoko_mode == YOKO_MODE) {
        top_xy[1] -= font_size_xy[1];
        num_xy[0] = ruby_count;
        num_xy[1] = 1;
    } else {
        top_xy[0] += body_info.font_size_xy[0];
        num_xy[0] = 1;
        num_xy[1] = ruby_count;
    }

    if (ruby_count * font_size_xy[tateyoko_mode] >=
        body_count * body_info.pitch_xy[tateyoko_mode]) {
        margin = (ruby_count * font_size_xy[tateyoko_mode] -
                  body_count * body_info.pitch_xy[tateyoko_mode] + 1) /
                 2;
    } else {
        int offset = 0;
        if (ruby_count > 0)
            offset = (body_count * body_info.pitch_xy[tateyoko_mode] -
                      ruby_count * font_size_xy[tateyoko_mode] + ruby_count) /
                     (ruby_count * 2);
        top_xy[tateyoko_mode] += offset;
        pitch_xy[tateyoko_mode] += offset * 2;
    }
    body_info.line_offset_xy[tateyoko_mode] += margin;

    clear();

    return margin;
}
