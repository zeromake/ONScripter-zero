#include <fstream>
#include <sstream>
#include <string>

extern "C" {
    #include <iconv/iconv.h>
}


int main(int argc, char* argv[]) {
    printf("input: %s\n", argv[1]);
    std::ifstream file(argv[1]);
    std::string line;
    iconv_t in = iconv_open("GB18030", "UTF-8");
    iconv_t out = iconv_open("UTF-8", "GB18030");
    char output[256];
    char input[256];
    size_t output_count = 256;
    iconv_t use = in;
    int is_in = 2;
    while (std::getline(file, line)) {
        std::stringstream stream;
        if (!strncmp("# charset", line.c_str(), 9)) {
            is_in--;
            use = is_in > 0 ? in : out;
            printf("----------------------------------------------\n");
            continue;
        }
        printf("line: %s\n", line.c_str());
        stream.str(std::move(line));
        memset(output, 0, 256);
        memset(input, 0, 256);
        uint32_t code;
        stream >> code;
        size_t input_count = 4;
        if (code > 0xFFFF) {
            input[0] = code >> 24 & 0x00FF;
            input[1] = code >> 16 & 0x00FF;
            input[2] = code >> 8 & 0x00FF;
            input[3] = code & 0x00FF;
        } else {
            input[0] = code >> 8 & 0x00FF;
            input[1] = code & 0x00FF;
            input_count = 2;
        }
        input[input_count] = 0;
        printf("input char: %d %d %d %d\n", input[0], input[1], input[2], input[3]);
        char* temp_output = output;
        char* temp_input = input;
        int n = iconv(use, (const char**)&temp_input, &input_count, &temp_output, &output_count);
        if (n == -1 || n == 0) continue;
        uint32_t out_code = 0;
        memcpy(&out_code, temp_input, 4);
        printf("code(%d): 0x%X(%d) -> 0x%X(%d)\n", n, code, code, out_code, out_code);
        break;
    }
    iconv_close(in);
    iconv_close(out);
}
