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
#include "tlkapp_system.h"


extern void btc_ll_system_tick_isr(void);
extern void btc_core_isr(void);



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
    btc_ll_system_tick_isr();
    ble_ll_system_tick_isr();
}



/**
 * @brief		Baseband interrupt handler.
 * @param[in]	none
 * @return      none
 */
_attribute_retention_code_ void rf_irq_handler(void)
{
	ble_sdk_rf_irq_handler();
}


extern void tlkusb_audirq_handler(void);
extern void tlkusb_mscirq_handler(void);
_attribute_retention_code_ 
void usb_endpoint_irq_handler(void)
{
	#if (TLK_USB_AUD_ENABLE)
	tlkusb_audirq_handler();
	#endif
	#if (TLK_USB_MSC_ENABLE)
	tlkusb_mscirq_handler();
	#endif
}


#if (TLK_DEV_XT2602E_ENABLE)
volatile unsigned int nnn=0;
extern volatile uint08 gTlkDevPspiEndIrqFlag;
_attribute_retention_code_ void pspi_irq_handler(void)
{
	if(spi_get_irq_status(PSPI_MODULE,SPI_END_INT))
	{
		spi_clr_irq_status(PSPI_MODULE, FLD_SPI_END_INT);//clr
		if(gTlkDevPspiEndIrqFlag == 0) gTlkDevPspiEndIrqFlag = 1;
	}
}
#endif
_attribute_retention_code_ void zb_bt_irq_handler(void)
{
    btc_core_isr();
}

