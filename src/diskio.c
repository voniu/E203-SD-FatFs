/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "sdcard.h"

/* Definitions of physical drive number for each drive */
#define DEV_MMC 0 /* Map MMC/SD card to physical drive 0 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv)
	{
	case DEV_MMC:
		// translate the reslut code here
		stat = RES_OK;

		return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv)
	{

	case DEV_MMC:
		/* GPIO Config */
		gpio_config();

		// 握手阶段 sd 卡频率必须控制在 100k~400k
		spi_setup_clk(SPI1, 0x47);
		delay_1ms(10);

		// 将 SD 卡切换到 SPI 模式
		int result = sd_init(SPI1);
		if (result == SD_SUCCESS)
		{
			stat = RES_OK;
		}
		else
		{
			stat - RES_NOTRDY;
		}
		spi_setup_clk(SPI1, 0x4);

		return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* Physical drive nmuber to identify the drive */
	BYTE *buff,	  /* Data buffer to store read data */
	LBA_t sector, /* Start sector in LBA */
	UINT count	  /* Number of sectors to read */
)
{
	DRESULT res;
	uint8_t token, result;

	switch (pdrv)
	{

	case DEV_MMC:
		result = sd_readMultiBlock(SPI1, sector, count, (uint8_t *)buff, &token);
		if (result >= 0x02 || token != 0xFE)
		{
			return RES_ERROR;
		}

		res = RES_OK;
		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	LBA_t sector,	  /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{
	DRESULT res;
	uint8_t token, result;

	switch (pdrv)
	{
	case DEV_MMC:
		result = sd_writeMultiBlock(SPI1, sector, count, (uint8_t *)buff, &token);
		if (result >= 0x02 || token != 0x05)
		{
			return RES_ERROR;
		}

		res = RES_OK;
		return res;
	}

	return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv)
	{
	case DEV_MMC:

		// Process of the command for the MMC/SD card

		return res;
	}

	return RES_PARERR;
}
