#include <stdio.h>
#include <stdlib.h>
#include "hbird_sdk_hal.h"
#include <sdcard.h>
#include <ff.h>

int main(void)
{
    printf("> sd card test start\r\n");

    FATFS fs;          /* Work area (filesystem object) for logical drives */
    FIL fsrc, fdst;    /* File objects */
    BYTE buffer[4096]; /* File copy buffer */
    FRESULT fr;        /* FatFs function common result code */
    UINT br, bw;       /* File read/write count */

    /* Give work areas to each logical drive */
    f_mount(&fs, "", 0);

    /* Open source file on the drive 1 */
    fr = f_open(&fsrc, "text.txt", FA_READ);
    if (fr)
        return (int)fr;

    /* Create destination file on the drive 0 */
    fr = f_open(&fdst, "copy.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fr)
        return (int)fr;

    /* Copy source to destination */
    do
    {
        fr = f_read(&fsrc, buffer, sizeof buffer, &br); /* Read a chunk of data from the source file */
        printf("read data length: %d\r\n", br);
        if (br == 0)
            break;                            /* error or eof */
        fr = f_write(&fdst, buffer, br, &bw); /* Write it to the destination file */
        printf("fr: %d,write data length: %d\r\n", fr, bw);
        if (bw < br)
            break; /* error or disk full */
    } while (0);

    /* Close open files */
    f_close(&fsrc);
    f_close(&fdst);

    /* Unregister work area prior to discard it */
    f_unmount("");

    printf("> sd card test end\r\n");
    return 0;
}