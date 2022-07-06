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
#include "tlkapp_system.h"


#if (TLK_CFG_USB_ENABLE)
extern bool tlkusb_setModule(uint08 modtype); //TLKUSB_MODTYPE_ENUM
#endif
extern int tlkmdi_btacl_connect(uint08 *pBtAddr, uint32 devClass, uint32 timeout);;
extern int tlkmmi_phone_bookSetParam(uint08 posi, uint08 type, uint08 sort, uint16 offset, uint16 number);
extern int tlkdev_xtsd04g_format(void);


#if (TLK_USB_UDB_ENABLE)
static void tlkapp_debug_usbHandler(uint08 *pData, uint16 dataLen);
#endif
static void tlkapp_debug_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen);
static void tlkapp_debug_cmdStartToneHandler(uint08 *pData, uint08 dataLen);
static void tlkapp_debug_cmdGetPhoneBookHandler(uint08 *pData, uint08 dataLen);
static void tlkapp_debug_cmdSetUSBModeHandler(uint08 *pData, uint08 dataLen);
static void tlkapp_debug_cmdSimulateKeyHandler(uint08 *pData, uint08 dataLen);


extern uint16 gTlkAppBtConnHandle;


/******************************************************************************
 * Function: tlkapp_debug_init
 * Descript: This function use to register callback for usb or cmd.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_debug_init(void)
{
	#if (TLK_USB_UDB_ENABLE)
	tlkusb_udb_regDbgCB(tlkapp_debug_usbHandler);
	#endif
	tlkmdi_comm_regDbgCB(tlkapp_debug_cmdHandler);
	
	return TLK_ENONE;
}




static void tlkapp_debug_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdHandler: msgID-%d", msgID);
	
	if(msgID == TLKPRT_COMM_CMDID_DBG_START_TONE){
		tlkapp_debug_cmdStartToneHandler(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_GET_PHONE_BOOK){
		tlkapp_debug_cmdGetPhoneBookHandler(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_SET_USB_MODE){
		tlkapp_debug_cmdSetUSBModeHandler(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_SIMULATE_KEY){
		tlkapp_debug_cmdSimulateKeyHandler(pData, dataLen);
	}
}
static void tlkapp_debug_cmdStartToneHandler(uint08 *pData, uint08 dataLen)
{
	uint16 index;
	uint08 count;
	
	if(dataLen < 3){
		tlkapi_error(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdStartToneHandler: length error - %d", dataLen);
		return;
	}

	tlkapi_array(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdStartToneHandler: start", pData, dataLen);
	index = ((uint16)pData[1]<<8)|pData[0];
	count = pData[2];
	tlkmmi_audio_startTone(index, count);
}
static void tlkapp_debug_cmdGetPhoneBookHandler(uint08 *pData, uint08 dataLen)
{
	uint08 posi;
	uint08 type;
	uint08 sort;
	uint16 offset;
	uint16 number;
	uint16 aclHandle;
	bth_acl_handle_t *pHandle;
	
	if(dataLen < 7){
		tlkapi_error(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdGetPhoneBookHandler: length error - %d", dataLen);
		return;
	}

	
	aclHandle = btp_pbapclt_getAnyConnHandle(0);
	if(aclHandle == 0){
		tlkapi_error(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdGetPhoneBookHandler: failure - no device");
		return;
	}

	pHandle = bth_handle_getConnAcl(aclHandle);
	if(pHandle == nullptr){
		tlkapi_error(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdGetPhoneBookHandler: fault - ACL handle not exist");
		return;
	}
	
	tlkapi_array(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdGetPhoneBookHandler: start", pData, 7);
	
	posi = pData[0];
	type = pData[1];
	sort = pData[2];
	offset = ((uint16)pData[4]<<8)|pData[3];
	number = ((uint16)pData[6]<<8)|pData[5];
	tlkmmi_phone_bookSetParam(posi, type, sort, offset, number);
	tlkmmi_phone_startSyncBook(pHandle->aclHandle, pHandle->btaddr, true);
}
static void tlkapp_debug_cmdSetUSBModeHandler(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 1){
		tlkapi_error(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdSetUSBModeHandler: length error - %d", dataLen);
		return;
	}
	tlkapi_array(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_cmdSetUSBModeHandler: ", pData, dataLen);
	#if (TLK_CFG_USB_ENABLE)
	tlkusb_setModule(pData[0]);
	#endif
}
static void tlkapp_debug_cmdSimulateKeyHandler(uint08 *pData, uint08 dataLen)
{
	
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

		case 0x10:
			break;
		case 0x11:
			break;
		
		case 0xf2:
			#if (TLK_DEV_XTSD04G_ENABLE)
			tlkdev_xtsd04g_format();
			tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkdev_xtsd04g_format");
			#endif
			break;
		break;
	    case 0xfe:
	    {
	    	tlkapp_debug_cmdGetPhoneBookHandler(pData+2,7);
	    }
	        break;

		case 0xFF:
		{
			tlkmdi_btacl_connect(pData+2,0x7a020c,0x3a98);
		}
			break;

	    default:
	        break;
	}

}
#endif


