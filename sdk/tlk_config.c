/********************************************************************************************************
 * @file     tlk_config.c
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
#include "tlk_config.h"
#include "tlk_define.h"
#include "tlkapi/tlkapi_stdio.h"
#include "drivers.h"


#define TLK_CONFIG_INFO_SIGN        0x3C
#define TLK_CONFIG_INFO_VERS        0x01
#define TLK_CONFIG_INFO_ADDR        TLK_CFG_FLASH_SYS_CONFIG_ADDR
#define TLK_CONFIG_INFO_SIZE        16


static uint08 sTlkWorkMode = TLK_WORK_MODE_NORMAL;
static tlkapi_save_ctrl_t sTlkConfigSave;


int tlkcfg_load(void)
{
	uint08 buffer[TLK_CONFIG_INFO_SIZE] = {0};

	sTlkWorkMode = TLK_WORK_MODE_NORMAL;
	tlkapi_save2_init(&sTlkConfigSave, TLK_CONFIG_INFO_SIGN, TLK_CONFIG_INFO_VERS,
		TLK_CONFIG_INFO_SIZE, TLK_CONFIG_INFO_ADDR);
	if(tlkapi_save2_load(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE) <= 0){
		return TLK_ENONE;
	}
	#if (TLK_CFG_TEST_ENABLE)
	if(buffer[0] >= TLK_WORK_MODE_NORMAL && buffer[0] <= TLK_WORK_MODE_TEST_USR){
		sTlkWorkMode = buffer[0];
	}
	#endif
	return TLK_ENONE;
}


uint tlkcfg_getWorkMode(void)
{
	return sTlkWorkMode;
}
void tlkcfg_setWorkMode(TLK_WORK_MODE_ENUM wmode)
{
	#if (TLK_CFG_TEST_ENABLE)
	uint08 buffer[TLK_CONFIG_INFO_SIZE] = {0};

	if(sTlkWorkMode == wmode) return;
	if(!(wmode >= TLK_WORK_MODE_NORMAL && wmode <= TLK_WORK_MODE_TEST_USR)){
		return;
	}
	sTlkWorkMode = wmode;
	tlkapi_save2_load(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	buffer[0] = sTlkWorkMode;
	tlkapi_save2_save(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	#endif //#if (TLK_CFG_TEST_ENABLE)
}


