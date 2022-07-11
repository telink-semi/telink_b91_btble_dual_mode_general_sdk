/********************************************************************************************************
 * @file     btp_hid.h
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
#ifdef  BTP_HID_H
#define BTP_HID_H


#include "tlkapi/tlkapi_stdio.h"
#include "tlkstk/bt/btp/btp_config.h"
#include "tlkstk/bt/bth/bth_stdio.h"


/* API_START */

uint08 * btp_hid_get_boot_descriptor_data(void);


uint16 btp_hid_get_boot_descriptor_len(void);
#endif
