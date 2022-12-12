#include <stdarg.h>
#include <stdio.h>
#include <memory.h>
#include <vector>

void ons_sprintf(char* buff, const char* format, int argc, void* args[]) {
    switch (argc)
    {
        case 0:
            sprintf(buff, format);
            break;
        case 1:
            sprintf(buff, format, args[0]);
            break;
        case 2:
            sprintf(buff, format, args[0], args[1]);
            break;
        case 3:
            sprintf(buff, format, args[0], args[1], args[2]);
            break;
        case 4:
            sprintf(buff, format, args[0], args[1], args[2], args[3]);
            break;
        case 5:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 6:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 7:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
            break;
        case 8:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
            break;
        case 9:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
            break;
        case 10:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
            break;
        case 11:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10]);
            break;
        case 12:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11]);
            break;
        case 13:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12]);
            break;
        case 14:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13]);
            break;
        case 15:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
            break;
        case 16:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15]);
            break;
        case 17:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16]);
            break;
        case 18:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17]);
            break;
        case 19:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18]);
            break;
        case 20:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19]);
            break;
        case 21:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20]);
            break;
        case 22:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21]);
            break;
        case 23:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22]);
            break;
        case 24:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23]);
            break;
        case 25:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24]);
            break;
        case 26:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25]);
            break;
        case 27:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26]);
            break;
        case 28:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27]);
            break;
        case 29:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28]);
            break;
        case 30:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29]);
            break;
        case 31:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29], args[30]);
            break;
        case 32:
            sprintf(buff, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29], args[30], args[31]);
            break;
    }
}

int main() {
    char* format = "%d - %d - %s\n";

    std::vector<void*> arr;
    int a = 1, b = 2;
    arr.push_back((char*)a);
    arr.push_back((char*)b);
    char* s = "测试";
    arr.push_back(s);


    // al.pa[0] = 'a';
    // al.pa[1] = '2';
    // al.pa[2] = '3';
    // al.pa[7] = '4';

    // size_t offset = 0;
    // uintptr_t value = (uintptr_t)11;
    // memcpy(al.pa+offset, &value, ptr_size);
    // offset += ptr_size;
    // const char* s = "111";
    // uintptr_t value2 = (uintptr_t)s;
    // memcpy(al.pa+offset, &value2, ptr_size);

    char* buff = new char[1024];
    ons_sprintf(buff, format, arr.size(), arr.data());
    printf(buff);
}
