/********************************************************************************************************
 * @file     tlkmdi_comm.c
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
#if (TLK_CFG_COMM_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"


#define TLKMDI_COMM_DBG_FLAG       ((TLK_MINOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_COMM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_COMM_DBG_SIGN       "[MDI]"



static void tlkmdi_comm_makeRecvFrame(uint08 rbyte);
static int  tlkmdi_comm_makeSendFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);


typedef struct{
	uint08 sendNumb;
	uint08 recvNumb;
	uint08 makeState;
	uint16 makeLength;
	uint16 recvFrameLen;
	uint16 sendFrameLen;
	uint08 recvFrame[TLKPRT_COMM_FRM_MAXLEN];
	uint08 sendFrame[TLKPRT_COMM_FRM_MAXLEN];
	uint08 taskList[TLKPRT_COMM_MTYPE_MAX];
}tlkmdi_comm_ctrl_t;
static tlkmdi_comm_ctrl_t sTlkMdiCommCtrl;
static tlkmdi_comm_datCB sTlkMdiCommDatCB[TLKMDI_COMM_DATA_CHANNEL_MAX] = {0};

/******************************************************************************
 * Function: tlkmdi_comm_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: None.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_init(void)
{
	tmemset(&sTlkMdiCommCtrl, 0, sizeof(tlkmdi_comm_ctrl_t));
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_regCB(tlkmdi_comm_input);
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_comm_clear(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_clear();
	#endif
}

void tlkmdi_comm_handler(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_handler();
	#endif
}

bool tlkmdi_comm_pmIsBusy(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	return tlkdev_serial_isBusy();
	#else
	return false;
	#endif
}

int tlkmdi_comm_getValidDatID(uint08 *pDatID)
{
	uint08 index;
	for(index=0; index<TLKMDI_COMM_DATA_CHANNEL_MAX; index++){
		if(sTlkMdiCommDatCB[index] == 0) break;
	}
	if(index == TLKMDI_COMM_DATA_CHANNEL_MAX) return -TLK_EQUOTA;
	if(pDatID != nullptr) *pDatID = index+1;
	return TLK_ENONE;
}

void tlkmdi_comm_incSendNumb(void)
{
	sTlkMdiCommCtrl.sendNumb ++;
}
void tlkmdi_comm_setSendNumb(uint08 value)
{
	sTlkMdiCommCtrl.sendNumb = value;
}
void tlkmdi_comm_getSendNumb(uint08 *pValue)
{
	if(pValue != nullptr) *pValue = sTlkMdiCommCtrl.sendNumb;
}

/******************************************************************************
 * Function: tlkmdi_comm_regCmdCB.
 * Descript: Registers a task for the message type in the system to distribute
 *     the received message to the specified task.
 * Params:
 *     @mtype[IN]--The Type of message.
 *     @taskID[IN]--The ID of the task.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID)
{
	if(mtype == 0 || mtype >= TLKPRT_COMM_MTYPE_MAX) return -TLK_EPARAM;
	sTlkMdiCommCtrl.taskList[mtype] = taskID;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_regDatCB.
 * Descript: Register the data callback.
 * Params:
 *        @datID[IN]--The data channel Id.
 *        @datCB[IN]--The data callback.
 *        @isForce[IN]--is force to process.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_regDatCB(uint08 datID, tlkmdi_comm_datCB datCB, bool isForce)
{
	if(datID == 0) return -TLK_EPARAM;
	if(datID > TLKMDI_COMM_DATA_CHANNEL_MAX) return -TLK_EQUOTA;
	if(!isForce && datCB != nullptr && sTlkMdiCommDatCB[datID-1] != nullptr){
		return -TLK_EREPEAT;
	}
	sTlkMdiCommDatCB[datID-1] = datCB;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_sendXXX.
 * Descript: send the command or Response or Event or data.
 * Params:
 *        @mType[IN]--The message type.
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendCmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[4];
	head[0] = mType; //Mtype
	head[1] = cmdID; //MsgID
	head[2] = sTlkMdiCommCtrl.sendNumb; //Numb
	head[3] = dataLen; //Lens
	ret = tlkmdi_comm_makeSendFrame(TLKPRT_COMM_PTYPE_CMD, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		#if (TLK_DEV_SERIAL_ENABLE)
		ret = tlkdev_serial_send(sTlkMdiCommCtrl.sendFrame, sTlkMdiCommCtrl.sendFrameLen);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
	}
	if(ret == TLK_ENONE) sTlkMdiCommCtrl.sendNumb++;
	return ret;
}
int tlkmdi_comm_sendRsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[6];
	head[0] = mType; //Mtype
	head[1] = cmdID; //MsgID
	head[2] = sTlkMdiCommCtrl.sendNumb; //Numb
	head[3] = 2+dataLen; //Lens
	head[4] = status;
	head[5] = reason;
	ret = tlkmdi_comm_makeSendFrame(TLKPRT_COMM_PTYPE_RSP, head, 6, pData, dataLen);
	if(ret == TLK_ENONE){
		#if (TLK_DEV_SERIAL_ENABLE)
		ret = tlkdev_serial_send(sTlkMdiCommCtrl.sendFrame, sTlkMdiCommCtrl.sendFrameLen);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
	}
	if(ret == TLK_ENONE) sTlkMdiCommCtrl.sendNumb++;
	return ret;
}
int tlkmdi_comm_sendEvt(uint08 mType, uint08 evtID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[4];
	head[0] = mType; //Mtype
	head[1] = evtID; //MsgID
	head[2] = sTlkMdiCommCtrl.sendNumb; //Numb
	head[3] = dataLen; //Lens
	ret = tlkmdi_comm_makeSendFrame(TLKPRT_COMM_PTYPE_EVT, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		#if (TLK_DEV_SERIAL_ENABLE)
		ret = tlkdev_serial_send(sTlkMdiCommCtrl.sendFrame, sTlkMdiCommCtrl.sendFrameLen);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
	}
	if(ret == TLK_ENONE) sTlkMdiCommCtrl.sendNumb++;
	return ret;
}

int tlkmdi_comm_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint08 head[4];
	
	if(pData == nullptr || dataLen == 0 || dataLen > 256) return -TLK_EPARAM;
	head[0] = (numb & 0x00FF); //Num
	head[1] = (numb & 0xFF00) >> 8;
	head[2] = datID; //DID
	head[3] = dataLen-1; //Lens
	ret = tlkmdi_comm_makeSendFrame(TLKPRT_COMM_PTYPE_DAT, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		#if (TLK_DEV_SERIAL_ENABLE)
		ret = tlkdev_serial_send(sTlkMdiCommCtrl.sendFrame, sTlkMdiCommCtrl.sendFrameLen);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
	}
	return ret;
}

int tlkmdi_comm_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	int ret;
	ret = tlkmdi_comm_makeSendFrame(pktType, pHead, headLen, pBody, bodyLen);
	if(ret == TLK_ENONE){
		#if (TLK_DEV_SERIAL_ENABLE)
		ret = tlkdev_serial_send(sTlkMdiCommCtrl.sendFrame, sTlkMdiCommCtrl.sendFrameLen);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
	}
	return ret;
}


void tlkmdi_comm_input(uint08 *pData, uint16 dataLen)
{
	uint08 mtype;
	uint08 ptype;
	uint16 index;
	uint16 rawCrc;
	uint16 calCrc;
	
	for(index=0; index<dataLen; index++){
		tlkmdi_comm_makeRecvFrame(pData[index]);
		if(sTlkMdiCommCtrl.makeState != TLKMDI_COMM_MSTATE_READY) continue;
		rawCrc = ((uint16)(sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen-3])<<8)
			| (sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen-4]);
		calCrc = tlkalg_crc16_calc(sTlkMdiCommCtrl.recvFrame+2, sTlkMdiCommCtrl.recvFrameLen-6);
		if(rawCrc != calCrc){
			tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Serial CRC Error: rawCrc[%x] - calCrc[%x]", rawCrc, calCrc);
			continue;
		}
		ptype = (sTlkMdiCommCtrl.recvFrame[3] & 0xF0) >> 4;
		
		if(ptype == TLKPRT_COMM_PTYPE_CMD){
			uint08 numb;
			uint08 lens;
			uint08 msgID;
			mtype = sTlkMdiCommCtrl.recvFrame[4];
			msgID = sTlkMdiCommCtrl.recvFrame[5];
			numb  = sTlkMdiCommCtrl.recvFrame[6];
			lens  = sTlkMdiCommCtrl.recvFrame[7];
			if(sTlkMdiCommCtrl.recvFrameLen < 4+TLKPRT_COMM_FRM_EXTLEN || sTlkMdiCommCtrl.recvFrameLen < 4+lens+TLKPRT_COMM_FRM_EXTLEN){
				tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv CmdOrRspOrEvtPkt Length Error: frmLen-%d lens-%d", sTlkMdiCommCtrl.recvFrameLen, lens);
				return;
			}
			if(mtype >= TLKPRT_COMM_MTYPE_MAX){
				tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv Error MsgType: mtype-%d", mtype);
				return;
			}
			if((uint08)(sTlkMdiCommCtrl.recvNumb+1) != numb){
				tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Stall Packet: %d %d", sTlkMdiCommCtrl.recvNumb, numb);
			}
			sTlkMdiCommCtrl.recvNumb = numb;
			if(mtype != 0 && mtype < TLKPRT_COMM_MTYPE_MAX && sTlkMdiCommCtrl.taskList[mtype] != 0){
				tlktsk_sendOuterMsg(sTlkMdiCommCtrl.taskList[mtype], mtype, msgID, sTlkMdiCommCtrl.recvFrame+8, lens);
			}
		}
		else if(ptype == TLKPRT_COMM_PTYPE_DAT){
			uint16 numb;
			uint16 lens;
			uint08 datID;
			datID = sTlkMdiCommCtrl.recvFrame[4];
			numb  = (((uint16)sTlkMdiCommCtrl.recvFrame[6]) << 8) | sTlkMdiCommCtrl.recvFrame[5];
			lens  = (((uint16)sTlkMdiCommCtrl.recvFrame[8]) << 8) | sTlkMdiCommCtrl.recvFrame[7];
			if(sTlkMdiCommCtrl.recvFrameLen < 4+TLKPRT_COMM_FRM_EXTLEN || sTlkMdiCommCtrl.recvFrameLen < 4+lens+TLKPRT_COMM_FRM_EXTLEN){
				tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv DatPkt Length Error: frmLen-%d lens-%d", sTlkMdiCommCtrl.recvFrameLen, lens);
				return;
			}
			if(datID != 0 && datID <= TLKMDI_COMM_DATA_CHANNEL_MAX && sTlkMdiCommDatCB[datID-1] != nullptr){
				sTlkMdiCommDatCB[datID-1](datID, numb, sTlkMdiCommCtrl.recvFrame+9, lens);
			}else{
				tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv DatPkt Unexpect: not used - datID[%d]", datID);
			}
		}
		else{
			tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv Error PktType: ptype-%d", ptype);
		}
		sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_HEAD;
		sTlkMdiCommCtrl.makeLength = 0;
	}	
}

static void tlkmdi_comm_makeRecvFrame(uint08 rbyte)
{
	if(sTlkMdiCommCtrl.makeState == TLKMDI_COMM_MSTATE_HEAD){
		if(sTlkMdiCommCtrl.makeLength == 0 && rbyte == TLKPRT_COMM_HEAD_SIGN0){
			sTlkMdiCommCtrl.makeLength ++;
		}else if(sTlkMdiCommCtrl.makeLength == 1 && rbyte == TLKPRT_COMM_HEAD_SIGN1){
			sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_ATTR;
			sTlkMdiCommCtrl.makeLength = 0;
			sTlkMdiCommCtrl.recvFrameLen = 0;
			sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = TLKPRT_COMM_HEAD_SIGN0;
			sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = TLKPRT_COMM_HEAD_SIGN1;
		}else{
			sTlkMdiCommCtrl.makeLength = 0;
		}
	}else if(sTlkMdiCommCtrl.makeState == TLKMDI_COMM_MSTATE_ATTR){
		sTlkMdiCommCtrl.makeLength ++;
		sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = rbyte;
		if(sTlkMdiCommCtrl.makeLength < 2) return;
		sTlkMdiCommCtrl.makeLength = (((uint16)(sTlkMdiCommCtrl.recvFrame[3] & 0x0F))<<8)|sTlkMdiCommCtrl.recvFrame[2];
		if(sTlkMdiCommCtrl.makeLength+4 > TLKPRT_COMM_FRM_MAXLEN || sTlkMdiCommCtrl.makeLength < 4){
			sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_HEAD;
			sTlkMdiCommCtrl.makeLength = 0;
		}else if(sTlkMdiCommCtrl.makeLength == 4){
			sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_CHECK;
			sTlkMdiCommCtrl.makeLength = 2;
		}else{
			sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_BODY;
			sTlkMdiCommCtrl.makeLength -= 4;
		}
	}else if(sTlkMdiCommCtrl.makeState == TLKMDI_COMM_MSTATE_BODY){
		sTlkMdiCommCtrl.makeLength --;
		sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = rbyte;
		if(sTlkMdiCommCtrl.makeLength != 0) return;
		sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_CHECK;
		sTlkMdiCommCtrl.makeLength = 0;
	}else if(sTlkMdiCommCtrl.makeState == TLKMDI_COMM_MSTATE_CHECK){
		sTlkMdiCommCtrl.makeLength ++;
		sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = rbyte;
		if(sTlkMdiCommCtrl.makeLength < 2) return;
		sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_TAIL;
		sTlkMdiCommCtrl.makeLength = 0;
	}else if(sTlkMdiCommCtrl.makeState == TLKMDI_COMM_MSTATE_TAIL){
		if(sTlkMdiCommCtrl.makeLength == 0 && rbyte == TLKPRT_COMM_TAIL_SIGN0){
			sTlkMdiCommCtrl.makeLength ++;
		}else if(sTlkMdiCommCtrl.makeLength == 1 && rbyte == TLKPRT_COMM_TAIL_SIGN1){
			sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = TLKPRT_COMM_TAIL_SIGN0;
			sTlkMdiCommCtrl.recvFrame[sTlkMdiCommCtrl.recvFrameLen++] = TLKPRT_COMM_TAIL_SIGN1;
			sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_READY;
			sTlkMdiCommCtrl.makeLength = 0;
		}else{
			sTlkMdiCommCtrl.makeState = TLKMDI_COMM_MSTATE_HEAD;
			sTlkMdiCommCtrl.makeLength = 0;
		}
	}
}
static int tlkmdi_comm_makeSendFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	uint16 tempVar;
	
//	my_dump_str_data(1, "send 01:", &sTlkDevSerialSendIsBusy, 1);
	if(headLen == 0 && pHead != nullptr) pHead = nullptr;
	if(bodyLen == 0 && pBody != nullptr) pBody = nullptr;
	if((headLen+bodyLen) == 0) return -TLK_EPARAM;
	if(headLen+bodyLen+TLKPRT_COMM_FRM_EXTLEN > TLKPRT_COMM_FRM_MAXLEN){
		return -TLK_ELENGTH;
	}
	
	sTlkMdiCommCtrl.sendFrameLen = 0;
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = TLKPRT_COMM_HEAD_SIGN0;
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = TLKPRT_COMM_HEAD_SIGN1;
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = 0x00; //Length[0~7]
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = (pktType & 0x0F) << 4; //Length[8~11]+pktType[0~4]
	if(headLen != 0){
		tmemcpy(sTlkMdiCommCtrl.sendFrame+sTlkMdiCommCtrl.sendFrameLen, pHead, headLen);
		sTlkMdiCommCtrl.sendFrameLen += headLen;
	}
	if(bodyLen != 0){
		tmemcpy(sTlkMdiCommCtrl.sendFrame+sTlkMdiCommCtrl.sendFrameLen, pBody, bodyLen);
		sTlkMdiCommCtrl.sendFrameLen += bodyLen;
	}
	sTlkMdiCommCtrl.sendFrame[2] |= (sTlkMdiCommCtrl.sendFrameLen & 0x00FF);
	sTlkMdiCommCtrl.sendFrame[3] |= (sTlkMdiCommCtrl.sendFrameLen & 0x0F00) >> 8;
	tempVar = tlkalg_crc16_calc(sTlkMdiCommCtrl.sendFrame+2, sTlkMdiCommCtrl.sendFrameLen-2);
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = (tempVar & 0x00FF);
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = (tempVar & 0xFF00) >> 8;
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = TLKPRT_COMM_TAIL_SIGN0;
	sTlkMdiCommCtrl.sendFrame[sTlkMdiCommCtrl.sendFrameLen++] = TLKPRT_COMM_TAIL_SIGN1;
	
	return TLK_ENONE;
}



#endif //#if (TLK_CFG_COMM_ENABLE)

