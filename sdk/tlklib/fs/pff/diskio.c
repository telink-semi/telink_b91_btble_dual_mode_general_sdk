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
/* Low level disk I/O module skeleton for Petit fatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"

#include "tlkstk/inner/tlkstk_inner.h"
#include "diskio.h"
#include "drivers.h"






#if (TLK_DEV_XT2602E_ENABLE)

extern const unsigned char dbr[512];
extern unsigned int fat_remap(unsigned int adr);
extern DRESULT disk_writep(BYTE* buff, DWORD sc);

/******************************************************************************
 * Function: disk_initialize
 * Descript: Initialize Disk Drive.
 * Params: None.
 * Return: Refer to DSTATUS.
 * Others: None.
*******************************************************************************/
DSTATUS disk_initialize (void)
{
	DSTATUS stat;

	stat = 0; // disk ok
	// Put your code here

	return stat;
}

/******************************************************************************
 * Function: disk_readp
 * Descript: Read Partial Sector.
 * Params:
 *     @sector[IN]--Sector number (LBA).
 *     @offset[IN]--Offset in the sector.
 *     @count[IN]--Byte count (bit15:destination).
 *     @buff[OUT]--Pointer to the destination object.
 * Return: Refer to DRESULT.
 * Others: None.
*******************************************************************************/
DRESULT disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count)
{
	DRESULT res;
	unsigned long  adr = (sector*512) + offset;

	// Put your code here
	#if (TLK_DEV_XT2602E_ENABLE)
    if(adr<512)
    {
    	memcpy(buff,&dbr[adr],count);
    }
    else if(adr>=512 && adr < 0x10200) // FAT page
    {
    	adr = fat_remap(adr);
		tlkdev_xt2602e_read(adr, buff, count);
    }
    else
    {
    	adr = adr - 512;
		tlkdev_xt2602e_read(adr, buff, count);
    }
	#else
	if(adr<512)
	{
		memcpy(buff,dbr+adr,count);
	}
	else
	{
		flash_read_page(0x90000+adr-512,count,buff);
	}
	#endif

	res = RES_OK;
	return res;
}


/******************************************************************************
 * Function: disk_readp
 * Descript: Write Partial Sector.
 * Params:
 *     @sc[IN]--Sector number (LBA) or Number of bytes to send.
 *     @buff[IN]--Pointer to the data to be written, NULL:Initiate/Finalize write operation.
 * Return: Refer to DRESULT.
 * Others: None.
*******************************************************************************/
DRESULT disk_writep(BYTE* buff, DWORD sc)
{
	if(!buff){
		if(sc){
			// Initiate write process
		}else{
			// Finalize write process
		}
	}else{
		// Send data to the disk
	}

	return RES_OK;
}

#endif
