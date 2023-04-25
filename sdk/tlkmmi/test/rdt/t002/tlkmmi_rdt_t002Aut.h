/********************************************************************************************************
 * @file     tlkmmi_rdt_t002Aut.h
 *
 * @brief    This is the header file for BTBLE SDK
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
#ifndef TLKMMI_RDT_T002_AUT_H
#define TLKMMI_RDT_T002_AUT_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T002_ENABLE)




typedef struct{
	uint08 enable;
	uint16 minIntval;
	uint16 maxIntval;
}tlkmmi_rdt_t002AutConn_t;
typedef struct{
	uint08 flags;
	uint08 busys;
	uint16 handle;
	uint32 intval;
	uint32 timer;
}tlkmmi_rdt_t003AutUnit_t;

typedef struct{
	uint08 isStart;
	uint16 sndCount;
	uint16 rcvCount;
	uint32 dbgTimer;
	uint32 sndTimer;
	uint32 rcvTimer;
	tlkmmi_rdt_t003AutUnit_t acl;
	tlkmmi_rdt_t003AutUnit_t sco;
}tlkmmi_rdt_t002Aut_t;


void tlkmmi_rdt_t002AutStart(void);
void tlkmmi_rdt_t002AutClose(void);
void tlkmmi_rdt_t002AutTimer(void);
void tlkmmi_rdt_t002AutInput(uint08 msgID, uint08 *pData, uint16 dataLen);




#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T002_AUT_H

