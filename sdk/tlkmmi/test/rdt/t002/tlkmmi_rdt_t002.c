/********************************************************************************************************
 * @file     tlkmmi_rdt_t002.c
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
#if (TLKMMI_RDT_CASE_T002_ENABLE)
#include "tlkmmi_rdt_t002.h"
#include "tlkmmi_rdt_t002Bt.h"



TLKMMI_RDT_MODINF_DEFINE(002);


static bool tlkmmi_rdt_t002Timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmmi_rdt_t002_t sTlkMmiRdtT002 = {0};


int tlkmmi_rdt_t002GetRole(void)
{
	return sTlkMmiRdtT002.devRole;
}
int tlkmmi_rdt_t002GetState(void)
{
	return sTlkMmiRdtT002.state;
}


static int tlkmmi_rdt_t002State(void)
{


	return TLK_STATE_CLOSED;
}
static int tlkmmi_rdt_t002Start(uint08 role)
{
	uint08 state;

	if(role != TLKMMI_RDT_ROLE_DUT && role != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start: error role[%d]",
			role);
		return -TLK_EROLE;
	}
	if(sTlkMmiRdtT002.state != TLK_STATE_CLOSED && sTlkMmiRdtT002.state != TLK_STATE_PAUSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start:role[%d]",role);

	state = sTlkMmiRdtT002.state;
	sTlkMmiRdtT002.devRole = role;
	sTlkMmiRdtT002.state = TLK_STATE_OPENED;
	if(state == TLK_STATE_CLOSED){
		tlkmmi_rdt_t002BtStart();
	}

	tlkmmi_test_adaptInitTimer(&sTlkMmiRdtT002.timer, tlkmmi_rdt_t002Timer,
		nullptr, 200000);
	tlkmmi_test_adaptInsertTimer(&sTlkMmiRdtT002.timer);

	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Pause(void)
{
	if(sTlkMmiRdtT002.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Pause: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT002.timer);
	sTlkMmiRdtT002.state = TLK_STATE_PAUSED;
	tlkmmi_rdt_t002BtClose();
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Pause");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Close(void)
{
	if(sTlkMmiRdtT002.state == TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Close: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT002.timer);
	sTlkMmiRdtT002.state = TLK_STATE_CLOSED;
	tlkmmi_rdt_t002BtClose();
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Close");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(sTlkMmiRdtT002.state != TLK_STATE_PAUSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Input: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Input: msgID[0x%x]", msgID);
	tlkmmi_rdt_t002BtInput(msgID, pData, dataLen);
	return TLK_ENONE;
}


static bool tlkmmi_rdt_t002Timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(sTlkMmiRdtT002.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Timer: error status[%d]",
			sTlkMmiRdtT002.state);
		return false;
	}
	tlkmmi_rdt_t002BtTimer();
	return true;
}



#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

