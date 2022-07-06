/********************************************************************************************************
 * @file     tlkapp_adapt.h
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

#ifndef TLKAPP_ADAPT_H
#define TLKAPP_ADAPT_H


/******************************************************************************
 * Function: tlkapp_adapt_init
 * Descript: 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
int  tlkapp_adapt_init(void);

/******************************************************************************
 * Function: tlkapp_adapt_run_once
 * Descript: 
 * Params:
 *     @adapt[IN]--The adapt self-manages handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_adapt_handler(void);

/******************************************************************************
 * Function: tlkapp_adapt_initTimer
 * Descript: 
 * Params:
 *     @pTimer[IN]--
 *     @pUsrArg[IN]--
 *     @timeout[IN]--Unit: us.
 *     @timerCB[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int  tlkapp_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, void *pUsrArg, uint32 timeout);
int  tlkapp_adapt_initProcs(tlkapi_procs_t *pProcs, TlkApiProcsCB procsCB, void *pUsrArg);
void tlkapp_adapt_deinitTimer(tlkapi_timer_t *pTimer);
void tlkapp_adapt_deinitProcs(tlkapi_procs_t *pProcs);

/******************************************************************************
 * Function: tlkapp_adapt_isHaveTimer, tlkapp_adapt_isHaveProcs
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pTimer[IN]--
 *     @pProcs[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
bool tlkapp_adapt_isHaveTimer(tlkapi_timer_t *pTimer);
bool tlkapp_adapt_isHaveProcs(tlkapi_procs_t *pProcs);

/******************************************************************************
 * Function: tlkapp_adapt_appendAgent, tlkapp_adapt_removeAgent
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pAgent[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int  tlkapp_adapt_appendProcs(tlkapi_procs_t *pProcs);
int  tlkapp_adapt_removeProcs(tlkapi_procs_t *pProcs);

/******************************************************************************
 * Function: tlkapp_adapt_updateTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 *     @timeout[IN]--Unit: us.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlkapp_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout);

/******************************************************************************
 * Function: tlkapp_adapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/

int  tlkapp_adapt_insertTimer(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkapp_adapt_removeTimer
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlkapp_adapt_removeTimer(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkapp_adapt_printfTimer
 * Descript:
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_adapt_printfTimer(void);



#endif //TLKAPP_ADAPT_H
