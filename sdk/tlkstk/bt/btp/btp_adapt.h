/********************************************************************************************************
 * @file     btp_adapt.h
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

#ifndef BTP_ADAPT_H
#define BTP_ADAPT_H

#if (TLK_STK_BTP_ENABLE)


int  btp_adapt_init(void);
void btp_adapt_handler(void);

int  btp_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  btp_adapt_initProcs(tlkapi_procs_t *pProcs, TlkApiProcsCB procsCB, uint32 userArg);
void btp_adapt_deinitTimer(tlkapi_timer_t *pTimer);
void btp_adapt_deinitProcs(tlkapi_procs_t *pProcs);

bool btp_adapt_isHaveTimer(tlkapi_timer_t *pTimer);
bool btp_adapt_isHaveProcs(tlkapi_procs_t *pProcs);

int  btp_adapt_appendProcs(tlkapi_procs_t *pProcs);
int  btp_adapt_removeProcs(tlkapi_procs_t *pProcs);


int  btp_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  btp_adapt_insertTimer(tlkapi_timer_t *pTimer);
int  btp_adapt_removeTimer(tlkapi_timer_t *pTimer);

void btp_adapt_printfTimer(void);


#endif //#if (TLK_STK_BTP_ENABLE)

#endif //BTP_ADAPT_H

