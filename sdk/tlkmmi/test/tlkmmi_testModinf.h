/********************************************************************************************************
 * @file     tlkmmi_testModinf.h
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
#ifndef TLKMMI_TEST_MODINF_H
#define TLKMMI_TEST_MODINF_H

#if (TLKMMI_TEST_ENABLE)



typedef enum
{
	TLKMMI_TEST_MODTYPE_NONE = 0,
	TLKMMI_TEST_MODTYPE_PTS,
	TLKMMI_TEST_MODTYPE_FAT,
	TLKMMI_TEST_MODTYPE_RDT,
	TLKMMI_TEST_MODTYPE_EMI,
	TLKMMI_TEST_MODTYPE_USR,
	TLKMMI_TEST_MODTYPE_MAX,
}TLKMMI_TEST_MODTYPE_ENUM;


int tlkmmi_test_modStart(TLKMMI_TEST_MODTYPE_ENUM type);
int tlkmmi_test_modPause(TLKMMI_TEST_MODTYPE_ENUM type);
int tlkmmi_test_modClose(TLKMMI_TEST_MODTYPE_ENUM type);
int tlkmmi_test_modInput(TLKMMI_TEST_MODTYPE_ENUM type, uint16 msgID, 
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);



#endif //#if (TLKMMI_TEST_MODINF_H)

#endif //TLKMMI_TEST_TASK_H

