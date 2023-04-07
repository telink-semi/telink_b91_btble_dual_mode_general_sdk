/********************************************************************************************************
 * @file     tlksys_pm.h
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
#ifndef TLKSYS_PM_H
#define TLKSYS_PM_H

#if (TLK_CFG_SYS_ENABLE)



#define TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB      10
#define TLKSYS_PM_ENTER_SLEEP_FUNC_MAX_NUMB     4
#define TLKSYS_PM_LEAVE_SLEEP_FUNC_MAX_NUMB     4


typedef bool(*TlkSysPmBusyCheckCallback)(void);
typedef void(*TlkSysPmEnterSleepCallback)(uint mode);
typedef void(*TlkSysPmLeaveSleepCallback)(uint wakeSrc);


typedef struct{
	uint08 reserve;
	uint08 busyCheckCount;
	uint08 enterSleepCount;
	uint08 leaveSleepCount;
	TlkSysPmBusyCheckCallback  busyCheck[TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB];
	TlkSysPmEnterSleepCallback enterSleep[TLKSYS_PM_ENTER_SLEEP_FUNC_MAX_NUMB];
	TlkSysPmLeaveSleepCallback leaveSleep[TLKSYS_PM_LEAVE_SLEEP_FUNC_MAX_NUMB];
}tlksys_pm_cbCtrl_t;


int tlksys_pm_init(void);

int tlksys_pm_appendBusyCheckCB(TlkSysPmBusyCheckCallback cb);
int tlksys_pm_removeBusyCheckCB(TlkSysPmBusyCheckCallback cb);

int tlksys_pm_appendEnterSleepCB(TlkSysPmEnterSleepCallback cb);
int tlksys_pm_removeEnterSleepCB(TlkSysPmEnterSleepCallback cb);

int tlksys_pm_appendLeaveSleepCB(TlkSysPmLeaveSleepCallback cb);
int tlksys_pm_removeLeaveSleepCB(TlkSysPmLeaveSleepCallback cb);


bool tlksys_pm_isBusy(void);
void tlksys_pm_enterSleep(uint mode);
void tlksys_pm_leaveSleep(uint wake);



#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_PM_H

