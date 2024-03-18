#include "charset.h"
#include "gb18030_2022.g"

int32_t charset_ucs4_to_gb18030(const uint32_t ch, uint8_t* out) {
    int n = 0;
    if (ch <= 0x7f) {
        if (out) *out = (uint8_t)ch;
        return 1;
    }
    size_t search_size = sizeof(g_charset_ucs4_to_gb18030_2022_search_data) /
                         sizeof(g_charset_ucs4_to_gb18030_2022_search_data[0]);
    int64_t index = -1;
    for (size_t i = 0; i < search_size; i++) {
        uint32_t *search_item = g_charset_ucs4_to_gb18030_2022_search_data[i];
        if (ch < search_item[0]) break;
        if (ch >= search_item[0] && ch < search_item[1]) {
            index = ch - search_item[0] + search_item[2];
            break;
        }
    }
    if (index == -1) return 0;
    uint32_t charset = g_charset_ucs4_to_gb18030_2022_table_data[index];
    if (charset == 0) return 0;
    int32_t offset = 0;
    if (charset > 0xFFFF) {
        offset = 4;
        if (out) {
            out[0] = (charset >> 24) & 0xFF;
            out[1] = (charset >> 16) & 0xFF;
            out[2] = (charset >> 8) & 0xFF;
            out[3] = charset & 0xFF;
        }
    } else {
        offset = 2;
        if (out) {
            out[0] = (charset >> 8) & 0xFF;
            out[1] = charset & 0xFF;
        }
    }
    return offset;
}

int32_t charset_gb18030_to_ucs4(const uint8_t* input, uint32_t* ch) {
    uint8_t first_ch = input[0];
    uint8_t second_ch = input[1];
    if (first_ch <= 0x7f) {
        if (ch) *ch = (uint32_t)first_ch;
        return 1;
    }
    int32_t n = 0;
    if (first_ch >= 0x81 && first_ch <= 0xFE) {
        if ((second_ch >= 0x40 && second_ch <= 0x7e) ||
            (second_ch >= 0x80 && second_ch <= 0xfe)) {
            n = 2;
        } else if (second_ch >= 0x30 && second_ch <= 0x39) {
            n = 4;
        }
    }
    if (n == 0) return n;
    uint32_t gb18030_ch = first_ch << 8 | second_ch;
    if (n == 4) {
        gb18030_ch <<= 16;
        first_ch = input[2];
        second_ch = input[3];
        gb18030_ch |= first_ch << 8 | second_ch;
    }

    size_t search_size = sizeof(g_charset_gb18030_2022_to_ucs4_search_data) /
                         sizeof(g_charset_gb18030_2022_to_ucs4_search_data[0]);
    int64_t index = -1;
    for (size_t i = 0; i < search_size; i++) {
        uint32_t *search_item = g_charset_gb18030_2022_to_ucs4_search_data[i];
        if (gb18030_ch < search_item[0]) break;
        if (gb18030_ch >= search_item[0] && gb18030_ch < search_item[1]) {
            index = gb18030_ch - search_item[0] + search_item[2];
            break;
        }
    }
    if (index == -1) return 0;
    uint32_t ucs4_ch = g_charset_gb18030_2022_to_ucs4_table_data[index];
    if (ucs4_ch == 0) return 0;
    if (ch) *ch = ucs4_ch;
    return n;
}

int32_t convert_gb18030_to_utf8(const uint8_t* sourceStart,
                                const uint8_t* sourceEnd,
                                uint8_t* targetStart,
                                const uint8_t* targetEnd,
                                conversion_result* result) {
    int32_t count = 0;
    conversion_result _result = CONVERSION_OK;
    const uint8_t* source = sourceStart;
    uint8_t* target = targetStart;
    while (source < sourceEnd) {
        uint32_t ucs4_ch = 0;
        int32_t n = charset_gb18030_to_ucs4(source, &ucs4_ch);
        if (n <= 0) {
            _result = SOURCE_ILLEGAL;
            break;
        }
        int32_t m = charset_ucs4_to_utf8(ucs4_ch, target);
        count += m;
        source += n;
        if (target != NULL) {
            target += m;
            if (target > targetEnd) {
                _result = TARGET_EXHAUSTED;
                break;
            }
        }
        if (source > sourceEnd) {
            _result = SOURCE_EXHAUSTED;
            break;
        }
    }
    if (result) {
        *result = _result;
    }
    return count;
}

int32_t convert_utf8_to_gb18030(const uint8_t* sourceStart,
                                const uint8_t* sourceEnd,
                                uint8_t* targetStart,
                                const uint8_t* targetEnd,
                                conversion_result* result) {
    int32_t count = 0;
    conversion_result _result = CONVERSION_OK;
    const uint8_t* source = sourceStart;
    uint8_t* target = targetStart;
    while (source < sourceEnd) {
        uint32_t ch = 0;
        int32_t n = charset_utf8_to_ucs4(source, &ch);
        if (n <= 0) {
            _result = SOURCE_ILLEGAL;
            break;
        }
        int32_t m = charset_ucs4_to_gb18030(ch, target);
        count += m;
        source += n;
        if (target != NULL) {
            target += m;
            if (target > targetEnd) {
                _result = TARGET_EXHAUSTED;
                break;
            }
        }
        if (source > sourceEnd) {
            _result = SOURCE_EXHAUSTED;
            break;
        }
    }
    if (result) {
        *result = _result;
    }
    return count;
}



/*
int32_t charset_ucs4_to_gb18030(const uint32_t ch, uint8_t* out) {
    int32_t n = 0;
    if (ch <= 0x7f) {
        n = 1;
        if (out) *out = (uint8_t)ch;
    } else if (ch & 0x0000ff00) {
        n = 2;
        if (out) {
            out[0] = (ch >> 8) & 0xff;
            out[1] = ch & 0xff;
        }
    } else if (ch & 0x7fffffff) {
        n = 4;
        if (out) {
            out[0] = (ch >> 24) & 0xff;
            out[1] = (ch >> 16) & 0xff;
            out[2] = (ch >> 8) & 0xff;
            out[3] = ch & 0xff;
        }
    }
    return n;
}

int32_t charset_gb18030_to_ucs4(const uint8_t* input, uint32_t* ch) {
    uint8_t first_ch = input[0];
    if (first_ch <= 0x7f) {
        if (ch) *ch = (uint32_t)first_ch;
        return 1;
    }
    uint32_t result = 0;
    int32_t n = 0;
    if (first_ch >= 0x81 && first_ch <= 0xfe) {
        result = (uint32_t)first_ch;
        uint8_t second_ch = input[1];
        if ((second_ch >= 0x40 && second_ch <= 0x7e) || (second_ch >= 0x80 && second_ch <= 0xfe)) {
            result = (result << 8) | second_ch;
            n = 2;
        } else if (second_ch >= 0x30 && second_ch <= 0x39) {
            result = ((result & 0x7f) << 8) | second_ch;
            uint8_t c = input[2];
			result = (result << 8) | c;
            c = input[3];
			result = (result << 8) | c;
            n = 4;
        }
    }
    if (n && ch) {
        *ch = result;
    }
    return n;
}
*/
