/********************************************************************************************************
 * @file     tlkapi_adapt.h
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

#ifndef TLKAPI_ADAPT_H
#define TLKAPI_ADAPT_H



#define TLKAPI_TIMEOUT_MAX      (0x07FFFFFF)// BIT(31) >> 4
#define TLKAPI_TIMEOUT_MIN      50  //unit: us



typedef struct tlkapi_timer_s  tlkapi_timer_t;
typedef struct tlkapi_procs_s  tlkapi_procs_t;


/******************************************************************************
 * Function: LstkProcsCB, LstkTimerCB
 * Descript: Defines the format of the user callback interface.
 * Params:
 *     @pProcs[IN]--The node of Process Control.
 *     @pTimer[IN]--The node of Timer Control.
 *     @pUsrArg[IN]--User Argument.
 * Return: Returning TRUE will automatically start the next round of detection.
 *     If FALSE is returned, the node is automatically removed.
*******************************************************************************/
typedef bool(*TlkApiProcsCB)(tlkapi_procs_t *pProcs, void *pUsrArg);
typedef bool(*TlkApiTimerCB)(tlkapi_timer_t *pTimer, void *pUsrArg);


struct tlkapi_timer_s{
	uint32 arrival;
	uint32 timeout;
	void  *pUsrArg;
	TlkApiTimerCB timerCB;
	struct tlkapi_timer_s *pNext;
};
struct tlkapi_procs_s{
	void *pUsrArg;
	TlkApiProcsCB procsCB;
	struct tlkapi_procs_s *pNext;
};
typedef struct{
	uint08 isUpdate;
	uint08 reserve0;
	uint16 reserve1;
	tlkapi_procs_t *pProcsList; //Circulation list
	tlkapi_timer_t *pTimerList; //Singly linked list
}tlkapi_adapt_t;





int  tlkapi_adapt_init(tlkapi_adapt_t *pAdapt);
void tlkapi_adapt_handler(tlkapi_adapt_t *pAdapt);

int  tlkapi_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, void *pUsrArg, uint32 timeout);
int  tlkapi_adapt_initProcs(tlkapi_procs_t *pProcs, TlkApiProcsCB procsCB, void *pUsrArg);
void tlkapi_adapt_deinitTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);
void tlkapi_adapt_deinitProcs(tlkapi_adapt_t *pAdapt, tlkapi_procs_t *pProcs);

bool tlkapi_adapt_isHaveTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);
bool tlkapi_adapt_isHaveProcs(tlkapi_adapt_t *pAdapt, tlkapi_procs_t *pProcs);

int  tlkapi_adapt_appendProcs(tlkapi_adapt_t *pAdapt, tlkapi_procs_t *pProcs);
int  tlkapi_adapt_removeProcs(tlkapi_adapt_t *pAdapt, tlkapi_procs_t *pProcs);

int  tlkapi_adapt_updateTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
/******************************************************************************
 * Function: tlkapi_adapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int  tlkapi_adapt_insertTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer, bool isUpdate);
int  tlkapi_adapt_removeTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);

uint tlkapi_adapt_timerInterval(tlkapi_adapt_t *pAdapt);
tlkapi_timer_t *tlkapi_adapt_takeFirstTimer(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_timer_compare
 * Descript: 
 * Params:
 *     @curTicks[IN]--
 *     @refTicks[IN]--
 * Return: 1:refTicks First to arrive; -1-curTicks First to arrive.
 * Others: None.
*******************************************************************************/
int tlkapi_adapt_timerCompare(uint32 curTicks, uint32 refTicks);


void tlkapi_adapt_printTimer(tlkapi_adapt_t *pAdapt);
void tlkapi_adapt_checkTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);



#endif //TLKAPI_ADAPT_H

