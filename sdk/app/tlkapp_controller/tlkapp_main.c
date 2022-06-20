/********************************************************************************************************
 * @file     main.c
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

#include "tlkstk/inner/tlkstk_inner.h"
#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"

#include "tlkstk/ble/ble.h"

#include "tlkapp_config.h"
#include "tlkapp.h"

extern void btc_ll_system_tick_isr(void);
extern void btc_core_isr(void);



volatile uint32 AAAA_irq_test001 = 0;
volatile uint32 AAAA_irq_test002 = 0;
volatile uint32 AAAA_irq_test003 = 0;
volatile uint32 AAAA_irq_test004 = 0;
volatile uint32 AAAA_irq_test005 = 0;
volatile uint32 AAAA_irq_test006 = 0;
volatile uint32 AAAA_irq_test007 = 0;
volatile uint32 AAAA_irq_test008 = 0;
volatile uint32 AAAA_irq_test009 = 0;
volatile uint32 AAAA_irq_test00A = 0;


/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
int main(void)
{
    blc_pm_select_internal_32k_crystal();

    sys_init(DCDC_1P4_LDO_1P8,VBAT_MAX_VALUE_GREATER_THAN_3V6);

    CCLK_48M_HCLK_48M_PCLK_24M;

    gpio_init(1);
	
    tlkapp_init();
		
	core_enable_interrupt();

	while(1)
	{
		
		tlkapp_process();
	}
	
    return 0;
} 


/**
 * @brief		System timer interrupt handler.
 * @param[in]	none
 * @return      none
 */
_attribute_retention_code_ void stimer_irq_handler(void)
{
	AAAA_irq_test001 ++;
    btc_ll_system_tick_isr();
	AAAA_irq_test002 ++;
    ble_ll_system_tick_isr();
	AAAA_irq_test003 ++;
}



/**
 * @brief		Baseband interrupt handler.
 * @param[in]	none
 * @return      none
 */
_attribute_retention_code_ void rf_irq_handler(void)
{
	AAAA_irq_test004 ++;
	ble_sdk_rf_irq_handler();
	AAAA_irq_test005 ++;
}


extern void tlkusb_audirq_handler(void);
extern void tlkusb_mscirq_handler(void);
_attribute_retention_code_ 
void usb_endpoint_irq_handler(void)
{
	AAAA_irq_test006 ++;
	#if (TLK_USB_AUD_ENABLE)
	tlkusb_audirq_handler();
	#endif
	AAAA_irq_test007 ++;
	#if (TLK_USB_MSC_ENABLE)
	tlkusb_mscirq_handler();
	#endif
	AAAA_irq_test008 ++;
}


static volatile unsigned int tdebug_zb_bt_irq = 0;
_attribute_retention_code_ void zb_bt_irq_handler(void)
{
	AAAA_irq_test009 ++;
    tdebug_zb_bt_irq++;
    btc_core_isr();
	AAAA_irq_test00A ++;

}

