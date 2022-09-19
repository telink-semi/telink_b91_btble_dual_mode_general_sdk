/********************************************************************************************************
 * @file     tlkmdi_led.h
 *
 * @brief    This is the header file for BTBLE SDK
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
#ifndef TLKMDI_LED_H
#define TLKMDI_LED_H

#if (TLK_MDI_LED_ENABLE)




#define TLKMDI_LED_TIMEOUT          20000 //100ms

#define TLKMDI_LED_NOR_MAX_NUMB     2
#define TLKMDI_LED_PWM_MAX_NUMB     2

#if (TLKMDI_LED_PWM_MAX_NUMB != 0)
#define TLKMDI_LED_PWM_PCLK_SPEED   1000000 //pwm clock 1M.
enum{
	TLKMDI_LED_PWM_CLOCK_1S  = TLKMDI_LED_PWM_PCLK_SPEED,
	TLKMDI_LED_PWM_CLOCK_1MS = (TLKMDI_LED_PWM_CLOCK_1S / 1000),
	TLKMDI_LED_PWM_CLOCK_1US = (TLKMDI_LED_PWM_CLOCK_1S / 1000000),
	TLKMDI_LED_PWM_CLOCK_10MS  = (TLKMDI_LED_PWM_CLOCK_1S / 100),
	TLKMDI_LED_PWM_CLOCK_100MS = (TLKMDI_LED_PWM_CLOCK_1S / 10),
	TLKMDI_LED_PWM_CLOCK_10US  = (TLKMDI_LED_PWM_CLOCK_1S / 100000),
	TLKMDI_LED_PWM_CLOCK_100US = (TLKMDI_LED_PWM_CLOCK_1S / 10000),
};
#endif //(TLKMDI_LED_PWM_MAX_NUMB != 0)


typedef struct{
	uint08 ledID;
	uint08 level;
	uint16 count;
	uint08 resv0;
	uint08 isCtrl;
	uint08 isOpen;
	uint08 isKeep; //keep on when ctrl is over
	uint32 ioPort;
	uint32 onTimer;
	uint32 offTimer;
	uint32 runTimer;
}tlkmdi_led_nor_unit_t;
typedef struct{
	uint08 ledID;
	uint08 pwmID;
	uint16 count;
	uint08 isInvt; //invert control
	uint08 isCtrl;
	uint08 isOpen;
	uint08 isKeep; //keep on when ctrl is over
	uint32 ioPort;
	uint16 dutyMax;
	uint16 curDuty;
	uint16 stepChg;
	uint16 flushTmo; //Flush Timeout
	uint32 onTimer;
	uint32 offTimer;
	uint32 runTimer;
	uint32 flushTmr; //Flush Timer
}tlkmdi_led_pwm_unit_t;
typedef struct{
	#if (TLKMDI_LED_NOR_MAX_NUMB != 0)
	tlkapi_timer_t norTimer;
	tlkmdi_led_nor_unit_t norUnit[TLKMDI_LED_NOR_MAX_NUMB];
	#endif
	#if (TLKMDI_LED_PWM_MAX_NUMB != 0)
	tlkapi_timer_t pwmTimer;
	tlkmdi_led_pwm_unit_t pwmUnit[TLKMDI_LED_PWM_MAX_NUMB];
	#endif
	#if ((TLKMDI_LED_NOR_MAX_NUMB != 0) && (TLKMDI_LED_PWM_MAX_NUMB != 0))
	uint32 reserve;
	#endif
}tlkmdi_led_ctrl_t;


int tlkmdi_led_init(void);

#if (TLKMDI_LED_NOR_MAX_NUMB != 0)

int  tlkmdi_led_insert(uint08 ledID, uint32 ioPort, uint08 upDown, uint08 onLevel);
int  tlkmdi_led_remove(uint08 ledID, uint08 upDown, bool enOutput);
bool tlkmdi_led_isOn(uint08 ledID);
bool tlkmdi_led_isCtrl(uint08 ledID);
bool tlkmdi_led_on(uint08 ledID);
bool tlkmdi_led_off(uint08 ledID);
bool tlkmdi_led_ctrl(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs, bool isKeepOn);
#endif // #if (TLKMDI_LED_NOR_MAX_NUMB != 0)

#if (TLKMDI_LED_PWM_MAX_NUMB != 0)
int  tlkmdi_pwmled_insert(uint08 ledID, uint32 ioPort, uint08 pwmID, bool isInvert);
int  tlkmdi_pwmled_remove(uint08 ledID, uint08 upDown, bool enOutput);
bool tlkmdi_pwmled_isOn(uint08 ledID);
bool tlkmdi_pwmled_isCtrl(uint08 ledID);
bool tlkmdi_pwmled_on(uint08 ledID, uint16 pwmPeriodUs, uint16 pwmDutyUs);
bool tlkmdi_pwmled_off(uint08 ledID);
bool tlkmdi_pwmled_ctrl(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs,
	 uint16 pwmPeriodUs, uint16 pwmDutyUs, uint16 dutyFlushUs, uint08 stepUs, uint08 isKeepOn);
#endif


#endif //#if (TLK_MDI_LED_ENABLE)

#endif //#ifndef TLKMDI_LED_H

