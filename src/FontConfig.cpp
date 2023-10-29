#include "FontConfig.h"

namespace ons_font {
const static FontConfig __DEFAULT_FONT_CONFIG =
    FontConfig{0, 0, {0xff, 0xff, 0xff}, false, 1, {0x0, 0x0, 0x0}, 0, 0};
const FontConfig* DEFAULT_FONT_CONFIG() { return &__DEFAULT_FONT_CONFIG; }
};  // namespace ons_font
