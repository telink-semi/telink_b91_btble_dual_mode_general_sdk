/********************************************************************************************************
 * @file     tlkmmi_config.h
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

#ifndef TLKMMI_CONFIG_H
#define TLKMMI_CONFIG_H


#define TLKMMI_FILE_ENABLE          1
#define TLKMMI_AUDIO_ENABLE         1
#define TLKMMI_BTMGR_ENABLE         (1 && TLK_STK_BT_ENABLE)
#define TLKMMI_LEMGR_ENABLE         (1 && TLK_STK_LE_ENABLE)
#define TLKMMI_PHONE_ENABLE         1

#define TLKMMI_HID_DBG_ENABLE       (1 && TLKMMI_CFG_DBG_ENABLE)

#define TLKMMI_FILE_DBG_ENABLE      (1 && TLKMMI_CFG_DBG_ENABLE)
#define TLKMMI_AUDIO_DBG_ENABLE     (1 && TLKMMI_CFG_DBG_ENABLE)
#define TLKMMI_BTMGR_DBG_ENABLE     (1 && TLKMMI_CFG_DBG_ENABLE)
#define TLKMMI_LEMGR_DBG_ENABLE     (1 && TLKMMI_CFG_DBG_ENABLE)
#define TLKMMI_PHONE_DBG_ENABLE     (1 && TLKMMI_CFG_DBG_ENABLE)
#define TLKMMI_PHONE_BOOK_DBG_ENABLE     (1 && TLKMMI_PHONE_DBG_ENABLE)


#define TLKMMI_BTMGR_BTREC_ENABLE         (1 && TLKMMI_BTMGR_ENABLE && TLK_MDI_BTREC_ENABLE)
#define TLKMMI_BTMGR_BTINQ_ENABLE         (1 && TLKMMI_BTMGR_ENABLE && TLK_MDI_BTINQ_ENABLE)
#define TLKMMI_BTMGR_BTACL_ENABLE         (1 && TLKMMI_BTMGR_ENABLE && TLK_MDI_BTACL_ENABLE)

#define TLKMMI_FILE_DFU_ENABLE            (1 && TLKMMI_FILE_ENABLE && TLK_MDI_FILE_ENABLE)
#define TLKMMI_FILE_MP3_ENABLE            (1 && TLKMMI_FILE_ENABLE && TLK_MDI_FILE_ENABLE)
#define TLKMMI_FILE_TONE_ENABLE           (1 && TLKMMI_FILE_ENABLE && TLK_MDI_FILE_ENABLE)



#define TLKMMI_PHONE_BOOK_ENABLE    (1 && TLKMMI_PHONE_ENABLE)


#endif //TLKMMI_CONFIG_H

