#include <stdbool.h>
#include <stdint.h>

#define static_write_u16_be(out, ch) *out++ = (ch >> 8 & 0xffff);*out++ = (ch & 0xffff)
#define static_write_u16_le(out, ch) *out++ = (ch & 0xffff);*out++ = (ch >> 8 & 0xffff)
#define static_read_u16_be(in) in[0] | in[1] << 8
#define static_read_u16_le(in) in[0] << 8 | in[1]

int32_t charset_ucs4_to_utf16(const uint32_t ch, uint8_t *out, bool be) {
    uint8_t *p = out;
    if (ch <= 0x0000ffff) {
        if (be) {static_write_u16_be(p, ch);}
        else {static_write_u16_le(p, ch);}
    } else if (ch > 0x0010ffff) {
        if (be) {static_write_u16_be(p, 0x0000fffd);}
        else {static_write_u16_le(p, 0x0000fffd);}
    } else {
        uint32_t swap_ch = ch - 0x0010000;
        if (be) {
            static_write_u16_be(p, ((swap_ch >> 10) + 0xd800));
            static_write_u16_be(p, ((swap_ch & 0x3ff) + 0xdc00));
        } else {
            static_write_u16_le(p, ((swap_ch >> 10) + 0xd800));
            static_write_u16_le(p, ((swap_ch & 0x3ff) + 0xdc00));
        }
    }
    return p - out;
}

int32_t charset_utf16_to_ucs4(const uint8_t *input, uint32_t *ch, bool be) {
    const uint8_t *p = input;
    uint32_t c = be ? static_read_u16_be(input) : static_read_u16_le(input);
    p += 2;
    if (c >= 0xd800 && c <= 0xdbff) {
        uint32_t c2 = be ? static_read_u16_be(p) : static_read_u16_le(p);
        if (c2 >= 0xdc00 && c2 <= 0xdfff) {
            c = ((c - 0xd800) << 10) + (c2 - 0xdc00) + 0x0010000;
            p += 2;
        }
    }
    *ch = c;
    return p - input;
}
