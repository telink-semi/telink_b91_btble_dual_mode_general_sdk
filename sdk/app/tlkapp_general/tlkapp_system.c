/********************************************************************************************************
 * @file     tlkapp_system.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlkmdi/tlkmdi_comm.h"
#include "tlkmdi/tlkmdi_btacl.h"
#if (TLK_MDI_KEY_ENABLE)
#include "tlkmdi/tlkmdi_key.h"
#endif
#if (TLK_MDI_LED_ENABLE)
#include "tlkmdi/tlkmdi_led.h"
#endif
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/bth/bth.h"

#include "tlkapp_config.h"
#include "tlkapp_system.h"



static void tlkapp_system_connectCompleteEvt(uint16 handle, uint08 status, uint08 *pBtAddr);
static void tlkapp_system_disconnCompleteEvt(uint16 handle, uint08 reason, uint08 *pBtAddr);
static void tlkapp_system_profileConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID);
static void tlkapp_system_profileDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID);


static void tlkapp_system_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen);
static void tlkapp_system_getVersionDeal(void);
static void tlkapp_system_rebootDeal(void);
static void tlkapp_system_powerOffDeal(void);
static void tlkapp_system_setHeartDeal(uint08 *pData, uint08 dataLen);


uint16 gTlkAppBtConnHandle = 0;
uint32 gTlkAppSystemBusyTimer = 0;
static tlkapp_system_ctrl_t sTlkAppSystemCtrl;


#if (TLK_MDI_KEY_ENABLE)
static void tlkapp_system_keyEventCB(uint08 keyID, uint08 evtID, uint08 isPress)
{
	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_system_keyEventCB: keyID-%d, evtID-%d, isPress-%d",
		keyID, evtID, isPress);	
}
#endif

/******************************************************************************
 * Function: tlkapp_system_init
 * Descript: This function is used to initialize system parameters and configuration.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
int  tlkapp_system_init(void)
{
	tmemset(&sTlkAppSystemCtrl, 0, sizeof(tlkapp_system_ctrl_t));
	#if (TLK_MDI_KEY_ENABLE)
	tlkmdi_key_insert(0x04, TLKMDI_KEY_EVTMSK_ALL, GPIO_PA2, 0, GPIO_PIN_PULLUP_1M, tlkapp_system_keyEventCB);
	tlkmdi_key_insert(0x05, TLKMDI_KEY_EVTMSK_ALL, GPIO_PA3, 0, GPIO_PIN_PULLUP_1M, tlkapp_system_keyEventCB);
	#endif
	#if (TLK_MDI_LED_ENABLE)
	tlkmdi_led_insert(0x01, GPIO_PD3, GPIO_PIN_PULLUP_10K, 1);
	tlkmdi_led_insert(0x02, GPIO_PD4, GPIO_PIN_PULLUP_10K, 1);
	tlkmdi_led_ctrl(0x01, true, 15, 200, 200, true);
	tlkmdi_led_ctrl(0x02, true, 3, 500, 500, false);
	#endif
	
	tlkmdi_comm_regSysCB(tlkapp_system_cmdHandler);
	
	tlkmdi_btmgr_regAclConnectCB(tlkapp_system_connectCompleteEvt);
	tlkmdi_btmgr_regAclDisconnCB(tlkapp_system_disconnCompleteEvt);
	tlkmdi_btmgr_regProfileConnectCB(tlkapp_system_profileConnectEvt);
	tlkmdi_btmgr_regProfileDisconnCB(tlkapp_system_profileDisconnEvt);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapp_system_handler
 * Descript: This function to control the mechine power off or restart 
 *           and register the heart beat.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkapp_system_handler(void)
{
	if(sTlkAppSystemCtrl.powerTimer != 0 && clock_time_exceed(sTlkAppSystemCtrl.powerTimer, 300000)){
		if(sTlkAppSystemCtrl.isBoot) start_reboot();
		else tlkapp_system_poweroff();
		sTlkAppSystemCtrl.powerTimer = 0;
	}
	if(sTlkAppSystemCtrl.heartTimer != 0 && clock_time_exceed(sTlkAppSystemCtrl.heartTimer, sTlkAppSystemCtrl.heartIntv)){
		sTlkAppSystemCtrl.heartTimer = clock_time()|1;
		tlkmdi_comm_sendSysEvt(TLKPRT_COMM_EVTID_SYS_HEART_BEAT, (uint08*)&sTlkAppSystemCtrl.heartNumb, 2);
		sTlkAppSystemCtrl.heartNumb ++;
	}	
}

/******************************************************************************
 * Function: tlkapp_system_poweroff
 * Descript: This Function for power off the meachine.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkapp_system_poweroff(void)
{
	usb_dp_pullup_en (0);
	
	cpu_sleep_wakeup(DEEPSLEEP_MODE, 0, 0);

	gpio_input_dis(TLKAPP_WAKEUP_PIN);	
	gpio_output_dis(TLKAPP_WAKEUP_PIN); 		
	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN,GPIO_PIN_PULLDOWN_100K);
	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_HIGH, 0); 
}

/******************************************************************************
 * Function: tlkapp_system_connectCompleteEvt
 * Descript: This function is used to handle connection completion events.
 * Params: [IN]pBdAddr - Address of the device where the connection occurred.
 *         [IN]status - State of connection. 0-success, others-failure.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
static void tlkapp_system_connectCompleteEvt(uint16 handle, uint08 status, uint08 *pBtAddr)
{
	gTlkAppSystemBusyTimer = 0;
	
	gTlkAppBtConnHandle = handle;

	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_system_connectCompleteEvt:{handle-0x%04x,status-%d}", handle, status);	

	#if (TLKMMI_BTMGR_BTREC_ENABLE)
	if(tlkmdi_btacl_getIdleCount() == 0){
		tlkmmi_btmgr_recClose();
		tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_system_connectCompleteEvt:001");	
	}else if(status != BTH_HCI_ERROR_OPERATION_CANCELED_BY_HOST){
		tlkmmi_btmgr_recStart(nullptr, 0, false, false);
		tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_system_connectCompleteEvt:002");	
	}
	#endif
}

/******************************************************************************
 * Function: tlkapp_system_disconnCompleteEvt
 * Descript: This function is used to handle disconnection completion events.
 * Params: [IN]pBdAddr - Address of the device where the disconnection occurred.
 *         [IN]reason - The reason of disconnected.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
static void tlkapp_system_disconnCompleteEvt(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	gTlkAppSystemBusyTimer = clock_time()|1;
	
	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_system_disconnCompleteEvt:{handle-0x%04x,reason-%d}", handle, reason);	
	
	btp_destroy(handle);
	bth_destroy(handle);
	#if (TLKMMI_AUDIO_ENABLE)
	tlkmmi_audio_disconn(handle);
	#endif

	if(tlkmdi_btacl_getIdleCount() == 0){
		tlkmmi_btmgr_recClose();	
	}else if(tlkmmi_btmgr_recIsPage() || tlkmmi_btmgr_recIsScan()){
		uint08 *pPageAddr = tlkmmi_btmgr_recPageAddr();
		if(pPageAddr != nullptr && tmemcmp(pBtAddr, pPageAddr, 6) == 0){
			tlkmmi_btmgr_recClose();
		}
		tlkmmi_btmgr_recStart(nullptr, 0, false, false);
	}else{
		if(reason == BTH_HCI_ERROR_CONN_TIMEOUT && bth_getAclCount() == 0){ //Start Reconnect
			bth_device_item_t *pItem = bth_device_getItem(pBtAddr, nullptr);
			if(pItem != nullptr){
				tlkmmi_btmgr_recStart(pBtAddr, pItem->devClass, true, true);
			}else{
				tlkmmi_btmgr_recStart(nullptr, 0, false, false);
			}
		}else{
			tlkmmi_btmgr_recStart(nullptr, 0, false, false);
		}
	}
}
static void tlkapp_system_profileConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID)
{
	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_system_profileConnectEvt:{handle-%d, status-%d, ptype-%d, usrID-%d}", handle, status, ptype, usrID);	
	if(status != TLK_ENONE) return;
	
	if(ptype == BTP_PTYPE_PBAP && usrID == BTP_USRID_CLIENT){
		#if (TLKMMI_PHONE_BOOK_ENABLE)
		tlkmmi_phone_startSyncBook(handle, bth_handle_getBtAddr(handle), false);
		#endif
	}
	#if (TLKMMI_AUDIO_ENABLE)
	tlkmmi_audio_connect(handle, ptype, usrID);
	#endif
}
static void tlkapp_system_profileDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID)
{
	if(ptype == BTP_PTYPE_PBAP && usrID == BTP_USRID_CLIENT){
		#if (TLKMMI_PHONE_BOOK_ENABLE)
		tlkmmi_phone_closeSyncBook(handle);
		#endif
	}
}



static void tlkapp_system_cmdHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "SysCmdHandler:msgID", msgID);

	if(msgID == TLKPRT_COMM_CMDID_SYS_VERSION){
		tlkapp_system_getVersionDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_REBOOT){
		tlkapp_system_rebootDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_POWER_OFF){
		tlkapp_system_powerOffDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_HEART){
		tlkapp_system_setHeartDeal(pData, dataLen);
	}
}
static void tlkapp_system_getVersionDeal(void)
{
	uint08 buffLen;
	uint08 buffer[12];
	uint16 prtVersion;
	uint32 libVersion;
	uint32 appVersion;

	libVersion = TLK_LIB_VERSION;
	prtVersion = TLK_PRT_VERSION;
	appVersion = TLK_APP_VERSION;
	
	buffLen = 0;
	buffer[buffLen++] = (libVersion & 0xFF000000) >> 24;
	buffer[buffLen++] = (libVersion & 0x00FF0000) >> 16;
	buffer[buffLen++] = (libVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (libVersion & 0x000000FF);
	buffer[buffLen++] = (prtVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (prtVersion & 0x000000FF);
	buffer[buffLen++] = (appVersion & 0xFF000000) >> 24;
	buffer[buffLen++] = (appVersion & 0x00FF0000) >> 16;
	buffer[buffLen++] = (appVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (appVersion & 0x000000FF);
	tlkmdi_comm_sendSysRsp(TLKPRT_COMM_CMDID_SYS_VERSION, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkapp_system_rebootDeal(void)
{
	sTlkAppSystemCtrl.isBoot = true;
	bth_hci_sendResetCmd();
	sTlkAppSystemCtrl.powerTimer = clock_time()|1;
	tlkmdi_comm_sendSysRsp(TLKPRT_COMM_CMDID_SYS_REBOOT, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkapp_system_powerOffDeal(void)
{
	sTlkAppSystemCtrl.isBoot = false;
	sTlkAppSystemCtrl.powerTimer = clock_time()|1;
	tlkmdi_comm_sendSysRsp(TLKPRT_COMM_CMDID_SYS_POWER_OFF, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkapp_system_setHeartDeal(uint08 *pData, uint08 dataLen)
{
	uint08 enable;
	uint16 timeout;
	
	sTlkAppSystemCtrl.heartTimer = 0;
	enable = pData[0];
	timeout = pData[1];
	if(timeout == 0) timeout = 3;
	if(dataLen < 2 || timeout > 100){
		tlkmdi_comm_sendSysRsp(TLKPRT_COMM_CMDID_SYS_SET_HEART, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(enable) sTlkAppSystemCtrl.heartTimer = clock_time()|1;
	else sTlkAppSystemCtrl.heartTimer = 0;
	sTlkAppSystemCtrl.heartIntv  = timeout*1000000;
	tlkmdi_comm_sendSysRsp(TLKPRT_COMM_CMDID_SYS_SET_HEART, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, &enable, 1);
}


