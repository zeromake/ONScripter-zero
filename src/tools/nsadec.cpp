/* -*- C++ -*-
 *
 *  nsadec.cpp - NSA archive decoder
 *
 *  Copyright (c) 2001-2015 Ogapee. All rights reserved.
 *            (C) 2014-2015 jh10001 <jh10001@live.cn>
 *
 *  ogapee@aqua.dti2.ne.jp
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>

#include "NsaReader.h"
#include "coding2utf16.h"
#include "gbk2utf16.h"
#ifdef _WIN32
#include <direct.h>
#include <io.h>

#include <algorithm>
#include <string>
inline int mkdir(const char *pathname, int unused) { return _mkdir(pathname); }
#else
#include <unistd.h>
#endif

extern int errno;
Coding2UTF16 *coding2utf16 = new GBK2UTF16();

int main(int argc, char **argv) {
    NsaReader cNR;
    unsigned int nsa_offset = 0;
    unsigned long length;
    unsigned char *buffer;
    char file_name[256], dir_name[256];
    unsigned int i, j, count;
    int archive_type = BaseReader::ARCHIVE_TYPE_NSA;
    FILE *fp;
    struct stat file_stat;
    char *out;
    bool useLower = false;
    bool vLog = false;
    if (argc >= 2) {
        while (argc > 2) {
            if (!strcmp(argv[1], "-ns2")) {
                archive_type = BaseReader::ARCHIVE_TYPE_NS2;
            } else if (!strcmp(argv[1], "-offset")) {
                nsa_offset = atoi(argv[2]);
                argc--;
                argv++;
            } else if (!strcmp(argv[1], "-out")) {
                out = argv[2];
                argc--;
                argv++;
            } else if (!strcmp(argv[1], "-lower")) {
                useLower = true;
            } else if (!strcmp(argv[1], "-v")) {
                vLog = true;
            }
            argc--;
            argv++;
        }
    }
    if (archive_type == BaseReader::ARCHIVE_TYPE_NS2 && nsa_offset == 0) {
        nsa_offset = 1;
    }
    if (argc != 2) {
        fprintf(
            stderr,
            "Usage: nsadec [-offset ##] [-ns2] [-out dir] [-lower] arc_file\n");
        exit(-1);
    }
    if (out) {
        if (access(out, 0) != 0) {
            mkdir(out, 0755);
        }
    }
    cNR.openForConvert(argv[1], archive_type, nsa_offset);
    count = cNR.getNumFiles();

    BaseReader::ArchiveInfo *sAI;
    BaseReader::FileInfo sFI;

    for (i = 0; i < count; i++) {
        sAI = cNR.getArchiveInfoByIndex(i);
        sFI = sAI->fi_list[i];
        length = cNR.getFileLengthSubByIndex(sAI, i);
        buffer = new unsigned char[length];
        unsigned int len;
        if ((len = cNR.getFileSubByIndex(sAI, i, buffer)) != length) {
            fprintf(stderr, "file %s is not fully retrieved %d %lu\n",
                    sFI.original_name, len, length);
            length = sFI.length;
            continue;
        }
        if (out[0] != '\0') {
            snprintf(file_name, 256, "%s/%s", out, sFI.original_name);
        } else {
            strcpy(file_name, sFI.original_name);
        }
        if (useLower) {
            std::string s = file_name;
            std::transform(s.begin(), s.end(), s.begin(), std::tolower);
            strcpy(file_name, s.c_str());
        }
        for (j = 0; j < strlen(file_name); j++) {
            if (file_name[j] == '\\' || file_name[j] == '/') {
                file_name[j] = DELIMITER;
                strncpy(dir_name, file_name, j);
                dir_name[j] = '\0';
                /* If the directory does'nt exist, create it */
                if (stat(dir_name, &file_stat) == -1 && errno == ENOENT)
                    mkdir(dir_name, 00755);
            }
        }

        if (vLog) {
            printf("\033[Kouting %s\t\t%.2fkb\t\t%d/%d\r", file_name,
                   float(length) / 1024.0f, i, count);
        } else {
            printf("\033[Kouting %s\t\t%d/%d\r", out, i, count);
        }
        if ((fp = fopen(file_name, "wb"))) {
            fwrite(buffer, 1, length, fp);
            fclose(fp);
        } else {
            printf("opening %s ... falied\n", file_name);
        }

        delete[] buffer;
    }

    exit(0);
}
