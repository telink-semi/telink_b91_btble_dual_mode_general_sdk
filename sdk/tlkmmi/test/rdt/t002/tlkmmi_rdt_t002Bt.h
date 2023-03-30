/********************************************************************************************************
 * @file     tlkmmi_rdt_t002Bt.h
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
#ifndef TLKMMI_RDT_T002_BT_H
#define TLKMMI_RDT_T002_BT_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T002_ENABLE)




typedef enum{
	RDT_CASE_T002_BTBUSY_NONE = 0,
	RDT_CASE_T002_BTBUSY_WAIT_PEER_ATTR = 0x0002,
	RDT_CASE_T002_BTBUSY_START_SCAN = 0x0002,
	RDT_CASE_T002_BTBUSY_CLOSE_SCAN = 0x0004,
}RDT_CASE_T002_BTBUSY_ENUM;


typedef struct{
	uint16 busys;
	uint08 initRole;
	uint32 devClass;
	uint08 btaddr[6];
}tlkmmi_rdt_t002Bt_t;


void tlkmmi_rdt_t002BtStart(void);
void tlkmmi_rdt_t002BtClose(void);
bool tlkmmi_rdt_t002BtTimer(void);

void tlkmmi_rdt_t002BtInput(uint08 msgID, uint08 *pData, uint16 dataLen);


#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T002_BT_H

