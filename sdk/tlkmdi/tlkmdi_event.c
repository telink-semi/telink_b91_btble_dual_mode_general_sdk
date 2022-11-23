/********************************************************************************************************
 * @file     tlkmdi_event.c
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
#include "tlkmdi/tlkmdi_event.h"


#define TLKMDI_EVT_DBG_FLAG       ((TLK_MINOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_EVT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_EVT_DBG_SIGN       "[MDI]"

extern unsigned int core_enter_critical(unsigned char preempt_en ,unsigned char threshold);
extern void core_leave_critical(unsigned char preempt_en ,unsigned int r);

static int tlkmdi_event_push(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen);
static int tlkmdi_event_pop(uint08 *pMajorID, uint08 *pMinorID, uint08 *pBuff, uint08 buffLen, uint08 *pLength);


#define TLKMDI_EVENT_BUFFER_SIZE     (TLKMDI_EVENT_UNIT_NUMB*TLKMDI_EVENT_UNIT_SIZE)
uint08 sTlkMdiEventBuffer[TLKMDI_EVENT_BUFFER_SIZE];
tlkapi_qfifo_t sTlkMdiEventQFifo;

static TlkMdiEventCallback sTlkMdiEventCB[TLKMDI_EVENT_MAJOR_MAX];


/******************************************************************************
 * Function: tlkmdi_event_init.
 * Descript: Trigger to Initial the Event Control block an qfifo.
 * Params: None.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_event_init(void)
{
	tmemset(sTlkMdiEventCB, 0, TLKMDI_EVENT_MAJOR_MAX*sizeof(TlkMdiEventCallback));
	return tlkapi_qfifo_init(&sTlkMdiEventQFifo, TLKMDI_EVENT_UNIT_NUMB, TLKMDI_EVENT_UNIT_SIZE, sTlkMdiEventBuffer, TLKMDI_EVENT_BUFFER_SIZE);
}

/******************************************************************************
 * Function: tlkmdi_event_regCB.
 * Descript: Register each Event's callback.
 * Params:
 *        @type[IN]--The event type.
 *        @cb[IN]--The event callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_event_regCB(TLKMDI_EVENT_MAJOR_ENUM type, TlkMdiEventCallback cb)
{
	if(type >= TLKMDI_EVENT_MAJOR_MAX) return;
	sTlkMdiEventCB[type] = cb;
}

/******************************************************************************
 * Function: tlkmdi_clear_event.
 * Descript: Clear the fifo.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_clear_event(void)
{
	tlkapi_qfifo_clear(&sTlkMdiEventQFifo);
}

/******************************************************************************
 * Function: tlkmdi_event_count.
 * Descript: Get the count of fifo which be filled.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
uint tlkmdi_event_count(void)
{
	return tlkapi_qfifo_usedNum(&sTlkMdiEventQFifo);
}

/******************************************************************************
 * Function: tlkmdi_event_handler.
 * Descript: handle the dequeue's event.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_event_handler(void)
{
	uint08 majorID;
	uint08 minorID;
	uint08 buffLen;
	uint08 buffer[TLKMDI_EVENT_UNIT_SIZE];

	if(tlkapi_qfifo_usedNum(&sTlkMdiEventQFifo) == 0) return;
	while(tlkmdi_event_pop(&majorID, &minorID, buffer, TLKMDI_EVENT_UNIT_SIZE, &buffLen) == TLK_ENONE){
		if(majorID >= TLKMDI_EVENT_MAJOR_MAX){
			tlkapi_error(TLKMDI_EVT_DBG_FLAG, TLKMDI_EVT_DBG_SIGN, "tlkmdi_event_handler: fault event %d", majorID);
			continue;
		}
		if(sTlkMdiEventCB[majorID] != nullptr){
			sTlkMdiEventCB[majorID](majorID, minorID, buffer, buffLen);
		}else if(sTlkMdiEventCB[0] != nullptr){
			sTlkMdiEventCB[0](majorID, minorID, buffer, buffLen);
		}
	}
}

/******************************************************************************
 * Function: tlkmdi_sendAudioEvent.
 * Descript: Send the audio event.
 * Params: None.
 * Return: Return TLK_ENONE is success, others failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_sendAudioEvent(uint08 minorID, void *pData, uint08 dataLen, bool isRealDeal)
{
	if(!isRealDeal){
		return tlkmdi_event_push(TLKMDI_EVENT_MAJOR_AUDIO, minorID, (uint08*)pData, dataLen);
	}else{
		if(sTlkMdiEventCB[TLKMDI_EVENT_MAJOR_AUDIO] != nullptr){
			sTlkMdiEventCB[TLKMDI_EVENT_MAJOR_AUDIO](TLKMDI_EVENT_MAJOR_AUDIO, minorID, pData, dataLen);
		}
		return TLK_ENONE;
	}
}

/******************************************************************************
 * Function: tlkmdi_sendPhoneEvent.
 * Descript: Send the phone event.
 * Params: None.
 * Return: Return TLK_ENONE is success, others failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_sendPhoneEvent(uint08 minorID, void *pData, uint08 dataLen, bool isRealDeal)
{
	if(!isRealDeal){
		return tlkmdi_event_push(TLKMDI_EVENT_MAJOR_PHONE, minorID, (uint08*)pData, dataLen);
	}else{
		if(sTlkMdiEventCB[TLKMDI_EVENT_MAJOR_PHONE] != nullptr){
			sTlkMdiEventCB[TLKMDI_EVENT_MAJOR_PHONE](TLKMDI_EVENT_MAJOR_PHONE, minorID, pData, dataLen);
		}
		return TLK_ENONE;
	}
}


static int tlkmdi_event_push(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 *pBuff;

	if(dataLen+3 > TLKMDI_EVENT_UNIT_SIZE){
		tlkapi_error(TLKMDI_EVT_DBG_FLAG, TLKMDI_EVT_DBG_SIGN, "tlkmdi_event_push: failure - msg too long  %d", dataLen);
		return -TLK_EPARAM;
	}
	unsigned int r=core_enter_critical(1,1);
	pBuff = tlkapi_qfifo_getBuff(&sTlkMdiEventQFifo);
	if(pBuff == nullptr){
		tlkapi_error(TLKMDI_EVT_DBG_FLAG, TLKMDI_EVT_DBG_SIGN, "tlkmdi_event_push: failure - no quota");
		ret = -TLK_EQUOTA;
	}else{
//		tlkapi_error(TLKMDI_EVT_DBG_FLAG, TLKMDI_EVT_DBG_SIGN, "tlkmdi_event_push: majorID-%d minorID-%d", majorID, minorID);
		pBuff[0] = majorID;
		pBuff[1] = minorID;
		pBuff[2] = dataLen;
		tmemcpy(pBuff+3, pData, dataLen);
		tlkapi_qfifo_dropBuff(&sTlkMdiEventQFifo);
		ret = TLK_ENONE;
	}
	core_leave_critical(1,r);
	return ret;
}
static int tlkmdi_event_pop(uint08 *pMajorID, uint08 *pMinorID, uint08 *pBuff, uint08 buffLen, uint08 *pLength)
{
	int ret;
	uint08 *pData;

	if(pLength != nullptr) *pLength = 0;
	if(pMajorID == nullptr || pMinorID == nullptr){
		tlkapi_error(TLKMDI_EVT_DBG_FLAG, TLKMDI_EVT_DBG_SIGN, "tlkmdi_event_pop: failure - error param");
		return -TLK_EPARAM;
	}

	unsigned int r=core_enter_critical(1,1);
	pData = tlkapi_qfifo_getData(&sTlkMdiEventQFifo);
	if(pData == nullptr){
		ret = -TLK_EEMPTY;
	}else{
		if(buffLen > pData[2]) buffLen = pData[2];
		if(buffLen+3 > TLKMDI_EVENT_UNIT_SIZE) buffLen = TLKMDI_EVENT_UNIT_SIZE-3;
		*pMajorID = pData[0];
		*pMinorID = pData[1];
		if(buffLen != 0) tmemcpy(pBuff, pData+3, buffLen);
		if(pLength != nullptr) *pLength = buffLen;
		tlkapi_qfifo_dropData(&sTlkMdiEventQFifo);
		ret = TLK_ENONE;
	}
	core_leave_critical(1,r);
	return ret;
}


