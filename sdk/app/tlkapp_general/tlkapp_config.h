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




#define TLKAPP_DBG_ENABLE       1
#define TLKAPP_DBG_FLAG         (TLKAPP_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKAPP_DBG_SIGN         "[APP]"


#define TLKAPP_WAKEUP_PIN       GPIO_PE1


#define TLKAPP_MEM_TOTAL_SIZE            (40*1024) //MP3(36740)+SrcEnc(4048) = 36740+4048=40788  -- Worst scenario: Music playing on the headphone


#define TLKAPP_CFG_BAT_CHECK_ENABLE      0



/////////////////////  BLE configuration ////////////////////////////
#define	MAX_BT_ACL_LINK							2				//BT LINK:  acl
#define	MAX_BLE_LINK							2				//BLE LINK: adv & acl
#define	EM_BT_AUDIOBUF_SIZE						256
#define ACL_DATA_BUF_SIZE            			800


#define BLMS_MAX_CONN_MASTER_NUM				0
#define	BLMS_MAX_CONN_SLAVE_NUM					1




#endif //TLKAPP_CONFIG_H

