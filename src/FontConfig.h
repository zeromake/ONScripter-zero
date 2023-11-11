
#ifndef __FONT_CONFIG_H__
#define __FONT_CONFIG_H__

namespace ons_font {
typedef unsigned char uchar3[3];
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
    uchar3 color;
    bool render_outline;
    int outline_size;
    uchar3 outline_color;
    int offset_x;
    int offset_y;
};
const FontConfig* DEFAULT_FONT_CONFIG();
}  // namespace ons_font

#endif