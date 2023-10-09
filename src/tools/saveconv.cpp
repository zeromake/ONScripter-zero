#include <stdio.h>
#include <stdlib.h>

#include <infra/filesystem.hpp>
#include <string>

int main(int argc, char *argv[]) {
    char ss[] = "webpp";
    int ssc = strlen(ss);
    char ss2[] = "webp";
    int ssc2 = strlen(ss2);
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        auto in = std::fopen(arg.c_str(), "rb");
        std::string argout = arg + ".s";
        auto out = std::fopen(argout.c_str(), "wb");
        std::fseek(in, 0, SEEK_END);
        size_t count = std::ftell(in);
        std::fseek(in, 0, SEEK_SET);
        char *buff = new char[count]{0};
        std::fread(buff, 1, count, in);
        char *curr = buff;
        for (int i = 0; i < count; i++) {
            char ch = buff[i];
            if (ch == '.') {
                bool has = true;
                for (int offset = 0; offset < ssc; offset++) {
                    ch = buff[i + 1 + offset];
                    if (ch != ss[offset]) {
                        has = true;
                        break;
                    }
                }
                std::fputc('.', out);
                if (has) {
                    i += ssc;
                    std::fwrite(ss2, 1, ssc2, out);
                }
            } else {
                std::fputc(ch, out);
            }
        }
        delete[] buff;
        std::fclose(in);
        std::fclose(out);
    }
    return 0;
}
