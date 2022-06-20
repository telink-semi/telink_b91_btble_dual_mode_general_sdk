/********************************************************************************************************
 * @file     tlkapp_debug.c
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

#include "types.h"
#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#include "tlkprt/tlkprt_comm.h"

#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"

#include "tlklib/usb/tlkusb_stdio.h"
#include "tlklib/usb/udb/tlkusb_udb.h"


#include "tlkapp_config.h"



#if (TLK_USB_UDB_ENABLE)
static void tlkapp_debug_usbHandler(uint08 *pData, uint16 dataLen);
#endif



int tlkapp_debug_init(void)
{
	#if (TLK_USB_UDB_ENABLE)
	tlkusb_udb_regDbgCB(tlkapp_debug_usbHandler);
	#endif

	return TLK_ENONE;
}









#if (TLK_USB_UDB_ENABLE)
static void tlkapp_debug_usbHandler(uint08 *pData, uint16 dataLen)
{
	tlkapi_array(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_usbHandler receive", pData, dataLen);

	if(pData[0] != 0x11) return;

	switch(pData[1])
	{
		case 0x09:
			flash_erase_sector(0xBF000);
			break;

		case 0x10:// clean all history pairing information 
			break;
		case 0x11:// clean all history pairing information 
			break;
		
		case 0xf2:

			break;
		break;
	    case 0xfe:
	    {

	    }
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


