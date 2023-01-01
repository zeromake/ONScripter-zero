
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
    struct FontConfig
    {
        int size;
        float size_ratio;
        uchar3 color;
        bool render_outline;
        int outline_size;
        uchar3 outline_color;
        int offset_x;
        int offset_y;
    };
    static ons_font::FontConfig* __DEFAULT_FONT_CONFIG = nullptr;
    static FontConfig* DEFAULT_FONT_CONFIG() {
        if (__DEFAULT_FONT_CONFIG == nullptr) {
            __DEFAULT_FONT_CONFIG = new FontConfig;
            __DEFAULT_FONT_CONFIG->size = 0;
            __DEFAULT_FONT_CONFIG->size_ratio = 0.0f;
            __DEFAULT_FONT_CONFIG->color[0] =
            __DEFAULT_FONT_CONFIG->color[1] =
            __DEFAULT_FONT_CONFIG->color[2] = 0xff;
            __DEFAULT_FONT_CONFIG->render_outline = false;
            __DEFAULT_FONT_CONFIG->outline_size = 1,
            __DEFAULT_FONT_CONFIG->outline_color[0] =
            __DEFAULT_FONT_CONFIG->outline_color[1] =
            __DEFAULT_FONT_CONFIG->outline_color[2] = 0;
            __DEFAULT_FONT_CONFIG->offset_x = 0;
            __DEFAULT_FONT_CONFIG->offset_y = 0;
        }
        return __DEFAULT_FONT_CONFIG;
    }
    
     
}

#endif