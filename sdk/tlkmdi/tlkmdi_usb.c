/********************************************************************************************************
 * @file     tlkmdi_hfp.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_USB_ENABLE)
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_usb.h"
#if (TLK_USB_MSC_ENABLE)
#include "tlklib/usb/msc/tlkusb_msc.h"
#endif


#include "drivers.h"



extern int tlkusb_init(uint16 usbID);
extern void tlkusb_process(void);

#if (TLK_USB_MSC_ENABLE)
extern int tlkdev_xtsd04g_read(uint08 *pBuff, uint32 sector, uint08 sectCnt);
extern int tlkdev_xtsd04g_write(uint08 *pData, uint32 sector, uint08 sectCnt);
static int tlkmdi_usbMscInit(void)
{
	return TLK_ENONE;
}
static int tlkmdi_usbMscRead(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff)
{
	return tlkdev_xtsd04g_read(pBuff, blkOffs, 1);
}
static int tlkmdi_usbMscWrite(uint32 blkOffs, uint16 blkNumb, uint08 *pData)
{
	return tlkdev_xtsd04g_write(pData, blkOffs, 1);
}
static tlkusb_msc_unit_t sTlkUsbMscDisk0 = {
	true, //isReady;
	false, //hotPlug; //1-Enable, 0-Disable
	512, //blkSize;
	0x0003D000, //blkCount;
	"TLK-DISK", //pVendorStr; //<=8
	"Telink Disk Demo", //pProductStr; //<=16
	"1.01", //pVersionStr; //<=4
	tlkmdi_usbMscInit, //int(*Init)(void);
	tlkmdi_usbMscRead, //int(*Read)(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff);
	tlkmdi_usbMscWrite, //int(*Write)(uint32 blkOffs, uint16 blkNumb, uint08 *pData);
};
#endif



int tlkmdi_usb_init(void)
{
	tlkusb_init(0x120);
	/* increase 1mA when test low power, so disable USB when PM used */
	#if (TLK_USB_MSC_ENABLE)
	tlkusb_msc_appendDisk(&sTlkUsbMscDisk0);
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_usb_process(void)
{
	tlkusb_process();
}



void tlkmdi_usb_shutdown(void)
{
	gpio_shutdown(GPIO_PA5|GPIO_PA6);
}




#endif //#if (TLK_MDI_USB_ENABLE)
