/********************************************************************************************************
 * @file     tlkdev_nand.c
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

#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_nand.h"

#include "drivers.h"

#include "tlkdev/ext/xtx/tlkdev_xt2602e.h"
#include "tlkdev/ext/xtx/tlkdev_xtsd04g.h"


#define TLKDEV_NAND_DBG_FLAG         (TLKDEV_CFG_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKDEV_NAND_DBG_SIGN         "[NAND]"


extern	uint32 sdcardblocknum;

volatile static uint08 sTlkDevNandIsReady = 0;
volatile static uint08 sTlkDevNandIsEnable = false;



int tlkdev_nand_init(void)
{
	sTlkDevNandIsReady = false;
	#if (TLK_DEV_XTSD04G_ENABLE)
		uint08 temp[512] = {0};
		if(tlkdev_xtsd04g_init() != TLK_ENONE) return -TLK_EFAIL;
		tlkdev_xtsd04g_read(temp, 0, 1);
		if((temp[510] != 0x55) || (temp[511] != 0xaa)) return -TLK_EFAIL;
		
		sdcardblocknum = tlkdev_xtsd04g_getSectorCount();
		uint32_t sdcardfatblocknum = (temp[461]<<24) + (temp[460]<<16) + (temp[459]<<8) + temp[458];
		if((sdcardfatblocknum != (sdcardblocknum - SDDISK_SECTOR_OFFSET))){
			tlkdev_xtsd04g_format();
			tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "sdcard format end, please restart");
		}
	#endif
	#if (TLK_DEV_XT2602E_ENABLE)
		tlkdev_xt2602e_init();
	#endif
	sTlkDevNandIsReady = true;
	return TLK_ENONE;
}


void tlkdev_nand_powerOn(void)
{
	if(sTlkDevNandIsEnable) return;
	#if (TLK_DEV_XTSD04G_ENABLE)
	tlkdev_xtsd04g_powerOn();
	#endif
	#if (TLK_DEV_XT2602E_ENABLE)
	tlkdev_xt2602e_powerOn();
	#endif
	sTlkDevNandIsEnable = true;
}

void tlkdev_nand_powerOff(void)
{
	if(!sTlkDevNandIsEnable) return;
	#if (TLK_DEV_XTSD04G_ENABLE)
	tlkdev_xtsd04g_powerOff();
	#endif
	#if (TLK_DEV_XT2602E_ENABLE)
	tlkdev_xt2602e_powerOff();
	#endif
	sTlkDevNandIsEnable = false;
}

void tlkdev_nand_shutdown(void)
{
	gpio_shutdown(GPIO_PC4|GPIO_PC5|GPIO_PC6|GPIO_PC7);
	tlkdev_nand_powerOff();
}

bool tlkdev_nand_isReady(void)
{
	return sTlkDevNandIsReady;
}
bool tlkdev_nand_isPowerOn(void)
{
	return sTlkDevNandIsEnable;
}





////beta function, do not use///////////!!!!!!!!!
void tlkdev_nand_test(void)
{
	#if (TLK_DEV_XTSD04G_ENABLE)
	unsigned int sector_addr = 1001;
	unsigned int test_addr = 0;
	unsigned char temp[512*10] = {0};
	
	tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "nand_flash_pressure_test_start");

	tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "tlkdev_xtsd04g_read");
	for(unsigned int i = 0; i < 250; i+=10)
	{
		if(tlkdev_xtsd04g_read(temp,i+sector_addr,10))
		{
			tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "tlkdev_xtsd04g_read");
			return;
		}
		for(unsigned int j = 0; j < (512*10); j++)
		{
			if(temp[j] != (*(unsigned char * )(test_addr+j+(i*512))))
			{
				return;
			}
		}
	}

	//change sector_addr for next test
	sector_addr = 1002;
	tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "tlkdev_xtsd04g_write");
	if(tlkdev_xtsd04g_write((unsigned char * )(test_addr),sector_addr,250))
	{
		tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "SD_WriteDisk_fail");
		return;
	}
	else
	{
		tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "SD_WriteDisk_ok");
	}

	tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "tlkdev_xtsd04g_read");
	for(unsigned int i = 0; i < 250; i+=10)
	{
		if(tlkdev_xtsd04g_read(temp,i+sector_addr,10))
		{
			tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "tlkdev_xtsd04g_read");
			return;
		}
		for(unsigned int j = 0; j < (512*10); j++)
		{
			if(temp[j] != (*(unsigned char * )(test_addr+j+(i*512))))
			{
				tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "SD_ReadDisk_error_inaddr:addr-0x%x,i%d,j:%d", test_addr, i, j);
				return;
			}
		}
		tlkapi_trace(TLKDEV_NAND_DBG_FLAG, TLKDEV_NAND_DBG_SIGN, "SD_ReadDisk_ok_inaddr:");
	}
	#endif
}


