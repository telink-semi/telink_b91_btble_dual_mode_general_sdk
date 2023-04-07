/********************************************************************************************************
 * @file     tlkdev_store.c
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
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkdrv/ext/store/tlkdrv_store.h"
#include "tlkdev/sys/tlkdev_store.h"
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys/tlksys_stdio.h"
#endif


#define TLKDEV_STORE_DEV        TLKDRV_STORE_DEV_XTSD01G


bool tlkdev_store_isOpen(void)
{
	return tlkdrv_store_isOpen(TLKDEV_STORE_DEV);
}

int tlkdev_store_init(void)
{
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendEnterSleepCB(tlkdev_store_enterSleep);
	#endif
	return tlkdrv_store_init(TLKDEV_STORE_DEV);
}
int tlkdev_store_open(void)
{
	return tlkdrv_store_open(TLKDEV_STORE_DEV);
}
int tlkdev_store_close(void)
{
	return tlkdrv_store_close(TLKDEV_STORE_DEV);
}
int tlkdev_store_erase(uint32 addr)
{
	return tlkdrv_store_erase(TLKDEV_STORE_DEV, addr);
}
int tlkdev_store_read(uint32 addr, uint08 *pBuff, uint16 buffLen)
{
	return tlkdrv_store_read(TLKDEV_STORE_DEV, addr, pBuff, buffLen);
}
int tlkdev_store_write(uint32 addr, uint08 *pData, uint16 dataLen)
{
	return tlkdrv_store_write(TLKDEV_STORE_DEV, addr, pData, dataLen);
}
int tlkdev_store_sdInit(void)
{
	return tlkdrv_store_sdInit(TLKDEV_STORE_DEV);
}
int tlkdev_store_format(void)
{
	return tlkdrv_store_format(TLKDEV_STORE_DEV);
}
int tlkdev_store_sdread(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	return tlkdrv_store_sdread(TLKDEV_STORE_DEV, pBuff, blkOffs, blkNumb);
}
int tlkdev_store_sdwrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	return tlkdrv_store_sdwrite(TLKDEV_STORE_DEV, pData, blkOffs, blkNumb);
}

int tlkdev_store_getPageSize(void)
{
	return tlkdrv_store_getPageSize(TLKDEV_STORE_DEV);
}
int tlkdev_store_getPageNumb(void)
{
	return tlkdrv_store_getPageNumb(TLKDEV_STORE_DEV);
}
int tlkdev_store_getSectorSize(void)
{
	return tlkdrv_store_getSectorSize(TLKDEV_STORE_DEV);
}
int tlkdev_store_getSectorNumb(void)
{
	return tlkdrv_store_getSectorNumb(TLKDEV_STORE_DEV);
}
int tlkdev_store_getSdBlockSize(void)
{
	return tlkdrv_store_getSdBlockSize(TLKDEV_STORE_DEV);
}
int tlkdev_store_getSdBlockNumb(void)
{
	return tlkdrv_store_getSdBlockNumb(TLKDEV_STORE_DEV);
}

void tlkdev_store_enterSleep(uint mode)
{
	tlkdev_store_close();
}


#endif //#if (TLK_DEV_STORE_ENABLE)

