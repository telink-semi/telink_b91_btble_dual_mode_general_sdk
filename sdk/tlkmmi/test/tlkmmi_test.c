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
#include "tlkmmi_testAdapt.h"
#include "tlkmmi_testModinf.h"
#include "drivers.h"


static bool tlkmmi_test_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmmi_test_rebootDeal(void);


extern unsigned char gTlkWorkMode;
static tlkmmi_test_ctrl_t sTlkMmiTestCtrl;



int tlkmmi_test_init(void)
{
	tmemset(&sTlkMmiTestCtrl, 0, sizeof(tlkmmi_test_ctrl_t));
	if(gTlkWorkMode == TLK_WORK_MODE_TEST_PTS) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_PTS;
	else if(gTlkWorkMode == TLK_WORK_MODE_TEST_RDT) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_RDT;
	else if(gTlkWorkMode == TLK_WORK_MODE_TEST_FAT) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_FAT;
	else if(gTlkWorkMode == TLK_WORK_MODE_TEST_EMI) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_EMI;
	else sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_NONE;
	if(sTlkMmiTestCtrl.wmode == TLKMMI_TEST_MODTYPE_NONE) return TLK_ENONE;

	tlkmmi_test_adaptInitTimer(&sTlkMmiTestCtrl.timer, tlkmmi_test_timer, NULL, TLKMMI_TEST_TIMEOUT);
	tlkmmi_test_taskInit();	
	
	return TLK_ENONE;
}


int tlkmmi_test_start(void)
{
	return tlkmmi_test_modStart(sTlkMmiTestCtrl.wmode);
}
int tlkmmi_test_pause(void)
{
	return tlkmmi_test_modPause(sTlkMmiTestCtrl.wmode);
}
int tlkmmi_test_close(void)
{
	return tlkmmi_test_modClose(sTlkMmiTestCtrl.wmode);
}
int tlkmmi_test_input(uint16 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_TEST_DBG_FLAG, TLKMMI_TEST_DBG_SIGN, "tlkmmi_test_input->%d", sTlkMmiTestCtrl.wmode);
	return tlkmmi_test_modInput(sTlkMmiTestCtrl.wmode, msgID, pData, dataLen);
}
void tlkmmi_test_reboot(uint16 timeout)
{
	tlkmmi_test_close();
	sTlkMmiTestCtrl.rebootTime = timeout/TLKMMI_TEST_TIMEOUT;
	if(timeout != 0 && sTlkMmiTestCtrl.rebootTime == 0) sTlkMmiTestCtrl.rebootTime = 1;
	if(sTlkMmiTestCtrl.rebootTime == 0){
		core_reboot();
	}else{
		sTlkMmiTestCtrl.busys |= TLKMMI_TEST_BUSY_REBOOT;
		tlkmmi_test_adaptInsertTimer(&sTlkMmiTestCtrl.timer);
	}
}
void tlkmmi_test_handler(void)
{
	tlkmmi_test_modHandler(sTlkMmiTestCtrl.wmode);
}


static bool tlkmmi_test_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if((sTlkMmiTestCtrl.busys & TLKMMI_TEST_BUSY_REBOOT) != 0)
	{
		sTlkMmiTestCtrl.busys &= ~TLKMMI_TEST_BUSY_REBOOT;
		tlkmmi_test_rebootDeal();
	}
	return true;
}
static void tlkmmi_test_rebootDeal(void)
{
	if(sTlkMmiTestCtrl.rebootTime != 0) sTlkMmiTestCtrl.rebootTime--;
	if(sTlkMmiTestCtrl.rebootTime != 0){
		sTlkMmiTestCtrl.busys |= TLKMMI_TEST_BUSY_REBOOT;
	}else{
		core_reboot();
	}
}



#endif //#if (TLKMMI_TEST_ENABLE)

