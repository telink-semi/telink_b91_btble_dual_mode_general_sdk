/********************************************************************************************************
 * @file     tlkmmi_fileComm.c
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
#if (TLKMMI_FILE_ENABLE)
#include "tlkprt/tlkprt_stdio.h"
#include "tlkmmi/file/tlkmmi_file.h"
#include "tlkmmi/file/tlkmmi_fileComm.h"
#include "tlkmmi/file/tlkmmi_fileCtrl.h"



static void tlkmmi_file_recvCmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen);
static void tlkmmi_file_recvStartCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_file_recvPauseCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_file_recvCloseCmdDeal(uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmmi_file_commInit
 * Descript: register the file manager cmd handler callback. 
 * Params:
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_file_commInit(void)
{
	tlkmdi_comm_regFileCB(tlkmmi_file_recvCmdHandler);
		
	return TLK_ENONE;
}

int tlkmmi_file_sendCloseEvt(uint08 *pData, uint16 dataLen)
{
	return tlkmdi_comm_sendFileEvt(TLKPRT_COMM_EVTID_FILE_CLOSE, pData, dataLen);
}
int tlkmmi_file_sendShakeEvt(uint08 *pData, uint16 dataLen)
{
	return tlkmdi_comm_sendFileEvt(TLKPRT_COMM_EVTID_FILE_SHAKE, pData, dataLen);
}
int tlkmmi_file_sendStartRsp(uint08 status, uint08 reason, uint08 *pData, uint16 dataLen)
{
	return tlkmdi_comm_sendFileRsp(TLKPRT_COMM_CMDID_FILE_START, status, reason, pData, dataLen);
}
int tlkmmi_file_sendCloseRsp(uint08 status, uint08 reason, uint08 *pData, uint16 dataLen)
{
	return tlkmdi_comm_sendFileRsp(TLKPRT_COMM_CMDID_FILE_CLOSE, status, reason, pData, dataLen);
}
int tlkmmi_file_sendPauseRsp(uint08 status, uint08 reason, uint08 *pData, uint16 dataLen)
{
	return tlkmdi_comm_sendFileRsp(TLKPRT_COMM_CMDID_FILE_PAUSE, status, reason, pData, dataLen);
}




static void tlkmmi_file_recvCmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: msgID-%d", msgID);
	
	if(msgID == TLKPRT_COMM_CMDID_FILE_START){
		tlkmmi_file_recvStartCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_FILE_PAUSE){
		tlkmmi_file_recvPauseCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_FILE_CLOSE){
		tlkmmi_file_recvCloseCmdDeal(pData, dataLen);
	}else{
		tlkmdi_comm_sendFileRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}



	
static void tlkmmi_file_recvStartCmdDeal(uint08 *pData, uint08 dataLen)
{	
	uint08 peerPort;
	uint08 fileType;
	uint08 waitNumb;
	uint08 nameLens;
	uint16 interval;
	uint16 unitLens;
	uint32 fileSize;

	if(dataLen < 28 || pData[3]+28 > dataLen){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvStartCmdDeal: error length");
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	peerPort = pData[0];
	fileType = pData[1];
	waitNumb = pData[2];
	nameLens = pData[3];
	interval = (((uint16)pData[5]) << 8) | pData[4];
	unitLens = (((uint16)pData[7]) << 8) | pData[6];
	fileSize = (((uint32)pData[11]) << 24) | (((uint32)pData[10]) << 16) | (((uint32)pData[9]) << 8) | pData[8];
	tlkmmi_file_start(peerPort, fileType, waitNumb, interval, unitLens, fileSize, pData+12, pData+28, nameLens);
}
static void tlkmmi_file_recvPauseCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 minePort;

	if(dataLen == 0){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvPauseCmdDeal: error length");
		tlkmmi_file_sendPauseRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	minePort = pData[0];
	tlkmmi_file_pause(minePort);
}
static void tlkmmi_file_recvCloseCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 status;
	uint08 minePort;
	
	if(dataLen < 18){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCloseCmdDeal: error length");
		tlkmmi_file_sendCloseRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	minePort = pData[0];
	status   = pData[1];
	tlkmmi_file_close(minePort, status, pData+2);
}




#endif //#if (TLKMMI_FILE_ENABLE)

