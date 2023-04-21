/********************************************************************************************************
 * @file     tlkmmi_sysDbg.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlkapi/tlkapi_stdio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmmi/system/tlkmmi_sys.h"
#include "tlkmmi/system/tlkmmi_sysDbg.h"

#include "tlklib/usb/tlkusb_stdio.h"
#include "tlklib/usb/udb/tlkusb_udb.h"

#if (TLK_DEV_STORE_ENABLE)
extern int tlkdev_store_format(void);
#endif

#if (TLK_USB_UDB_ENABLE)
static void tlkmmi_sys_dbgInput(uint08 *pData, uint16 dataLen);
#endif


/******************************************************************************
 * Function: tlkapp_debug_init
 * Descript: This function use to register callback for usb or cmd.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkmmi_sys_dbgInit(void)
{
	#if (TLK_USB_UDB_ENABLE)
	tlkusb_udb_regDbgCB(tlkmmi_sys_dbgInput);
	#endif
	
	return TLK_ENONE;
}


#if (TLK_USB_UDB_ENABLE)
static void tlkmmi_sys_dbgInput(uint08 *pData, uint16 dataLen)
{
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkapp_debug_usbHandler receive", pData, dataLen);

	if(pData[0] != 0x11) return;

	switch(pData[1])
	{
//		case 0x01:
//			tlkdev_lcd_init();
//			break;
//		case 0x02:
//			tlkdev_lcd_open();
//			break;
//		case 0x03:
//			tlkdev_touch_init();
//			break;
//		case 0x05:
//			tlkdev_touch_open();
//			break;
//		case 0x06:
//			tlkdev_lcd_open();
//			tlkdev_touch_open();
//			break;

		case 0x10:
			break;
		case 0x11:
			break;
		
		case 0xf2:
			#if (TLK_DEV_STORE_ENABLE)
			tlkdev_store_format();
			tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdev_store_format");
			#endif
			break;
	    case 0xfe:
	    	bth_hci_sendSniffModeCmd(0x0010, 8,
	    						8, 2, 0);
	        break;

		case 0xFF:
		{

		}
			break;

	    default:
	        break;
	}

}
#endif









#endif //#if (TLKMMI_SYSTEM_ENABLE)

