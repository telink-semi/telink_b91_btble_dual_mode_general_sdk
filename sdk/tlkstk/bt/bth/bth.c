/********************************************************************************************************
 * @file     bth.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_adapt.h"
#include "tlkstk/bt/bth/bth_l2cap.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth_hcievt.h"
#include "tlkstk/bt/bth/bth.h"
#include "tlkstk/bt/bth/bth_acl.h"


/******************************************************************************
 * Function: BTH Init interface
 * Descript: This interface be used to initial bth layer.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int bth_init(void)
{
	bth_adapt_init();
	bth_hcievt_init();
	bth_event_init();
	bth_handle_init();
	bth_l2cap_init();
	bth_device_init();
	return TLK_ENONE;
}

bool bth_isBusy(void)
{
	return bth_l2cap_isBusy();
}

bool bth_pmIsBusy(void)
{
	uint08 count;
	bth_acl_handle_t *pHandle;

	if(bth_l2cap_isBusy()) return true;
	
	count = bth_handle_getUsedAclCount();
	if(count == 0) return false;
	if(count != 1) return true;
	
	pHandle = bth_handle_getFirstConnAcl();
	if(pHandle != nullptr && pHandle->curRole == BTH_ROLE_SLAVE) return false;
	return true;
}

uint bth_getAclCount(void)
{
	return bth_handle_getUsedAclCount();
}

/******************************************************************************
 * Function: BTH Process interface
 * Descript: This interface be used to process the event and timer evt, seems 
 *           like a schdluer of System.
 * Params: None.
 * Return: None.
*******************************************************************************/
void bth_process(void)
{
	bth_adapt_handler();
}

/******************************************************************************
 * Function: BTH Destroy interface
 * Descript: This interface be used to release the resource which allocate.
 * Params: @aclhandle[IN]--The acl link handle.
 * Return: None.
*******************************************************************************/
void bth_destroy(uint16 aclHandle)
{
	bth_acl_destroy(aclHandle);
	bth_sco_destroyByAclHandle(aclHandle);
	bth_l2cap_aclDisconn(aclHandle);
}

/******************************************************************************
 * Function: bth_isHeadset
 * Descript: This interface be used to verify peer device is headset or 
 *           others device.
 * Params: @aclhandle[IN]--The acl link handle.
 * Return: Return true means peer device type headset, false means others .
*******************************************************************************/
bool bth_isHeadset(uint16 aclHandle)
{
	uint08 devType;
	bth_acl_handle_t *pHandle;

	pHandle = bth_handle_getUsedAcl(aclHandle);
	if(pHandle == nullptr) return false;
	
	devType = bth_devClassToDevType(pHandle->devClass);
	if(devType == BTH_REMOTE_DTYPE_HEADSET) return true;
	return false;
}

/******************************************************************************
 * Function: bth_devClassToDevType
 * Descript: This interface be used to reflect from devClass to device type.
 * Params: @devClass[IN]--The device class.
 * Return: The Device type.
*******************************************************************************/
uint08 bth_devClassToDevType(uint32 devClass)
{
	uint08 dtype;
	uint32 majorDType;

	if(devClass == 0) return BTH_REMOTE_DTYPE_OTHER;

	majorDType = (devClass & BTH_COD_MAJOR_CLASS_MASK);
	if(majorDType == BTH_COD_MAJOR_CLASS_COMPUTER){
		dtype = BTH_REMOTE_DTYPE_COMPUTER; //TLKMDI_BTINQ_DTYPE_PC;
	}else if(majorDType == BTH_COD_MAJOR_CLASS_PHONE){
		dtype = BTH_REMOTE_DTYPE_PHONE; //TLKMDI_BTINQ_DTYPE_PHONE;
	}else if(majorDType == BTH_COD_MAJOR_CLASS_AUDIO){
		dtype = BTH_REMOTE_DTYPE_HEADSET; //TLKMDI_BTINQ_DTYPE_HEADSET;
	}else if(majorDType != BTH_COD_MAJOR_CLASS_MISC || (devClass & BTH_COD_SERVICE_CLASS_AUDIO) == 0
		|| (devClass & BTH_COD_SERVICE_CLASS_RENDER) == 0){
		dtype = BTH_REMOTE_DTYPE_OTHER; //TLKMDI_BTINQ_DTYPE_UNKNOWN;
	}else{
		dtype = BTH_REMOTE_DTYPE_OTHER;
	}
	return dtype;
}


int bth_sendEnterSleepCmd(void)
{
	bth_acl_handle_t *pHandle;
	
	pHandle = bth_handle_getFirstConnAcl();
	if(pHandle == nullptr) return -TLK_ENOOBJECT;
	if(pHandle->curMode == BTH_LM_SNIFF_MODE){
		return TLK_ENONE;
	}
	if(pHandle->curMode == BTH_LM_ACTIVE_MODE){
		if((pHandle->flags & BTH_ACL_FLAG_WAIT_SNIFF_RESULT) == 0){
			bth_hci_sendSniffModeCmd(pHandle->aclHandle, HCI_CFG_SNIFF_MAX_INTERVAL,
					HCI_CFG_SNIFF_MIN_INTERVAL, HCI_CFG_SNIFF_ATTEMPT, HCI_CFG_SNIFF_TIMEOUT);
			pHandle->flags |= BTH_ACL_FLAG_WAIT_SNIFF_RESULT;

		}
	}

	return TLK_ENONE;
}
int bth_sendLeaveSleepCmd(void)
{
	bth_acl_handle_t *pHandle;

	pHandle = bth_handle_getFirstConnAcl();
	if(pHandle == nullptr) return -TLK_ENOOBJECT;
	if(pHandle->curMode == BTH_LM_ACTIVE_MODE){
		return TLK_ENONE;
	}
	if(pHandle->curMode == BTH_LM_SNIFF_MODE){
		if((pHandle->flags & BTH_ACL_FLAG_WAIT_UNSNIFF_RESULT) == 0){
			bth_hci_sendExitSniffModeCmd(pHandle->aclHandle);
			pHandle->flags |= BTH_ACL_FLAG_WAIT_UNSNIFF_RESULT;
		}
	}

	return TLK_ENONE;
}

