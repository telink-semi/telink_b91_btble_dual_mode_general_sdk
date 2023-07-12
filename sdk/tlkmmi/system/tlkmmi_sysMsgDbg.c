/********************************************************************************************************
 * @file	tlkmmi_sysMsgDbg.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi/system/tlkmmi_sys.h"
#include "tlkmmi/system/tlkmmi_sysMsgDbg.h"

#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"
#include "tlklib/dbg/tlkdbg_config.h"
#include "tlklib/dbg/tlkdbg_hpudwn.h"



extern int tlkmmi_phone_bookSetParam(uint08 posi, uint08 type, uint08 sort, uint16 offset, uint16 number);
extern int tlkmmi_phone_startSyncBook(uint16 aclHandle, uint08 *pBtAddr, bool isForce);


static void tlkmmi_sys_recvDbgStartToneDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvDbgGetPhoneBookDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvDbgSimulateKeyDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvDbgFirmwareUpdateDeal(uint08 *pData, uint08 dataLen);



int tlkmmi_sys_dbgMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_sysDbgHandler: msgID-%d", msgID);
	
	if(msgID == TLKPRT_COMM_CMDID_DBG_START_TONE){
		tlkmmi_sys_recvDbgStartToneDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_GET_PHONE_BOOK){
		tlkmmi_sys_recvDbgGetPhoneBookDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_SIMULATE_KEY){
		tlkmmi_sys_recvDbgSimulateKeyDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE){
		tlkmmi_sys_recvDbgFirmwareUpdateDeal(pData, dataLen);
	}
	return TLK_ENONE;
}



static void tlkmmi_sys_recvDbgStartToneDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 3){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgStartToneDeal: length error - %d", dataLen);
		return;
	}
	
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgStartToneDeal: start", pData, dataLen);
	tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_START_TONE_CMD, pData, 3);
}
static void tlkmmi_sys_recvDbgGetPhoneBookDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_STK_BT_ENABLE)
	uint08 posi;
	uint08 type;
	uint08 sort;
	uint16 offset;
	uint16 number;
	uint16 aclHandle;
	bth_acl_handle_t *pHandle;
	
	if(dataLen < 7){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgGetPhoneBookDeal: length error - %d", dataLen);
		return;
	}
		
	aclHandle = btp_pbapclt_getAnyConnHandle(0);
	if(aclHandle == 0){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgGetPhoneBookDeal: failure - no device");
		return;
	}
	
	pHandle = bth_handle_getConnAcl(aclHandle);
	if(pHandle == nullptr){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgGetPhoneBookDeal: fault - ACL handle not exist");
		return;
	}
	
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgGetPhoneBookDeal: start", pData, 7);
	
	posi = pData[0];
	type = pData[1];
	sort = pData[2];
	offset = ((uint16)pData[4]<<8)|pData[3];
	number = ((uint16)pData[6]<<8)|pData[5];
	tlkmmi_phone_bookSetParam(posi, type, sort, offset, number);
	tlkmmi_phone_startSyncBook(pHandle->aclHandle, pHandle->btaddr, true);
#endif
}
static void tlkmmi_sys_recvDbgSimulateKeyDeal(uint08 *pData, uint08 dataLen)
{
	
}
static void tlkmmi_sys_recvDbgFirmwareUpdateDeal(uint08 *pData, uint08 dataLen)
{
#if (TLKDBG_CFG_HPU_DWN_ENABLE)
	tlkdbg_hpudbg_recvCmd(pData, dataLen);
#endif
}



#endif //#if (TLKMMI_SYSTEM_ENABLE)

