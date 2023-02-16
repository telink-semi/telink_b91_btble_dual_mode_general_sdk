/********************************************************************************************************
 * @file     tlkmdi_battery.c
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
#if (TLK_MDI_BATTERY_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkdev/sys/tlkdev_battery.h"
#include "tlkmdi_battery.h"
#include "drivers.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkmdi/misc/tlkmdi_comm.h"


#define TLKMDI_BAT_DBG_FLAG         ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_MISC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BAT_DBG_SIGN         "[APP]"


static bool tlkmdi_battery_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmdi_battery_enterDeep(uint08 setFlag);
static void tlkmdi_battery_leaveDeep(uint08 clrFlag);



static tlkmdi_battery_t sTlkMdiBatCtrl;


/******************************************************************************
 * Function: tlkapp_battery_init
 * Descript: Initial the battery's gpio and register the callback.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkmdi_battery_init(void)
{
	tmemset(&sTlkMdiBatCtrl, 0, sizeof(tlkmdi_battery_t));
	sTlkMdiBatCtrl.voltage = 4200;
	sTlkMdiBatCtrl.preValue = 0xFFFF;
	sTlkMdiBatCtrl.level = 100;

	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_battery_init();
	#endif
	tlkapi_timer_initNode(&sTlkMdiBatCtrl.timer, tlkmdi_battery_timer, NULL, TLKMDI_BAT_CHECK_INTERVAL);
	
	#if (TLKMDI_BAT_LOW_PROTECT_ENABLE)
	if((analog_read(TLKMDI_BAT_ANALOG_REG) & TLKMDI_BAT_FLAG_LOW_POWER) != 0){
		tlkmdi_battery_check();
		if(sTlkMdiBatCtrl.voltage < TLKMDI_BAT_LOW_RESTORE_VOLTAGE){
			tlkmdi_battery_enterDeep(0x00);
		}else{
			tlkmdi_battery_leaveDeep(TLKMDI_BAT_FLAG_LOW_POWER);
		}
	}
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_battery_startCheck(void)
{
	tlkapi_timer_insertNode(&sTlkMdiBatCtrl.timer);
}
void tlkmdi_battery_closeCheck(void)
{
	tlkapi_timer_removeNode(&sTlkMdiBatCtrl.timer);
}

/******************************************************************************
 * Function: tlkmdi_battery_check
 * Descript: This function use to check the voltage.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
void tlkmdi_battery_check(void)
{
	int j;
	uint16 temp;
	uint08 index;
	uint08 count;
	uint16 voltage;
	uint16 adcBuff[8];
	uint16 adcData[8];
	uint08 battery[4];
	
	count = 0;
	voltage = 0;
	for(index=0; index<8; index++){
		if(tlkdev_battery_getVoltage(&voltage) == TLK_ENONE){
			adcBuff[count++] = voltage & 0x1FFF;
		}
	}
	if(count != 8){
		tlkapi_error(TLKMDI_BAT_DBG_FLAG, TLKMDI_BAT_DBG_SIGN, "app_battery_check: get voltage failure");
		return;//return -TLK_EFAIL;
	}
	
	/*get adc sample data and sort these data */
	for(index=0; index<8; index++){
		adcData[index] = adcBuff[index];
		if(index != 0 && adcData[index] < adcData[index-1]){
			temp = adcData[index];
			adcData[index] = adcData[index-1];
			for(j=index-1; j>=0 && adcData[j] > temp; j--){
				adcData[j+1] = adcData[j];
			}
			adcData[j+1] = temp;
		}
	}
	voltage = (adcData[2] + adcData[3] + adcData[4] + adcData[5]) >> 2;
	
	sTlkMdiBatCtrl.voltage = (voltage<<2)/3;
	#if 1
	if(sTlkMdiBatCtrl.preValue == 0xFFFF || sTlkMdiBatCtrl.preValue < sTlkMdiBatCtrl.voltage){
		sTlkMdiBatCtrl.preValue = sTlkMdiBatCtrl.voltage;
	}
	
	if(sTlkMdiBatCtrl.voltage < 2400 && sTlkMdiBatCtrl.preValue-sTlkMdiBatCtrl.voltage > 40){
		sTlkMdiBatCtrl.voltage += 40;
	}
	if(sTlkMdiBatCtrl.preValue-sTlkMdiBatCtrl.voltage < 50 && sTlkMdiBatCtrl.preValue-sTlkMdiBatCtrl.voltage > 20){
		if(sTlkMdiBatCtrl.preValue-sTlkMdiBatCtrl.voltage > 40) sTlkMdiBatCtrl.voltage += 36;
		else if(sTlkMdiBatCtrl.preValue-sTlkMdiBatCtrl.voltage > 30) sTlkMdiBatCtrl.voltage += 26;
		else sTlkMdiBatCtrl.voltage += 15;
	}
	
	if(sTlkMdiBatCtrl.voltage > sTlkMdiBatCtrl.preValue) sTlkMdiBatCtrl.voltage = sTlkMdiBatCtrl.preValue;
	else sTlkMdiBatCtrl.preValue = sTlkMdiBatCtrl.voltage;

	if(sTlkMdiBatCtrl.voltage >= 3000) sTlkMdiBatCtrl.level = 100;
	else if(sTlkMdiBatCtrl.voltage < 2080) sTlkMdiBatCtrl.level = 0;		
	else sTlkMdiBatCtrl.level = (sTlkMdiBatCtrl.voltage-2000)/10;
	#endif

	tlkapi_trace(TLKMDI_BAT_DBG_FLAG, TLKMDI_BAT_DBG_SIGN, "app_battery_check: voltage-%d", sTlkMdiBatCtrl.voltage);

	battery[0] = 0x00;
	battery[1] = (sTlkMdiBatCtrl.voltage & 0x00FF);
	battery[2] = (sTlkMdiBatCtrl.voltage & 0xFF00) >> 8;
	tlktsk_sendInnerMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_BATTERY_REPORT, battery, 3);
	
#if (TLKMDI_BAT_LOW_PROTECT_ENABLE)
	if(sTlkMdiBatCtrl.voltage < TLKMDI_BAT_LOW_PROTECT_VOLTAGE){
		tlkmdi_battery_enterDeep(TLKMDI_BAT_FLAG_LOW_POWER);
	}
#endif
}


static bool tlkmdi_battery_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkmdi_battery_check();
	return true;
}


static void tlkmdi_battery_enterDeep(uint08 setFlag)
{
	if(setFlag != 0){
		uint08 flag;
		flag = analog_read(TLKMDI_BAT_ANALOG_REG) | setFlag;
		analog_write(TLKMDI_BAT_ANALOG_REG, flag);
	}
	
	core_enter_deep(PM_WAKEUP_PAD);  //deepsleep
}
static void tlkmdi_battery_leaveDeep(uint08 clrFlag)
{
	if(clrFlag != 0){
		uint08 flag;
		flag = analog_read(TLKMDI_BAT_ANALOG_REG) & (~clrFlag);
		analog_write(TLKMDI_BAT_ANALOG_REG, flag);
	}
}



#endif //TLK_MDI_BATTERY_ENABLE

