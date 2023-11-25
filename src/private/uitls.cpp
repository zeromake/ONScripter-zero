#include <stdarg.h>
#include <stdio.h>

#include <chrono>

#include "utils.h"

#ifdef ANDROID
#include <android/log.h>
#elif defined(WINRT)
#include "debugapi.h"
#include "windows.h"

static BOOL MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize) {
    DWORD dwMinSize;
    dwMinSize = MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, NULL, 0);
    if (dwSize < dwMinSize) {
        return FALSE;
    }
    MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);
    return TRUE;
}
#endif

namespace utils {


std::chrono::steady_clock::time_point now() {
    return std::chrono::steady_clock::now();
}

float duration(std::chrono::steady_clock::time_point prev) {
    return std::chrono::duration_cast<std::chrono::microseconds>(now() - prev)
               .count() /
           1000.0f;
}

void printDebug(const char *format, ...) {
#ifndef NDEBUG
    va_list ap;
    va_start(ap, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_DEBUG, "DEBUG", format, ap);
#elif defined(WINRT)
    const int __size = 256;
    char *buf = new char[__size]{0};
    vsnprintf(buf, __size, format, ap);
    LPWSTR wstr = new WCHAR[128];
    MByteToWChar(buf, wstr, 256);
    OutputDebugStringW(wstr);
#else
    vprintf(format, ap);
#endif
    va_end(ap);
#endif
}

void printInfo(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_VERBOSE, "INFO", format, ap);
#elif defined(WINRT)
    const int __size = 256;
    char *buf = new char[__size]{0};
    vsnprintf(buf, __size, format, ap);
    LPWSTR wstr = new WCHAR[128];
    MByteToWChar(buf, wstr, 256);
    OutputDebugStringW(wstr);
#else
    vprintf(format, ap);
#endif
    va_end(ap);
}

void printError(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_ERROR, "ERR", format, ap);
#elif defined(WINRT)
    char *buf = new char[256]{0};
    vsnprintf(buf, 256, format, ap);
    LPWSTR wstr = new WCHAR[128];
    MByteToWChar(buf, wstr, 256);
    OutputDebugStringW(wstr);
#else
    vfprintf(stderr, format, ap);
#endif
    va_end(ap);
}

    static unsigned char hexToDec(char ch) {
        if ('0' <= ch && ch <= '9')
            return ch - '0';
        else if ('a' <= ch && ch <= 'f')
            return ch - 'a' + 10;
        else if ('A' <= ch && ch <= 'F')
            return ch - 'A' + 10;
        return 0;
    }
    int readColor(const char *buf, uchar4 *color) {
        int offset = 0;
        while (buf[offset] == ' ' || buf[offset] == '\t') offset++;
        if (buf[offset] != '#') {
            return 0;
        }
        offset++;
        (*color).rgba[3] = 255;
        int size = 0;
        char ch = buf[offset+size];
        while (
            ('0' <= ch && ch <= '9') ||
            ('a' <= ch && ch <= 'f') ||
            ('A' <= ch && ch <= 'F') ) {
            size++;
            ch = buf[offset+size];
        }
        bool is_repeat = size == 3 || size == 4;
        int sep = is_repeat ? 1 : 2;
        int start = offset;
        int end = offset+size;
        int i = start;
        int color_offset = 0;
        for (i = start; i < end; i += sep) {
            (*color).rgba[color_offset] = hexToDec(buf[i]) << 4 | hexToDec(is_repeat ? buf[i] : buf[i+1]);
            color_offset++;
        }
        offset += size;
        return offset;
    }
}  // namespace utils