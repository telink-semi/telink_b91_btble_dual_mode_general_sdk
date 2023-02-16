/********************************************************************************************************
 * @file     tlkmmi_test.c
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
#include "tlkmmi_test.h"
#include "tlkmmi_testTask.h"
#include "tlkmmi_testModinf.h"


extern unsigned char gTlkWorkMode;
uint08 gTlkMmiTestWorkMode = TLKMMI_TEST_MODTYPE_NONE;

int tlkmmi_test_init(void)
{
	if(gTlkWorkMode == TLK_WORK_MODE_TEST_PTS) gTlkMmiTestWorkMode = TLKMMI_TEST_MODTYPE_PTS;
	else if(gTlkWorkMode == TLK_WORK_MODE_TEST_RDT) gTlkMmiTestWorkMode = TLKMMI_TEST_MODTYPE_RDT;
	else if(gTlkWorkMode == TLK_WORK_MODE_TEST_FAT) gTlkMmiTestWorkMode = TLKMMI_TEST_MODTYPE_FAT;
	else if(gTlkWorkMode == TLK_WORK_MODE_TEST_EMI) gTlkMmiTestWorkMode = TLKMMI_TEST_MODTYPE_EMI;
	else gTlkMmiTestWorkMode = TLKMMI_TEST_MODTYPE_NONE;
	if(gTlkMmiTestWorkMode == TLKMMI_TEST_MODTYPE_NONE) return TLK_ENONE;
	
	tlkmmi_test_taskInit();
	
	return TLK_ENONE;
}






#endif //#if (TLKMMI_TEST_ENABLE)

