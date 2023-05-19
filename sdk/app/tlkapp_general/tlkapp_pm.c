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
#if (TLK_CFG_PM_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkmmi/audio/tlkmmi_audio.h"
#include "tlkapp_config.h"
#include "tlkapp.h"
#include "tlkapp_pm.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys/tlksys_pm.h"
#endif


#include "drivers.h"
#include "tlkstk/hci/hci_cmd.h"
#include "tlkstk/bt/btc/btc_stdio.h"
#include "tlkstk/btble/btble.h"
#include "tlkstk/btble/btble_pm.h"
#include "tlkstk/tlkstk.h"

extern bool tlkstk_pmIsBusy(void);
extern bool tlkapp_pmIsBusy(void);

extern void btc_context_restore(unsigned long * bt_reg, unsigned long * ip_reg, unsigned long * modem_reg, unsigned long * radio_reg, unsigned long * pdzb_reg);
extern void btc_ll_set_sniff_lp_mode(bt_sniff_lp_mode_t mode);
extern int bth_sendEnterSleepCmd(void);
extern int bth_sendLeaveSleepCmd(void);
extern uint bth_getAclCount(void);
extern bool tlkmmi_btmgr_recIsBusy(void);
extern void tlkmdi_btmgr_regAclDisconnCB(TlkMmiBtMgrAclDisconnCallback discCB);

static void tlkapp_pm_btaclDisconnCb(uint16 handle, uint08 reason, uint08 *pBtAddr);
static void tlkapp_pm_enterSleepHandler(uint08 evtID, uint08 *pData, int dataLen);
static void tlkapp_pm_leaveSleepHandler(uint08 evtID, uint08 *pData, int dataLen);



extern unsigned long  DEBUG_BTREG_INIT[];
extern unsigned long  DEBUG_IPREG_INIT[];
extern unsigned long  DEBUG_TL_MODEMREG_INIT[];
extern unsigned long  DEBUG_TL_RADIOREG_INIT[];
extern unsigned long  DEBUG_TL_PDZB_INIT[];



static uint08 sTlkAppPmState = TLKAPP_PM_STATE_IDLE;
static uint08 gTlkAppPmSchIdleCount = 0;
static uint32 gTlkAppPmSysIdleTimer = 0;
static uint32 sTlkAppPmTraceTimer = 0;


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
	btble_contr_registerControllerEventCallback(CONTR_EVT_PM_SLEEP_ENTER,  tlkapp_pm_enterSleepHandler);
	btble_contr_registerControllerEventCallback(CONTR_EVT_PM_SUSPEND_EXIT, tlkapp_pm_leaveSleepHandler);
	tlkmdi_btmgr_regAclDisconnCB(tlkapp_pm_btaclDisconnCb);

	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_LOW, 1);
	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN, GPIO_PIN_PULLUP_1M);
	gpio_set_input(TLKAPP_WAKEUP_PIN, 1);
	
	btc_ll_set_sniff_lp_mode(BT_SNIFF_LP_MODE_SUSPEND);
	gTlkAppPmSysIdleTimer = clock_time();
	
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkapp_pm_handler
 * Descript: Callback for sdk.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_pm_handler(void)
{
	bool isBusy = false;

	if(!gpio_read(TLKAPP_WAKEUP_PIN)){
		isBusy = true;
	}
	#if (TLK_CFG_SYS_ENABLE)
	if(!isBusy && tlksys_pm_isBusy()) isBusy = true;
	#endif
	
	if(sTlkAppPmTraceTimer == 0 || clock_time_exceed(sTlkAppPmTraceTimer, 1000000)){
		sTlkAppPmTraceTimer = clock_time()|1;
		tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "PM-BUSY:%d %d %d %d", 
			isBusy, tlkapp_pmIsBusy(), tlkstk_pmIsBusy(), !gpio_read(TLKAPP_WAKEUP_PIN));
		if(isBusy){
			const char *pName = tlksys_pm_getBusyName();
			tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "PM-Busy Module-> %s", pName);
		}
	}
	
	if(isBusy){
		btble_pm_setSleepEnable(SLEEP_DISABLE);
		bth_sendLeaveSleepCmd();
		gTlkAppPmSysIdleTimer = clock_time()|1;
	}else{		
		//enter deepsleep when system is idle
		if(!tlkstk_state() && !tlkmmi_btmgr_recIsBusy() && (gTlkAppPmSysIdleTimer == 0 || clock_time_exceed(gTlkAppPmSysIdleTimer, 1000000))){
			cpu_sleep_wakeup_32k_rc(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
			gTlkAppPmSysIdleTimer = clock_time()|1;
		}
		if(gTlkAppPmSysIdleTimer != 0 && clock_time_exceed(gTlkAppPmSysIdleTimer, 1000000)){
			gTlkAppPmSysIdleTimer =  0;
		}else{
			btc_pscan_low_power_enable(PSCAN_LOW_POWER_ENABLE, NULL);
			btc_iscan_low_power_enable(ISCAN_LOW_POWER_ENABLE);
			btble_pm_setSleepEnable(SLEEP_BT_ACL_SLAVE | SLEEP_BT_INQUIRY_SCAN | SLEEP_BT_PAGE_SCAN | SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE);
			btble_pm_setWakeupSource(PM_WAKEUP_PAD);
			bth_sendEnterSleepCmd();
		}
	}
}

static void tlkapp_pm_btaclDisconnCb(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	//Solve the problem that Android phones are difficult to connect
	gTlkAppPmSysIdleTimer = clock_time()|1;
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
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_enterSleep(SUSPEND_MODE);
	#endif
	
	sTlkAppPmState = TLKAPP_PM_STATE_SLEEP;
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
	sTlkAppPmState = TLKAPP_PM_STATE_IDLE;
	gTlkAppPmSysIdleTimer = clock_time();
	gTlkAppPmSchIdleCount = 0;
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_leaveSleep(evtID);
	#endif
}


#endif

