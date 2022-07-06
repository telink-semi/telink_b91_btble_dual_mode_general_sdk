/********************************************************************************************************
 * @file     tlkapp_battery.c
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
#include "tlkapp_battery.h"


#if (TLKAPP_CFG_BAT_CHECK_ENABLE)
#include "tlkapp_adapt.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkprt/tlkprt_stdio.h"
#include "tlkmdi/tlkmdi_comm.h"

#include "drivers.h"


static bool tlkapp_battery_timer(tlkapi_timer_t *pTimer, void *pUsrArg);
static void tlkapp_battery_enterDeep(uint08 setFlag);
static void tlkapp_battery_leaveDeep(uint08 clrFlag);



_attribute_data_retention_ static uint16 sTlkAppBatPreValue = 0xFFFF;
_attribute_data_retention_ static uint16 sTlkAppBatVoltage = 4200;
_attribute_data_retention_ static uint08 sTlkAppBatLevel = 100;
_attribute_data_retention_ static tlkapi_timer_t sTlkAppBatTimer;


/******************************************************************************
 * Function: tlkapp_battery_init
 * Descript: Initial the battery's gpio and register the callback.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_battery_init(void)
{
#if (TLKDEV_CFG_BAT_ENABLE)
	tlkdev_bat_init(TLKAPP_BAT_GPIO_PIN, TLKAPP_BAT_ADC_PIN);
	tlkapp_adapt_initTimer(&sTlkAppBatTimer, tlkapp_battery_timer, nullptr, TLKAPP_BAT_CHECK_INTERVAL);
	tlkapp_adapt_insertTimer(&sTlkAppBatTimer);
#endif

#if (TLKAPP_BAT_LOW_PROTECT_ENABLE)
	tlkapp_battery_check();
	if((analog_read(TLKAPP_BAT_ANALOG_REG) & TLKAPP_BAT_FLAG_LOW_POWER) != 0){
		if(sTlkAppBatVoltage < TLKAPP_BAT_LOW_RESTORE_VOLTAGE){
			tlkapp_battery_enterDeep(0x00);
		}else{
			tlkapp_battery_leaveDeep(TLKAPP_BAT_FLAG_LOW_POWER);
		}
	}
#endif	
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapp_battery_check
 * Descript: This function use to check the voltage.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_battery_check(void)
{
#if (TLKDEV_CFG_BAT_ENABLE)
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
		if(tlkdev_bat_getVoltage(&voltage) == TLK_ENONE){
			adcBuff[count++] = voltage & 0x1FFF;
		}
	}
	if(count != 8){
		tlkapi_error(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "app_battery_check: get voltage failure");
		return -TLK_EFAIL;
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
	
	sTlkAppBatVoltage = (voltage<<2)/3;
	#if 1
	if(sTlkAppBatPreValue == 0xFFFF || sTlkAppBatPreValue < sTlkAppBatVoltage){
		sTlkAppBatPreValue = sTlkAppBatVoltage;
	}
	
	if(sTlkAppBatVoltage < 2400 && sTlkAppBatPreValue-sTlkAppBatVoltage > 40){
		sTlkAppBatVoltage += 40;
	}
	if(sTlkAppBatPreValue-sTlkAppBatVoltage < 50 && sTlkAppBatPreValue-sTlkAppBatVoltage > 20){
		if(sTlkAppBatPreValue-sTlkAppBatVoltage > 40) sTlkAppBatVoltage += 36;
		else if(sTlkAppBatPreValue-sTlkAppBatVoltage > 30) sTlkAppBatVoltage += 26;
		else sTlkAppBatVoltage += 15;
	}
	
	if(sTlkAppBatVoltage > sTlkAppBatPreValue) sTlkAppBatVoltage = sTlkAppBatPreValue;
	else sTlkAppBatPreValue = sTlkAppBatVoltage;

	if(sTlkAppBatVoltage >= 3000) sTlkAppBatLevel = 100;
	else if(sTlkAppBatVoltage < 2080) sTlkAppBatLevel = 0;		
	else sTlkAppBatLevel = (sTlkAppBatVoltage-2000)/10;
	#endif

	tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "app_battery_check: voltage-%d", sTlkAppBatVoltage);

	battery[0] = 0x00;
	battery[1] = (sTlkAppBatVoltage & 0x00FF);
	battery[2] = (sTlkAppBatVoltage & 0xFF00) >> 8;
	tlkmdi_comm_sendSysEvt(TLKPRT_COMM_EVTID_SYS_BATTERY, battery, 3);
#endif

#if (TLKAPP_BAT_LOW_PROTECT_ENABLE)
	if(sTlkAppBatVoltage < TLKAPP_BAT_LOW_PROTECT_VOLTAGE){
		tlkapp_battery_enterDeep(TLKAPP_BAT_FLAG_LOW_POWER);
	}
#endif

	return TLK_ENONE;
}


static bool tlkapp_battery_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	tlkapp_battery_check();
	return true;
}


static void tlkapp_battery_enterDeep(uint08 setFlag)
{
	if(setFlag != 0){
		uint08 flag;
		flag = analog_read(TLKAPP_BAT_ANALOG_REG) | setFlag;
		analog_write(TLKAPP_BAT_ANALOG_REG, flag);
	}
	
	gpio_function_en(TLKAPP_WAKEUP_PIN);
	gpio_output_dis(TLKAPP_WAKEUP_PIN); 		
	gpio_input_en(TLKAPP_WAKEUP_PIN);		
	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN,GPIO_PIN_PULLDOWN_100K);
	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_HIGH, 1);
	NDS_FENCE_IORW; //ensure that MIE bit of mstatus reg is cleared at hardware level
	
	cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
}
static void tlkapp_battery_leaveDeep(uint08 clrFlag)
{
	if(clrFlag != 0){
		uint08 flag;
		flag = analog_read(TLKAPP_BAT_ANALOG_REG) & (~clrFlag);
		analog_write(TLKAPP_BAT_ANALOG_REG, flag);
	}
}



#endif //#if (TLKAPP_CFG_BAT_CHECK_ENABLE)

