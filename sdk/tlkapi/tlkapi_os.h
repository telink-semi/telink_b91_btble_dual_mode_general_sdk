/********************************************************************************************************
 * @file     tlkapi_os.h
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

#ifndef TLKAPI_OS_H
#define TLKAPI_OS_H

#if (TLK_CFG_OS_ENABLE)


#include "tlklib/os/tlkos_stdio.h"


extern uint tlkos_enter_critical(void);
extern void tlkos_leave_critical(uint irqMsk);
extern uint tlkos_disable_interrupt(void);
extern void tlkos_restore_interrupt(uint irqMsk);

extern void tlkos_delay(uint value);
extern void tlkos_start_run(void);


extern int tlkos_task_create(tlkos_task_t *pTask, char *pName, uint08 priority, uint16 stackSize, 
	TlkOsTaskEnterCB enter, void *pUsrArg);
extern int tlkos_task_destory(tlkos_task_t *pTask);
extern int tlkos_task_suspend(tlkos_task_t *pTask);
extern int tlkos_task_resume(tlkos_task_t *pTask);
extern int tlkos_task_suspendAll(void);
extern int tlkos_task_resumeAll(void);
extern uint tlkos_task_getState(tlkos_task_t *pTask);
extern uint tlkos_task_getPriority(tlkos_task_t *pTask);
extern void tlkos_task_setPriority(tlkos_task_t *pTask, uint priority);


extern int tlkos_msgq_create(tlkos_msgq_t *pMsgq, uint numb, uint size);
extern int tlkos_msgq_destory(tlkos_msgq_t *pMsgq);
extern int tlkos_msgq_send(tlkos_msgq_t *pMsgq, uint08 *pData, uint16 dataLen, uint timeout);
extern int tlkos_msgq_read(tlkos_msgq_t *pMsgq, uint08 *pBuff, uint16 buffLen, uint timeout);
extern void tlkos_msgq_clear(tlkos_msgq_t *pMsgq);


extern int tlkos_mutex_create(tlkos_mutex_t *pMutex);
extern int tlkos_mutex_destory(tlkos_mutex_t *pMutex);
extern int tlkos_mutex_lock(tlkos_mutex_t *pMutex);
extern int tlkos_mutex_unlock(tlkos_mutex_t *pMutex);


extern int tlkos_timer_create(tlkos_timer_t *pTimer, char *pName, uint period, bool autoReload,
	TlkOsTimerEnterCB enter, void *pUsrArg);
extern int tlkos_timer_destory(tlkos_timer_t *pTimer);
extern int tlkos_timer_start(tlkos_timer_t *pTimer, uint delay);
extern int tlkos_timer_reset(tlkos_timer_t *pTimer);
extern int tlkos_timer_stop(tlkos_timer_t *pTimer);
extern int tlkos_timer_setPeriod(tlkos_timer_t *pTimer, uint period);


extern bool tlkos_sem_isFull(tlkos_sem_t *pSem);
extern bool tlkos_sem_isEmpty(tlkos_sem_t *pSem);
extern int tlkos_sem_create(tlkos_sem_t *pSem, uint maxCount, uint iniCount);
extern int tlkos_sem_destory(tlkos_sem_t *pSem);
extern int tlkos_sem_take(tlkos_sem_t *pSem, uint timeout);
extern int tlkos_sem_post(tlkos_sem_t *pSem);




#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKAPI_OS_H

