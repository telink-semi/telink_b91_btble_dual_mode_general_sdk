/********************************************************************************************************
 * @file     tlkmmi_btmgr.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_BTMGR_ENABLE)
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrComm.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrCtrl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrAcl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrInq.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrRec.h"


#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"

/******************************************************************************
 * Function: bt_ll_set_bd_addr
 * Descript: Set Bt address.
 * Params:
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
extern int bt_ll_set_bd_addr(uint8_t *bdAddr);

/******************************************************************************
 * Function: bt_ll_set_local_name
 * Descript: Set Bt local name.
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
extern void bt_ll_set_local_name(char *name);

/******************************************************************************
 * Function: tlkmmi_btmgr_init
 * Descript: Handle bt Manager initial including register callback, and reset 
 *           control block and read the bt name and bt address and initial 
 *           the acl resource.
 * Params:
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_init(void)
{
	bth_device_item_t *pDevice;

	tlkmmi_btmgr_commInit();
	tlkmmi_btmgr_ctrlInit();
	#if (TLKMMI_BTACL_ENABLE)
	tlkmmi_btmgr_aclInit();
	#endif
	#if (TLK_MDI_BTINQ_ENABLE)
	tlkmmi_btmgr_inqInit();
	#endif
	#if (TLK_MDI_BTREC_ENABLE)
	tlkmmi_btmgr_recInit();
	#endif

	bt_ll_set_bd_addr(tlkmmi_btmgr_getBtAddr());


    bth_hci_sendWriteLocalNameCmd((uint08 *)tlkmmi_btmgr_getBtName());
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_BTMGR_DEVICE_CLASS);
//	bth_hci_sendWriteScanEnableCmd(0);//(INQUIRY_SCAN_ENABLE | PAGE_SCAN_ENABLE);
    bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode

	pDevice = bth_device_getLast();
	if(pDevice != nullptr){
		tlkmmi_btmgr_recStart(pDevice->devAddr, pDevice->devClass, true, true);
	}else{
		tlkmmi_btmgr_recStart(nullptr, 0, false, false);
	}
	
	return TLK_ENONE;
}









#endif //#if (TLKMMI_BTMGR_ENABLE)

