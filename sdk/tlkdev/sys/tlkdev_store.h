/********************************************************************************************************
 * @file	tlkdev_store.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef TLKDEV_STORE_H
#define TLKDEV_STORE_H

#if (TLK_DEV_STORE_ENABLE)



bool tlkdev_store_isOpen(void);

int tlkdev_store_init(void);
int tlkdev_store_open(void);
int tlkdev_store_close(void);
int tlkdev_store_erase(uint32 addr);
int tlkdev_store_read(uint32 addr, uint08 *pBuff, uint16 buffLen);
int tlkdev_store_write(uint32 addr, uint08 *pData, uint16 dataLen);
int tlkdev_store_sdInit(void);
int tlkdev_store_format(void);
int tlkdev_store_sdread(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
int tlkdev_store_sdwrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb);

int tlkdev_store_getPageSize(void);
int tlkdev_store_getPageNumb(void);
int tlkdev_store_getSectorSize(void);
int tlkdev_store_getSectorNumb(void);
int tlkdev_store_getSdBlockSize(void);
int tlkdev_store_getSdBlockNumb(void);

void tlkdev_store_enterSleep(uint mode);


#endif //TLK_DEV_STORE_ENABLE

#endif //TLKDEV_STORE_H

