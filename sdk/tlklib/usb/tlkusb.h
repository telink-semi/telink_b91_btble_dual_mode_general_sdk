/********************************************************************************************************
 * @file     tlkusb.h
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
#ifndef TLKUSB_H
#define TLKUSB_H

#if (TLK_CFG_USB_ENABLE)



int tlkusb_init(uint16 usbID);

void tlkusb_handler(void);


#if (TLK_USB_MSC_ENABLE)
void tlkusb_irq_handler(void);
#endif
bool tlkusb_setModule(uint08 modtype);

bool tlkusb_mount(uint08 modtype);


void tlkusb_enterSleep(uint mode);


#endif //#if (TLK_CFG_USB_ENABLE)

#endif //TLKUSB_H

