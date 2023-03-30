/********************************************************************************************************
 * @file     tlkmmi_rdt_t001.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#if (TLKMMI_RDT_CASE_T001_ENABLE)
#include "tlkmmi_rdt_t001.h"
#include "tlkmmi_rdt_t001Bt.h"
#include "tlkmmi_rdt_t001Le.h"


TLKMMI_RDT_MODINF_DEFINE(001);
static bool tlkmmi_rdt_t001Timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmmi_rdt_t001_t sTlkMmiRdtT001 = {0};


int tlkmmi_rdt_t001GetRole(void)
{
	return sTlkMmiRdtT001.devRole;
}
int tlkmmi_rdt_t001GetState(void)
{
	return sTlkMmiRdtT001.state;
}


static int tlkmmi_rdt_t001State(void)
{
	
	
	return TLK_STATE_CLOSED;
}
static int tlkmmi_rdt_t001Start(uint08 role)
{
	uint08 state;
	
	if(role != TLKMMI_RDT_ROLE_DUT && role != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Start: error role[%d]",
			role);
		return -TLK_EROLE;
	}
	if(sTlkMmiRdtT001.state != TLK_STATE_CLOSED && sTlkMmiRdtT001.state != TLK_STATE_PAUSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Start: error status[%d]",
			sTlkMmiRdtT001.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Start");

	state = sTlkMmiRdtT001.state;
	sTlkMmiRdtT001.devRole = role;
	sTlkMmiRdtT001.state = TLK_STATE_OPENED;
	if(state == TLK_STATE_CLOSED){		
		tlkmmi_rdt_t001BtStart();
		tlkmmi_rdt_t001LeStart();
	}
	
	tlkmmi_test_adaptInitTimer(&sTlkMmiRdtT001.timer, tlkmmi_rdt_t001Timer, 
		nullptr, 200000);
	tlkmmi_test_adaptInsertTimer(&sTlkMmiRdtT001.timer);
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t001Pause(void)
{
	if(sTlkMmiRdtT001.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Pause: error status[%d]",
			sTlkMmiRdtT001.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT001.timer);
	sTlkMmiRdtT001.state = TLK_STATE_PAUSED;
	tlkmmi_rdt_t001BtClose();
	tlkmmi_rdt_t001LeClose();
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Pause");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t001Close(void)
{
	if(sTlkMmiRdtT001.state == TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Close: error status[%d]",
			sTlkMmiRdtT001.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT001.timer);
	sTlkMmiRdtT001.state = TLK_STATE_CLOSED;
	tlkmmi_rdt_t001BtClose();
	tlkmmi_rdt_t001LeClose();
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Close");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t001Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(sTlkMmiRdtT001.state != TLK_STATE_PAUSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Input: error status[%d]",
			sTlkMmiRdtT001.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Input: msgID[0x%x]", msgID);
	tlkmmi_rdt_t001BtInput(msgID, pData, dataLen);
	return TLK_ENONE;
}


static bool tlkmmi_rdt_t001Timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(sTlkMmiRdtT001.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t001Timer: error status[%d]",
			sTlkMmiRdtT001.state);
		return false;
	}
	tlkmmi_rdt_t001BtTimer();
	tlkmmi_rdt_t001LeTimer();
	return true;
}




#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

