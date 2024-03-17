#include <gtest/gtest.h>
#include "charset/charset.h"


TEST(GBKTest, Byte2) {
    const uint8_t* input = (uint8_t*)u8"\u4E40";
    const uint8_t* target = (uint8_t*)"\x81\x54";
    uint8_t result[8] = {0};
    int32_t n = convert_utf8_to_gb18030(input, input+4, result, result+8, NULL);
    EXPECT_STREQ((char*)result, (char*)target);
}

TEST(GBKTest, Byte4) {
    const uint8_t* input = (uint8_t*)u8"\u322A";
    const uint8_t* target = (uint8_t*)"\x81\x39\xC1\x32";
    uint8_t result[128] = {0};
    memset(result, 0, 128);
    int32_t n = convert_utf8_to_gb18030(input, input+4, result, result+128, NULL);
    EXPECT_STREQ((char*)result, (char*)target);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
