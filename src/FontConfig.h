
#ifndef __FONT_CONFIG_H__
#define __FONT_CONFIG_H__
#include "private/utils.h"

namespace ons_font {
enum FONT_TYPE {
    GLOBAL_FONT,
    SENTENCE_FONT,
    ANIM_FONT,
    MENU_FONT,
    DAILOG_FONT,
    RUBY_FONT,
};
struct FontConfig {
    int size;
    float size_ratio;
    utils::uchar4 color;
    bool render_outline;
    int outline_size;
    utils::uchar4 outline_color;
    int offset_x;
    int offset_y;
};
const FontConfig* DEFAULT_FONT_CONFIG();
const int readFontConfig(const char *buf, ons_font::FontConfig *cfg);
}  // namespace ons_font

#endif
