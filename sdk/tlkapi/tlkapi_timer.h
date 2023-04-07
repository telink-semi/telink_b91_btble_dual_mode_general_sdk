/********************************************************************************************************
 * @file     tlkmdi_timer.h
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
#ifndef TLKAPI_TIMER_H
#define TLKAPI_TIMER_H



int tlkapi_timer_init(void);


/******************************************************************************
 * Function: tlkapi_timer_isbusy, tlkapi_timer_isPmBusy
 * Descript: If there is a process or timer and less than 1ms, the module 
 *           is considered busy, it is the basis of the schdule. 
 * Params:  
 * Return: true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isbusy(void);
bool tlkapi_timer_isPmBusy(void);

void tlkapi_timer_handler(void);

/******************************************************************************
 * Function: tlkapi_timer_isout
 * Descript: Detects whether the current timer has timed out.
 * Params:
 *     @timer[IN]--Reference timer.
 *     @ticks[IN]--Timeout. unit: 1/16 us:
 * Return: True means the timer is timeout, false means not.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isout(uint32 timer, uint32 ticks);

/******************************************************************************
 * Function: tlkapi_timer_init
 * Descript: 
 * Params:
 *     @pTimer[IN]--
 *     @pUsrArg[IN]--
 *     @timeout[IN]--Unit: us.
 *     @timerCB[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_initNode(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);

/******************************************************************************
 * Function: tlkmdi_timer_isHave
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pTimer[IN]--
 *     @pProcs[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isHaveNode(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkmdi_timer_update
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 *     @timeout[IN]--Unit: us.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_updateNode(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);

/******************************************************************************
 * Function: tlkmdi_timer_insert
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_insertNode(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkmdi_timer_remove
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 * 	@pAdapt[IN]--The adapt self-manages handle.
 * 	@pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_removeNode(tlkapi_timer_t *pTimer);

uint tlkapi_timer_interval(void);




#endif //#ifndef TLKAPI_TIMER_H

