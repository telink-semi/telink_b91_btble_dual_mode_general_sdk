/********************************************************************************************************
 * @file     diskio.c
 *
 * @brief    This is the source file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#if (TLK_FS_FAT_ENABLE)
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "drivers.h"


extern int tlkdev_xtsd04g_read(uint08 *pBuff, uint32 sector, uint08 sectCnt);
extern int tlkdev_xtsd04g_write(uint08 *pData, uint32 sector, uint08 sectCnt);
extern void tlkdev_xtsd04g_spiCsLow(void);
extern uint08 tlkdev_xtsd04g_waitReady(void);
extern uint32 tlkdev_xtsd04g_getSectorCount(void);
extern void tlkdev_xtsd04g_spiCsHigh(void);



/* Definitions of physical drive number for each drive */
#define DEV_NOR_FLASH	0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */



/******************************************************************************
 * Function: disk_status
 * Descript: Get Drive Status.
 * Params:
 *     @pdrv[IN]--Physical drive nmuber to identify the drive.
 * Return: Refer to DSTATUS.
 * Others: None.
*******************************************************************************/
DSTATUS disk_status(BYTE pdrv)
{
	pdrv++;
	return 0;
}

/******************************************************************************
 * Function: disk_initialize
 * Descript: Inidialize a Drive.
 * Params:
 *     @pdrv[IN]--Physical drive nmuber to identify the drive.
 * Return: Refer to DSTATUS.
 * Others: None.
*******************************************************************************/
DSTATUS disk_initialize(BYTE pdrv)
{
	u8 res=0;
	switch(pdrv)
	{
		#if (TLK_DEV_XTSD04G_ENABLE)
		case DEV_MMC://
			res = 0;//sd_nand_flash_init();//sd_nand_flash_init in app_nand_flash.c, No need to repeat at here
			break;
		#endif
		default:
			res=1;
	}
	if(res) return STA_NOINIT;
	else return 0;
}

/******************************************************************************
 * Function: disk_read
 * Descript: Read Sector(s).
 * Params:
 *     @pdrv[IN]--Physical drive nmuber to identify the drive.
 *     @sector[IN]--Start sector in LBA.
 *     @count[IN]--Number of sectors to read.
 *     @buff[OUT]--Data buffer to store read data.
 * Return: Refer to DRESULT.
 * Others: None.
*******************************************************************************/
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
	u8 res=0;

	if (!count)return RES_PARERR;
	switch(pdrv)
	{
		#if (TLK_DEV_XTSD04G_ENABLE)
		case DEV_MMC :
			if(tlkdev_xtsd04g_read(buff, sector, count)) res=RES_ERROR;
			break;
		#endif
		default:
			res=1;
	}
	if(res == 0x00)return RES_OK;
	else return RES_ERROR;
}


/******************************************************************************
 * Function: disk_write
 * Descript: Write Sector(s).
 * Params:
 *     @pdrv[IN]--Physical drive nmuber to identify the drive.
 *     @buff[IN]--Data to be written.
 *     @sector[IN]--Start sector in LBA.
 *     @count[IN]--Number of sectors to write.
 * Return: Refer to DRESULT.
 * Others: None.
*******************************************************************************/
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
	u8 res=0;
	if(!count) return RES_PARERR;
	switch (pdrv) {
		#if (TLK_DEV_XTSD04G_ENABLE)
		case DEV_MMC :
			if(tlkdev_xtsd04g_write((u8*)buff,sector,count)) res=RES_ERROR;
			break;
		#endif
		default:
			res=1;
	}

	if(res == 0x00)return RES_OK;
	else return RES_ERROR;
}



/******************************************************************************
 * Function: disk_ioctl
 * Descript: Miscellaneous Functions.
 * Params:
 *     @pdrv[IN]--Physical drive nmuber (0..) .
 *     @cmd[IN]--Control code.
 *     @buff[IN/OUT]--Buffer to send/receive control data.
 * Return: Refer to DRESULT.
 * Others: None.
*******************************************************************************/
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
	u8 res = 0;
	#if (TLK_DEV_XTSD04G_ENABLE)
	if(pdrv==DEV_MMC)
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				tlkdev_xtsd04g_spiCsLow();
				if(tlkdev_xtsd04g_waitReady() == 0)res = RES_OK;
				else res = RES_ERROR;
				tlkdev_xtsd04g_spiCsHigh();
				break;
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
				res = RES_OK;
				break;
			case GET_BLOCK_SIZE:
				*(WORD*)buff = 8;
				res = RES_OK;
				break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = tlkdev_xtsd04g_getSectorCount();
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
				break;
		}
	}
	#endif

	return res;

}

/******************************************************************************
 * Function: get_fattime
 * Descript: None.
 * Params: None.
 * Return: Refer to DRESULT.
 * Others: None.
*******************************************************************************/
DWORD get_fattime (void)
{
	return 0;
}


#endif //#if (TLK_FS_FAT_ENABLE)

