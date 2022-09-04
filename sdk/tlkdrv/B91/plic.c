/********************************************************************************************************
 * @file     plic.c
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
#include "tlkapi/tlkapi_debug.h"
#include "drivers.h"

#if IRQ_DEBUG_ENABLE
_attribute_data_retention_sec_  volatile EXCEPT_HANDLER_S_T except_handler_b;
#endif 

_attribute_data_retention_sec_ volatile EXCEPT_HANDLER_S_T except_handler_e;
_attribute_data_retention_sec_ unsigned char      g_plic_preempt_en = 1;
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

volatile uint32 sCoreCriticalCount0 = 0;
volatile uint32 sCoreCriticalCount1 = 0;


_attribute_ram_code_sec_noinline_ 
unsigned int core_enter_critical(unsigned char preempt_en, unsigned char threshold)
{
	unsigned int r = 0;
	if(g_plic_preempt_en && preempt_en && threshold != 0)
	{
		unsigned int r_inq = core_disable_interrupt();
		unsigned char thrd = reg_irq_threshold & 0xFF;
		sCoreCriticalCount0 ++;
		if(thrd < threshold){
			plic_set_threshold(threshold);
		}else{
			
		}
		core_restore_interrupt(r_inq);
	}
	else
	{
	   r = core_disable_interrupt();
	}
	return r ;
}
_attribute_ram_code_sec_noinline_ 
void core_leave_critical(unsigned char preempt_en, unsigned int r)
{
	if(g_plic_preempt_en && preempt_en)
	{
		unsigned int r_inq = core_disable_interrupt();
		if(sCoreCriticalCount0 == 1){
			plic_set_threshold(0);
		}
		if(sCoreCriticalCount0 != 0) sCoreCriticalCount0--;
		core_restore_interrupt(r_inq);
	}
	else
	{
		core_restore_interrupt(r);
	}
}


_attribute_ram_code_sec_noinline_
void default_irq_handler(void)
{
	//printf("Default interrupt handler\n");
}
void stimer_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void analog_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void timer1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void timer0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void dma_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void bmc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_ctrl_ep_setup_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_ctrl_ep_data_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_ctrl_ep_status_irq_handler(void)  __attribute__((weak, alias("default_irq_handler")));
void usb_ctrl_ep_setinf_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_endpoint_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void zb_dm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void rf_ble_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void zb_bt_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void rf_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void pwm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void pke_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void uart1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void uart0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void audio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void i2c_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void hspi_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void pspi_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_pwdn_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void gpio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void gpio_risc0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void gpio_risc1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void soft_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void npe_bus0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus3_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus4_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_250us_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void usb_reset_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void npe_bus7_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus8_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void npe_bus13_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus14_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void npe_bus15_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus17_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void npe_bus21_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus22_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus23_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus24_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus25_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus26_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus27_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus28_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void npe_bus29_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus30_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_bus31_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void npe_comb_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void pm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void eoc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));




_attribute_bt_data_retention_ volatile int irq_stack_overflow=0;

_attribute_retention_code_ void irq_stack_check(uint32_t sp)
{
	volatile uint32_t* sp_ptr = (uint32_t*)sp;

	if(sp_ptr[0]!=0x55555555)
	{
		irq_stack_overflow++;
		//log_tick(SL_APP_SP_EN,SLET_sp_overflow);
	}
}

extern void tlkapi_debug_process(void);
/**
 * @brief  exception handler.this defines an exception handler to handle all the platform pre-defined exceptions.
 * @return none
 */
_attribute_retention_code_ __attribute__((weak)) void except_handler(long cause, long epc,long* reg)
{
	core_disable_interrupt();

	#if (TLK_CFG_DBG_ENABLE)
	tlkapi_debug_delayForPrint(10000);
	#endif
	
	except_handler_e.pc = epc;
	except_handler_e.lr = reg[0];
	except_handler_e.sp = reg[1];
	except_handler_e.gp = reg[2];
	except_handler_e.cause = cause;

	#if (TLK_CFG_DBG_ENABLE)
	for(volatile unsigned int i = 0; i < 20; i++)
	{		
		tlkapi_debug_sendU32s("cause",
					except_handler_e.pc, 
					except_handler_e.lr, 
					except_handler_e.sp, 
					except_handler_e.cause);
		tlkapi_debug_sendU32s("PC,LR,SP,GP",
					except_handler_e.pc, 
					except_handler_e.lr, 
					except_handler_e.sp, 
					except_handler_e.gp);
		tlkapi_debug_delayForPrint(10000);
	}
	#endif
	while(1){ asm("nop"); }
}

_attribute_retention_code_  __attribute__((weak)) void trap_entry(void) __attribute__ ((interrupt ("machine") , aligned(4)));
void trap_entry(void)
{
	register long ra asm("x1"), sp asm("x2"), gp asm("x3");
	long mcause = read_csr(NDS_MCAUSE);
	long mepc = read_csr(NDS_MEPC);
	long reg[3];
	reg[0] = ra;
	reg[1] = sp;
	reg[2] = gp;
	except_handler(mcause,mepc,reg);
}

/**
 * @brief system timer interrupt handler.
 * @return none
 */



/// @} DRIVERS
