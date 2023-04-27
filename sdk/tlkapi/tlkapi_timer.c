/********************************************************************************************************
 * @file     tlkmdi_timer.c
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
#include "tlkapi_timer.h"
#include "drivers.h"


#define TLKAPI_TIMER_ID     TIMER0


extern void tlkbt_hci_h2cHandler(void);


//static tlkapi_timer_t *tlkmdi_timer_takeFirst(void);
static void tlkapi_timer_start(uint32 intervalUs);
static void tlkapi_timer_close(void);


static uint08 sTlkMdiTimerIsBusy = false;
static uint08 sTlkMdiTimerIsStart = false;
static uint32 sTlkMdiTimerInterval = 0xFFFFFFFF;

static tlkapi_adapt_t sTlkMdiTimerAdapt = {
	true, //uint08 isUpdate;
	0, //uint08 reserve0;
	0, //uint16 reserve1;
	nullptr, //tlkapi_queue_t *pQueueList; //Circulation list
	nullptr, //tlkapi_timer_t *pTimerList; //Singly linked list
};




int tlkapi_timer_init(void)
{
	#if (TLKAPI_TIMER_ID == TIMER0)
	plic_set_priority(IRQ4_TIMER0, 1);
	#else
	plic_set_priority(IRQ3_TIMER1, 1);
	#endif
	
	reg_tmr_tick(TLKAPI_TIMER_ID) = 0;
	reg_tmr_capt(TLKAPI_TIMER_ID) = 2*1000*sys_clk.pclk;
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_sta |= FLD_TMR_STA_TMR0; //clear irq status
 	reg_tmr_ctrl0 &= (~FLD_TMR0_MODE);
 	reg_tmr_ctrl0 |= TIMER_MODE_SYSCLK;
	#else
	reg_tmr_sta |= FLD_TMR_STA_TMR1; //clear irq status
	reg_tmr_ctrl0 &= (~FLD_TMR1_MODE);
	reg_tmr_ctrl0 |= (TIMER_MODE_SYSCLK<<4);
	#endif

	#if (TLKAPI_TIMER_ID == TIMER0)
	plic_interrupt_enable(IRQ4_TIMER0);
	#else
	plic_interrupt_enable(IRQ3_TIMER1);
	#endif
		
	return TLK_ENONE;
}

void tlkapi_timer_handler(void)
{
	uint32 timeIntval;

	tlkapi_timer_close();
	
//	uint32 irq = read_csr(NDS_MIE);
//	set_csr(NDS_MSTATUS, irq | BIT(11)); //global interrupts enable
//	fence_iorw;
	
	sTlkMdiTimerIsBusy = true;
	tlkapi_adapt_handler(&sTlkMdiTimerAdapt);

	timeIntval = tlkapi_adapt_timerInterval(&sTlkMdiTimerAdapt);
	if(timeIntval < 50) timeIntval = 50;
	sTlkMdiTimerIsBusy = false;
	tlkbt_hci_h2cHandler();
	if(timeIntval != 0xFFFFFFFF){
		tlkapi_timer_start(timeIntval);
	}
	
//	set_csr(NDS_MSTATUS, irq); //global interrupts enable
//	fence_iorw;
}

/******************************************************************************
 * Function: tlkapi_timer_isbusy
 * Descript: If there is a process or timer and less than 1ms, the module 
 *           is considered busy, it is the basis of the schdule. 
 * Params:  
 * Return: true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isbusy(void)
{
	//If there is a process or timer and less than 1ms, the module is considered busy
	if(tlkapi_timer_interval() < 1000){
		return true;
	}else{
		return false;
	}
}
bool tlkapi_timer_isPmBusy(void)
{
	return false;
}

/******************************************************************************
 * Function: tlkapi_timer_isout
 * Descript: Detects whether the current timer has timed out.
 * Params:
 *     @timer[IN]--Reference timer.
 *     @ticks[IN]--Timeout. unit: 1/16 us:
 * Return: True means the timer is timeout, false means not.
 * Others: None.
*******************************************************************************/
//_attribute_ram_code_sec_noinline_
bool tlkapi_timer_isout(uint32 timer, uint32 ticks)
{
	if((uint32)(clock_time()-timer) >= ticks) return true;
	else return false;
}


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
int tlkapi_timer_initNode(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	int ret;
	core_enter_critical();
	if(tlkapi_adapt_isHaveTimer(&sTlkMdiTimerAdapt, pTimer)){
		core_leave_critical();
		return -TLK_EEXIST;
	}
	ret = tlkapi_adapt_initTimer(pTimer, timerCB, userArg, timeout);
	core_leave_critical();
	return ret;
}

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
bool tlkapi_timer_isHaveNode(tlkapi_timer_t *pTimer)
{
	bool ret;
	core_enter_critical();
	ret = tlkapi_adapt_isHaveTimer(&sTlkMdiTimerAdapt, pTimer);
	core_leave_critical();
	return ret;
}

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
int tlkapi_timer_updateNode(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	int ret;
	core_enter_critical();
	ret = tlkapi_adapt_updateTimer(&sTlkMdiTimerAdapt, pTimer, timeout, isInsert);
	core_leave_critical();
	if(!sTlkMdiTimerIsBusy){
		uint interval = tlkapi_timer_interval();
		if(interval < 50) interval = 50;
		if(interval != 0xFFFFFFFF) tlkapi_timer_start(interval);
	}
	return ret;
}
/******************************************************************************
 * Function: tlkmdi_adapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkapi_timer_insertNode(tlkapi_timer_t *pTimer)
{
	int ret;
	core_enter_critical();
	ret = tlkapi_adapt_insertTimer(&sTlkMdiTimerAdapt, pTimer, true);
	core_leave_critical();
	if(!sTlkMdiTimerIsBusy){
		uint interval = tlkapi_timer_interval();
		if(interval < 50) interval = 50;
		if(interval != 0xFFFFFFFF) tlkapi_timer_start(interval);
	}
	return ret;
}
/******************************************************************************
 * Function: tlkmdi_timer_remove
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. LSLP_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkapi_timer_removeNode(tlkapi_timer_t *pTimer)
{
	int ret;
	core_enter_critical();
	ret = tlkapi_adapt_removeTimer(&sTlkMdiTimerAdapt, pTimer);
	core_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_timer_interval
 * Descript: Gets the time required for the last timer to arrive. Unit:us.
 * Params:
 *     @pAdapt[IN]--The adapter for a System Task .
 * Return: None.
 * Others: arrival_interval.
*******************************************************************************/
uint tlkapi_timer_interval(void)
{
	uint ret;
	core_enter_critical();
	ret = tlkapi_adapt_timerInterval(&sTlkMdiTimerAdapt);
	core_leave_critical();
	return ret;
}


/******************************************************************************
 * Function: tlkapi_timer_take_first_timeout
 * Descript: Retrieves the first timer to arrive in the adapter.
 * Params:
 *     @pAdapt[IN]--The adapter for a System Task .
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkapi_timer_start(uint32 intervalUs)
{
	core_enter_critical();
	if(sTlkMdiTimerIsStart && sTlkMdiTimerInterval <= intervalUs){
		core_leave_critical();
		return;
	}
	if(sTlkMdiTimerIsStart){
		#if (TLKAPI_TIMER_ID == TIMER0)
		reg_tmr_ctrl0 &= (~FLD_TMR0_EN); //stop TIMER0
		timer_clr_irq_status(FLD_TMR_STA_TMR0);
		#else
		reg_tmr_ctrl0 &= (~FLD_TMR1_EN);
		timer_clr_irq_status(FLD_TMR_STA_TMR1);
		#endif
	}
	sTlkMdiTimerIsStart = true;
	sTlkMdiTimerInterval = intervalUs;
	reg_tmr_tick(TLKAPI_TIMER_ID) = 0;
	reg_tmr_capt(TLKAPI_TIMER_ID) = intervalUs*sys_clk.pclk;
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_sta |= FLD_TMR_STA_TMR0; //clear irq status
 	reg_tmr_ctrl0 &= (~FLD_TMR0_MODE);
 	reg_tmr_ctrl0 |= TIMER_MODE_SYSCLK;
	reg_tmr_ctrl0 |= FLD_TMR0_EN;
	#else
	reg_tmr_sta |= FLD_TMR_STA_TMR1; //clear irq status
	reg_tmr_ctrl0 &= (~FLD_TMR1_MODE);
	reg_tmr_ctrl0 |= (TIMER_MODE_SYSCLK<<4);
	reg_tmr_ctrl0 |= FLD_TMR1_EN;
	#endif
	core_leave_critical();
}
static void tlkapi_timer_close(void)
{
	core_enter_critical();
	sTlkMdiTimerIsStart = false;
	sTlkMdiTimerInterval = 0xFFFFFFFF;
	
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_ctrl0 &= (~FLD_TMR0_EN); //stop TIMER0
	timer_clr_irq_status(FLD_TMR_STA_TMR0);
	#else
	reg_tmr_ctrl0 &= (~FLD_TMR1_EN);
	timer_clr_irq_status(FLD_TMR_STA_TMR1);
	#endif
	core_leave_critical();
}




