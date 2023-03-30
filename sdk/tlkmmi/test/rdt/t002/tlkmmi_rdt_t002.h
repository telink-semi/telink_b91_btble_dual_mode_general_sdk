/********************************************************************************************************
 * @file     tlkmmi_rdt_t002.h
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
#ifndef TLKMMI_RDT_T002_H
#define TLKMMI_RDT_T002_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T002_ENABLE)


typedef enum{
	TLKMMI_RDT_T002_MSGID_NONE = 0,
	TLKMMI_RDT_T002_MSGID_SET_PEER_INFO = 1,
}TLKMMI_RDT_T002_MSGID_ENUM;


typedef struct{
	uint08 state;
	uint08 stage;
	uint08 devRole;
	tlkapi_timer_t timer;
}tlkmmi_rdt_t002_t;


int tlkmmi_rdt_t002GetRole(void);
int tlkmmi_rdt_t002GetState(void);







#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T002_H

