/********************************************************************************************************
 * @file     app_config.h
 *
 * @brief    This is the header file for BTBLE SDK
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

#ifndef APP_CONFIG_H
#define APP_CONFIG_H



#ifdef TLKAPP_DONGLE_ENABLE
	#include "app/tlkapp_dongle/tlkapp_config.h"
#endif
#ifdef TLKAPP_GENERAL_ENABLE
	#include "app/tlkapp_general/tlkapp_config.h"
#endif
#ifdef TLKAPP_CONTROLLER_ENABLE
	#include "app/tlkapp_controller/tlkapp_config.h"
#endif
#ifdef TLKAPP_BOOTLOADER_ENABLE
	#include "app/tlkapp_bootloader/tlkapp_config.h"
#endif


#ifndef JTAG_DEBUG_ENABLE
	//PE4 ~ PE7 as JTAG will cost some power
	#define PE4_FUNC			AS_GPIO
	#define PE5_FUNC			AS_GPIO  //save 120 uA
	#define PE6_FUNC			AS_GPIO
	#define PE7_FUNC			AS_GPIO

	#define PE4_INPUT_ENABLE	0
	#define PE5_INPUT_ENABLE	0
	#define PE6_INPUT_ENABLE	0
	#define PE7_INPUT_ENABLE	0
#endif


#endif /* APP_CONFIG_H */

