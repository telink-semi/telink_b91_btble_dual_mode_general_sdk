/********************************************************************************************************
 * @file     tlkmmi_phoneComm.c
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
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneComm.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"
#include "tlkmmi/phone/tlkmmi_phoneStatus.h"

#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"



static void tlkmmi_phone_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen);

static void tlkmmi_phone_recvDialCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvHoldCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvRedialCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvActiveCmdDeal(uint08 *pData, uint08 dataLen);;
static void tlkmmi_phone_recvRejectCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvHungUpCmdDeal(uint08 *pData, uint08 dataLen);



/******************************************************************************
 * Function: tlkmmi_phone_commInit
 * Descript: Register the callback of phone command handler. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_commInit(void)
{
	tlkmdi_comm_regCallCB(tlkmmi_phone_cmdHandler);
		
	
	return TLK_ENONE;
}



static void tlkmmi_phone_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_cmdHandler: msgID-%d", msgID);
	
	if(msgID == TLKPRT_COMM_CMDID_CALL_DIAL){
		tlkmmi_phone_recvDialCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_HOLD){
		tlkmmi_phone_recvHoldCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_REDIAL){
		tlkmmi_phone_recvRedialCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_ACCEPT){
		tlkmmi_phone_recvActiveCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_REJECT){
		tlkmmi_phone_recvRejectCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_HUNGUP){
		tlkmmi_phone_recvHungUpCmdDeal(pData, dataLen);
	}
}
static void tlkmmi_phone_recvDialCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;
	uint08 numbLen;

	if(dataLen < 2){
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	numbLen = pData[1];
	if(numbLen == 0 || numbLen+2 > dataLen){
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_dial(aclHandle, (char*)(pData+2), numbLen) == TLK_ENONE){
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_DIAL);
		}else{
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_phone_recvHoldCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_HOLD, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}
static void tlkmmi_phone_recvRedialCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: failure - param");
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_redial(aclHandle) == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: success");
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_DIAL);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: failure - reject");
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: failure - role - %d", role);
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_phone_recvActiveCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_answer(aclHandle) == TLK_ENONE){
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_ANSWER);
		}else{
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_phone_recvRejectCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_hungUp(aclHandle) == TLK_ENONE){
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_HUNGUP);
		}else{
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_phone_recvHungUpCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_hungUp(aclHandle) == TLK_ENONE){
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_HUNGUP);
		}else{
			tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkmdi_comm_sendCallRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}




#endif //#if (TLKMMI_PHONE_ENABLE)

