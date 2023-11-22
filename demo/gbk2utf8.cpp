#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "coding2utf16.h"
#include "gbk2utf16.h"

#include <iconv.h>

#include "charset/gb2312.h"

// https://github.com/python/cpython/blob/main/Modules/cjkcodecs/_codecs_cn.c

static auto cd = iconv_open("UTF-8", "GBK");

void gb_to_utf8(char* src, char* dst, size_t len) {
    int ret = 0;
    size_t inlen = strlen(src) + 1;
    ret = iconv(cd, &src, &inlen, &dst, &len);
    if (ret != 0)
        printf("iconv failed err: %s\n", strerror(errno));
}

int gbk2utf8(const char *in, char *out, int size) {
    int i = 0;
    int j = 0;
    int32_t n = 0;
    while (in[i] != '\0')
    {
        uint8_t c = in[i];
        if (c <= 0x7f) {
            out[j] = in[i];
        } else {
            uint8_t next_c = in[i+1];
            uint32_t code = (uint32_t)c;
            code = code << 8 | (uint32_t)next_c;
            code = charset_gb2312_to_ucs4(code);
            i++;
            n = charset_ucs4_to_utf8(code, (uint8_t*)out+j);
            printf("[%d, %d] => [", c, next_c);
            for (int ii = 0; ii < n; ii++) {
                printf("%d, ", out[j+ii]);
            }
            j += n - 1;
            printf("]\n");
        }
        i++;
        j++;
    }
    out[j] = '\0';
}

int main(int argc, char **argv) {
    FILE* fp = fopen(argv[1], "rb");
    char buf[2048]{0};
    size_t n = fread(buf, 1, 256, fp);
    buf[n] = '\0';
    char out[2048]{0};
    gb_to_utf8(buf, out, 2048);
    printf("out: %s\n", out);
    n = strlen(out);
    for (int ii = 0; ii < n; ii++) {
        char c = out[ii];
        printf("%d:%d, ", (uint8_t)c, (c & 0xc0) != 0x80);
    }
    printf("\n");
}
