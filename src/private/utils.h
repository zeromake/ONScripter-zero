/* -*- C++ -*-
 *
 *  Utils.h
 *
 *  Copyright (C) 2014 jh10001 <jh10001@live.cn>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __UTILS_H__
#define __UTILS_H__
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

namespace utils {

typedef struct _uchar4 {
    unsigned char rgba[4];
} uchar4;

std::chrono::steady_clock::time_point now();

float duration(std::chrono::steady_clock::time_point prev);

void printDebug(const char *format, ...);

void printInfo(const char *format, ...);

void printError(const char *format, ...);

int readColor(const char *buf, uchar4 *color);
void split(std::vector<std::string> &output,
           std::string &input,
           const char sep);
void setStr(char **dst, const char *src, int num);
bool streamIsDigits(std::stringstream &ss);

template <typename T>
T(min)
(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
T(clamp)
(T x, T min, T max) {
    return x < min ? min : (x > max ? max : x);
}
}  // namespace utils

#endif
