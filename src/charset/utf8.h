#ifndef __CHARSET_UTF8_H
#define __CHARSET_UTF8_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */
#include <stdint.h>

int32_t charset_ucs4_to_utf8(const uint32_t ch, uint8_t* out);
int32_t charset_utf8_to_ucs4(const uint8_t* input, uint32_t* ch);

typedef enum {
    CONVERSION_OK,    /* conversion successful */
    SOURCE_EXHAUSTED, /* partial character in source, but hit end */
    TARGET_EXHAUSTED, /* insufficient room in target for conversion */
    SOURCE_ILLEGAL    /* source sequence is illegal/malformed */
} conversion_result;

conversion_result convert_utf8_to_ucs4(const uint8_t* sourceStart,
                                       const uint8_t* sourceEnd,
                                       uint32_t* targetStart,
                                       const uint32_t* targetEnd,
                                       int32_t* count);

conversion_result convert_ucs4_to_utf8(const uint32_t* sourceStart,
                                       const uint32_t* sourceEnd,
                                       uint8_t* targetStart,
                                       const uint8_t* targetEnd,
                                       int32_t* count);

#define convert_utf8_to_ucs4_len(                              \
    sourceStart, sourceCount, targetStart, targetCount, count) \
    convert_utf8_to_ucs4(sourceStart,                          \
                         sourceStart + sourceCount,            \
                         targetStart,                          \
                         targetStart + targetCount,            \
                         count)

#define convert_ucs4_to_utf8_len(                              \
    sourceStart, sourceCount, targetStart, targetCount, count) \
    convert_ucs4_to_utf8(sourceStart,                          \
                         sourceStart + sourceCount,            \
                         targetStart,                          \
                         targetStart + targetCount,            \
                         count)

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif
