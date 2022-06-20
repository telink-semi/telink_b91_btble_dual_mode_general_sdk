/********************************************************************************************************
 * @file     tlkmmi_lemgrCtrl.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_LEMGR_ENABLE)
#include "tlkstk/ble/ble.h"
#include "blt_common.h"
#include "ble_device_manage.h"
#include "ble_simple_sdp.h"
#include "ble_custom_pair.h"
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrComm.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrCtrl.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAtt.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAcl.h"




int tlkmmi_lemgr_ctrlInit(void)
{

	return TLK_ENONE;
}



void app_ble_volumeInc(void)
{
	uint08 index;
	uint16 consumer_key;
	for(index=TLKMMI_LEMGR_MASTER_MAX_NUM; index < (TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM); index++){ //slave index is from "TLKMMI_LEMGR_MASTER_MAX_NUM" to "TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM - 1"
		if(conn_dev_list[index].conn_state){
			consumer_key = 0x00e9; //MKEY_VOL_UP;
			blc_gatt_pushHandleValueNotify(conn_dev_list[index].conn_handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
			consumer_key = 0;
			blc_gatt_pushHandleValueNotify(conn_dev_list[index].conn_handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
		}
	}
}
void app_ble_volumeDec(void)
{
	uint08 index;
	uint16 consumer_key;
	for(index=TLKMMI_LEMGR_MASTER_MAX_NUM; index < (TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM); index++){ //slave index is from "TLKMMI_LEMGR_MASTER_MAX_NUM" to "TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM - 1"
		if(conn_dev_list[index].conn_state){
			consumer_key = 0x00ea; //MKEY_VOL_DN;
			blc_gatt_pushHandleValueNotify(conn_dev_list[index].conn_handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
			consumer_key = 0;
			blc_gatt_pushHandleValueNotify(conn_dev_list[index].conn_handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
		}
	}
}


#if 0//(UI_KEYBOARD_ENABLE)

_attribute_ble_data_retention_	int 	key_not_released;


#define CONSUMER_KEY   	   			1
#define OTHER_KEY 		  	   		2

_attribute_ble_data_retention_	uint08 		key_type;


/**
 * @brief   Check changed key value.
 * @param   none.
 * @return  none.
 */
void key_change_proc(void)
{

	uint08 key0 = kb_event.keycode[0];
	uint08 key1 = kb_event.keycode[1];
//	uint08 key_buf[8] = {0,0,0,0,0,0,0,0};

	key_not_released = 1;
	if(kb_event.cnt > 2)
	{

	}
	if (kb_event.cnt == 2)   //two key press
	{
		if((key0 == KEY_SW2 && key1 == KEY_SW4) || (key0 == KEY_SW4 && key1 == KEY_SW2))//scan device
		{
			my_dump_str_data(TLKMMI_LEMGR_DBG_ENABLE, "scan device", 0, 0);
		}
		if((key0 == KEY_SW3 && key1 == KEY_SW4) || (key0 == KEY_SW4 && key1 == KEY_SW3))//cancel scan device
		{
			my_dump_str_data(TLKMMI_LEMGR_DBG_ENABLE, "cancel scan device", 0, 0);
		}
	}
	else if(kb_event.cnt == 1)
	{
		if(key0 == KEY_SW2 || key0 == KEY_SW3)  //used for BLE consumer key volume up/down
		{
			key_type = CONSUMER_KEY;
			uint16 consumer_key;
			if(key0 == KEY_SW3){  	//volume up
				consumer_key = 0x00e9; //MKEY_VOL_UP;
				my_dump_str_data(TLKMMI_LEMGR_DBG_ENABLE, "UI send Vol+", 0, 0);
			}
			else if(key0 == KEY_SW2){ //volume down
				consumer_key = 0x00ea; //MKEY_VOL_DN;
				my_dump_str_data(TLKMMI_LEMGR_DBG_ENABLE, "UI send Vol-", 0, 0);
			}


			/*Here is just Telink Demonstration effect. Cause the demo board has limited key to use, when Vol+/Vol- key pressed, we
			send media key "Vol+" or "Vol-" to master for all slave in connection.
			For users, you should known that this is not a good method, you should manage your device and GATT data transfer
			according to  conn_dev_list[]
			 * */
			for(int i=TLKMMI_LEMGR_MASTER_MAX_NUM; i < (TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM); i++){ //slave index is from "TLKMMI_LEMGR_MASTER_MAX_NUM" to "TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM - 1"
				if(conn_dev_list[i].conn_state){
					blc_gatt_pushHandleValueNotify (conn_dev_list[i].conn_handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08 *)&consumer_key, 2);
				}
			}
		}
		else{
			key_type = OTHER_KEY;

			if(key0 == KEY_SW4)
			{
				my_dump_str_data(TLKMMI_LEMGR_DBG_ENABLE, "SW4 press", 0, 0);
			}
			else if(key0 == KEY_SW5)
			{
				my_dump_str_data(TLKMMI_LEMGR_DBG_ENABLE, "SW5 press", 0, 0);
			}

		}

	}
	else   //kb_event.cnt == 0,  key release
	{
		key_not_released = 0;
		if(key_type == CONSUMER_KEY)
		{
			uint16 consumer_key = 0;
			//Here is just Telink Demonstration effect. for all slave in connection, send release for previous "Vol+" or "Vol-" to master
			for(int i=TLKMMI_LEMGR_MASTER_MAX_NUM; i < (TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM); i++){ //slave index is from "TLKMMI_LEMGR_MASTER_MAX_NUM" to "TLKMMI_LEMGR_MASTER_MAX_NUM + TLKMMI_LEMGR_SLAVE_MAX_NUM - 1"
				if(conn_dev_list[i].conn_state){
					blc_gatt_pushHandleValueNotify ( conn_dev_list[i].conn_handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08 *)&consumer_key, 2);
				}
			}
		}
		else{

		}
	}


}

#define GPIO_WAKEUP_KEYPROC_CNT				3

_attribute_data_retention_	static u32 keyScanTick = 0;
_attribute_data_retention_	static int gpioWakeup_keyProc_cnt = 0;

/**
 * @brief      keyboard task handler
 * @param[in]  e    - event type
 * @param[in]  p    - Pointer point to event parameter.
 * @param[in]  n    - the length of event parameter.
 * @return     none.
 */
void proc_keyboard (uint08 e, uint08 *p, int n)
{
	//when key press gpio wakeup suspend, proc keyscan at least GPIO_WAKEUP_KEYPROC_CNT times
	//regardless of 8000 us interval
	if(e == CONTR_EVT_PM_GPIO_EARLY_WAKEUP){
		gpioWakeup_keyProc_cnt = GPIO_WAKEUP_KEYPROC_CNT;
	}
	else if(gpioWakeup_keyProc_cnt){
		gpioWakeup_keyProc_cnt --;
	}


	if(gpioWakeup_keyProc_cnt || clock_time_exceed(keyScanTick, 8000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}


	kb_event.keycode[0] = 0;
	int det_key = kb_scan_key (0, 1);

	if (det_key){
		key_change_proc();
	}
}




/**
 * @brief      keyboard initialization
 * @param[in]  none
 * @return     none.
 */
void keyboard_init(void)
{
#if (TLK_CFG_PM_ENABLE)
	/////////// keyboard GPIO wakeup init ////////
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++){
		pm_set_gpio_wakeup (pin[i], WAKEUP_LEVEL_HIGH, 1);  //drive pin pad high level wakeup deepsleep
	}

	btble_contr_registerControllerEventCallback (CONTR_EVT_PM_GPIO_EARLY_WAKEUP, &proc_keyboard);
#endif
}



/**
 * @brief      callback function of Controller Event "CONTR_EVT_PM_SLEEP_ENTER"
 * @param[in]  e - BT BLE Controller Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
_attribute_ram_code_ void app_set_kb_wakeup(uint08 e, uint08 *p, int n)
{
#if (TLK_CFG_PM_ENABLE)
	//sleep time > 50ms, add GPIO PAD wakeup
	if(((u32)(btble_pm_getSystemWakeupTick() - clock_time())) > 50 * SYSTEM_TIMER_TICK_1MS){
		btble_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
	}
#endif
}

#endif





#endif //TLKMMI_LEMGR_ENABLE

