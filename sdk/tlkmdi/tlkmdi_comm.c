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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlkmdi/tlkmdi_comm.h"
#include "drivers.h"

#define TLKMDI_BTREC_DBG_FLAG         (TLKMDI_BTREC_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_BTREC_DBG_SIGN         TLKMDI_DBG_SIGN

static void tlkmdi_comm_recvHandler(uint08 *pFrame, uint16 frmLen);


static uint08 sTlkMdiCommSendNumb = 0x00;
static uint08 sTlkMdiCommRecvNumb = 0xFF;
static tlkmdi_comm_cmdCB sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_MAX] = {0};
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
	tlkdev_serial_init();
	tlkdev_serial_regCB(tlkmdi_comm_recvHandler);
	
	return TLK_ENONE;
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
	sTlkMdiCommSendNumb ++;
}
void tlkmdi_comm_setSendNumb(uint08 value)
{
	sTlkMdiCommSendNumb = value;
}
void tlkmdi_comm_getSendNumb(uint08 *pValue)
{
	if(pValue != nullptr) *pValue = sTlkMdiCommSendNumb;
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
 * Function: tlkmdi_comm_regSysCB.
 * Descript: Register the data callback.
 * Params:
 *        @cmdCB[IN]--Handle System Message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regSysCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_SYS] = cmdCB;
}

/******************************************************************************
 * Function: tlkmdi_comm_regBtCB.
 * Descript: Register the Bt callback.
 * Params:
 *        @cmdCB[IN]--Handle BT message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regBtCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_BT] = cmdCB;
}

/******************************************************************************
 * Function: tlkmdi_comm_regLeCB.
 * Descript: Register the BLE callback.
 * Params:
 *        @cmdCB[IN]--Handle BLE message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regLeCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_LE] = cmdCB;
}

/******************************************************************************
 * Function: tlkmdi_comm_regDbgCB.
 * Descript: Register the data callback.
 * Params:
 *        @cmdCB[IN]--Handle Debug Message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regDbgCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_DBG] = cmdCB;
}

/******************************************************************************
 * Function: tlkmdi_comm_regFileCB.
 * Descript: Register the File operation callback.
 * Params:
 *        @cmdCB[IN]--Handle FILE message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regFileCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_FILE] = cmdCB;
}

/******************************************************************************
 * Function: tlkmdi_comm_regCallCB.
 * Descript: Register the Phone Call callback.
 * Params:
 *        @cmdCB[IN]--Handle phone call message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regCallCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_CALL] = cmdCB;
}

/******************************************************************************
 * Function: tlkmdi_comm_regAudioCB.
 * Descript: Register the Audio callback.
 * Params:
 *        @cmdCB[IN]--Handle Audio message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regAudioCB(tlkmdi_comm_cmdCB cmdCB)
{
	sTlkMdiCommCmdCB[TLKPRT_COMM_MTYPE_AUDIO] = cmdCB;
}


/******************************************************************************
 * Function: tlkmdi_comm_SendSysXXX.
 * Descript: send the sys command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendSysCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_SYS, cmdID, pData, dataLen);
}
int tlkmdi_comm_sendSysRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_SYS, cmdID, status, reason, pData, dataLen);
}
int tlkmdi_comm_sendSysEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_SYS, evtID, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendBtXXX.
 * Descript: send the BT command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendBtCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_BT, cmdID, pData, dataLen);
}
int tlkmdi_comm_sendBtRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_BT, cmdID, status, reason, pData, dataLen);
}
int tlkmdi_comm_sendBtEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_BT, evtID, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendLeXXX.
 * Descript: send the BLE command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendLeCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_LE, cmdID, pData, dataLen);
}
int tlkmdi_comm_sendLeRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_LE, cmdID, status, reason, pData, dataLen);
}
int tlkmdi_comm_sendLeEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_LE, evtID, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendFileXXX.
 * Descript: send the File command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendFileCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_FILE, cmdID, pData, dataLen);
}
int tlkmdi_comm_sendFileRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_FILE, cmdID, status, reason, pData, dataLen);
}
int tlkmdi_comm_sendFileEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_FILE, evtID, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendCallXXX.
 * Descript: send the Call command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendCallCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_CALL, cmdID, pData, dataLen);
}
int tlkmdi_comm_sendCallRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_CALL, cmdID, status, reason, pData, dataLen);
}
int tlkmdi_comm_sendCallEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_CALL, evtID, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendAudioXXX.
 * Descript: send the Audio command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendAudioCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_AUDIO, cmdID, pData, dataLen);
}
int tlkmdi_comm_sendAudioRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_AUDIO, cmdID, status, reason, pData, dataLen);
}
int tlkmdi_comm_sendAudioEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_AUDIO, evtID, pData, dataLen);
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
	head[2] = sTlkMdiCommSendNumb; //Numb
	head[3] = dataLen; //Lens
	ret = tlkdev_serial_send(TLKPRT_COMM_PTYPE_CMD, head, 4, pData, dataLen);
	if(ret == TLK_ENONE) sTlkMdiCommSendNumb++;
	return ret;
}
int tlkmdi_comm_sendRsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[6];
	head[0] = mType; //Mtype
	head[1] = cmdID; //MsgID
	head[2] = sTlkMdiCommSendNumb; //Numb
	head[3] = 2+dataLen; //Lens
	head[4] = status;
	head[5] = reason;
	ret = tlkdev_serial_send(TLKPRT_COMM_PTYPE_RSP, head, 6, pData, dataLen);
	if(ret == TLK_ENONE) sTlkMdiCommSendNumb++;
	return ret;
}
int tlkmdi_comm_sendEvt(uint08 mType, uint08 evtID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[4];
	head[0] = mType; //Mtype
	head[1] = evtID; //MsgID
	head[2] = sTlkMdiCommSendNumb; //Numb
	head[3] = dataLen; //Lens
	ret = tlkdev_serial_send(TLKPRT_COMM_PTYPE_EVT, head, 4, pData, dataLen);
	if(ret == TLK_ENONE) sTlkMdiCommSendNumb++;
	return ret;
}

int tlkmdi_comm_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen)
{
	uint08 head[4];
	if(pData == nullptr || dataLen == 0 || dataLen > 256) return -TLK_EPARAM;
	head[0] = (numb & 0x00FF); //Num
	head[1] = (numb & 0xFF00) >> 8;
	head[2] = datID; //DID
	head[3] = dataLen-1; //Lens
	return tlkdev_serial_send(TLKPRT_COMM_PTYPE_DAT, head, 4, pData, dataLen);
}

int tlkmdi_comm_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	return tlkdev_serial_send(pktType, pHead, headLen, pBody, bodyLen);
}


static void tlkmdi_comm_recvHandler(uint08 *pFrame, uint16 frmLen)
{
	uint08 mtype;
	uint08 ptype;

	ptype = (pFrame[3] & 0xF0) >> 4;

	//tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "tlkmdi_comm_recvHandler: %d 0x%x 0x%x 0x%x", ptype, pFrame[4], pFrame[5], pFrame[6]);
	
	if(ptype == TLKPRT_COMM_PTYPE_CMD){
		uint08 numb;
		uint08 lens;
		uint08 msgID;
		mtype = pFrame[4];
		msgID = pFrame[5];
		numb  = pFrame[6];
		lens  = pFrame[7];
		if(frmLen < 4+TLKPRT_COMM_FRM_EXTLEN || frmLen < 4+lens+TLKPRT_COMM_FRM_EXTLEN){
			tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "Recv CmdOrRspOrEvtPkt Length Error: frmLen-%d lens-%d", frmLen, lens);
			return;
		}
		if(mtype >= TLKPRT_COMM_MTYPE_MAX){
			tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "Recv Error MsgType: mtype-%d", mtype);
			return;
		}
		if((uint08)(sTlkMdiCommRecvNumb+1) != numb){
			tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "Stall Packet: %d %d", sTlkMdiCommRecvNumb, numb);
		}
		sTlkMdiCommRecvNumb = numb;
		if(sTlkMdiCommCmdCB[mtype] != nullptr){
			sTlkMdiCommCmdCB[mtype](msgID, pFrame+8, lens);
		}
	}
	else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint16 numb;
		uint16 lens;
		uint08 datID;
		datID = pFrame[4];
		numb  = (((uint16)pFrame[6]) << 8) | pFrame[5];
		lens  = (((uint16)pFrame[8]) << 8) | pFrame[7];
		if(frmLen < 4+TLKPRT_COMM_FRM_EXTLEN || frmLen < 4+lens+TLKPRT_COMM_FRM_EXTLEN){
			tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "Recv DatPkt Length Error: frmLen-%d lens-%d", frmLen, lens);
			return;
		}
		if(datID != 0 && datID <= TLKMDI_COMM_DATA_CHANNEL_MAX && sTlkMdiCommDatCB[datID-1] != nullptr){
			sTlkMdiCommDatCB[datID-1](datID, numb, pFrame+9, lens);
		}else{
			tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "Recv DatPkt Unexpect: not used - datID[%d]", datID);
		}
	}
	else{
		tlkapi_error(TLKMDI_CFG_DBG_ENABLE|TLKMDI_DBG_FLAG, TLKMDI_DBG_SIGN, "Recv Error PktType: ptype-%d", ptype);
	}
}


