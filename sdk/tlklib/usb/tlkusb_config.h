/********************************************************************************************************
 * @file     tlkusb_config.h
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

#ifndef TLKUSB_CONFIG_H
#define TLKUSB_CONFIG_H



#define TLKUSB_UDB_ENABLE        (1 && TLK_USB_UDB_ENABLE)
#define TLKUSB_MSC_ENABLE        (1 && TLK_USB_MSC_ENABLE)
#define TLKUSB_AUD_ENABLE        (1 && TLK_USB_AUD_ENABLE)
#define TLKUSB_CDC_ENABLE        (0 && TLK_CFG_USB_ENABLE)
#define TLKUSB_USR_ENABLE        (0 && TLK_CFG_USB_ENABLE)

//VCD - MSC - AUD   Mutex




#endif //TLKUSB_CONFIG_H

