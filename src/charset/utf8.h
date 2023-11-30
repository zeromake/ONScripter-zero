#ifndef __CHARSET_UTF8_H
#define __CHARSET_UTF8_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */
#include <stdint.h>

int32_t charset_ucs4_to_utf8(const uint32_t ch, uint8_t *out);
int32_t charset_utf8_to_ucs4(const uint8_t *input, uint32_t *ch);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif
