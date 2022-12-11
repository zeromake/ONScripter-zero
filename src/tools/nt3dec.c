#include <stdio.h>
#include <stdlib.h>

int decodeNT3(void *sh, char **buf, unsigned int size, FILE *fp)
{
    int key;
    int tmp;
    int flag;
    char ch;
    char *cur;
    int data_size;

    if(size < 0x920) return -1;
    fseek(fp, 0x91C, 0);
    fread(&key, 4, 1, fp); // 1346323918
    printf("key: %d\n", key);
    data_size = fread(*buf, 1, size-0x920, fp);
    cur = *buf;
    if(data_size)
    {
        int i=1;
        do{
            key ^=  *cur;
            tmp = key + (*cur)*(data_size+1-i) + 0x5D588B65;
            key = tmp;
            *cur^=tmp;
            i++;
            cur++;
        }
        while (data_size>=i);
    }
    *cur = 10;
    cur++;
    return 0;
}

int extractNT3(char *inpath, char *outpath)
{
    FILE *fp = fopen(inpath, "rb");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int buff_size = size - 0x920;
    char *buf = malloc(buff_size);
    memset(buf, 0, buff_size);
    int ret = decodeNT3(NULL, &buf, size, fp);
    FILE *fp2 = fopen(outpath, "wb");
    fwrite(buf, sizeof(char), buff_size, fp2);
    free(buf);
    fclose(fp);
    fclose(fp2);
    return ret;
}

int main(int argc, char **argv)
{
    printf("extract onscript.nt3 (by devseed), extract_nt3 input [output]\n");
    char *outpath;
    if(argc<=2)
        outpath = "result.txt";
    else outpath = argv[2];
    int ret = extractNT3(argv[1], outpath);
    if(ret==0) printf("%s extracted successfully!\n", outpath);
    else printf("%s extract faiiled!\n", outpath);
    return 0;
}