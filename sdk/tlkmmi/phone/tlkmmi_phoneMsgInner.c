/********************************************************************************************************
 * @file     tlkmmi_phoneMsgInner.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"
#include "tlkmmi/phone/tlkmmi_phoneMsgInner.h"
#include "tlkmmi/phone/tlkmmi_phoneAdapt.h"

#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "drivers.h"

static void tlkmmi_phone_hfCallCloseEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallStartEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallActiveEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallResumeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallWaitEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_hfCallHoldEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_syncBookCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_closeSyncCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_scoUpdateDeal(uint08 *pData, uint08 dataLen);
static bool tlkmmi_phoneActive_timer(tlkapi_timer_t *pTimer,uint32 userArg);

static bool sTlkPhoneScoIsConn = false;
static uint32 sTlkPhoneActiveTicks;
tlkapi_timer_t sTlkMmiPhoneActiveTimer;


/******************************************************************************
 * Function: tlkmmi_phone_sendCommXxx.
 * Descript: send the phone command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 head[4];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_CALL;
	head[headLen++] = cmdID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}
int tlkmmi_phone_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 head[8];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_CALL;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}
int tlkmmi_phone_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 head[4];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_CALL;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}


int tlkmmi_phone_innerMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_innerMsgHandler: %d", msgID);
	if(msgID == TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT){
		tlkmmi_phone_hfCallCloseEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CALL_START_EVT){
		tlkmmi_phone_hfCallStartEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CALL_ACTIVE_EVT){
		tlkmmi_phone_hfCallActiveEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CALL_RESUME_EVT){
		tlkmmi_phone_hfCallResumeEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CALL_WAIT_EVT){
		tlkmmi_phone_hfCallWaitEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CALL_HOLD_EVT){
		tlkmmi_phone_hfCallHoldEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_SYNC_BOOK_CMD){
		tlkmmi_phone_syncBookCmdDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CANCEL_SYNC_CMD){
		tlkmmi_phone_closeSyncCmdDeal(pData, dataLen);
	}else if(msgID == TLKPTI_PHONE_MSGID_CALL_SCO_UPDATE_EVT){
		tlkmmi_phone_scoUpdateDeal(pData,dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}

static void tlkmmi_phone_hfCallCloseEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_bthfp_getHfCallNumber(pEvt->handle,pEvt->callNum);
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
	tlkmmi_phone_sendCommEvt(TLKPRT_COMM_EVTID_CALL_CLOSE, buffer, buffLen);
		
	tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_IDLE);
}
static void tlkmmi_phone_hfCallStartEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	nameLen = 0;
	pNumber = tlkmdi_bthfp_getHfCallNumber(pEvt->handle, pEvt->callNum);
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
	tlkmmi_phone_sendCommEvt(TLKPRT_COMM_EVTID_CALL_START, buffer, buffLen);
		
	if(pEvt->callDir == 1){
		tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_INCOMING);
	}else if(pEvt->callDir == 2){
		tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_OUTGOING);
	}
}
static void tlkmmi_phone_hfCallActiveEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_bthfp_getHfCallNumber(pEvt->handle, pEvt->callNum);
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
	tlkmmi_phone_sendCommEvt(TLKPRT_COMM_EVTID_CALL_ACTIVE, buffer, buffLen);
		
	tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_ACTIVE);
	if(!bth_sco_isConn(pEvt->handle) && sTlkPhoneScoIsConn == false){
		tlkmmi_phone_adaptInitTimer(&sTlkMmiPhoneActiveTimer, tlkmmi_phoneActive_timer, pEvt->handle, 50000);
		sTlkPhoneActiveTicks = clock_time();
		tlkmmi_phone_adaptInsertTimer(&sTlkMmiPhoneActiveTimer);
	}
}
static bool tlkmmi_phoneActive_timer(tlkapi_timer_t *pTimer,uint32 userArg)
{
	uint16 handle = (uint16)userArg;


	if(sTlkPhoneActiveTicks !=0 && clock_time_exceed(sTlkPhoneActiveTicks, 500000)){
		if( !bth_sco_isConn(handle) ){
			bth_sco_connect(handle, TLK_SCO_LINK_TYPE_ESCO, TLK_SCO_AIRMODE_CVSD);
			sTlkPhoneActiveTicks = clock_time() | 1;
		}else{
			sTlkPhoneActiveTicks = 0;
			return false;
		}
	}
	return true;
}

static void tlkmmi_phone_hfCallResumeEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;
	
	pNumber = tlkmdi_bthfp_getHfCallNumber(pEvt->handle, pEvt->callNum);
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
	tlkmmi_phone_sendCommEvt(TLKPRT_COMM_EVTID_CALL_RESUME, buffer, buffLen);
		
	tlkmmi_phone_setHfCallStatus(pEvt->handle, TLKMMI_PHONE_CALL_STATUS_ACTIVE);
}
static void tlkmmi_phone_hfCallWaitEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 nameLen;
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN+TLKMMI_PHONE_NAME_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_bthfp_getHfCallNumber(pEvt->handle, pEvt->callNum);
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
	tlkmmi_phone_sendCommEvt(TLKPRT_COMM_EVTID_CALL_WAIT, buffer, buffLen);
}
static void tlkmmi_phone_hfCallHoldEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 buffLen;
	uint08 *pNumber;
	uint08 buffer[4+TLKMDI_HFPHF_NUMBER_MAX_LEN];
	tlkmdi_hfphf_statusEvt_t *pEvt;
	
	pEvt = (tlkmdi_hfphf_statusEvt_t*)pData;

	pNumber = tlkmdi_bthfp_getHfCallNumber(pEvt->handle, pEvt->callNum);
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
	tlkmmi_phone_sendCommEvt(TLKPRT_COMM_EVTID_CALL_HELD, buffer, buffLen);
}
static void tlkmmi_phone_syncBookCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint16 handle;
	handle = ((uint16)pData[7] << 8) | pData[6];
	tlkmmi_phone_startSyncBook(handle, pData, pData[8]);
}
static void tlkmmi_phone_closeSyncCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint16 handle;
	handle = ((uint16)pData[7] << 8) | pData[6];
	tlkmmi_phone_closeSyncBook(handle);
}

static void tlkmmi_phone_scoUpdateDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_array(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoUpdateDeal:",pData,dataLen);
	if(dataLen < 3){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoUpdateDeal - failure:invalid param");
		return;
	}
	uint08 index;
	uint16 oldHandle = 0,newHandle = 0;
	oldHandle |= pData[0];
	oldHandle |= (pData[1] << 8);

	if(pData[2] == 1){
		sTlkPhoneScoIsConn = true;
		if(tlkmmi_phone_adaptIsHaveTimer(&sTlkMmiPhoneActiveTimer)){
			tlkmmi_phone_adaptRemoveTimer(&sTlkMmiPhoneActiveTimer);
		}
		return;
	}
	for(index = 0;index < TLKMDI_HFPHF_MAX_NUMBER;index++){
		newHandle = tlkmdi_bthfp_getHandle(index);
		if(newHandle != 0 && newHandle != oldHandle && !bth_sco_isConn(oldHandle)){
			bth_sco_connect(newHandle, TLK_SCO_LINK_TYPE_ESCO, TLK_SCO_AIRMODE_CVSD);
		}
	}
}

#endif //#if (TLKMMI_PHONE_ENABLE)

