#ifndef TB_CHARSET_H
#define TB_CHARSET_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#include <stdint.h>
#include <stddef.h>

uint32_t tb_charset_gb2312_from_ucs4(const uint32_t ch);
uint32_t charset_gb2312_to_ucs4(const uint32_t ch);
int32_t charset_ucs4_to_utf8(const uint32_t ch, uint8_t *out);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif
