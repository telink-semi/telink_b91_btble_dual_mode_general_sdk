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



TLKMMI_RDT_MODINF_DEFINE(002);



static int tlkmmi_rdt_t002State(void)
{
	return TLK_STATE_CLOSED;
}
static int tlkmmi_rdt_t002Start(uint08 role)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Pause(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Pause");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Close(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Close");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Input: msgID[0x%x]", msgID);
	return TLK_ENONE;
}




#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

