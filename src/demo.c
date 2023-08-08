#include <stdio.h>
#include <stdlib.h>
#include "hbird_sdk_hal.h"
#include <sdcard.h>
#include <ff.h>
int main(void)
{
    printf("> sd card test start\r\n");

    FATFS fs;                       // FATFS文件系统结构体对象
    FIL fil;                        // FIL文件结构体对象
    FRESULT card;                   // 文件操作结果结构体对象
    int fnum;                       // Number of bytes read/write
    uint8_t ReadBuffer[1024] = {0}; // 读缓冲区

    f_mount(&fs, "", 0);

    // 数据文件
    // card = f_open(&fil, "data.bin", FA_READ);

    // 文本文件
    card = f_open(&fil, "text.txt", FA_READ);

    if (card == FR_OK)
    {
        printf("> open file success\r\n");
        card = f_read(&fil, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (card == FR_OK)
        {
            printf("> data size: %dB\r\n", fnum);

            // 数据输出
            // printf("> data(hex): ");
            // for (long i = 0; i < fnum; i++)
            // {
            //     printf("%02X", ReadBuffer[i]);
            // }
            // printf("\r\n");

            // 文本输出
            printf("> data(string): %s\r\n", ReadBuffer);
        }
        else
        {
            printf("> read data failed: (%d)", card);
        }
    }
    else
    {
        printf("> open file failed\r\n");
        return (int)card;
    }

    f_close(&fil);
    f_unmount("");

    printf("> sd card test end\r\n");
    return 0;
}