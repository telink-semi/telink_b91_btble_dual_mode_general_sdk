/********************************************************************************************************
 * @file     tlkapp_config.h
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
#ifndef TLKAPP_CONFIG_H
#define TLKAPP_CONFIG_H



#define TLKAPP_DBG_FLAG         ((TLK_MAJOR_DBGID_APP << 24) | (TLK_MINOR_DBGID_APP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKAPP_DBG_SIGN         "[APP]"


#define TLKAPP_WAKEUP_PIN       GPIO_PA2


#define TLKAPP_MEM_TOTAL_SIZE            (40*1024) //MP3(36740)+SrcEnc(4048) = 36740+4048=40788  -- Worst scenario: Music playing on the headphone







#endif //TLKAPP_CONFIG_H

