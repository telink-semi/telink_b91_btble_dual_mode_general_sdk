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
#include "tlkdev/sys/tlkdev_serial.h"
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
extern void btc_set_sniff_req_enable(uint8_t enable);// 1 : enable, 0 :disable
extern void btc_ll_set_sniff_lp_mode(bt_sniff_lp_mode_t mode);
extern int bth_sendEnterSleepCmd(void);
extern int bth_sendLeaveSleepCmd(void);
extern void tlkdev_xtsd01g_shutDown(void);


static void tlkapp_pm_enterSleepHandler(uint08 evtID, uint08 *pData, int dataLen);
static void tlkapp_pm_leaveSleepHandler(uint08 evtID, uint08 *pData, int dataLen);

static uint08 sTlkAppPmState = TLKAPP_PM_STATE_IDLE;

/******************************************************************************
 * Function: tlkapp_pm_init
 * Descript: This function for initial the pm module.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_pm_init(void)
{
	btble_pm_initPowerManagement_module();
	btble_pm_setSleepEnable(SLEEP_BT_ACL_SLAVE | SLEEP_BT_INQUIRY_SCAN | SLEEP_BT_PAGE_SCAN | SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE);
	btble_contr_registerControllerEventCallback(CONTR_EVT_PM_SLEEP_ENTER,  tlkapp_pm_enterSleepHandler);
	btble_contr_registerControllerEventCallback(CONTR_EVT_PM_SUSPEND_EXIT, tlkapp_pm_leaveSleepHandler);

//	gpio_function_en(TLKAPP_WAKEUP_PIN);
//	gpio_output_dis(TLKAPP_WAKEUP_PIN);
//	gpio_input_en(TLKAPP_WAKEUP_PIN);
	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_LOW, 1);
	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN,GPIO_PIN_PULLUP_1M);
	

	btc_set_sniff_req_enable(1);//enable
	btc_ll_set_sniff_lp_mode(BT_SNIFF_LP_MODE_SUSPEND);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapp_pm_init_deepRetn
 * Descript:
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
_attribute_bt_retention_code_
void tlkapp_pm_init_deepRetn(void)
{
	
}


extern uint32 gTlkAppSystemBusyTimer;

static uint32 sTlkAppPmTraceTimer = 0;

/******************************************************************************
 * Function: tlkapp_pm_handler
 * Descript: Callback for sdk.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_pm_handler(void)
{
	uint08 apiIsBusy = tlkapi_pmIsBusy();
	uint08 devIsBusy = tlkdev_pmIsBusy();
	uint08 mdiIsBusy = tlkmdi_pmIsbusy();
	uint08 mmiIsBusy = tlkmmi_pmIsbusy();
	uint08 stkIsBusy = tlkstk_pmIsBusy();
	uint08 appIsBusy = tlkapp_pmIsBusy();
	uint08 padIsBusy = !gpio_read(TLKAPP_WAKEUP_PIN);
	
	if(gTlkAppSystemBusyTimer != 0 && clock_time_exceed(gTlkAppSystemBusyTimer, 1000000)){
		//Solve the problem that Android phones are difficult to connect
		gTlkAppSystemBusyTimer = 0;
	}

	if(sTlkAppPmTraceTimer == 0 || clock_time_exceed(sTlkAppPmTraceTimer, 1000000)){
		sTlkAppPmTraceTimer = clock_time()|1;
		tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "PM-BUSY:%d %d %d %d %d %d %d", appIsBusy,
			stkIsBusy, apiIsBusy, devIsBusy, mdiIsBusy, mmiIsBusy, padIsBusy);
	}
	
	if(sTlkAppPmState == TLKAPP_PM_STATE_IDLE){
		
	}
	if(//1 ||
			gTlkAppSystemBusyTimer != 0 || appIsBusy || stkIsBusy || apiIsBusy
			|| devIsBusy || mdiIsBusy || mmiIsBusy || padIsBusy)
	{
		btble_pm_setSleepEnable(SLEEP_DISABLE);
		btc_set_sniff_req_enable(0);//disable
		bth_sendLeaveSleepCmd();
	}
	else{
			btc_set_sniff_req_enable(1);//enable
			btc_pscan_low_power_enable(PSCAN_LOW_POWER_ENABLE, NULL);
			btc_iscan_low_power_enable(ISCAN_LOW_POWER_ENABLE);
			btble_pm_setSleepEnable(SLEEP_BT_ACL_SLAVE | SLEEP_BT_INQUIRY_SCAN | SLEEP_BT_PAGE_SCAN | SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE);
			btble_pm_setWakeupSource(PM_WAKEUP_PAD);
			bth_sendEnterSleepCmd();

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
	    /*remove USB pin current leakage*/
		usb_set_pin_dis();
	#endif
	
	#if (TLK_DEV_XTSD01G_ENABLE)
	tlkdev_xtsd01g_shutDown();
	#endif
	#if (TLK_DEV_XT26G0X_ENABLE)
	tlkdev_xt26g0x_shutDown();
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
}


#endif

