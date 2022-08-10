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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkapi_timer.h"
#if (TLKAPI_TIMER_ENABLE)
#include "drivers.h"


#define TLKAPI_TIMER_ID     TIMER0


extern void hci_host_to_controller(void);
extern void hci_set_tx_critical_sec_en(uint08 flow);

extern unsigned int core_enter_critical(unsigned char preempt_en ,unsigned char threshold);
extern void core_leave_critical(unsigned char preempt_en ,unsigned int r);

//static tlkapi_timer_t *tlkmdi_timer_takeFirst(void);
static void tlkmdi_timer_start(uint32 intervalUs);
static void tlkmdi_timer_close(void);


static uint08 sTlkMdiTimerIsBusy = false;
static uint08 sTlkMdiTimerIsStart = false;
static uint32 sTlkMdiTimerInterval = 0xFFFFFFFF;

static tlkapi_adapt_t sTlkMdiTimerAdapt = {
	true, //uint08 isUpdate;
	0, //uint08 reserve0;
	0, //uint16 reserve1;
	nullptr, //tlkapi_procs_t *pProcsList; //Circulation list
	nullptr, //tlkapi_timer_t *pTimerList; //Singly linked list
};


volatile uint32 AAAA_irq_test201 = 0;
volatile uint32 AAAA_irq_test202 = 0;
volatile uint32 AAAA_irq_test203 = 0;
volatile uint32 AAAA_irq_test204 = 0;
volatile uint32 AAAA_irq_test205 = 0;
volatile uint32 AAAA_irq_test206 = 0;
volatile uint32 AAAA_irq_test207 = 0;
volatile uint32 AAAA_irq_test208 = 0;


int tlkapi_timer_init(void)
{
	plic_set_priority(IRQ4_TIMER0, 1);
	
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

	AAAA_irq_test201 ++;
	
	plic_interrupt_enable(IRQ4_TIMER0);

	hci_set_tx_critical_sec_en(1);
	
	return TLK_ENONE;
}

void tlkapi_timer_handler(void)
{
	uint32 timeIntval;

	AAAA_irq_test202 ++;
	tlkmdi_timer_close();
	
	uint32 irq = read_csr(NDS_MIE);
	core_restore_interrupt(irq | BIT(11));

	AAAA_irq_test203 ++;
	sTlkMdiTimerIsBusy = true;
	tlkapi_adapt_handler(&sTlkMdiTimerAdapt);
	AAAA_irq_test204 ++;
	timeIntval = tlkapi_adapt_timerInterval(&sTlkMdiTimerAdapt);
	if(timeIntval < 50) timeIntval = 50;
	sTlkMdiTimerIsBusy = false;
	hci_host_to_controller();
	AAAA_irq_test205 ++;
	if(timeIntval != 0xFFFFFFFF){
		AAAA_irq_test206 ++;
		tlkmdi_timer_start(timeIntval);
	}
	
	core_restore_interrupt(irq);
	AAAA_irq_test207 ++;
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
int tlkapi_timer_initNode(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, void *pUsrArg, uint32 timeout)
{
	int ret;
	uint irq = core_enter_critical(1, 1);
	if(tlkapi_adapt_isHaveTimer(&sTlkMdiTimerAdapt, pTimer)){
		core_leave_critical(1, irq);
		return -TLK_EEXIST;
	}
	ret = tlkapi_adapt_initTimer(pTimer, timerCB, pUsrArg, timeout);
	core_leave_critical(1, irq);
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
	uint irq = core_enter_critical(1, 1);
	ret = tlkapi_adapt_isHaveTimer(&sTlkMdiTimerAdapt, pTimer);
	core_leave_critical(1, irq);
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
	uint irq = core_enter_critical(1, 1);
	ret = tlkapi_adapt_updateTimer(&sTlkMdiTimerAdapt, pTimer, timeout, isInsert);
	core_leave_critical(1, irq);
	if(!sTlkMdiTimerIsBusy){
		uint interval = tlkapi_timer_interval();
		if(interval < 50) interval = 50;
		if(interval != 0xFFFFFFFF) tlkmdi_timer_start(interval);
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
	uint irq = core_enter_critical(1, 1);
	ret = tlkapi_adapt_insertTimer(&sTlkMdiTimerAdapt, pTimer, true);
	core_leave_critical(1, irq);
	if(!sTlkMdiTimerIsBusy){
		uint interval = tlkapi_timer_interval();
		if(interval < 50) interval = 50;
		if(interval != 0xFFFFFFFF) tlkmdi_timer_start(interval);
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
	uint irq = core_enter_critical(1, 1);
	ret = tlkapi_adapt_removeTimer(&sTlkMdiTimerAdapt, pTimer);
	core_leave_critical(1, irq);
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
	uint irq = core_enter_critical(1, 1);
	ret = tlkapi_adapt_timerInterval(&sTlkMdiTimerAdapt);
	core_leave_critical(1, irq);
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
static void tlkmdi_timer_start(uint32 intervalUs)
{
	uint32 r = core_disable_interrupt();
	if(sTlkMdiTimerIsStart && sTlkMdiTimerInterval <= intervalUs){
		core_restore_interrupt(r);
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
	core_restore_interrupt(r);
}
static void tlkmdi_timer_close(void)
{
	uint32 r = core_disable_interrupt();
	
	sTlkMdiTimerIsStart = false;
	sTlkMdiTimerInterval = 0xFFFFFFFF;
	
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_ctrl0 &= (~FLD_TMR0_EN); //stop TIMER0
	timer_clr_irq_status(FLD_TMR_STA_TMR0);
	#else
	reg_tmr_ctrl0 &= (~FLD_TMR1_EN);
	timer_clr_irq_status(FLD_TMR_STA_TMR1);
	#endif
	core_restore_interrupt(r);
}



#endif //#if (TLKAPI_TIMER_ENABLE)
