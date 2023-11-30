#include <stdbool.h>
#include <stdint.h>

int32_t charset_utf16_to_ucs4(const uint32_t ch, uint8_t *out, bool be);
int32_t charset_ucs4_to_utf16(const uint8_t *input, uint32_t *ch, bool be);
