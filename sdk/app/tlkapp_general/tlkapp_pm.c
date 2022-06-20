/********************************************************************************************************
 * @file     tlkapp_pm.c
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
#include "tlkmmi/tlkmmi_stdio.h"
#include "tlkmmi/audio/tlkmmi_audio.h"
#include "tlkdev/tlkdev_serial.h"
#include "tlkapp_config.h"
#include "tlkapp_pm.h"
#include "tlkapp_system.h"


#if (TLK_CFG_PM_ENABLE)

#include "drivers.h"
#include "tlkstk/hci/hci_cmd.h"
#include "tlkstk/bt/btc/btc_stdio.h"
#include "tlkstk/btble/btble.h"
#include "tlkstk/btble/btble_pm.h"


extern bool tlkstk_pmIsBusy(void);
extern bool tlkmdi_pmIsbusy(void);
extern bool tlkapp_pmIsBusy(void);


extern unsigned long  DEBUG_BTREG_INIT[];
extern unsigned long  DEBUG_IPREG_INIT[];
extern unsigned long  DEBUG_TL_MODEMREG_INIT[];
extern unsigned long  DEBUG_TL_RADIOREG_INIT[];
extern unsigned long  DEBUG_TL_PDZB_INIT[];
extern void btc_context_restore(unsigned long * bt_reg, unsigned long * ip_reg, unsigned long * modem_reg, unsigned long * radio_reg, unsigned long * pdzb_reg);
extern void btc_ll_set_sniff_lp_mode(bt_sniff_lp_mode_t mode);
extern int bth_sendEnterSleepCmd(void);
extern int bth_sendLeaveSleepCmd(void);


static void tlkapp_pm_enterSleepHandler(uint08 evtID, uint08 *pData, int dataLen);
static void tlkapp_pm_leaveSleepHandler(uint08 evtID, uint08 *pData, int dataLen);


//static uint32 sTlkAppPmEnterTimer;
static uint08 sTlkAppPmState = TLKAPP_PM_STATE_IDLE;



int tlkapp_pm_init(void)
{
	btble_pm_initPowerManagement_module();
	btble_pm_setSleepEnable(SLEEP_BT_ACL_SLAVE | SLEEP_BT_INQUIRY_SCAN | SLEEP_BT_PAGE_SCAN | SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE);
	btble_contr_registerControllerEventCallback(CONTR_EVT_PM_SLEEP_ENTER,  tlkapp_pm_enterSleepHandler);
	btble_contr_registerControllerEventCallback(CONTR_EVT_PM_SUSPEND_EXIT, tlkapp_pm_leaveSleepHandler);
	
	gpio_function_en(TLKAPP_WAKEUP_PIN);
	gpio_output_dis(TLKAPP_WAKEUP_PIN); 		
	gpio_input_en(TLKAPP_WAKEUP_PIN);		
	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN,GPIO_PIN_PULLDOWN_100K);
	
	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_HIGH, 1); 

	btc_ll_set_sniff_lp_mode(BT_SNIFF_LP_MODE_SUSPEND);
		
	return TLK_ENONE;
}
_attribute_bt_retention_code_
void tlkapp_pm_init_deepRetn(void)
{
//	plic_interrupt_enable(IRQ1_SYSTIMER);
//	plic_interrupt_enable(IRQ14_ZB_BT);
//
//	btc_ll_sniff_ret_clkrestore();
//	btc_context_restore(DEBUG_BTREG_INIT, DEBUG_IPREG_INIT, DEBUG_TL_MODEMREG_INIT, DEBUG_TL_RADIOREG_INIT, DEBUG_TL_PDZB_INIT);
//
//	btc_intack_clear(0xFFFFFFFF);
//	plic_interrupt_claim(IRQ14_ZB_BT);//claim,clear eip
//	plic_interrupt_complete(IRQ14_ZB_BT);//complete
//    btc_ll_set_sniff_lp_mode(BT_SNIFF_LP_MODE_SUSPEND);
}
void tlkapp_pm_enableScan(void)
{
	btc_iscan_low_power_enable(ISCAN_LOW_POWER_ENABLE);
	
	//bth_hci_sendWriteScanEnableCmd(3);
	btc_pscan_low_power_enable(PSCAN_LOW_POWER_ENABLE, NULL);	
	btc_iscan_low_power_enable(ISCAN_LOW_POWER_ENABLE);
	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "enable low power inquiry-page scan......");
}


extern uint32 gTlkAppSystemBusyTimer;

void tlkapp_pm_handler(void)
{
	uint08 apiIsBusy = tlkapi_pmIsBusy();
	uint08 devIsBusy = tlkdev_pmIsBusy();
	uint08 mdiIsBusy = tlkmdi_pmIsbusy();
	uint08 mmiIsBusy = tlkmmi_pmIsbusy();
	uint08 stkIsBusy = tlkstk_pmIsBusy();
	uint08 appIsBusy = tlkapp_pmIsBusy();
	uint08 padIsBusy = gpio_read(TLKAPP_WAKEUP_PIN);
	
	if(gTlkAppSystemBusyTimer != 0 && clock_time_exceed(gTlkAppSystemBusyTimer, 1000000)){
		//Solve the problem that Android phones are difficult to connect
		gTlkAppSystemBusyTimer = 0;
	}
	
	if(sTlkAppPmState == TLKAPP_PM_STATE_IDLE){
		
	}
	if(1 || gTlkAppSystemBusyTimer != 0 || appIsBusy || stkIsBusy || apiIsBusy || devIsBusy || mdiIsBusy || mmiIsBusy || padIsBusy)
	{
		btble_pm_setSleepEnable(SLEEP_DISABLE);
		if(sTlkAppPmState == TLKAPP_PM_STATE_SLEEP){
			int ret = bth_sendLeaveSleepCmd();
			if(ret == TLK_ENONE || ret == -TLK_ENOOBJECT) sTlkAppPmState = TLKAPP_PM_STATE_IDLE;
		}
	}
	else
	{
		btble_pm_setSleepEnable(SLEEP_BT_ACL_SLAVE | SLEEP_BT_INQUIRY_SCAN | SLEEP_BT_PAGE_SCAN | SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE);
		if(sTlkAppPmState == TLKAPP_PM_STATE_IDLE && bth_sendEnterSleepCmd() == TLK_ENONE){
			sTlkAppPmState = TLKAPP_PM_STATE_SLEEP;
		}
	}
}


/******************************************************************************
 * Function: tlkapp_pm_enterSleepHandler
 * Descript: callback function of Controller Event "CONTR_EVT_PM_SLEEP_ENTER".
 * Params:
 *     @e - BT BLE Controller Event type
 *     @p - data pointer of event
 *     @n - data length of event
 * Return: None.
 * Others: None.
*******************************************************************************/  
static void tlkapp_pm_enterSleepHandler(uint08 evtID, uint08 *pData, int dataLen)
{
	#if (TLK_CFG_USB_ENABLE)
		usb_set_pin_dis();  //remove USB pin current leakage
	#endif
	
	#if (TLK_DEV_XTSD04G_ENABLE)
	if(tlkdev_nand_isPowerOn()){
		tlkdev_usb_shutdown();
		tlkdev_nand_shutdown();
		tlkdev_nand_powerOff();
	}
	#endif
	
	tlkdev_codec_reset();
}


/******************************************************************************
 * Function: tlkapp_pm_leaveSleepHandler
 * Descript: callback function of Controller Event "CONTR_EVT_PM_SUSPEND_EXIT".
 * Params:
 *     @e - BT BLE Controller Event type
 *     @p - data pointer of event
 *     @n - data length of event
 * Return: None.
 * Others: None.
*******************************************************************************/ 
static void tlkapp_pm_leaveSleepHandler(uint08 evtID, uint08 *pData, int dataLen)
{
	tlkdev_serial_wakeup();

//	sTlkAppPmState = TLKAPP_PM_STATE_IDLE;
}


#endif

