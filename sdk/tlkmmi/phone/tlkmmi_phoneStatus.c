/********************************************************************************************************
 * @file     tlkmmi_phoneStatus.c
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
#include "tlkprt/tlkprt_comm.h"
#include "tlkmdi/tlkmdi_bthfp.h"
#include "tlkmdi/tlkmdi_audsco.h"
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneComm.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"
#include "tlkmmi/phone/tlkmmi_phoneStatus.h"

#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"



static void tlkmmi_phone_hfCallEvt(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallCloseEvt(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallStartEvt(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallActiveEvt(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallResumeEvt(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallWaitEvt(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallHoldEvt(uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmmi_phone_statusInit
 * Descript: Register the phone call event callback.
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_statusInit(void)
{
	tlkmdi_event_regCB(TLKMDI_EVENT_MAJOR_PHONE, tlkmmi_phone_hfCallEvt);
	
	return TLK_ENONE;
}


static void tlkmmi_phone_hfCallEvt(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_hfCallEvt: %d", minorID);
	if(minorID == TLKMDI_HFPHF_EVTID_CALL_CLOSE){
		tlkmmi_phone_hfCallCloseEvt(pData, dataLen);
	}else if(minorID == TLKMDI_HFPHF_EVTID_CALL_START){
		tlkmmi_phone_hfCallStartEvt(pData, dataLen);
	}else if(minorID == TLKMDI_HFPHF_EVTID_CALL_ACTIVE){
		tlkmmi_phone_hfCallActiveEvt(pData, dataLen);
	}else if(minorID == TLKMDI_HFPHF_EVTID_CALL_RESUME){
		tlkmmi_phone_hfCallResumeEvt(pData, dataLen);
	}else if(minorID == TLKMDI_HFPHF_EVTID_CALL_WAIT){
		tlkmmi_phone_hfCallWaitEvt(pData, dataLen);
	}else if(minorID == TLKMDI_HFPHF_EVTID_CALL_HOLD){
		tlkmmi_phone_hfCallHoldEvt(pData, dataLen);
	}
}
static void tlkmmi_phone_hfCallCloseEvt(uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_hfphf_getCallNumber(pEvt->callNum);
	if(pNumber == nullptr) pEvt->numbLen = 0;
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_CALL_ROLE_CLIENT;
	buffer[buffLen++] = pEvt->callDir;
	buffer[buffLen++] = pEvt->numbLen;
	if(pEvt->numbLen != 0){
		tmemcpy(buffer+buffLen, pNumber, pEvt->numbLen);
		buffLen += pEvt->numbLen;
	}
	buffer[buffLen++] = 0; //NameLen
	tlkmdi_comm_sendCallEvt(TLKPRT_COMM_EVTID_CALL_CLOSE, buffer, buffLen);
	
	tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_IDLE);
}
static void tlkmmi_phone_hfCallStartEvt(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	nameLen = 0;
	pNumber = tlkmdi_hfphf_getCallNumber(pEvt->callNum);
	if(pNumber == nullptr) pEvt->numbLen = 0;
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_CALL_ROLE_CLIENT;
	buffer[buffLen++] = pEvt->callDir;
	buffer[buffLen++] = pEvt->numbLen;
	if(pEvt->numbLen == 0){
		buffer[buffLen++] = 0; //NameLen
	}else{
		tmemcpy(buffer+buffLen, pNumber, pEvt->numbLen);
		buffLen += pEvt->numbLen;
		if(tlkmmi_phone_bookGetName(pNumber, pEvt->numbLen, buffer+buffLen+1, TLKMMI_PHONE_NAME_MAX_LEN, &nameLen) == TLK_ENONE){
			buffer[buffLen++] = nameLen; //NameLen
			buffLen += nameLen;
		}else if((pEvt->numbLen >= 8 && tmemcmp(pNumber, "10000000", 8) == 0) || (pEvt->numbLen >= 11 && tmemcmp(pNumber, "00000000000", 11) == 0)){
			buffer[buffLen++] = 0x08; //NameLen
			buffer[buffLen++] = 0xAE;
			buffer[buffLen++] = 0x5F;
			buffer[buffLen++] = 0xE1;
			buffer[buffLen++] = 0x4F;
			buffer[buffLen++] = 0xED;
			buffer[buffLen++] = 0x8B;
			buffer[buffLen++] = 0xF3;
			buffer[buffLen++] = 0x97;
		}else{
			buffer[buffLen++] = 0; //NameLen
		}
	}
	
	tlkmdi_comm_sendCallEvt(TLKPRT_COMM_EVTID_CALL_START, buffer, buffLen);
	
	if(pEvt->callDir == 1){
		tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_INCOMING);
	}else if(pEvt->callDir == 2){
		tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_OUTGOING);
	}
}
static void tlkmmi_phone_hfCallActiveEvt(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_hfphf_getCallNumber(pEvt->callNum);
	if(pNumber == nullptr) pEvt->numbLen = 0;
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_CALL_ROLE_CLIENT;
	buffer[buffLen++] = pEvt->callDir;
	buffer[buffLen++] = pEvt->numbLen;
	if(pEvt->numbLen == 0){
		buffer[buffLen++] = 0; //NameLen
	}else{
		tmemcpy(buffer+buffLen, pNumber, pEvt->numbLen);
		buffLen += pEvt->numbLen;
		if(tlkmmi_phone_bookGetName(pNumber, pEvt->numbLen, buffer+buffLen+1, TLKMMI_PHONE_NAME_MAX_LEN, &nameLen) == TLK_ENONE){
			buffer[buffLen++] = nameLen; //NameLen
			buffLen += nameLen;
		}else if((pEvt->numbLen >= 8 && tmemcmp(pNumber, "10000000", 8) == 0) || (pEvt->numbLen >= 11 && tmemcmp(pNumber, "00000000000", 11) == 0)){
			buffer[buffLen++] = 0x08; //NameLen
			buffer[buffLen++] = 0xAE;
			buffer[buffLen++] = 0x5F;
			buffer[buffLen++] = 0xE1;
			buffer[buffLen++] = 0x4F;
			buffer[buffLen++] = 0xED;
			buffer[buffLen++] = 0x8B;
			buffer[buffLen++] = 0xF3;
			buffer[buffLen++] = 0x97;
		}else{
			buffer[buffLen++] = 0; //NameLen
		}
	}
	tlkmdi_comm_sendCallEvt(TLKPRT_COMM_EVTID_CALL_ACTIVE, buffer, buffLen);
	
	tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_ACTIVE);
}
static void tlkmmi_phone_hfCallResumeEvt(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_hfphf_getCallNumber(pEvt->callNum);
	if(pNumber == nullptr) pEvt->numbLen = 0;
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_CALL_ROLE_CLIENT;
	buffer[buffLen++] = pEvt->callDir;
	buffer[buffLen++] = pEvt->numbLen;
	if(pEvt->numbLen == 0){
		buffer[buffLen++] = 0; //NameLen
	}else{
		tmemcpy(buffer+buffLen, pNumber, pEvt->numbLen);
		buffLen += pEvt->numbLen;
		if(tlkmmi_phone_bookGetName(pNumber, pEvt->numbLen, buffer+buffLen+1, TLKMMI_PHONE_NAME_MAX_LEN, &nameLen) == TLK_ENONE){
			buffer[buffLen++] = nameLen; //NameLen
			buffLen += nameLen;
		}else if((pEvt->numbLen >= 8 && tmemcmp(pNumber, "10000000", 8) == 0) || (pEvt->numbLen >= 11 && tmemcmp(pNumber, "00000000000", 11) == 0)){
			buffer[buffLen++] = 0x08; //NameLen
			buffer[buffLen++] = 0xAE;
			buffer[buffLen++] = 0x5F;
			buffer[buffLen++] = 0xE1;
			buffer[buffLen++] = 0x4F;
			buffer[buffLen++] = 0xED;
			buffer[buffLen++] = 0x8B;
			buffer[buffLen++] = 0xF3;
			buffer[buffLen++] = 0x97;
		}else{
			buffer[buffLen++] = 0; //NameLen
		}
	}
	tlkmdi_comm_sendCallEvt(TLKPRT_COMM_EVTID_CALL_RESUME, buffer, buffLen);
	
	tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_ACTIVE);
}
static void tlkmmi_phone_hfCallWaitEvt(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_hfphf_getCallNumber(pEvt->callNum);
	if(pNumber == nullptr) pEvt->numbLen = 0;
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_CALL_ROLE_CLIENT;
	buffer[buffLen++] = pEvt->callDir;
	buffer[buffLen++] = pEvt->numbLen;
	if(pEvt->numbLen == 0){
		buffer[buffLen++] = 0; //NameLen
	}else{
		tmemcpy(buffer+buffLen, pNumber, pEvt->numbLen);
		buffLen += pEvt->numbLen;
		if(tlkmmi_phone_bookGetName(pNumber, pEvt->numbLen, buffer+buffLen+1, TLKMMI_PHONE_NAME_MAX_LEN, &nameLen) == TLK_ENONE){
			buffer[buffLen++] = nameLen; //NameLen
			buffLen += nameLen;
		}else if((pEvt->numbLen >= 8 && tmemcmp(pNumber, "10000000", 8) == 0) || (pEvt->numbLen >= 11 && tmemcmp(pNumber, "00000000000", 11) == 0)){
			buffer[buffLen++] = 0x08; //NameLen
			buffer[buffLen++] = 0xAE;
			buffer[buffLen++] = 0x5F;
			buffer[buffLen++] = 0xE1;
			buffer[buffLen++] = 0x4F;
			buffer[buffLen++] = 0xED;
			buffer[buffLen++] = 0x8B;
			buffer[buffLen++] = 0xF3;
			buffer[buffLen++] = 0x97;
		}else{
			buffer[buffLen++] = 0; //NameLen
		}
	}
	tlkmdi_comm_sendCallEvt(TLKPRT_COMM_EVTID_CALL_WAIT, buffer, buffLen);
}
static void tlkmmi_phone_hfCallHoldEvt(uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_hfphf_getCallNumber(pEvt->callNum);
	if(pNumber == nullptr) pEvt->numbLen = 0;
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_CALL_ROLE_CLIENT;
	buffer[buffLen++] = pEvt->callDir;
	buffer[buffLen++] = pEvt->numbLen;
	if(pEvt->numbLen != 0){
		tmemcpy(buffer+buffLen, pNumber, pEvt->numbLen);
		buffLen += pEvt->numbLen;
	}
	buffer[buffLen++] = 0; //NameLen
	tlkmdi_comm_sendCallEvt(TLKPRT_COMM_EVTID_CALL_HELD, buffer, buffLen);
}



#endif //#if (TLKMMI_PHONE_ENABLE)

