/********************************************************************************************************
 * @file     main.c
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
#include "tlkapp_config.h"
#include "tlkapp.h"

#include "tlkapp_dfu.h"
#include "drivers.h"



/******************************************************************************
 * Function: main
 * Descript: This is main function.
 * Params: None.
 * Return: 0 is successs.
 * Others: None.
*******************************************************************************/
int main(void)
{
    blc_pm_select_internal_32k_crystal();

    sys_init(DCDC_1P4_LDO_1P8,VBAT_MAX_VALUE_GREATER_THAN_3V6);
	
    CCLK_48M_HCLK_48M_PCLK_24M;

	#if 1
	tlkapp_dfu_load();
	TLKAPP_JUMP_TO_APP();
	while(1){ core_reboot(); }
	#else
    tlkapp_init();
	while(1)
	{
		tlkapp_handler();
	}
	#endif
    return 0;
} 

