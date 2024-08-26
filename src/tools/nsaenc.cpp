#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "NsaReader.h"
#include "charset/utf8.h"
#include "gbk2utf16.h"

Coding2UTF16 *coding2utf16 = new GBK2UTF16();

void help() {
    fprintf(stderr, "Usage: nsaenc [-ns version] src_dir dst_archive_file\n");
    fprintf(stderr, "           version   ... 1, 2(default is 1)\n");
    exit(-1);
}

#ifdef main
#undef main
#endif

bool startsWith(const std::string &str, const std::string prefix) {
    return (str.rfind(prefix, 0) == 0);
}

bool endsWith(const std::string &str, const std::string suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return (str.rfind(suffix) == (str.length() - suffix.length()));
}

std::string normalPath(const std::string p) {
    onscripter::fs::path path(p);
    return path.string();
}

int processFile(NsaReader::ArchiveInfo *ai,
                NsaReader::FileInfo *fi,
                const std::string &fullname,
                const std::string &name,
                unsigned long &offset,
                bool enhanced_flag,
                int base_offset) {
    std::FILE *fp = NULL;
    char magic[5];
    char filename[4096]{0};
    char unicode_name[4096]{0};
    strcpy(filename, name.c_str());
    int filenamelen = strlen(filename);
    for (int i = 0; i < filenamelen; i++) {
        if (REPLACE_DELIMITER == filename[i]) {
            filename[i] = DEFAULT_DELIMITER;
        }
    }
#ifdef UTF8_FILESYSTEM
    strcpy(unicode_name, filename);
    coding2utf16->convUTF8ToCoing(unicode_name, filename, 4096);
#else
    coding2utf16->convCoingToUTF8(filename, unicode_name, 4096);
#endif
    strcpy(fi->unicode_name, unicode_name);
    strcpy(fi->name, filename);
    std::string fullpath = normalPath(fullname);
    if ((fp = std::fopen(fullpath.c_str(), "rb")) == NULL) {
        fprintf(stderr, "can't open file %s, skipping\n", fullpath.c_str());
        return -1;
    }
    ons_fseek64(fp, 0, SEEK_END);
    fi->length = ons_ftell64(fp);
    ons_fseek64(fp, 0, SEEK_SET);
    magic[0] = 0;
    int len = std::fread(magic, 1, 4, fp);
    magic[len] = 0;
    std::fclose(fp);

    if (enhanced_flag) {
        fi->compression_type = BaseReader::NBZ_COMPRESSION;
    }

    if ((strstr(filename, ".nbz") != NULL) ||
        (strstr(filename, ".NBZ") != NULL))
        fi->compression_type = BaseReader::NBZ_COMPRESSION;
    else if (enhanced_flag && ((((strstr(filename, ".bmp") != NULL) ||
                                 (strstr(filename, ".BMP") != NULL)) &&
                                (magic[0] == 'B') && (magic[1] == 'M')) ||
                               (((strstr(filename, ".wav") != NULL) ||
                                 (strstr(filename, ".WAV") != NULL)) &&
                                (magic[0] == 'R') && (magic[1] == 'I') &&
                                (magic[2] == 'F') && (magic[3] == 'F')) ||
                               ((strstr(filename, ".ogg") != NULL) ||
                                (strstr(filename, ".ogg") != NULL)))) {
        // If enhanced, use NBZ compression on (true) BMP & WAV files in NSA
        // archive
        fi->compression_type = BaseReader::NBZ_COMPRESSION;
    } else {
        fi->compression_type = BaseReader::NO_COMPRESSION;
    }

    if (fi->compression_type > BaseReader::NO_COMPRESSION &&
        fi->length < 10 * 1024) {
        fi->compression_type = BaseReader::NO_COMPRESSION;
    }

    fi->original_length = fi->length;
    fi->offset = offset;
    offset += fi->length;
    ai->base_offset +=
        strlen((ai->flags & BaseReader::ArchiveFlag::ARCHIVE_FLAG_ENCODING_UTF8)
                   ? fi->unicode_name
                   : fi->name) +
        base_offset;
    ai->num_of_files++;
    return 0;
}

int file_iterator(onscripter::fs::path dirPath, onscripter::Vector<onscripter::String> &files) {
    int code = 0;
    for (auto &itr : onscripter::fs::directory_iterator(dirPath)) {
        if (!startsWith(itr.path().string(), "..") &&
            startsWith(itr.path().string(), ".")) {
            continue;
        }
        if (onscripter::fs::is_directory(itr.status())) {
            code = file_iterator(itr.path(), files);
            if (code != 0) {
                return code;
            }
        } else {
            files.push_back(itr.path().string());
        }
    }
    return code;
}

// https://github.com/playmer/onscripter-en/blob/22135bb2ac543cfad5b9e6b6b5820cb219a48ca3/tools/arcmake.cpp
int main(int argc, char *argv[]) {
    argc--;  // skip command name
    argv++;
    unsigned int nsa_offset = 0;
    bool enhanced_flag = false;
    char archive_flags = 0;
    int archive_type = 0;
    int base_offset = 14;
    int init_base_offset = 6;
    while (argc > 2) {
        if (!strcmp(argv[0], "-ns")) {
            argc--;
            argv++;
            int version = atoi(argv[0]);
            nsa_offset = version - 1;
            if (version == 2) {
                archive_type = BaseReader::ARCHIVE_TYPE_NS2;
                init_base_offset = nsa_offset + 4;  // nsa_offset + size(4)
                base_offset = 7;  // compression_type(1),double_quote(2),size(4)
            } else {
                archive_type = BaseReader::ARCHIVE_TYPE_NSA;
            }
        } else if (!strcmp(argv[0], "-e")) {
            enhanced_flag = true;
        } else if (!strcmp(argv[0], "-u")) {
            archive_flags |=
                BaseReader::ArchiveFlag::ARCHIVE_FLAG_ENCODING_UTF8;
        }
        argc--;
        argv++;
    }
    onscripter::String basePath = argv[0];

    onscripter::fs::path dirPath(basePath);

    if (!onscripter::fs::exists(dirPath)) {
        fprintf(stderr, "can't find dir %s.\n", basePath.c_str());
        exit(-1);
    }
    onscripter::String output = argv[1];
    if (archive_type == 0) {
        if (endsWith(output, ".ns2")) {
            nsa_offset = 1;
            archive_type = BaseReader::ARCHIVE_TYPE_NS2;
            init_base_offset = nsa_offset + 4;  // nsa_offset + size(4)
            base_offset = 7;  // compression_type(1),double_quote(2),size(4)
        } else if (endsWith(output, ".nsa")) {
            archive_type = BaseReader::ARCHIVE_TYPE_NSA;
            nsa_offset = 0;
        }
    }
    if (nsa_offset == 0) {
        archive_flags = 0;
    }

    NsaReader cSR;
    NsaReader::ArchiveInfo *sAI =
        cSR.openForCreate(output.c_str(), archive_type, nsa_offset);
    sAI->flags = archive_flags;

    onscripter::Vector<onscripter::String> files;
    int code = file_iterator(dirPath, files);
    if (code != 0) {
        fprintf(stderr, "file iter error.\n");
        exit(-1);
    }
    std::sort(files.begin(), files.end());
    sAI->num_of_files = 0;
    sAI->base_offset = init_base_offset;
    unsigned long offset = 0;
    int count = files.size();
    sAI->fi_list = new NsaReader::FileInfo[count];
    NsaReader::FileInfo *sFI = sAI->fi_list;
    size_t dirLength = dirPath.string().size();
    if (dirLength > 0) {
        dirLength++;
    }
    for (auto &itr : files) {
        processFile(sAI,
                    sFI,
                    itr,
                    itr.substr(dirLength),
                    offset,
                    enhanced_flag,
                    base_offset);
        sFI++;
    }
    sFI = sAI->fi_list;
    for (int i = 0; i < sAI->num_of_files; i++, sFI++) {
        sFI->offset += sAI->base_offset;
    }
    cSR.writeHeader(sAI->file_handle, archive_type, nsa_offset);

    unsigned long offset_sub = 0;
    unsigned long length, buffer_length = 0;
    unsigned char *buffer = NULL;
    std::FILE *fp = nullptr;
    sFI = sAI->fi_list;
    for (int i = 0; i < sAI->num_of_files; i++, sFI++) {
#ifdef UTF8_FILESYSTEM
        const char *name = sFI->unicode_name;
#else
        const char *name = sFI->name;
#endif
        printf("adding %d of %d (%s), length=%d\n",
               i + 1,
               sAI->num_of_files,
               name,
               (int)sFI->original_length);
        length = sFI->original_length;
        if (length > buffer_length) {
            if (buffer) delete[] buffer;
            buffer = new unsigned char[length];
            buffer_length = length;
        }
        std::string file_path = files.at(i);
        if ((fp = fopen(file_path.c_str(), "rb")) == NULL) {
            fprintf(stderr, "can't open file %s, exiting\n", file_path.c_str());
            exit(-1);
        }
        if (!enhanced_flag) {
            sFI->compression_type = BaseReader::NO_COMPRESSION;
        }
        sFI->offset -= offset_sub;
        cSR.addFile(sAI, fp, i, sFI->offset, buffer);
        if (sFI->original_length != sFI->length) {
            offset_sub += sFI->original_length - sFI->length;
            printf("    NBZ compressed: %d -> %d (%d%%)\n",
                   (int)sFI->original_length,
                   (int)sFI->length,
                   (int)(sFI->length * 100 / sFI->original_length));
        }
        fclose(fp);
    }
    cSR.writeHeader(sAI->file_handle, archive_type, nsa_offset);
    if (buffer) delete[] buffer;
    return 0;
}
