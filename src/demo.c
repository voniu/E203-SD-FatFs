
/* TODO Add any manufacture supplied header files can be included
here. */
#include <stdio.h>
#include <stdlib.h>
#include "hbird_sdk_hal.h"
#include <sdcard.h>
#include <ff.h>

int main(void)
{
    printf("sd card Test Start\r\n");
    FATFS fs;                     // FATFS文件系统结构体对象
    FIL fil;                      // FIL文件结构体对象
    FRESULT card;                 // 文件操作结果结构体对象
    int fnum;                     // Number of bytes read/write
    uint8_t ReadBuffer[1024] = {0}; // 读缓冲区

    f_mount(&fs, "", 0);

    card = f_open(&fil, "data.bin", FA_READ);
    if (card == FR_OK)
    {
        printf("open file success\r\n");
        card = f_read(&fil, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (card == FR_OK)
        {
            printf("data size: %dB\r\n", fnum);
            printf("data: ", ReadBuffer);
            for (long i = 0; i < sizeof(ReadBuffer); i++)
            {
                printf("%02X", ReadBuffer[i]);
            }
            printf("\r\n");
        }
        else
        {
            printf("read data failed: (%d)", card);
        }
    }
    else
    {
        printf("open file failed\r\n");
        return (int)card;
    }

    printf("sd card Test Start\r\n");

    return 0;
}
