#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

static const size_t BUFF_LENGHT = 4096;
static const uint8_t DEFAULT_MARK = 0x84;
static const uint8_t magic[5] = {0x79, 0x57, 0x0d, 0x80, 0x04};

uint8_t *createKeyTable(const char *key_exe) {
    if (!key_exe) return NULL;
    auto *fp = std::fopen(key_exe, "rb");
    if (fp == NULL) {
        fprintf(stderr, "createKeyTable: can't open EXE file %s\n", key_exe);
        return NULL;
    }
    auto key_table = new uint8_t[256];
    int i;
    for (i = 0; i < 256; i++) key_table[i] = i;

    unsigned char ring_buffer[256];
    int ring_start = 0, ring_last = 0;

    int ch, count;
    while ((ch = std::fgetc(fp)) != EOF) {
        i = ring_start;
        count = 0;
        while (i != ring_last && ring_buffer[i] != ch) {
            count++;
            i = (i + 1) % 256;
        }
        if (i == ring_last && count == 255) break;
        if (i != ring_last) ring_start = (i + 1) % 256;
        ring_buffer[ring_last] = ch;
        ring_last = (ring_last + 1) % 256;
    }
    std::fclose(fp);
    if (ch == EOF) {
        fprintf(stderr, "createKeyTable: can't find a key table.\n");
        return NULL;
    }
    // Key table creation
    ring_buffer[ring_last] = ch;
    for (i = 0; i < 256; i++)
        key_table[ring_buffer[(ring_start + i) % 256]] = i;
    return key_table;
}

bool endswith(const std::string &str, const std::string &end) {
    int srclen = str.size();
    int endlen = end.size();
    if (srclen >= endlen) {
        std::string temp = str.substr(srclen - endlen, endlen);
        if (temp == end) return true;
    }
    return false;
}

unsigned long getfilesize(std::FILE *file_ptr) {
    unsigned long posCur = 0;
    unsigned long posEnd = 0;
    posCur = std::ftell(file_ptr);
    std::fseek(file_ptr, 0L, SEEK_END);
    posEnd = std::ftell(file_ptr);
    std::fseek(file_ptr, posCur, SEEK_SET);
    return posEnd;
}

bool startsWith(const std::string &str, const std::string prefix) {
    return (str.rfind(prefix, 0) == 0);
}

bool endsWith(const std::string &str, const std::string suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return (str.rfind(suffix) == (str.length() - suffix.length()));
}

int main(int argc, char *argv[]) {
    int mode = -1;
    const char *keyExe = NULL;
    if (argc >= 3) {
        while (argc > 3) {
            if (!strcmp(argv[1], "-mode")) {
                mode = atoi(argv[2]);
                argc--;
                argv++;
            } else if (!strcmp(argv[1], "-key-exe")) {
                keyExe = argv[2];
                argc--;
                argv++;
            }
            argc--;
            argv++;
        }
    }
    auto key_table = createKeyTable(keyExe);
    std::string input = *(argv + 1);
    if (mode == -1) {
        mode = 0;
        if (endsWith(input, "nscr_sec.dat")) {
            mode = 2;
        } else if (endsWith(input, ".___")) {
            mode = 3;
        } else if (endsWith(input, ".dat")) {
            mode = 1;
        } else if (endsWith(input, ".nt")) {
            mode = 15;
        } else if (endsWith(input, ".nt2")) {
            mode = 16;
        } else if (endsWith(input, ".nt3")) {
            mode = 17;
        }
    }
    if (mode == 0) {
        mode = 1;
    }
    if (mode == 3 && key_table == NULL) {
        return 1;
    }
    auto *pFile = std::fopen(input.data(), "rb");
    if (pFile == nullptr) {
        fprintf(stderr,
                "file input %s is open err: %s\n",
                *(argv + 1),
                strerror(errno));
        return 1;
    }
    std::string outFile = "0.txt";
    if (argc >= 3) {
        std::string outFile = *(argv + 2);
    }
    auto *pOutFile = std::fopen(outFile.data(), "wb");
    if (pFile == nullptr) {
        fprintf(stderr,
                "file output %s is open err: %s\n",
                *(argv + 2),
                strerror(errno));
        return 1;
    }
    printf("nsdecode\n  extract: %s\n  output: %s\n  mode: %d\n",
           input.data(),
           outFile.data(),
           mode);
    size_t data_size = getfilesize(pFile) - 0x920;
    unsigned char key_buf[4];
    int32_t key = 0;
    int32_t tmp = 0;
    if (mode == 17) {
        std::fseek(pFile, 0x91C, 0);
        std::fread(&key, 4, 1, pFile);
    }
    char *buffer = new char[BUFF_LENGHT]{0};
    int result = std::fread(buffer, 1, BUFF_LENGHT, pFile);
    int magic_counter = 0;
    int offset = 0;
    while (result > 0) {
        for (int i = 0; i < result; ++i) {
            uint8_t ch = buffer[i];
            offset++;
            if (mode == 1) {
                ch ^= DEFAULT_MARK;
            } else if (mode == 2) {
                ch = (ch ^ magic[magic_counter++]) & 0xff;
                if (magic_counter == 5) magic_counter = 0;
            } else if (mode == 3) {
                ch = key_table[(unsigned char)ch] ^ DEFAULT_MARK;
            } else if (mode == 15 || mode == 16) {
                ch ^= 0x85 & 0x97;
                ch -= 1;
            } else if (mode == 17) {
                key ^= ch;
                tmp = key + (ch) * (data_size + 1 - offset) + 0x5D588B65;
                key = tmp;
                char src = ch;
                ch ^= tmp;
            }
            buffer[i] = ch;
        }
        std::fwrite(buffer, 1, result, pOutFile);
        result = std::fread(buffer, 1, BUFF_LENGHT, pFile);
    }
    delete[] buffer;
    std::fclose(pFile);
    std::fclose(pOutFile);
    printf("extracted successfully!\n");
    return 0;
}
