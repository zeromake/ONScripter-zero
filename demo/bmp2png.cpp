#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define panic(msg) printf(msg); return -1 

int main(int argc, char* argv[]) {
    std::string input  = argv[1];
    std::string output = argv[2];
    std::string filPath = input.substr(0, input.size()-4) + ".fil";
    std::vector<uint8_t> filBuf;
    if (std::filesystem::exists(filPath)) {
        auto filFile = std::fopen(filPath.c_str(), "rb");
        auto fileSize = std::filesystem::file_size(filPath);
        filBuf.resize(fileSize);
        std::fread(filBuf.data(), 1, fileSize, filFile);
        std::fclose(filFile);
    }

    stbi_set_unpremultiply_on_load(1);
	stbi_convert_iphone_png_to_rgb(1);
    int w, h, n;
    stbi_uc *pixels = stbi_load(input.c_str(), &w, &h, &n, 4);
    uint8_t *alphap = pixels+3;
    if (filBuf.empty()) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++,alphap += 4)
                *alphap = filBuf.at(y * w + x);
        }
    } else {
        uint32_t *p = (uint32_t *)pixels;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++,p++,alphap += 4)
                if (*p == 0 || *p == 0xff)
                    *alphap = 0;
        }
    }
    stbi_write_png(output.c_str(), w, h, 4, pixels, w * 4);
    stbi_image_free(pixels);
}


