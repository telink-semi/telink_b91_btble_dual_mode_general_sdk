/********************************************************************************************************
 * @file     tlkapp_adapt.c
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
#include "tlkapp_config.h"
#include "tlkapp_adapt.h"



static tlkapi_adapt_t sTlkAppAdapt;



/******************************************************************************
 * Function: tlkapp_adapt_init
 * Descript: 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlkapp_adapt_init(void)
{
	return tlkapi_adapt_init(&sTlkAppAdapt);
}

/******************************************************************************
 * Function: tlkapp_adapt_run_once
 * Descript: 
 * Params:
 *     @adapt[IN]--The adapt self-manages handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_adapt_handler(void)
{
	tlkapi_adapt_handler(&sTlkAppAdapt);
}

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
int tlkapp_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, void *pUsrArg, uint32 timeout)
{
	if(tlkapp_adapt_isHaveTimer(pTimer)) return -TLK_EEXIST;
	return tlkapi_adapt_initTimer(pTimer, timerCB, pUsrArg, timeout);
}
int tlkapp_adapt_initProcs(tlkapi_procs_t *pProcs, TlkApiProcsCB procsCB, void *pUsrArg)
{
	if(tlkapp_adapt_isHaveProcs(pProcs)) return -TLK_EEXIST;
	return tlkapi_adapt_initProcs(pProcs, procsCB, pUsrArg);
}
void tlkapp_adapt_deinitTimer(tlkapi_timer_t *pTimer)
{
	tlkapi_adapt_deinitTimer(&sTlkAppAdapt, pTimer);
}
void tlkapp_adapt_deinitProcs(tlkapi_procs_t *pProcs)
{
	tlkapi_adapt_deinitProcs(&sTlkAppAdapt, pProcs);
}

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
bool tlkapp_adapt_isHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_isHaveTimer(&sTlkAppAdapt, pTimer);
}
bool tlkapp_adapt_isHaveProcs(tlkapi_procs_t *pProcs)
{
	return tlkapi_adapt_isHaveProcs(&sTlkAppAdapt, pProcs);
}

/******************************************************************************
 * Function: tlkapp_adapt_appendAgent, tlkapp_adapt_removeAgent
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pAgent[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlkapp_adapt_appendProcs(tlkapi_procs_t *pProcs)
{	
	return tlkapi_adapt_appendProcs(&sTlkAppAdapt, pProcs);
}
int tlkapp_adapt_removeProcs(tlkapi_procs_t *pProcs)
{
	return tlkapi_adapt_removeProcs(&sTlkAppAdapt, pProcs);
}

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
int tlkapp_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout)
{
	return tlkapi_adapt_updateTimer(&sTlkAppAdapt, pTimer, timeout);
}
/******************************************************************************
 * Function: tlkapp_adapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkapp_adapt_insertTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_insertTimer(&sTlkAppAdapt, pTimer, true);
}
/******************************************************************************
 * Function: tlkapp_adapt_removeTimer
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkapp_adapt_removeTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_removeTimer(&sTlkAppAdapt, pTimer);
}

/******************************************************************************
 * Function: tlkapp_adapt_printfTimer
 * Descript:
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_adapt_printfTimer(void)
{
	return tlkapi_adapt_printTimer(&sTlkAppAdapt);
}



