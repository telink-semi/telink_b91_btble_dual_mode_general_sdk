/********************************************************************************************************
 * @file     tlkdev_usb.c
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
#include "tlkdev/tlkdev_stdio.h"
#if (TLK_CFG_USB_ENABLE)
#include "tlkdev/sys/tlkdev_nand.h"
#include "tlkdev/sys/tlkdev_usb.h"

#include "drivers.h"

#define TLKDEV_USB_DBG_FLAG         (TLKDEV_CFG_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKDEV_USB_DBG_SIGN         "[DEVUSB]"

#define TLKDEV_USB_DBGID_FLASH_ADDR         (0x200BFFF0)     // usb id flash store address


extern int tlkusb_init(uint16 usbID);


int tlkdev_usb_init(void)
{
	/////////////////////////////////  USB ////////////////////////////////////////////////
//	uint16 usbID = 0x120;
//	uint08 flag = *(uint08 *)(TLKDEV_USB_DBGID_FLASH_ADDR);
//	usbID = (0xff == flag)?0x120:(0x0100 + flag);
	tlkusb_init(0x120);
	/* increase 1mA when test low power, so disable USB when PM used */
		
	return TLK_ENONE;
}

void tlkdev_usb_shutdown(void)
{
	gpio_shutdown(GPIO_PA5|GPIO_PA6);
}


#endif

