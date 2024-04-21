#include "FontConfig.h"
#include <string.h>
#include <stdlib.h>

namespace ons_font {
const static FontConfig __DEFAULT_FONT_CONFIG =
    FontConfig{0, 0, {0xff, 0xff, 0xff}, false, 1, {0x0, 0x0, 0x0}, 0, 0};
const FontConfig* DEFAULT_FONT_CONFIG() { return &__DEFAULT_FONT_CONFIG; }

const int readFontConfig(const char *buf, ons_font::FontConfig *cfg) {
    ons_font::FONT_TYPE types = ons_font::GLOBAL_FONT;
    int offset = 0;
    if (buf[offset] >= '0' && buf[offset] <= '9') {
        types = ons_font::FONT_TYPE(buf[offset] - '0');
    }
    offset++;
    if (buf[offset] != ':') {
        return 0;
    }
    offset++;
    auto default_config = ons_font::DEFAULT_FONT_CONFIG();
    memcpy(cfg, default_config, sizeof(ons_font::FontConfig));
    int start = offset;
    int field = 0;
    char buff[256];
    #define IS_FONT_CONFIG_END(c) (c == '\0' || c == ';' || c == ' ' || c == '\n')
    while (1) {
        if (buf[offset] == ',' || IS_FONT_CONFIG_END(buf[offset])) {
            int size = offset - start;
            if (size > 0) {
                memcpy(buff, buf + start, size);
                buff[size] = '\0';
            } else {
                buff[0] = '\0';
            }
            start = offset + 1;
            if (buff[0] != '\0') {
                switch (field) {
                    case 0:
                        cfg->size = atoi(buff);
                        break;
                    case 1:
                        cfg->size_ratio = atof(buff);
                        break;
                    case 2:
                        utils::readColor(buff, &cfg->color);
                        break;
                    case 3:
                        cfg->render_outline = buff[0] != '0' && buff[0] != 'f';
                        break;
                    case 4:
                        cfg->outline_size = atoi(buff);
                        break;
                    case 5:
                        readColor(buff, &cfg->outline_color);
                        break;
                    case 6:
                        cfg->offset_x = atoi(buff);
                        break;
                    case 7:
                        cfg->offset_y = atoi(buff);
                        break;
                }
            }
            field++;
        }
        if (IS_FONT_CONFIG_END(buf[offset])) {
            break;
        }
        offset++;
    }
    return offset;
}
};  // namespace ons_font
