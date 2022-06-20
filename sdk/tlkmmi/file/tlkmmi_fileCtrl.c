/********************************************************************************************************
 * @file     tlkmmi_fileCtrl.c
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
#include "tlk_config.h" 
#include "tlkapi/tlkapi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_FILE_ENABLE)
#include "tlkprt/tlkprt_stdio.h"
#include "tlklib/fs/filesystem.h"
#include "tlkalg/md5/tlkalg_md5.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_adapt.h"
#include "tlkmmi/file/tlkmmi_file.h"
#include "tlkmmi/file/tlkmmi_fileComm.h"
#include "tlkmmi/file/tlkmmi_fileCtrl.h"


#define TLKMMI_FILE_NAME_LENS        80


typedef struct{
	uint08 state;
	uint08 busys;
	uint08 attrs;
	uint08 reason;
	uint16 reserve;
	uint16 timeout;
	uint32 curNumb;
	uint32 pktNumb;
	uint08 minePort;
	uint08 peerPort;
	uint08 pathLens;
	uint08 waitMask;
	uint16 interval;
	uint16 unitLens;
	uint32 fileSize;
	uint08 digest[16];
	uint08 filePath[TLKMMI_FILE_NAME_LENS];
	tlkapi_timer_t timer;
	tlkalg_md5_contex_t context;
	#if (TLK_FS_FAT_ENABLE)
	FIL fileHandle;
	#else
	uint08 fileHandle;
	#endif
}tlkmmi_file_unit_t;
typedef struct{
	tlkmmi_file_unit_t unit[TLKMMI_FILE_UNIT_COUNT];
}tlkmmi_file_ctrl_t;


static bool tlkmmi_file_timer(tlkapi_timer_t *pTimer, void *pUsrArg);
static void tlkmmi_file_procs(tlkmmi_file_unit_t *pNode);

static void tlkmmi_file_sendStartRspProc(tlkmmi_file_unit_t *pNode);
static void tlkmmi_file_sendCloseRspProc(tlkmmi_file_unit_t *pNode);
static void tlkmmi_file_sendPauseRspProc(tlkmmi_file_unit_t *pNode);
static void tlkmmi_file_sendCloseEvtProc(tlkmmi_file_unit_t *pNode);
static void tlkmmi_file_sendShakeEvtProc(tlkmmi_file_unit_t *pNode);

static void tlkmmi_file_recvDataProc(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen);

static void tlkmmi_file_resetNode(tlkmmi_file_unit_t *pNode);
static tlkmmi_file_unit_t *tlkmmi_file_getIdleNode(void);
static tlkmmi_file_unit_t *tlkmmi_file_getUsedNode(uint08 minePort);
static tlkmmi_file_unit_t *tlkmmi_file_getUsedNodeByPeerPort(uint08 peerPort);

static void tlkmmi_file_getOtaPath(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen);
static void tlkmmi_file_getMp3Path(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen);
static void tlkmmi_file_getTonePath(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen);
static void tlkmmi_file_getPath(uint08 fileType, uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen);


static tlkmmi_file_ctrl_t sTlkMmiFileCtrl;


int tlkmmi_file_ctrlInit(void)
{
	tmemset(&sTlkMmiFileCtrl, 0, sizeof(tlkmmi_file_ctrl_t));
	
	return TLK_ENONE;
}

void tlkmmi_file_start(uint08 peerPort, uint08 fileType, uint08 waitNumb, uint16 interval, uint16 unitLens,
	uint32 fileSize, uint08 *pDigest, uint08 *pFileName, uint08 nameLens)
{
	uint08 pathLen;
	uint08 minePort;
	tlkmmi_file_unit_t *pNode;

	if(fileSize == 0 || fileType == 0 || peerPort == 0 || unitLens < TLKMMI_FILE_UNITLENS_MIN || nameLens == 0){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - param{fileType-%d, peerPort-%d, unitLens-%d, nameLens-%d}", fileType, peerPort, unitLens, nameLens);
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	if(waitNumb != 0 && waitNumb != 1 && waitNumb != 2 && waitNumb != 4 && waitNumb != 8 && waitNumb != 16
		&& waitNumb != 32 && waitNumb != 64){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: error waitNumb %d{0,1,2,4,8,16,32,64}", waitNumb);
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	if(waitNumb > TLKMMI_FILE_WAITNUMB_MAX) waitNumb = TLKMMI_FILE_WAITNUMB_MAX;
	if(interval < TLKMMI_FILE_INTERVAL_MIN) interval = TLKMMI_FILE_INTERVAL_MIN;
	if(unitLens > TLKMMI_FILE_UNITLENS_MAX) unitLens = TLKMMI_FILE_UNITLENS_MAX;
	
	pNode = tlkmmi_file_getUsedNodeByPeerPort(peerPort);
	if(pNode != nullptr){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - exist %d", peerPort);
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EEXIST, nullptr, 0);
		return;
	}
	if(tlkmdi_comm_getVolidDatID(&minePort) != TLK_ENONE){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - no quota DatID");
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EQUOTA, nullptr, 0);
		return;
	}
	
	pNode = tlkmmi_file_getIdleNode();
	if(pNode == nullptr){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - no quota Node");
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EQUOTA, nullptr, 0);
		return;
	}
	
	pathLen = 0;
	tlkmmi_file_getPath(fileType, pNode->filePath, TLKMMI_FILE_NAME_LENS, &pathLen);
	if(pathLen == 0){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - filetype - %d", fileType);
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
		return;
	}
	if(pathLen+nameLens+10 > TLKMMI_FILE_NAME_LENS){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - length %d %d %d",
			pathLen, nameLens, TLKMMI_FILE_NAME_LENS);
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ELENGTH, nullptr, 0);
		return;
	}
	tmemcpy(pNode->filePath+pathLen, pFileName, nameLens);
	pathLen += nameLens;
	#if (TLK_FS_FAT_ENABLE)
	pNode->filePath[pathLen++] = '.';
	pNode->filePath[pathLen++] = 0x00;
	pNode->filePath[pathLen++] = 't';
	pNode->filePath[pathLen++] = 0x00;
	pNode->filePath[pathLen++] = 'm';
	pNode->filePath[pathLen++] = 0x00;
	pNode->filePath[pathLen++] = 'p';
	pNode->filePath[pathLen++] = 0x00;
	pNode->filePath[pathLen++] = 0x00;
	pNode->filePath[pathLen++] = 0x00;
	#endif
	pNode->filePath[pathLen] = 0x00;
	tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: path", pNode->filePath, pathLen);
	
	if(tlkapi_file_open(&pNode->fileHandle, (const FCHAR*)pNode->filePath,  TLKAPI_FM_OPEN_ALWAYS | TLKAPI_FM_WRITE) != TLK_ENONE){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: failure - open file");
		tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		return;
	}

	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_start: {fileType-%d, peerPort-%d, unitLens-%d, nameLens-%d}", fileType, peerPort, unitLens, nameLens);

	pNode->state = TLK_STATE_CONNECT;
	pNode->timeout = TLKMMI_FILE_IDLE_TIMEOUT;
	pNode->pathLens = pathLen;
	pNode->peerPort = peerPort;
	pNode->minePort = minePort;
	if(waitNumb == 0 || waitNumb == 1) pNode->waitMask = 0;
	else pNode->waitMask = waitNumb-1;
	pNode->interval = interval;
	pNode->fileSize = fileSize;
	pNode->unitLens = unitLens;
	pNode->pktNumb  = (fileSize+unitLens-1)/unitLens;
	tmemcpy(pNode->digest, pDigest, 16);
	tlkalg_md5_init(&pNode->context);
	tlkmmi_adapt_initTimer(&pNode->timer, tlkmmi_file_timer, pNode, TLKMMI_FILE_TIMEOUT);
	tlkmdi_comm_regDatCB(pNode->minePort, tlkmmi_file_recvDataProc, true);
	tlkmmi_adapt_insertTimer(&pNode->timer);
	tlkmmi_file_sendStartRspProc(pNode);
}
void tlkmmi_file_pause(uint08 mineProt)
{
	tlkmmi_file_unit_t *pNode;

	pNode = tlkmmi_file_getUsedNode(mineProt);
	if(pNode == nullptr){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_pause: failure - error node");
		tlkmmi_file_sendPauseRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		return;
	}
	tlkmmi_file_sendPauseRspProc(pNode);
}
void tlkmmi_file_close(uint08 mineProt, uint08 status, uint08 *pDigest)
{
	tlkmmi_file_unit_t *pNode;
	
	pNode = tlkmmi_file_getUsedNode(mineProt);
	if(pNode == nullptr){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_close: failure - error node");
		tlkmmi_file_sendCloseRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		return;
	}
	
	if(status != TLKPRT_COMM_RSP_STATUE_SUCCESS){
		pNode->reason = TLK_EFAIL;
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_close: failure - status %d", status);
	}else if(pNode->curNumb != pNode->pktNumb){
		pNode->reason = TLK_EFAIL;
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_close: failure - not complete %d %d", 
			pNode->curNumb, pNode->pktNumb);
	}else if(tmemcmp(pNode->digest, pDigest, 16) != 0){
		pNode->reason = TLK_EFAIL;
		tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN,, "tlkmmi_file_close: failure - RawDigest", pNode->digest, 16);
		tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_close: failure - CalDigest", pDigest, 16);
	}else{
		tlkmdi_comm_regDatCB(pNode->minePort, nullptr, true);
		tlkapi_file_close(&pNode->fileHandle);
		#if (TLK_FS_FAT_ENABLE)
		uint08 filePath[TLKMMI_FILE_NAME_LENS];
		tmemcpy(filePath, pNode->filePath, pNode->pathLens-10);
		tlkapi_file_rename((const FCHAR*)pNode->filePath, (const FCHAR*)filePath);
		#endif
	}
	tlkmmi_file_sendCloseRspProc(pNode);
	tlkmmi_file_sendCloseEvtProc(pNode);
}


static bool tlkmmi_file_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	tlkmmi_file_unit_t *pNode;

	pNode = (tlkmmi_file_unit_t*)pUsrArg;
	if(pNode == nullptr || pNode->state != TLK_STATE_CONNECT){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_timer: timer fault!!!");
		return false;
	}
	
	if(pNode->timeout != 0) pNode->timeout--;
	if(pNode->timeout == 0 && pNode->state == TLK_STATE_CONNECT){
		pNode->state = TLK_STATE_DISCING;
		pNode->reason = TLK_ETIMEOUT;
		pNode->timeout = TLKMMI_FILE_IDLE_TIMEOUT;
		tlkmmi_file_sendCloseEvtProc(pNode);
	}else if(pNode->timeout == 0 && pNode->state == TLK_STATE_DISCING){
		tlkmmi_file_resetNode(pNode);
	}
	tlkmmi_file_procs(pNode);
	if(pNode->timeout != 0) return true;
	return false;
}
static void tlkmmi_file_procs(tlkmmi_file_unit_t *pNode)
{
	if(pNode->busys == TLKMMI_FILE_BUSY_NONE) return;
	if((pNode->busys & TLKMMI_FILE_BUSY_SEND_START_RSP) != 0){
		pNode->busys &= ~TLKMMI_FILE_BUSY_SEND_START_RSP;
		tlkmmi_file_sendStartRspProc(pNode);
	}else if((pNode->busys & TLKMMI_FILE_BUSY_SEND_CLOSE_RSP) != 0){
		pNode->busys &= ~TLKMMI_FILE_BUSY_SEND_CLOSE_RSP;
		tlkmmi_file_sendCloseRspProc(pNode);
	}else if((pNode->busys & TLKMMI_FILE_BUSY_SEND_PAUSE_RSP) != 0){
		pNode->busys &= ~TLKMMI_FILE_BUSY_SEND_PAUSE_RSP;
		tlkmmi_file_sendPauseRspProc(pNode);
	}else if((pNode->busys & TLKMMI_FILE_BUSY_SEND_SHAKE_EVT) != 0){
		pNode->busys &= ~TLKMMI_FILE_BUSY_SEND_SHAKE_EVT;
		tlkmmi_file_sendShakeEvtProc(pNode);
	}else if((pNode->busys & TLKMMI_FILE_BUSY_SEND_CLOSE_EVT) != 0){
		pNode->busys &= ~TLKMMI_FILE_BUSY_SEND_CLOSE_EVT;
		tlkmmi_file_sendCloseEvtProc(pNode);
	}else{
		pNode->busys = TLKMMI_FILE_BUSY_NONE;
	}
}


static void tlkmmi_file_sendStartRspProc(tlkmmi_file_unit_t *pNode)
{
	int ret;
	uint08 buffLen;
	uint08 buffer[16];
	buffLen = 0;
	buffer[buffLen++] = pNode->peerPort;
	buffer[buffLen++] = pNode->minePort;
	buffer[buffLen++] = (pNode->interval & 0x00FF);
	buffer[buffLen++] = (pNode->interval & 0xFF00) >> 8;
	buffer[buffLen++] = (pNode->unitLens & 0x00FF);
	buffer[buffLen++] = (pNode->unitLens & 0xFF00) >> 8;
	ret = tlkmmi_file_sendStartRsp(TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
	if(ret != TLK_ENONE){
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_START_RSP;
	}
}
static void tlkmmi_file_sendCloseRspProc(tlkmmi_file_unit_t *pNode)
{
	int ret;
	uint08 buffLen;
	uint08 buffer[8];
	buffLen = 0;
	buffer[buffLen++] = pNode->peerPort;
	ret = tlkmmi_file_sendCloseRsp(TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
	if(ret != TLK_ENONE){
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_CLOSE_RSP;
	}else{
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_CLOSE_EVT;
	}
}
static void tlkmmi_file_sendPauseRspProc(tlkmmi_file_unit_t *pNode)
{
	int ret;
	uint08 buffLen;
	uint08 buffer[8];
	buffLen = 0;
	buffer[buffLen++] = pNode->peerPort;
	ret = tlkmmi_file_sendPauseRsp(TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, buffer, buffLen);
	if(ret != TLK_ENONE){
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_PAUSE_RSP;
	}
}
static void tlkmmi_file_sendShakeEvtProc(tlkmmi_file_unit_t *pNode)
{
	int ret;
	uint32 curNumb;
	uint08 buffLen;
	uint08 buffer[16];
	
	buffLen = 0;
	curNumb = pNode->curNumb;
	if(pNode->curNumb != pNode->pktNumb) curNumb += 1;
	buffer[buffLen++] = pNode->peerPort;
	buffer[buffLen++] = (pNode->curNumb & 0x000000FF);
	buffer[buffLen++] = (pNode->curNumb & 0x0000FF00) >> 8;
	buffer[buffLen++] = (pNode->curNumb & 0x00FF0000) >> 16;
	buffer[buffLen++] = (pNode->curNumb & 0xFF000000) >> 24;
	buffer[buffLen++] = (curNumb & 0x000000FF);
	buffer[buffLen++] = (curNumb & 0x0000FF00) >> 8;
	buffer[buffLen++] = (curNumb & 0x00FF0000) >> 16;
	buffer[buffLen++] = (curNumb & 0xFF000000) >> 24;
	ret = tlkmmi_file_sendShakeEvt(buffer, buffLen);
	if(ret != TLK_ENONE){
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_SHAKE_EVT;
	}
}
static void tlkmmi_file_sendCloseEvtProc(tlkmmi_file_unit_t *pNode)
{
	int ret;
	uint08 buffLen;
	uint08 buffer[8];
	buffLen = 0;
	buffer[buffLen++] = pNode->peerPort;
	if(pNode->reason == 0x00) buffer[buffLen++] = TLKPRT_COMM_RSP_STATUE_SUCCESS;
	else buffer[buffLen++] = TLKPRT_COMM_RSP_STATUE_FAILURE;
	buffer[buffLen++] = pNode->reason;
	ret = tlkmmi_file_sendCloseEvt(buffer, buffLen);
	if(ret != TLK_ENONE){
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_CLOSE_EVT;
	}else{
		tlkmmi_file_resetNode(pNode);
	}
}

static void tlkmmi_file_recvDataProc(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen)
{
	uint32 curNumb;
	tlkmmi_file_unit_t *pNode;
	pNode = tlkmmi_file_getUsedNode(datID);
	if(pNode == nullptr || pNode->state != TLK_STATE_CONNECT || dataLen < 4){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: failure DatID or State");
		return;
	}
	
	curNumb = (((uint32)pData[3]) << 24) | (((uint32)pData[2]) << 16) | (((uint32)pData[1]) << 8) | pData[0];
	if(dataLen > 4+pNode->unitLens || curNumb == 0){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: fault param {DataLen-%d,UnitLens-%d,RcvNumb-%d,CurNumb-%d}",
			dataLen, pNode->unitLens, curNumb, pNode->curNumb);
		return;
	}
	
	if(pNode->waitMask == 0 || (curNumb & pNode->waitMask) == 0 || pNode->curNumb == pNode->pktNumb){
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_SHAKE_EVT;
	}
	if(pNode->curNumb+1 > curNumb){
		tlkapi_warn(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: repeat packet {RcvNumb-%d,CurNumb-%d,PktNumb-%d}", 
			curNumb, pNode->curNumb, pNode->pktNumb);
	}else if(pNode->curNumb+1 < curNumb){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: stall packet {RcvNumb-%d,CurNumb-%d,PktNumb-%d}",
			curNumb, pNode->curNumb, pNode->pktNumb);
		pNode->busys |= TLKMMI_FILE_BUSY_SEND_SHAKE_EVT;
	}else if(pNode->curNumb+1 > pNode->pktNumb){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: fault packet {RcvNumb-%d,CurNumb-%d,PktNumb-%d}",
			curNumb, pNode->curNumb, pNode->pktNumb, 0);
	}else{
		int ret;
		uint32 wrLens = 0;
		if(pNode->curNumb+1 != pNode->pktNumb && dataLen != 4+pNode->unitLens){
			tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: error number {dataLen-%d,unitLens-%d,curNumb-%d,PktNumb-%d}",
				dataLen-4, pNode->unitLens, pNode->curNumb, pNode->pktNumb);
			return;
		}
		pNode->curNumb = curNumb;
		pNode->timeout = TLKMMI_FILE_IDLE_TIMEOUT;
		tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvDataProc: {RcvNumb-%d,CurNumb-%d,PktNumb-%d,dataLen-%d}",
			curNumb, pNode->curNumb, pNode->pktNumb, dataLen-4);
		ret = tlkapi_file_write(&pNode->fileHandle, pData+4, dataLen-4, &wrLens);
		if(ret == TLK_ENONE){
			pNode->curNumb = curNumb;
			pNode->timeout = TLKMMI_FILE_IDLE_TIMEOUT;
			tlkalg_md5_update(&pNode->context, pData+4, dataLen-4);
		}
	}
	if((pNode->busys & TLKMMI_FILE_BUSY_SEND_SHAKE_EVT) != 0){
		pNode->busys &= ~TLKMMI_FILE_BUSY_SEND_SHAKE_EVT;
		tlkmmi_file_sendShakeEvtProc(pNode);
	}
}



static void tlkmmi_file_getPath(uint08 fileType, uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	if(fileType == TLKPRT_FILE_TYPE_OTA){
		tlkmmi_file_getOtaPath(pBuffer, buffLen, pGetLen);
	}else if(fileType == TLKPRT_FILE_TYPE_MP3){
		tlkmmi_file_getMp3Path(pBuffer, buffLen, pGetLen);
	}else if(fileType == TLKPRT_FILE_TYPE_TONE){
		tlkmmi_file_getTonePath(pBuffer, buffLen, pGetLen);
	}else{
		if(pGetLen != nullptr) *pGetLen = 0;
	}
}
static void tlkmmi_file_getOtaPath(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	uint08 dataLen = 0;
	
	#if !(TLK_FS_FAT_ENABLE)
		if(buffLen >= 6){
			pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen++] = (uint08)'O';
			pBuffer[dataLen++] = (uint08)'T';
			pBuffer[dataLen++] = (uint08)'A';
			if(1) pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen+0] = 0x00;
		}
	#else
		if(buffLen >= 16){
			pBuffer[dataLen++] = (uint08)'1';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)':';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'O';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'T';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'A';
			pBuffer[dataLen++] = 0x00;
			if(1){
				pBuffer[dataLen++] = (uint08)'/';
				pBuffer[dataLen++] = 0x00;
			}
			pBuffer[dataLen+0] = 0x00;
			pBuffer[dataLen+1] = 0x00;
		}
	#endif
	if(pGetLen != nullptr) *pGetLen = dataLen;
}
static void tlkmmi_file_getMp3Path(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	uint08 dataLen = 0;
	
	#if !(TLK_FS_FAT_ENABLE)
		if(buffLen >= 6){
			pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen++] = (uint08)'M';
			pBuffer[dataLen++] = (uint08)'P';
			pBuffer[dataLen++] = (uint08)'3';
			if(1) pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen+0] = 0x00;
		}
	#else
		if(buffLen >= 16){
			pBuffer[dataLen++] = (uint08)'1';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)':';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'M';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'P';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'3';
			pBuffer[dataLen++] = 0x00;
			if(1){
				pBuffer[dataLen++] = (uint08)'/';
				pBuffer[dataLen++] = 0x00;
			}
			pBuffer[dataLen+0] = 0x00;
			pBuffer[dataLen+1] = 0x00;
		}
	#endif
	if(pGetLen != nullptr) *pGetLen = dataLen;
}
static void tlkmmi_file_getTonePath(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	uint08 dataLen = 0;
	
	#if !(TLK_FS_FAT_ENABLE)
		if(buffLen >= 7){
			pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen++] = (uint08)'T';
			pBuffer[dataLen++] = (uint08)'O';
			pBuffer[dataLen++] = (uint08)'N';
			pBuffer[dataLen++] = (uint08)'E';
			if(1) pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen+0] = 0x00;
		}
	#else
		if(buffLen >= 18){
			pBuffer[dataLen++] = (uint08)'1';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)':';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'/';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'T';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'O';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'N';
			pBuffer[dataLen++] = 0x00;
			pBuffer[dataLen++] = (uint08)'E';
			pBuffer[dataLen++] = 0x00;
			if(1){
				pBuffer[dataLen++] = (uint08)'/';
				pBuffer[dataLen++] = 0x00;
			}
			pBuffer[dataLen+0] = 0x00;
			pBuffer[dataLen+1] = 0x00;
		}
	#endif
	if(pGetLen != nullptr) *pGetLen = dataLen;
}

static void tlkmmi_file_resetNode(tlkmmi_file_unit_t *pNode)
{
	if(pNode->state == TLK_STATE_CLOSED) return;
	tlkmdi_comm_regDatCB(pNode->minePort, nullptr, true);
	tlkmmi_adapt_removeTimer(&pNode->timer);
	tlkapi_file_close(&pNode->fileHandle);
	tmemset(pNode, 0, sizeof(tlkmmi_file_unit_t));
}
static tlkmmi_file_unit_t *tlkmmi_file_getIdleNode(void)
{
	uint08 index;
	for(index=0; index<TLKMMI_FILE_UNIT_COUNT; index++){
		if(sTlkMmiFileCtrl.unit[index].state == TLK_STATE_CLOSED) break;
	}
	if(index == TLKMMI_FILE_UNIT_COUNT) return nullptr;
	return &sTlkMmiFileCtrl.unit[index];
}
static tlkmmi_file_unit_t *tlkmmi_file_getUsedNode(uint08 minePort)
{
	uint08 index;
	if(minePort == 0) return nullptr;
	for(index=0; index<TLKMMI_FILE_UNIT_COUNT; index++){
		if(sTlkMmiFileCtrl.unit[index].state != TLK_STATE_CLOSED
			&& sTlkMmiFileCtrl.unit[index].minePort == minePort) break;
	}
	if(index == TLKMMI_FILE_UNIT_COUNT) return nullptr;
	return &sTlkMmiFileCtrl.unit[index];
}
static tlkmmi_file_unit_t *tlkmmi_file_getUsedNodeByPeerPort(uint08 peerPort)
{
	uint08 index;
	if(peerPort == 0) return nullptr;
	for(index=0; index<TLKMMI_FILE_UNIT_COUNT; index++){
		if(sTlkMmiFileCtrl.unit[index].state != TLK_STATE_CLOSED
			&& sTlkMmiFileCtrl.unit[index].peerPort == peerPort) break;
	}
	if(index == TLKMMI_FILE_UNIT_COUNT) return nullptr;
	return &sTlkMmiFileCtrl.unit[index];
}



#endif //#if (TLKMMI_FILE_ENABLE)

