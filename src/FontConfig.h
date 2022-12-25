
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
        int ratio1;
        int ratio2;
        uchar3 color;
        int outline_size;
        uchar3 outline_color;
        bool render_outline;

    };
    static ons_font::FontConfig* __DEFAULT_FONT_CONFIG = nullptr;
    static FontConfig* DEFAULT_FONT_CONFIG() {
        if (__DEFAULT_FONT_CONFIG == nullptr) {
            __DEFAULT_FONT_CONFIG = new FontConfig;
            __DEFAULT_FONT_CONFIG->ratio1 = 1;
            __DEFAULT_FONT_CONFIG->ratio2 = 1;
            __DEFAULT_FONT_CONFIG->color[0] =
            __DEFAULT_FONT_CONFIG->color[1] =
            __DEFAULT_FONT_CONFIG->color[2] = 0xff;
            __DEFAULT_FONT_CONFIG->outline_size = 1,
            __DEFAULT_FONT_CONFIG->outline_color[0] =
            __DEFAULT_FONT_CONFIG->outline_color[1] =
            __DEFAULT_FONT_CONFIG->outline_color[2] = 0;
            __DEFAULT_FONT_CONFIG->render_outline = false;
        }
        return __DEFAULT_FONT_CONFIG;
    }
    
     
}

#endif