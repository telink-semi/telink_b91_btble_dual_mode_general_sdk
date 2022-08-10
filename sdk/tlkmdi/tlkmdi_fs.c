/********************************************************************************************************
 * @file     tlkmdi_fs.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_FS_ENABLE)
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/tlkmdi_fs.h"
#if (TLK_DEV_XTSD01G_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xtsd01g.h"
#endif
#if (TLK_DEV_XT26G0X_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xt26g0x.h"
#endif


#if (TLK_DEV_XTSD01G_ENABLE)
static const tlkfs_disk_t sTlkMdiFsDiskXtsd01g = {
	true,//isReady;
	0,//reserve;
	TLKDEV_XTSD01G_DISK_BLOCK_SIZE,//blkSize;
	TLKDEV_XTSD01G_DISK_BLOCK_NUMB,//blkCount;
	tlkdev_xtsd01g_diskInit,//int(*Init)(void);
	tlkdev_xtsd01g_diskRead,//int(*Read)(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff);
	tlkdev_xtsd01g_diskWrite,//int(*Write)(uint32 blkOffs, uint16 blkNumb, uint08 *pData);
};
#elif (TLK_DEV_XT26G0X_ENABLE)
static const tlkfs_disk_t sTlkMdiFsDiskXt26g02 = {
	true,//isReady;
	0,//reserve;
	TLKDEV_XT26G0X_DISK_BLOCK_SIZE,//blkSize;
	TLKDEV_XT26G0X_DISK_BLOCK_NUMB1,//blkCount;
	tlkdev_xt26g0x_diskInit, //int(*Init)(void);
	tlkdev_xt26g0x_diskRead, //int(*Read)(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff);
	tlkdev_xt26g0x_diskWrite,//int(*Write)(uint32 blkOffs, uint16 blkNumb, uint08 *pData);
};
#endif



int tlkmdi_fs_init(void)
{
	#if (TLK_DEV_XTSD01G_ENABLE)
	tlkfs_addDisk((tlkfs_disk_t*)&sTlkMdiFsDiskXtsd01g, 1);
	#elif (TLK_DEV_XT26G0X_ENABLE)
	tlkfs_addDisk((tlkfs_disk_t*)&sTlkMdiFsDiskXt26g02, 1);
	#endif
	#if (TLK_DEV_XTSD01G_ENABLE || TLK_DEV_XT26G0X_ENABLE)
	tlkapi_file_mount(L"1:", 1);
	#endif
	
	return TLK_ENONE;
}






#endif //#if (TLK_MDI_FS_ENABLE)

