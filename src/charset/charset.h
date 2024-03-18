#ifndef __CHARSET_UTF8_H
#define __CHARSET_UTF8_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

int32_t charset_ucs4_to_utf8(const uint32_t ch, uint8_t* out);
int32_t charset_utf8_to_ucs4(const uint8_t* input, uint32_t* ch);
int32_t charset_utf16_to_ucs4(const uint8_t* input, uint32_t* ch, bool be);
int32_t charset_ucs4_to_utf16(const uint32_t ch, uint8_t* out, bool be);
int32_t charset_ucs4_to_gb18030(const uint32_t ch, uint8_t* out);
int32_t charset_gb18030_to_ucs4(const uint8_t* input, uint32_t* ch);
int32_t charset_ucs4_to_sjis(const uint32_t ch, uint8_t* out);
int32_t charset_sjis_to_ucs4(const uint8_t* input, uint32_t* ch);

typedef enum {
    CONVERSION_OK,    /* conversion successful */
    SOURCE_EXHAUSTED, /* partial character in source, but hit end */
    TARGET_EXHAUSTED, /* insufficient room in target for conversion */
    SOURCE_ILLEGAL    /* source sequence is illegal/malformed */
} conversion_result;

int32_t convert_utf8_to_ucs4(const uint8_t* sourceStart,
                             const uint8_t* sourceEnd,
                             uint32_t* targetStart,
                             const uint32_t* targetEnd,
                             conversion_result* result);

int32_t convert_ucs4_to_utf8(const uint32_t* sourceStart,
                             const uint32_t* sourceEnd,
                             uint8_t* targetStart,
                             const uint8_t* targetEnd,
                             conversion_result* result);

int32_t convert_utf8_to_utf16(const uint8_t* sourceStart,
                              const uint8_t* sourceEnd,
                              uint8_t* targetStart,
                              const uint8_t* targetEnd,
                              const bool be,
                              conversion_result* result);

int32_t convert_utf16_to_utf8(const uint8_t* sourceStart,
                              const uint8_t* sourceEnd,
                              uint8_t* targetStart,
                              const uint8_t* targetEnd,
                              const bool be,
                              conversion_result* result);

int32_t convert_utf8_to_utf16(const uint8_t* sourceStart,
                              const uint8_t* sourceEnd,
                              uint8_t* targetStart,
                              const uint8_t* targetEnd,
                              const bool be,
                              conversion_result* result);

int32_t convert_gb18030_to_utf8(const uint8_t* sourceStart,
                                const uint8_t* sourceEnd,
                                uint8_t* targetStart,
                                const uint8_t* targetEnd,
                                conversion_result* result);

int32_t convert_utf8_to_gb18030(const uint8_t* sourceStart,
                                const uint8_t* sourceEnd,
                                uint8_t* targetStart,
                                const uint8_t* targetEnd,
                                conversion_result* result);

int32_t convert_sjis_to_utf8(const uint8_t* sourceStart,
                             const uint8_t* sourceEnd,
                             uint8_t* targetStart,
                             const uint8_t* targetEnd,
                             conversion_result* result);

int32_t convert_utf8_to_sjis(const uint8_t* sourceStart,
                             const uint8_t* sourceEnd,
                             uint8_t* targetStart,
                             const uint8_t* targetEnd,
                             conversion_result* result);

#define convert_utf8_to_ucs4_len(                               \
    sourceStart, sourceCount, targetStart, targetCount, result) \
    convert_utf8_to_ucs4(sourceStart,                           \
                         sourceStart + sourceCount,             \
                         targetStart,                           \
                         targetStart + targetCount,             \
                         result)

#define convert_ucs4_to_utf8_len(                               \
    sourceStart, sourceCount, targetStart, targetCount, result) \
    convert_ucs4_to_utf8(sourceStart,                           \
                         sourceStart + sourceCount,             \
                         targetStart,                           \
                         targetStart + targetCount,             \
                         result)

#define convert_utf8_to_utf16_len(                                  \
    sourceStart, sourceCount, targetStart, targetCount, be, result) \
    convert_utf8_to_utf16(sourceStart,                              \
                          sourceStart + sourceCount,                \
                          targetStart,                              \
                          targetStart + targetCount,                \
                          be,                                       \
                          result)

#define convert_utf16_to_utf8_len(                                  \
    sourceStart, sourceCount, targetStart, targetCount, be, result) \
    convert_utf16_to_utf8(sourceStart,                              \
                          sourceStart + sourceCount,                \
                          targetStart,                              \
                          targetStart + targetCount,                \
                          be,                                       \
                          result)

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif
