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
#include "tlkalg/digest/md5/tlkalg_md5.h"
#include "tlklib/fs/tlklib_fs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_adapt.h"
#include "tlkmdi/tlkmdi_file.h"
#include "tlkmmi/file/tlkmmi_fileConfig.h"
#include "tlkmmi/file/tlkmmi_file.h"
#include "tlkmmi/file/tlkmmi_fileCtrl.h"
//#if (TLKMMI_FILE_CHN_BT_SPP_ENABLE)
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"
//#endif



#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
static void tlkmmi_file_serialRecvProc(uint08 msgID, uint08 *pData, uint08 dataLen);
static int  tlkmmi_file_serialSendProc(uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);
#endif
#if (TLKMMI_FILE_CHN_BT_SPP_ENABLE)
static void tlkmmi_file_btSppRecvProc(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
static int  tlkmmi_file_btSppSendProc(uint16 handle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);
#endif

static tlkmmi_file_recvIntf_t *tlkmmi_file_getRecvIntf(uint16 fileType);

static int tlkmmi_file_recvStart(tlkmdi_file_unit_t *pUnit, bool isFast);
static int tlkmmi_file_recvClose(tlkmdi_file_unit_t *pUnit, uint08 status);
static int tlkmmi_file_recvParam(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen);
static int tlkmmi_file_recvSave(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen);
static int tlkmmi_file_recvSend(uint08 optChn, uint16 handle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);


extern const tlkmmi_file_recvIntf_t gcTlkMmiFileDfuIntf;
extern const tlkmmi_file_recvIntf_t gcTlkMmiFileMp3Intf;
const static tlkmmi_file_recvIntf_t *scTlkMmiFileRecvIntfs[TLKMMI_FILE_INTF_ITEM_COUNT] = {
	&gcTlkMmiFileDfuIntf,
	&gcTlkMmiFileMp3Intf,
};


int tlkmmi_file_ctrlInit(void)
{
	uint08 index;
	uint08 usrAuthCode[16] = {0};
	uint08 usrCrypCode[16] = {0};
	const tlkmmi_file_recvIntf_t *pIntf;

	for(index=0; index<TLKMMI_FILE_INTF_ITEM_COUNT; index++){
		pIntf = scTlkMmiFileRecvIntfs[index];
		if(pIntf != nullptr && pIntf->Init != nullptr) pIntf->Init();
	}
	tlkmdi_file_setRecvInfs(tlkmmi_file_recvStart, tlkmmi_file_recvClose, tlkmmi_file_recvParam, 
		tlkmmi_file_recvSave, tlkmmi_file_recvSend);
	#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
	tlkmdi_comm_regFileCB(tlkmmi_file_serialRecvProc);
	#endif
	#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
	btp_spp_regDataCB(tlkmmi_file_btSppRecvProc);
	#endif

	tmemcpy(usrAuthCode, "Telink File Tran", strlen("Telink File Tran"));
	tmemcpy(usrCrypCode, "Telink File Tran", strlen("Telink File Tran"));
	tlkmdi_file_setAuthCode(usrAuthCode);
	tlkmdi_file_setCrypCode(usrCrypCode);
	
	return TLK_ENONE;
}


#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
static void tlkmmi_file_serialRecvProc(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen;
	uint08 header[8];
	
	if(dataLen < 2){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: error length[%d]", dataLen);
		return;
	}
		
	headLen = 0;
	header[headLen++] = 0x00; //Cmd Packet
	tmemcpy(header+headLen, pData-4, 4);
	headLen += 4;
	header[headLen++] = pData[0];
	header[headLen++] = pData[1];
	if(header[4] < 2 || header[4] > dataLen){
		tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: error length");
		return;
	}
	
	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: msgID-%d", msgID);
	
	tlkmdi_file_recvHandler(TLKMDI_FILE_OPTCHN_SERIAL, 0xFFFF, header, headLen, pData+2, dataLen-2);
}
static int tlkmmi_file_serialSendProc(uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint08 value;
	
	if(headLen == 0 || headLen > 64){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_serialSendProc: error headLen[%d]", headLen);
		return -TLK_EFAIL;
	}

//	tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_serialSendProc[Head]: ", pHead, headLen);
//	tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_serialSendProc[Data]: ", pData, dataLen);
//	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_serialSendProc: headLen[%d], dataLen[%d]", headLen, dataLen);
	value = 0;
	tlkmdi_comm_getSendNumb(&value);
	pHead[3] = value;
	ret = tlkmdi_comm_send(pHead[0], pHead+1, headLen-1, pData, dataLen);
	if(ret == TLK_ENONE) tlkmdi_comm_incSendNumb();
	return ret;
}
#endif //#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
#if (TLKMMI_FILE_CHN_BT_SPP_ENABLE)
static void tlkmmi_file_btSppRecvProc(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	if(dataLen < 7){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_btSppRecvProc: error dataLen[%d]", dataLen);
		return;
	}
//	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "btSppRecv: %x", aclHandle);
	if(pData[0] == TLKPRT_COMM_PTYPE_DAT){
		tlkmdi_file_recvHandler(TLKMDI_FILE_OPTCHN_BT_SPP, aclHandle, pData, 6, pData+6, dataLen-6);
	}else{
		tlkmdi_file_recvHandler(TLKMDI_FILE_OPTCHN_BT_SPP, aclHandle, pData, 7, pData+7, dataLen-7);
	}
}
static int tlkmmi_file_btSppSendProc(uint16 handle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	ret = btp_spp_sendData(handle, pHead, headLen, pData, dataLen);
	if(ret == TLK_ENONE){
		//tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "btSppSend[Head]:", pHead, headLen);
		//tlkapi_array(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "btSppSend[Data]:", pData, dataLen > 16 ? 16 : dataLen);
	}else{
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "btSppSend: failure");
	}
	return ret;
}
#endif //#if (TLKMMI_FILE_CHN_BT_SPP_ENABLE)


static int tlkmmi_file_recvStart(tlkmdi_file_unit_t *pUnit, bool isFast)
{
	tlkmmi_file_recvIntf_t *pIntf;
	if(pUnit == nullptr) return -TLK_EPARAM;
	pIntf = tlkmmi_file_getRecvIntf(pUnit->fileType);
	if(pIntf == nullptr || pIntf->Start == nullptr) return -TLK_ENOSUPPORT;
	return pIntf->Start(pUnit, isFast);
}
static int tlkmmi_file_recvClose(tlkmdi_file_unit_t *pUnit, uint08 status)
{
	tlkmmi_file_recvIntf_t *pIntf;
	pIntf = tlkmmi_file_getRecvIntf(pUnit->fileType);
	if(pIntf == nullptr || pIntf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pIntf->Close(pUnit, status);
}
static int tlkmmi_file_recvParam(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen)
{
	tlkmmi_file_recvIntf_t *pIntf;
	if(pUnit == nullptr) return -TLK_EPARAM;
	pIntf = tlkmmi_file_getRecvIntf(pUnit->fileType);
	if(pIntf == nullptr || pIntf->Param == nullptr) return -TLK_ENOSUPPORT;
	return pIntf->Param(pUnit, paramType, pParam, paramLen);
}

static int tlkmmi_file_recvSave(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen)
{
	tlkmmi_file_recvIntf_t *pIntf;
	if(pUnit == nullptr) return -TLK_EPARAM;
	pIntf = tlkmmi_file_getRecvIntf(pUnit->fileType);
	if(pIntf == nullptr || pIntf->Save == nullptr) return -TLK_ENOSUPPORT;
	return pIntf->Save(pUnit, offset, pData, dataLen);
}
static int tlkmmi_file_recvSend(uint08 optChn, uint16 handle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen)
{
	if(optChn == TLKMDI_FILE_OPTCHN_SERIAL){
		#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
		return tlkmmi_file_serialSendProc(pHead, headLen, pData, dataLen);
		#endif
	}else if(optChn == TLKMDI_FILE_OPTCHN_BT_SPP){
		#if (TLKMMI_FILE_CHN_BT_SPP_ENABLE)
		return tlkmmi_file_btSppSendProc(handle, pHead, headLen, pData, dataLen);
		#endif
	}else if(optChn == TLKMDI_FILE_OPTCHN_BT_ATT){
		return -TLK_ENOSUPPORT;
	}else if(optChn == TLKMDI_FILE_OPTCHN_LE_ATT){
		return -TLK_ENOSUPPORT;
	}
	return -TLK_ENOSUPPORT;
}


static tlkmmi_file_recvIntf_t *tlkmmi_file_getRecvIntf(uint16 fileType)
{
	uint08 index;
	for(index=0; index<TLKMMI_FILE_INTF_ITEM_COUNT; index++){
		if(scTlkMmiFileRecvIntfs[index] != nullptr && scTlkMmiFileRecvIntfs[index]->fileType == fileType) break;
	}
	if(index == TLKMMI_FILE_INTF_ITEM_COUNT) return nullptr;
	return (tlkmmi_file_recvIntf_t*)scTlkMmiFileRecvIntfs[index];
}


#if 0
void tlkmmi_file_getPath(uint08 fileType, uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	if(fileType == TLKPRT_FILE_TYPE_DFU){
		tlkmmi_file_getDfuPath(pBuffer, buffLen, pGetLen);
	}else if(fileType == TLKPRT_FILE_TYPE_MP3){
		tlkmmi_file_getMp3Path(pBuffer, buffLen, pGetLen);
	}else if(fileType == TLKPRT_FILE_TYPE_TONE){
		tlkmmi_file_getTonePath(pBuffer, buffLen, pGetLen);
	}else{
		if(pGetLen != nullptr) *pGetLen = 0;
	}
}
static void tlkmmi_file_getDfuPath(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	uint08 dataLen = 0;
	
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
		pBuffer[dataLen++] = (uint08)'/';
		pBuffer[dataLen++] = 0x00;
		pBuffer[dataLen+0] = 0x00;
		pBuffer[dataLen+1] = 0x00;
	}
	if(pGetLen != nullptr) *pGetLen = dataLen;
}
static void tlkmmi_file_getMp3Path(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	uint08 dataLen = 0;
	
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
		pBuffer[dataLen++] = (uint08)'/';
		pBuffer[dataLen++] = 0x00;
		pBuffer[dataLen+0] = 0x00;
		pBuffer[dataLen+1] = 0x00;
	}
	if(pGetLen != nullptr) *pGetLen = dataLen;
}
static void tlkmmi_file_getTonePath(uint08 *pBuffer, uint08 buffLen, uint08 *pGetLen)
{
	uint08 dataLen = 0;
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
		pBuffer[dataLen++] = (uint08)'/';
		pBuffer[dataLen++] = 0x00;
		pBuffer[dataLen+0] = 0x00;
		pBuffer[dataLen+1] = 0x00;
	}
	if(pGetLen != nullptr) *pGetLen = dataLen;
}

#endif


#endif //#if (TLKMMI_FILE_ENABLE)

