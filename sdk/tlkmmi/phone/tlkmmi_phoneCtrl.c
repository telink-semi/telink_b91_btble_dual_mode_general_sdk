/********************************************************************************************************
 * @file     tlkmmi_phoneCtrl.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneComm.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"
#include "tlkmmi/phone/tlkmmi_phoneStatus.h"
#if (TLKMMI_PHONE_SCO_MANAGE)
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "tlkmmi/tlkmmi_adapt.h"
#include "drivers.h"
#endif

#if (TLKMMI_PHONE_SCO_MANAGE)
static void tlkmmi_phone_scoConnCB(uint16 aclHandle, uint16 scoHandle, bool isConn);
static bool tlkmmi_phone_timer(tlkapi_timer_t *pTimer, void *pUsrArg);
#endif


#if (TLKMMI_PHONE_SCO_MANAGE)
static tlkmmi_phone_ctrl_t sTlkMmiPhoneCtrl;
#endif


/******************************************************************************
 * Function: tlkmmi_phone_ctrlInit
 * Descript: 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_ctrlInit(void)
{
	#if (TLKMMI_PHONE_SCO_MANAGE)
	tmemset(&sTlkMmiPhoneCtrl, 0, sizeof(tlkmmi_phone_ctrl_t));
	tlkmmi_adapt_initTimer(&sTlkMmiPhoneCtrl.timer, tlkmmi_phone_timer, &sTlkMmiPhoneCtrl, TLKMMI_PHONE_TIMEOUT);
	tlkmdi_audsco_regCB(tlkmmi_phone_scoConnCB);
	#endif
		
	return TLK_ENONE;
}

#if (TLKMMI_PHONE_SCO_MANAGE)
void tlkmmi_phone_resetHfCtrl(void)
{
	sTlkMmiPhoneCtrl.aclHandle = 0;
	sTlkMmiPhoneCtrl.scoHandle = 0;
	
	sTlkMmiPhoneCtrl.optTimer = 0;
	sTlkMmiPhoneCtrl.hfOpcode = 0;
	tlkmmi_adapt_removeTimer(&sTlkMmiPhoneCtrl.timer);
}
#endif
void tlkmmi_phone_setHfCallStatus(uint16 aclHandle, uint08 hfStatus)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus");
	#if (TLKMMI_PHONE_SCO_MANAGE)
	if(sTlkMmiPhoneCtrl.scoHandle != 0 && sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_IDLE){
		if(sTlkMmiPhoneCtrl.aclHandle != aclHandle){
			tlkmmi_phone_resetHfCtrl();
		}
	}
	if(sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.aclHandle != aclHandle){
		if(sTlkMmiPhoneCtrl.hfStatus != TLKMMI_PHONE_CALL_STATUS_IDLE){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: Conflicting Call Status");
			return;
		}
		sTlkMmiPhoneCtrl.scoHandle = 0;
	}
	if(sTlkMmiPhoneCtrl.hfStatus == hfStatus){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: Repeat Status");
		return;
	}
	
	if(hfStatus == TLKMMI_PHONE_CALL_STATUS_INCOMING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_INCOMING");
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.hfStatus  = hfStatus;
	}else if(hfStatus == TLKMMI_PHONE_CALL_STATUS_OUTGOING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_OUTGOING");
		if(sTlkMmiPhoneCtrl.scoHandle != 0 && sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_DIAL){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 0: manual disconn SCO");
			bth_sco_disconn(sTlkMmiPhoneCtrl.scoHandle, 0x13);
			tlkmmi_phone_resetHfCtrl();
		}else{
			sTlkMmiPhoneCtrl.aclHandle = aclHandle;
			sTlkMmiPhoneCtrl.hfStatus  = hfStatus;
		}
	}else if(hfStatus == TLKMMI_PHONE_CALL_STATUS_ACTIVE){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_ACTIVE");
		if(sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.scoHandle != 0){
			if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_ANSWER){
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 1: manual disconn SCO");
				bth_sco_disconn(sTlkMmiPhoneCtrl.scoHandle, 0x13);
			}
		}
		tlkmmi_phone_resetHfCtrl();
	}else{
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_IDLE");
		tlkmmi_phone_resetHfCtrl();
	}
	#endif
}
void tlkmmi_phone_setHfManualCode(uint16 aclHandle, uint08 hfOpcode)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfManualCode");
	#if (TLKMMI_PHONE_SCO_MANAGE)
	if(sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.aclHandle != aclHandle){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfManualCode: Conflicting Manual Code");
		return;
	}
	if(sTlkMmiPhoneCtrl.hfOpcode == hfOpcode){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfManualCode: Repeat Opcode");
		return;
	}
		
	if(hfOpcode == TLKMMI_PHONE_MANUAL_CODE_DIAL){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_MANUAL_CODE_DIAL");
		if(sTlkMmiPhoneCtrl.hfStatus != TLKMMI_PHONE_CALL_STATUS_IDLE){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 0: Invalid Operate");
		}else{
			sTlkMmiPhoneCtrl.aclHandle = aclHandle;
			sTlkMmiPhoneCtrl.optTimer  = clock_time()|1;
			sTlkMmiPhoneCtrl.hfOpcode  = hfOpcode;
		}
	}else if(hfOpcode == TLKMMI_PHONE_MANUAL_CODE_ANSWER){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_MANUAL_CODE_ANSWER");
		if(sTlkMmiPhoneCtrl.aclHandle != aclHandle || sTlkMmiPhoneCtrl.hfStatus != TLKMMI_PHONE_CALL_STATUS_INCOMING){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 1: Invalid Operate");
		}else{
			sTlkMmiPhoneCtrl.aclHandle = aclHandle;
			sTlkMmiPhoneCtrl.optTimer = clock_time()|1;
			sTlkMmiPhoneCtrl.hfOpcode = hfOpcode;
		}
	}else if(hfOpcode == TLKMMI_PHONE_MANUAL_CODE_HUNGUP){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_MANUAL_CODE_HUNGUP");
		if(sTlkMmiPhoneCtrl.aclHandle != aclHandle || sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_IDLE){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 2: Invalid Operate");
		}else{
			tlkmmi_phone_resetHfCtrl();
		}
	}else{
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: Error Opcode");
		return;
	}
//	if(sTlkMmiPhoneCtrl.optTimer != 0){
//		tlkmmi_adapt_insertTimer(&sTlkMmiPhoneCtrl.timer);
//	}
	#endif
}


#if (TLKMMI_PHONE_SCO_MANAGE)
static void tlkmmi_phone_scoConnCB(uint16 aclHandle, uint16 scoHandle, bool isConn)
{
	tlkapi_debug_delayForPrint(30000);

	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB");
	if((sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.aclHandle != aclHandle)
		|| (sTlkMmiPhoneCtrl.scoHandle != 0 && sTlkMmiPhoneCtrl.scoHandle != scoHandle)){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: Conflicting SCO connections");
		return;
	}
	if(!isConn){
		tlkmmi_phone_resetHfCtrl();
		return;
	}
	
	if(sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_INCOMING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_MANUAL_CODE_ANSWER");
		if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_NONE){
			sTlkMmiPhoneCtrl.hfOpcode = TLKMMI_PHONE_MANUAL_CODE_NONE;
			sTlkMmiPhoneCtrl.optTimer = 0;
		}
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.scoHandle = scoHandle;
	}else if(sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_OUTGOING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_CALL_STATUS_OUTGOING");
		if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_DIAL){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB 0: manual disconn SCO");
			bth_sco_disconn(scoHandle, 0x13);
		}
		tlkmmi_phone_resetHfCtrl();
	}else if(sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_ACTIVE){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_CALL_STATUS_ACTIVE");
		if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_ANSWER){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB 1: manual disconn SCO");
			bth_sco_disconn(scoHandle, 0x13);
		}
		tlkmmi_phone_resetHfCtrl();
	}else{
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_CALL_STATUS_IDLE");
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.scoHandle = scoHandle;
	}
}
#endif

#if (TLKMMI_PHONE_SCO_MANAGE)
static bool tlkmmi_phone_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	
	return false;
}
#endif



#endif //#if (TLKMMI_PHONE_ENABLE)

