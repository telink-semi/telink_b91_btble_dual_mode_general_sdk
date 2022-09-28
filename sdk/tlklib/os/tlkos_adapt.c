/********************************************************************************************************
 * @file     tlkos_adapt.c
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
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_adapt.h"


static tlkapi_adapt_t sTlkOsAdapt;



/******************************************************************************
 * Function: tlkos_adapt_init
 * Descript: 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_init(void)
{
	return tlkapi_adapt_init(&sTlkOsAdapt);
}


/******************************************************************************
 * Function: tlkos_adapt_run_once
 * Descript: 
 * Params:
 *     @adapt[IN]--The adapt self-manages handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkos_adapt_handler(void)
{
	tlkapi_adapt_handler(&sTlkOsAdapt);
}


/******************************************************************************
 * Function: tlkos_adapt_initTimer
 * Descript: 
 * Params:
 *     @pTimer[IN]--
 *     @userArg[IN]--
 *     @timeout[IN]--Unit: us.
 *     @timerCB[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	if(tlkos_adapt_isHaveTimer(pTimer)) return -TLK_EEXIST;
	return tlkapi_adapt_initTimer(pTimer, timerCB, userArg, timeout);
}
int tlkos_adapt_initProcs(tlkapi_procs_t *pProcs, TlkApiProcsCB procsCB, uint32 userArg)
{
	if(tlkos_adapt_isHaveProcs(pProcs)) return -TLK_EEXIST;
	return tlkapi_adapt_initProcs(pProcs, procsCB, userArg);
}
void tlkos_adapt_deinitTimer(tlkapi_timer_t *pTimer)
{
	tlkapi_adapt_deinitTimer(&sTlkOsAdapt, pTimer);
}
void tlkos_adapt_deinitProcs(tlkapi_procs_t *pProcs)
{
	tlkapi_adapt_deinitProcs(&sTlkOsAdapt, pProcs);
}

/******************************************************************************
 * Function: tlkos_adapt_isHaveTimer, tlkos_adapt_isHaveProcs
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pTimer[IN]--
 *     @pProcs[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
bool tlkos_adapt_isHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_isHaveTimer(&sTlkOsAdapt, pTimer);
}
bool tlkos_adapt_isHaveProcs(tlkapi_procs_t *pProcs)
{
	return tlkapi_adapt_isHaveProcs(&sTlkOsAdapt, pProcs);
}

/******************************************************************************
 * Function: tlkos_adapt_appendAgent, tlkos_adapt_removeAgent
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pAgent[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_appendProcs(tlkapi_procs_t *pProcs)
{	
	return tlkapi_adapt_appendProcs(&sTlkOsAdapt, pProcs);
}
int tlkos_adapt_removeProcs(tlkapi_procs_t *pProcs)
{
	return tlkapi_adapt_removeProcs(&sTlkOsAdapt, pProcs);
}

/******************************************************************************
 * Function: tlkos_adapt_updateTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 *     @timeout[IN]--Unit: us.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlkapi_adapt_updateTimer(&sTlkOsAdapt, pTimer, timeout, isInsert);
}
/******************************************************************************
 * Function: tlkos_adapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_insertTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_insertTimer(&sTlkOsAdapt, pTimer, true);
}
/******************************************************************************
 * Function: tlkos_adapt_removeTimer
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_removeTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_removeTimer(&sTlkOsAdapt, pTimer);
}





#endif //#if (TLK_CFG_OS_ENABLE)

