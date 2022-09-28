/********************************************************************************************************
 * @file     tlkos_adapt.h
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
#ifndef TLKOS_ADAPT_H
#define TLKOS_ADAPT_H

#if (TLK_CFG_OS_ENABLE)




int  tlkos_adapt_init(void);
void tlkos_adapt_handler(void);

int  tlkos_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlkos_adapt_initProcs(tlkapi_procs_t *pProcs, TlkApiProcsCB procsCB, uint32 userArg);
void tlkos_adapt_deinitTimer(tlkapi_timer_t *pTimer);
void tlkos_adapt_deinitProcs(tlkapi_procs_t *pProcs);

bool tlkos_adapt_isHaveTimer(tlkapi_timer_t *pTimer);
bool tlkos_adapt_isHaveProcs(tlkapi_procs_t *pProcs);

int  tlkos_adapt_appendProcs(tlkapi_procs_t *pProcs);
int  tlkos_adapt_removeProcs(tlkapi_procs_t *pProcs);


int  tlkos_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  tlkos_adapt_insertTimer(tlkapi_timer_t *pTimer);
int  tlkos_adapt_removeTimer(tlkapi_timer_t *pTimer);





#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_ADAPT_H

