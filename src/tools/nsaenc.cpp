#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <vector>
#include "NsaReader.h"
#include "gbk2utf16.h"

Coding2UTF16 *coding2utf16 = new GBK2UTF16();

void help()
{
    fprintf(stderr, "Usage: nsaenc [-ns version] src_dir dst_archive_file\n");
    fprintf(stderr, "           version   ... 1, 2(default is 1)\n");
    exit(-1);
}


#ifdef main
#undef main
#endif

int processFile(
    NsaReader::ArchiveInfo *ai,
    NsaReader::FileInfo *fi,
    const char *fullname,
    const char *name,
    unsigned long &offset,
    bool enhanced_flag,
    int base_offset)
{
    FILE *fp = NULL;
    char magic[5];
    char fullpath[512];
    if (*name == '/' || *name == '\\') {
        strcpy(fi->name, name+1);
    } else {
        strcpy(fi->name, name);
    }
    for (unsigned int j=0; j<strlen(fi->name); j++){
        if (fi->name[j] == '/')
            fi->name[j] = '\\';
    }
    strcpy(fullpath, fullname);
    for (unsigned int j=0; j<strlen(fullpath); j++){
        if ( (fullpath[j] == '/') || (fullpath[j] == '\\') )
            fullpath[j] = DELIMITER;
    }
    if ( (fp = fopen( fullpath, "rb" ) ) == NULL ){
        fprintf( stderr, "can't open file %s, skipping\n", fullpath );
        return -1;
    }
    fseek( fp, 0, SEEK_END );
    fi->length = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    magic[0] = 0;
    int len = fread(magic, 1, 4, fp);
    magic[len] = 0;
    fclose(fp);

    if ( (strstr( fi->name, ".nbz" ) != NULL) ||
         (strstr( fi->name, ".NBZ" ) != NULL) )
        fi->compression_type = BaseReader::NBZ_COMPRESSION;
    else if (enhanced_flag &&
             ( (( (strstr( fi->name, ".bmp" ) != NULL) ||
                  (strstr( fi->name, ".BMP" ) != NULL) ) &&
                (magic[0] == 'B') && (magic[1] == 'M')) ||
               (( (strstr( fi->name, ".wav" ) != NULL) ||
                  (strstr( fi->name, ".WAV" ) != NULL) ) &&
                (magic[0] == 'R') && (magic[1] == 'I') &&
                (magic[2] == 'F') && (magic[3] == 'F')) )){
        // If enhanced, use NBZ compression on (true) BMP & WAV files in NSA archive
        fi->compression_type = BaseReader::NBZ_COMPRESSION;
    }
    else
        fi->compression_type = BaseReader::NO_COMPRESSION;

    fi->original_length = fi->length;
    fi->offset = offset;
    offset += fi->length;
    ai->base_offset += strlen(fi->name) + base_offset;
    ai->num_of_files++;
    return 0;
}


int file_iterator(std::filesystem::path dirPath, std::vector<std::string> &files) {
    int code = 0;
    for (auto &itr : std::filesystem::directory_iterator(dirPath)) {
        if (std::filesystem::is_directory(itr.status())) {
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
    argc--; // skip command name
    argv++;
    unsigned int nsa_offset = 0;
    bool enhanced_flag = false;
    int archive_type = BaseReader::ARCHIVE_TYPE_NSA;
    int base_offset = 6;
    while (argc > 2){
        if (!strcmp(argv[0], "-ns")) {
            argc--;
            argv++;
            int version = atoi(argv[0]);
            if (version == 2) {
                archive_type = BaseReader::ARCHIVE_TYPE_NS2;
                base_offset = 14;
            }
            nsa_offset = version - 1;
        } else if (!strcmp(argv[0], "-e")){
            enhanced_flag = true;
        }
        argc--;
        argv++;
    }
    std::error_code ec;
    std::string basePath = argv[0];

    std::filesystem::path dirPath(basePath);

    if (!std::filesystem::exists(dirPath)) {
        fprintf(stderr, "can't find dir %s.\n", basePath.c_str());
        exit(-1);
    }
    NsaReader cSR;
    NsaReader::ArchiveInfo *sAI = cSR.openForCreate(argv[1], archive_type, nsa_offset);

    std::vector<std::string> files;
    int code = file_iterator(dirPath, files);
    if (code != 0) {
        fprintf(stderr, "file iter error.\n");
        exit(-1);
    }
    sAI->num_of_files = 0;
    sAI->base_offset = 5;
    unsigned long offset = 0;
    int count = files.size();
    sAI->fi_list = new NsaReader::FileInfo[count];
    NsaReader::FileInfo *sFI = sAI->fi_list;
    for (auto &itr : files) {
        processFile(
            sAI,
            sFI,
            itr.c_str(),
            itr.substr(dirPath.string().length()).c_str(),
            offset,
            enhanced_flag,
            base_offset
        );
        sFI++;
    }
    sFI = sAI->fi_list;
    for (int i = 0 ; i < sAI->num_of_files; i++, sFI++ )
        sFI->offset += sAI->base_offset;
    cSR.writeHeader(sAI->file_handle, archive_type);

    unsigned long offset_sub = 0;
    unsigned long length, buffer_length = 0;
    unsigned char *buffer = NULL;
    char *fnptr = new char[512];
    char *file_path = new char[512];
    for (int i=0 ; i<sAI->num_of_files; i++, sFI++ ){
        printf( "adding %d of %d (%s), length=%d\n", i+1, sAI->num_of_files, sFI->name, (int)sFI->original_length );
        length = sFI->original_length;
        if ( length > buffer_length ){
            if ( buffer ) delete[] buffer;
            buffer = new unsigned char[length];
            buffer_length = length;
        }
        sprintf(fnptr, "%s", sFI->name);
        sprintf(file_path, "%s", file_name);
        for (unsigned int j=0; j<strlen(file_path); j++) {
            if ( (file_path[j] == '\\') || (file_path[j] == '/') )
                file_path[j] = DELIMITER;
        }
        if ( (fp = fopen( file_path, "rb" ) ) == NULL ){
            fprintf( stderr, "can't open file %s, exiting\n", file_path );
            exit(-1);
        }

    }
    return 0;
}
