#ifndef __CHARSET_UTF8_H
#define __CHARSET_UTF8_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum {
    BOM_BE = 1,
} charset_flag;

/* 从 RET_SHIFT_ILSEQ RET_TOOFEW 解出 n 的值 */
#define DECODE_SHIFT_ILSEQ(r)  ((int32_t)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((int32_t)(RET_TOOFEW(0) - (r)) / 2)
/* 是否为 RET_SHIFT_ILSEQ code */
#define IS_SHIFT_ILSEQ(r) (int32_t)(-1-r) % 2 == (int32_t)(-1-RET_ILSEQ) % 2

/* mbtowc 读取了 n 个字符后，出现无法转换 */
#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
/* mbtowc 无法转换（未在该编码支持范围内，可以尝试其它编码）*/
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* mbtowc 输入 buffer 不够长，n 为已使用的数量 */
#define RET_TOOFEW(n)       (-2-2*(n))
/* 给 RET_SHIFT_ILSEQ,RET_TOOFEW 做最大值，或者作为编码转换的最大字符串长度 */
#define RET_COUNT_MAX       ((INT_MAX / 2) - 1)

/* wctomb 无法转换（未在该编码支持范围内，可以尝试其它编码）*/
#define RET_ILUNI           -1
/* wctomb 输出 buffer 不够长 */
#define RET_TOOSMALL        -2

typedef struct
{
    uint32_t istate; // 输入额外参数/结果
    uint32_t ostate; // 输出额外参数/结果
    uint32_t iflags; // 输入选项
    uint32_t oflags; // 输出选项
    int32_t transliterate; // 无法转换字符，替换为备用字符。
    int32_t discard_ilseq; // 丢弃无法转换的字符。
    int32_t (*mbtowc)(charset* conv, uint32_t* pwc, const uint8_t* cpmb, size_t mb_n); // 输入字符串转为 ucs4
    int32_t (*wctomb)(charset* conv, uint8_t* pmb, const uint32_t cwc, size_t mb_n); // ucs4 转为字符串输出
} charset;

int32_t charset_ucs4_to_utf32(const uint32_t ch, uint8_t* out, bool be);
int32_t charset_utf32_to_ucs4(const uint8_t* input, uint32_t* ch, bool be);
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
