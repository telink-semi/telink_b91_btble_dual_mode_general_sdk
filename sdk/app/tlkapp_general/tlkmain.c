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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapp_config.h"
#include "tlkapp.h"
#include "drivers.h"


#if (TLK_STK_BT_ENABLE)
extern void btc_ll_system_tick_isr(void);
extern void btc_core_isr(void);
#endif
#if (TLK_CFG_USB_ENABLE)
extern void tlkusb_irqHandler(void);
#endif
#if (TLK_STK_LE_ENABLE)
extern void ble_sdk_rf_irq_handler(void);
extern void ble_ll_system_tick_isr(void);
#endif


/******************************************************************************
 * Function: main
 * Descript: This is main function.
 * Params: None.
 * Return: 0 is successs.
 * Others: None.
*******************************************************************************/
int main(void)
{
    blc_pm_select_internal_32k_crystal();

    sys_init(DCDC_1P4_LDO_1P8,VBAT_MAX_VALUE_GREATER_THAN_3V6);

    CCLK_48M_HCLK_48M_PCLK_24M;

    gpio_init(1);
    tlkapp_init();

	#if (TLK_CFG_WDG_ENABLE)
	wd_set_interval_ms(3000);
	wd_start();
	#endif
	
	core_enable_interrupt();

	while(1)
	{
		#if (TLK_CFG_WDG_ENABLE)
		wd_clear();
		#endif
		tlkapp_handler();
	}
    return 0;
} 

/******************************************************************************
 * Function: stimer_irq_handler
 * Descript: System timer interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
_attribute_retention_code_ 
void stimer_irq_handler(void)
{
	#if (TLK_STK_BT_ENABLE)
    btc_ll_system_tick_isr();
	#endif
	#if (TLK_STK_LE_ENABLE)
    ble_ll_system_tick_isr();
	#endif
//	systimer_clr_irq_status();
}

/******************************************************************************
 * Function: rf_irq_handler
 * Descript: Baseband interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
_attribute_retention_code_ 
void rf_irq_handler(void)
{
	#if (TLK_STK_LE_ENABLE)
	ble_sdk_rf_irq_handler();
	#endif
}

/******************************************************************************
 * Function: zb_bt_irq_handler
 * Descript: This function for Bt core handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
_attribute_retention_code_ 
void zb_bt_irq_handler(void)
{
	#if (TLK_STK_BT_ENABLE)
    btc_core_isr();
	#endif
}

/******************************************************************************
 * Function: usb_endpoint_irq_handler
 * Descript: This function for audio interrupt handler 
 *           and USB MSC interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
_attribute_retention_code_ 
void usb_endpoint_irq_handler(void)
{
	#if (TLK_CFG_USB_ENABLE)
	tlkusb_irqHandler();
	#endif
}

void timer0_irq_handler(void)
{
	tlkapi_timer_handler();
}


