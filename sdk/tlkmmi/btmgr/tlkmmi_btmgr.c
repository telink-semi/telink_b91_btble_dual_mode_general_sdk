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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlkmmi_btmgr.h"
#include "tlkmmi_btmgrAdapt.h"
#include "tlkmmi_btmgrCtrl.h"
#include "tlkmmi_btmgrAcl.h"
#include "tlkmmi_btmgrInq.h"
#include "tlkmmi_btmgrRec.h"
#include "tlkmmi_btmgrTask.h"

#include "tlkmdi/bt/tlkmdi_bt.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"


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

	tlkmmi_btmgr_taskInit();
	tlkmmi_btmgr_adaptInit();
	tlkmmi_btmgr_ctrlInit();
	#if (TLKMMI_BTMGR_BTACL_ENABLE)
	tlkmmi_btmgr_aclInit();
	#endif
	#if (TLK_MDI_BTINQ_ENABLE)
	tlkmmi_btmgr_inqInit();
	#endif
	#if (TLK_MDI_BTREC_ENABLE)
	tlkmmi_btmgr_recInit();
	#endif
	
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_BTMGR_DEVICE_CLASS);
    bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode

	pDevice = bth_device_getLast();
	if(pDevice != nullptr){
		#if TLKMMI_BTMGR_BTREC_ENABLE
		tlkmmi_btmgr_recStart(pDevice->devAddr, pDevice->devClass, true, true);
		#endif
	}else{
		#if TLKMMI_BTMGR_BTREC_ENABLE
		tlkmmi_btmgr_recStart(nullptr, 0, false, false);
		#endif
	}
	
	return TLK_ENONE;
}







#endif //#if (TLKMMI_BTMGR_ENABLE)

