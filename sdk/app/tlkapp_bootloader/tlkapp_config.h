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




#define TLKAPP_CFG_BAT_CHECK_ENABLE      0


#define TLKAPP_BOOT_START_ADDRESS        0x8000

#define TLK_MDI_FILE_ENABLE              1


#define	MAX_BT_ACL_LINK							2				//BT LINK:  acl
#define	MAX_BLE_LINK							2				//BLE LINK: adv & acl
#define	EM_BT_AUDIOBUF_SIZE						256
#define ACL_DATA_BUF_SIZE            			800

#endif //TLKAPP_CONFIG_H

