/********************************************************************************************************
 * @file     tlkmmi_btmgrCtrl.c
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

#include "string.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_BTMGR_ENABLE)
#include "tlkmdi/tlkmdi_btacl.h"
#include "tlkmdi/tlkmdi_btinq.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrComm.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrCtrl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrAcl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrInq.h"

#define TLKMMI_BTMGR_NAME_DEF     "TLK DualMode 2.0"

extern void generateRandomNum(int len, unsigned char *data);

tlkmmi_btmgr_ctrl_t gTlkMmiBtmgrCtrl;


/******************************************************************************
 * Function: tlkmmi_btmgr_ctrlInit
 * Descript: Initial the Bt manager ctrl block, and read local name and 
 *           Bt address. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_ctrlInit(void)
{
	uint16 index;
	uint08 bttemp[16];

	tmemset(&gTlkMmiBtmgrCtrl, 0, sizeof(tlkmmi_btmgr_ctrl_t));

	// Read Local Name
	tlkapi_flash_read(TLK_CFG_FLASH_BT_NAME_ADDR, gTlkMmiBtmgrCtrl.btname, TLKMMI_BTMGR_BTNAME_LENS-1);
	gTlkMmiBtmgrCtrl.btname[TLKMMI_BTMGR_BTNAME_LENS-1] = 0;
	for(index=0; index<31; index++){
		if(gTlkMmiBtmgrCtrl.btname[index] == 0xFF) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_BTMGR_NAME_DEF);
		tmemcpy(gTlkMmiBtmgrCtrl.btname, TLKMMI_BTMGR_NAME_DEF, index);
	}
	gTlkMmiBtmgrCtrl.btname[index] = 0x00;
	// Read Local Address
	tlkapi_flash_read(TLK_CFG_FLASH_BT_ADDR_ADDR, bttemp, 6+2+4+1);
	if(bttemp[0] == 0xFF && bttemp[1] == 0xFF){
	    uint08 randValue[6];
	    tlkapi_random(randValue, 6);
	    bttemp[0] = randValue[5];
	    bttemp[1] = randValue[4];
	    bttemp[2] = randValue[3];
	    bttemp[3] = randValue[2];
	    bttemp[4] = randValue[1];
	    bttemp[5] = randValue[0];
	    tlkapi_flash_write(TLK_CFG_FLASH_BT_ADDR_ADDR, bttemp, 6);
	}
	for(index=0; index<6; index++){
		gTlkMmiBtmgrCtrl.btaddr[index] = bttemp[5-index];
	}
		
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_getBtName
 * Descript: Get BT Name.
 * Params:
 * Return: Return Bt name is success.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_btmgr_getBtName(void)
{
	return gTlkMmiBtmgrCtrl.btname;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_getBtAddr
 * Descript: Get the Bt address. 
 * Params:
 * Return: Return Bt Address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_btmgr_getBtAddr(void)
{
	return gTlkMmiBtmgrCtrl.btaddr;
}


#endif //#if (TLKMMI_BTMGR_ENABLE)

