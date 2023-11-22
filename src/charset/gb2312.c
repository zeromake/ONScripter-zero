#include "gb2312.h"
#include "gb2312.g"

uint32_t tb_charset_gb2312_from_ucs4(const uint32_t ch)
{
    // is ascii?
    if (ch <= 0x7f) return ch;

    // find the gb2312 character
    int32_t left = 0;
    int32_t right = (g_charset_ucs4_to_gb2312_table_size / sizeof(g_charset_ucs4_to_gb2312_table_data[0])) - 1;
    while (left <= right)
    {
        // the middle character
        int32_t       mid = (left + right) >> 1;
        uint16_t     mid_ucs4 = g_charset_ucs4_to_gb2312_table_data[mid][0];
        // find it?
        if (mid_ucs4 == ch)
            return g_charset_ucs4_to_gb2312_table_data[mid][1];
        if (ch > mid_ucs4) left = mid + 1;
        else right = mid - 1;
    }
    return 0;
}

uint32_t charset_gb2312_to_ucs4(const uint32_t ch) {
    if (ch <= 0x7f) return ch;
    if (ch >= 0xa1a1 && ch <= 0xf7fe)
        return g_charset_gb2312_to_ucs4_table_data[ch - 0xa1a1];
    return 0;
}

int32_t charset_ucs4_to_utf8(const uint32_t ch, uint8_t *out) {
    uint8_t* p = out;
    int32_t n = 0;
    if (ch <= 0x0000007f) {
        *p++ = ch;
        n = 1;
    } else if (ch <= 0x000007ff) {
        *p++ = ((ch >> 6) & 0x1f) | 0xc0;
        *p++ = (ch & 0x3f) | 0x80;
        n = 2;
    } else if (ch <= 0x0000ffff) {
        *p++ = ((ch >> 12) & 0x0f) | 0xe0;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
        n = 3;
    } else if (ch <= 0x001fffff) {
        *p++ = ((ch >> 18) & 0x07) | 0xf0;
        *p++ = ((ch >> 12) & 0x3f) | 0x80;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
        n = 4;
    } else if (ch <= 0x03ffffff) {
        *p++ = ((ch >> 24) & 0x03) | 0xf8;
        *p++ = ((ch >> 18) & 0x3f) | 0x80;
        *p++ = ((ch >> 12) & 0x3f) | 0x80;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
        n = 5;
    } else if (ch <= 0x7fffffff) {
        *p++ = ((ch >> 30) & 0x01) | 0xfc;
        *p++ = ((ch >> 24) & 0x3f) | 0x80;
        *p++ = ((ch >> 18) & 0x3f) | 0x80;
        *p++ = ((ch >> 12) & 0x3f) | 0x80;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
        n = 6;
    }
    return n;
}