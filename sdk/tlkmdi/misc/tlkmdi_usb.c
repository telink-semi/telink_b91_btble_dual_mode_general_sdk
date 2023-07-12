/********************************************************************************************************
 * @file	tlkmdi_usb.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_MDI_USB_ENABLE)
#include "tlkmdi/misc/tlkmdi_usb.h"
#if (TLK_USB_MSC_ENABLE)
#include "tlklib/usb/msc/tlkusb_msc.h"
#endif
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/sys/tlkdev_store.h"
#endif

#include "drivers.h"


extern int  tlkusb_init(uint16 usbID);
extern void tlkusb_handler(void);

#if (TLK_USB_MSC_ENABLE && TLK_DEV_STORE_ENABLE)
static const tlkusb_msc_disk_t sTlkUsbMscDisk0;
#endif



int tlkmdi_usb_init(void)
{
	tlkusb_init(0x120);
	/* increase 1mA when test low power, so disable USB when PM used */
	#if (TLK_USB_MSC_ENABLE && TLK_DEV_STORE_ENABLE)
	tlkusb_msc_appendDisk((tlkusb_msc_disk_t*)&sTlkUsbMscDisk0);
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_usb_handler(void)
{
	tlkusb_handler();
}



void tlkmdi_usb_shutdown(void)
{
	gpio_shutdown(GPIO_PA5|GPIO_PA6);
}




#if (TLK_USB_MSC_ENABLE && TLK_DEV_STORE_ENABLE)
static int tlkmdi_usb_diskInit(void)
{
	if(!tlkdev_store_isOpen()) tlkdev_store_open();
	return tlkdev_store_sdInit();
}
static int tlkmdi_usb_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	if(!tlkdev_store_isOpen()) tlkdev_store_open();
	return tlkdev_store_sdread(pBuff, blkOffs, blkNumb);
}
static int tlkmdi_usb_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	if(!tlkdev_store_isOpen()) tlkdev_store_open();
	return tlkdev_store_sdwrite(pData, blkOffs, blkNumb);
}
static const tlkusb_msc_disk_t sTlkUsbMscDisk0 = {
	true, //isReady;
	true, //hotPlug; //1-Enable, 0-Disable
	512,//blkSize;
	0x0003E000,//blkCount; 128M-2M=126M
	"TLK-DISK", //pVendorStr; //<=8
	"Telink Disk Demo", //pProductStr; //<=16
	"1.02", //pVersionStr; //<=4
	tlkmdi_usb_diskInit,//int(*Init)(void);
	tlkmdi_usb_diskRead,//int(*Read)(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff);
	tlkmdi_usb_diskWrite,//int(*Write)(uint32 blkOffs, uint16 blkNumb, uint08 *pData);
};
#endif


#endif //#if (TLK_MDI_USB_ENABLE)

