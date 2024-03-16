#include "utf8.h"

int32_t charset_ucs4_to_utf8(const uint32_t ch, uint8_t* out) {
    int32_t n = 0;
    if (ch <= 0x0000007f) {
        n = 1;
    } else if (ch <= 0x000007ff) {
        n = 2;
    } else if (ch <= 0x0000ffff) {
        n = 3;
    } else if (ch <= 0x001fffff) {
        n = 4;
    } else if (ch <= 0x03ffffff) {
        n = 5;
    } else if (ch <= 0x7fffffff) {
        n = 6;
    }
    if (!out || n == 0) {
        return n;
    }
    uint8_t* p = out;
    switch (n) {
        case 1:
            *p++ = ch;
            break;
        case 2:
            *p++ = ((ch >> 6) & 0x1f) | 0xc0;
            *p++ = (ch & 0x3f) | 0x80;
            break;
        case 3:
            *p++ = ((ch >> 12) & 0x0f) | 0xe0;
            *p++ = ((ch >> 6) & 0x3f) | 0x80;
            *p++ = (ch & 0x3f) | 0x80;
            break;
        case 4:
            *p++ = ((ch >> 18) & 0x07) | 0xf0;
            *p++ = ((ch >> 12) & 0x3f) | 0x80;
            *p++ = ((ch >> 6) & 0x3f) | 0x80;
            *p++ = (ch & 0x3f) | 0x80;
            break;
        case 5:
            *p++ = ((ch >> 24) & 0x03) | 0xf8;
            *p++ = ((ch >> 18) & 0x3f) | 0x80;
            *p++ = ((ch >> 12) & 0x3f) | 0x80;
            *p++ = ((ch >> 6) & 0x3f) | 0x80;
            *p++ = (ch & 0x3f) | 0x80;
            break;
        case 6:
            *p++ = ((ch >> 30) & 0x01) | 0xfc;
            *p++ = ((ch >> 24) & 0x3f) | 0x80;
            *p++ = ((ch >> 18) & 0x3f) | 0x80;
            *p++ = ((ch >> 12) & 0x3f) | 0x80;
            *p++ = ((ch >> 6) & 0x3f) | 0x80;
            *p++ = (ch & 0x3f) | 0x80;
            break;
    }
    return n;
}

int32_t charset_utf8_to_ucs4(const uint8_t* input, uint32_t* ch) {
    const uint8_t* p = input;
    int32_t n = 0;
    if (!(*p & 0x80)) {
        n = 1;
    } else if ((*p & 0xe0) == 0xc0) {
        n = 2;
    } else if ((*p & 0xf0) == 0xe0) {
        n = 3;
    } else if ((*p & 0xf8) == 0xf0) {
        n = 4;
    } else if ((*p & 0xfc) == 0xf8) {
        n = 5;
    } else if ((*p & 0xfe) == 0xfc) {
        n = 6;
    }
    if (!ch || n == 0) {
        return n;
    }
    switch (n) {
        case 1:
            *ch = p[0];
            break;
        case 2:
            *ch = ((((uint32_t)(p[0] & 0x1f)) << 6) | (p[1] & 0x3f));
            break;
        case 3:
            *ch = ((((uint32_t)(p[0] & 0x0f)) << 12) |
                   (((uint32_t)(p[1] & 0x3f)) << 6) | (p[2] & 0x3f));
            break;
        case 4:
            *ch = ((((uint32_t)(p[0] & 0x07)) << 18) |
                   (((uint32_t)(p[1] & 0x3f)) << 12) |
                   (((uint32_t)(p[2] & 0x3f)) << 6) | (p[3] & 0x3f));
            break;
        case 5:
            *ch = ((((uint32_t)(p[0] & 0x03)) << 24) |
                   (((uint32_t)(p[1] & 0x3f)) << 18) |
                   (((uint32_t)(p[2] & 0x3f)) << 12) |
                   (((uint32_t)(p[3] & 0x3f)) << 6) | (p[4] & 0x3f));
            break;
        case 6:
            *ch = ((((uint32_t)(p[0] & 0x01)) << 30) |
                   (((uint32_t)(p[1] & 0x3f)) << 24) |
                   (((uint32_t)(p[2] & 0x3f)) << 18) |
                   (((uint32_t)(p[3] & 0x3f)) << 12) |
                   (((uint32_t)(p[4] & 0x3f)) << 6) | (p[5] & 0x3f));
            break;
    }
    return n;
}

conversion_result convert_utf8_to_ucs4(const uint8_t* sourceStart,
                                       const uint8_t* sourceEnd,
                                       uint32_t* targetStart,
                                       const uint32_t* targetEnd,
                                       int32_t* count) {
    if (sourceEnd == NULL) {
        sourceEnd = sourceStart;
        while (sourceEnd != NULL) sourceEnd++;
    }
    if (targetEnd == NULL && targetStart != NULL) {
        targetEnd = targetStart;
        while (targetEnd != NULL) targetEnd++;
    }
    conversion_result result = CONVERSION_OK;
    const uint8_t* source = sourceStart;
    uint32_t* target = targetStart;
    int32_t resultCount = 0;
    while (source < sourceEnd) {
        int32_t n = charset_utf8_to_ucs4(source, target);
        if (n == 0) {
            result = SOURCE_ILLEGAL;
            break;
        }
        resultCount++;
        source += n;
        if (target != NULL) {
            target++;
            if (target > targetEnd) {
                result = TARGET_EXHAUSTED;
                break;
            }
        }
        if (source > sourceEnd) {
            result = SOURCE_EXHAUSTED;
            break;
        }
    }
    if (count) {
        *count = resultCount;
    }
    return result;
}

conversion_result convert_ucs4_to_utf8(const uint32_t* sourceStart,
                                       const uint32_t* sourceEnd,
                                       uint8_t* targetStart,
                                       const uint8_t* targetEnd,
                                       int32_t* count) {
    if (sourceEnd == NULL) {
        sourceEnd = sourceStart;
        while (sourceEnd != NULL) sourceEnd++;
    }
    if (targetEnd == NULL && targetStart != NULL) {
        targetEnd = targetStart;
        while (targetEnd != NULL) targetEnd++;
    }
    conversion_result result = CONVERSION_OK;
    const uint32_t* source = sourceStart;
    uint8_t* target = targetStart;
    int32_t resultCount = 0;
    while (source < sourceEnd) {
        int32_t n = charset_ucs4_to_utf8(*source, target);
        if (n <= 0) {
            result = SOURCE_ILLEGAL;
            break;
        }
        resultCount++;
        source++;
        if (target != NULL) {
            target += n;
            if (target > targetEnd) {
                result = TARGET_EXHAUSTED;
                break;
            }
        }
        if (source > sourceEnd) {
            result = SOURCE_EXHAUSTED;
            break;
        }
    }
    if (count) {
        *count = resultCount;
    }
    return result;
}
