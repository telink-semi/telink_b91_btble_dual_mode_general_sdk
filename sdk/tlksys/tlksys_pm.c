/********************************************************************************************************
 * @file     tlksys_task.c
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
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys_stdio.h"
#include "tlksys.h"
#include "tlksys_pm.h"


static tlksys_pm_cbCtrl_t sTlkSysPmCbCtrl = {0};


int tlksys_pm_init(void)
{
	return TLK_ENONE;
}

int tlksys_pm_appendBusyCheckCB(TlkSysPmBusyCheckCallback cb)
{
	if(sTlkSysPmCbCtrl.busyCheckCount >= TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB){
		return -TLK_EQUOTA;
	}
	sTlkSysPmCbCtrl.busyCheck[sTlkSysPmCbCtrl.busyCheckCount++] = cb;
	return TLK_ENONE;
}
int tlksys_pm_removeBusyCheckCB(TlkSysPmBusyCheckCallback cb)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.busyCheckCount; index++){
		if(sTlkSysPmCbCtrl.busyCheck[index] == 0 || sTlkSysPmCbCtrl.busyCheck[index] == cb) break;
	}
	if(index == sTlkSysPmCbCtrl.busyCheckCount){
		return -TLK_ENOOBJECT;
	}
	if(index+1 == sTlkSysPmCbCtrl.busyCheckCount){
		sTlkSysPmCbCtrl.busyCheck[index] = 0;
	}else{
		tmemcpy(sTlkSysPmCbCtrl.busyCheck[index], sTlkSysPmCbCtrl.busyCheck[index+1], 
			(sTlkSysPmCbCtrl.busyCheckCount-index-1)*sizeof(TlkSysPmBusyCheckCallback));
	}
	sTlkSysPmCbCtrl.busyCheckCount --;
	return TLK_ENONE;
}

int tlksys_pm_appendEnterSleepCB(TlkSysPmEnterSleepCallback cb)
{
	if(sTlkSysPmCbCtrl.enterSleepCount >= TLKSYS_PM_ENTER_SLEEP_FUNC_MAX_NUMB){
		return -TLK_EQUOTA;
	}
	sTlkSysPmCbCtrl.enterSleep[sTlkSysPmCbCtrl.enterSleepCount++] = cb;
	return TLK_ENONE;
}
int tlksys_pm_removeEnterSleepCB(TlkSysPmEnterSleepCallback cb)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.enterSleepCount; index++){
		if(sTlkSysPmCbCtrl.enterSleep[index] == nullptr || sTlkSysPmCbCtrl.enterSleep[index] == cb) break;
	}
	if(index == sTlkSysPmCbCtrl.enterSleepCount){
		return -TLK_ENOOBJECT;
	}
	if(index+1 == sTlkSysPmCbCtrl.enterSleepCount){
		sTlkSysPmCbCtrl.enterSleep[index] = 0;
	}else{
		tmemcpy(sTlkSysPmCbCtrl.enterSleep[index], sTlkSysPmCbCtrl.enterSleep[index+1], 
			(sTlkSysPmCbCtrl.enterSleepCount-index-1)*sizeof(TlkSysPmBusyCheckCallback));
	}
	sTlkSysPmCbCtrl.enterSleepCount --;
	return TLK_ENONE;
}

int tlksys_pm_appendLeaveSleepCB(TlkSysPmLeaveSleepCallback cb)
{
	if(sTlkSysPmCbCtrl.leaveSleepCount >= TLKSYS_PM_LEAVE_SLEEP_FUNC_MAX_NUMB){
		return -TLK_EQUOTA;
	}
	sTlkSysPmCbCtrl.leaveSleep[sTlkSysPmCbCtrl.leaveSleepCount++] = cb;
	return TLK_ENONE;
}
int tlksys_pm_removeLeaveSleepCB(TlkSysPmLeaveSleepCallback cb)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.leaveSleepCount; index++){
		if(sTlkSysPmCbCtrl.leaveSleep[index] == nullptr || sTlkSysPmCbCtrl.leaveSleep[index] == cb) break;
	}
	if(index == sTlkSysPmCbCtrl.leaveSleepCount){
		return -TLK_ENOOBJECT;
	}
	if(index+1 == sTlkSysPmCbCtrl.leaveSleepCount){
		sTlkSysPmCbCtrl.leaveSleep[index] = 0;
	}else{
		tmemcpy(sTlkSysPmCbCtrl.leaveSleep[index], sTlkSysPmCbCtrl.leaveSleep[index+1], 
			(sTlkSysPmCbCtrl.leaveSleepCount-index-1)*sizeof(TlkSysPmBusyCheckCallback));
	}
	sTlkSysPmCbCtrl.leaveSleepCount --;
	return TLK_ENONE;
}


bool tlksys_pm_isBusy(void)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.busyCheckCount; index++){
		if(sTlkSysPmCbCtrl.busyCheck[index] != nullptr && sTlkSysPmCbCtrl.busyCheck[index]()) break;
	}
	if(index == sTlkSysPmCbCtrl.busyCheckCount) return false;
	return true;
}

void tlksys_pm_enterSleep(uint mode)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.enterSleepCount; index++){
		if(sTlkSysPmCbCtrl.enterSleep[index] != nullptr) sTlkSysPmCbCtrl.enterSleep[index](mode);
	}
}
void tlksys_pm_leaveSleep(uint wake)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.leaveSleepCount; index++){
		if(sTlkSysPmCbCtrl.leaveSleep[index] != nullptr) sTlkSysPmCbCtrl.leaveSleep[index](wake);
	}
}


#endif //#if (TLK_CFG_SYS_ENABLE)

