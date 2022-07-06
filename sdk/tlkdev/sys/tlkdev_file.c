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
#include "tlklib/fs/filesystem.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkdev/sys/tlkdev_nand.h"
#include "tlkdev/sys/tlkdev_file.h"


extern void nand_flash_fat_init(int reset);



int tlkdev_file_init(void)
{
	tlkdev_nand_powerOn();
	
	#if (TLK_DEV_XT2602E_ENABLE)
		nand_flash_fat_init(0);
		#if (TLK_FS_PFF_ENABLE)
		tlkapi_file_mount("", 0);
		#endif
	#endif
	#if (TLK_DEV_XTSD04G_ENABLE)
	if(tlkdev_nand_isReady()){
		#if (TLK_FS_FAT_ENABLE)
		tlkapi_file_mount(L"1:", 1);
		#endif
	}
	#endif
	
	return TLK_ENONE;
}





