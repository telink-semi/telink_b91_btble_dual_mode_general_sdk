/********************************************************************************************************
 * @file     tlkmdi_led.c
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
#if (TLK_MDI_LED_ENABLE)
#include "tlkmdi_led.h"
#include "drivers.h"


#define TLKMDI_LED_DBG_FLAG       ((TLK_MINOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_LED << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_LED_DBG_SIGN       "[MDI]"


#if (TLKMDI_LED_NOR_MAX_NUMB != 0)
static bool tlkmdi_led_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmdi_led_handler(tlkmdi_led_nor_unit_t *pUnit);
static tlkmdi_led_nor_unit_t *tlkmdi_led_getIdleUnit(void);
static tlkmdi_led_nor_unit_t *tlkmdi_led_getUsedUnit(uint08 ledID);
#endif //#if (TLKMDI_LED_NOR_MAX_NUMB != 0)
#if (TLKMDI_LED_PWM_MAX_NUMB != 0)
static bool tlkmdi_pwmled_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmdi_pwmled_handler(tlkmdi_led_pwm_unit_t *pUnit);
static tlkmdi_led_pwm_unit_t *tlkmdi_pwmled_getIdleUnit(void);
static tlkmdi_led_pwm_unit_t *tlkmdi_pwmled_getUsedUnit(uint08 ledID);
#endif //#if (TLKMDI_LED_PWM_MAX_NUMB != 0)

static tlkmdi_led_ctrl_t sTlkMdiLedCtrl;

/******************************************************************************
 * Function: tlkmdi_led_init
 * Descript: Initial I/O and PWM timers of the led
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkmdi_led_init(void)
{
	tmemset(&sTlkMdiLedCtrl, 0, sizeof(tlkmdi_led_ctrl_t));
	#if (TLKMDI_LED_NOR_MAX_NUMB != 0)
	tlkapi_timer_initNode(&sTlkMdiLedCtrl.norTimer, tlkmdi_led_timer,    (uint32)&sTlkMdiLedCtrl, TLKMDI_LED_TIMEOUT);
	#endif
	#if (TLKMDI_LED_PWM_MAX_NUMB != 0)
	tlkapi_timer_initNode(&sTlkMdiLedCtrl.pwmTimer, tlkmdi_pwmled_timer, (uint32)&sTlkMdiLedCtrl, TLKMDI_LED_TIMEOUT);
	#endif
	
	return TLK_ENONE;
}

#if (TLKMDI_LED_NOR_MAX_NUMB != 0)

/******************************************************************************
 * Function: tlkmdi_led_insert
 * Descript: Insert a led and initial the GPIO for the led.
 * Params: @ledID[IN]--The ledID, for instance 0x01, 0x02.
 * 		   @ioPort[IN]--ioPort
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @onLevel[IN]--LED effective level.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int tlkmdi_led_insert(uint08 ledID, uint32 ioPort, uint08 upDown, uint08 onLevel)
{
	tlkmdi_led_nor_unit_t *pUnit;

	if(ledID == 0 || ioPort == 0){
		tlkapi_error(TLKMDI_LED_DBG_FLAG, TLKMDI_LED_DBG_SIGN, "tlkmdi_led_insert: failure - error param");
		return -TLK_EPARAM;
	}
	
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit != nullptr) return -TLK_EREPEAT;

	pUnit = tlkmdi_led_getIdleUnit();
	if(pUnit == nullptr) return -TLK_EQUOTA;
	
	pUnit->ledID = ledID;
	if(onLevel != 0) pUnit->level = 1;
	else pUnit->level = 0;
	pUnit->ioPort = ioPort;
	
	gpio_function_en(pUnit->ioPort);
	gpio_set_input_en(pUnit->ioPort, 0);
    gpio_set_output_en(pUnit->ioPort, 1);
	gpio_setup_up_down_resistor(pUnit->ioPort, upDown);
	gpio_set_level(pUnit->ioPort, !pUnit->level);
		
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_led_remove
 * Descript: Disable the gpio for the led and remove a timer.
 * Params: @ledID[IN]--The ledID.
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @enOutput[IN]--enOutput, true enable output, false disable output.
 * Return: TLK_ENONE is success, other value is false.
*******************************************************************************/
int tlkmdi_led_remove(uint08 ledID, uint08 upDown, bool enOutput)
{
	uint08 index;
	tlkmdi_led_nor_unit_t *pUnit;
	
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return -TLK_EPARAM;
	
	gpio_set_level(pUnit->ioPort, !pUnit->level);
	gpio_function_en(pUnit->ioPort);
	gpio_set_input_en(pUnit->ioPort, 0);
    gpio_set_output_en(pUnit->ioPort, enOutput);
	gpio_setup_up_down_resistor(pUnit->ioPort, upDown);
	tmemcpy(pUnit, 0, sizeof(tlkmdi_led_nor_unit_t));
	for(index=0; index<TLKMDI_LED_NOR_MAX_NUMB; index++){
		if(sTlkMdiLedCtrl.norUnit[index].ledID != 0 && sTlkMdiLedCtrl.norUnit[index].isCtrl) break;
	}
	if(index == TLKMDI_LED_NOR_MAX_NUMB){
		tlkapi_timer_removeNode(&sTlkMdiLedCtrl.norTimer);
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_led_isOn
 * Descript: Get the led switch status.
 * Params: @ledID[IN]--The ledID.
 * Return: True is on, false is off.
*******************************************************************************/
bool tlkmdi_led_isOn(uint08 ledID)
{
	tlkmdi_led_nor_unit_t *pUnit;
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	return pUnit->isOpen;
}

/******************************************************************************
 * Function: tlkmdi_led_isAuto
 * Descript: Get the led control status.
 * Params: @ledID[IN]--The ledID.
 * Return: true is controlled, false is not controlled.
*******************************************************************************/
bool tlkmdi_led_isAuto(uint08 ledID)
{
	tlkmdi_led_nor_unit_t *pUnit;
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	return pUnit->isCtrl;
}

/******************************************************************************
 * Function: tlkmdi_led_on
 * Descript: Turn on a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkmdi_led_on(uint08 ledID)
{
	tlkmdi_led_nor_unit_t *pUnit;
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	pUnit->isCtrl = false;
	pUnit->isOpen = true;
	gpio_set_level(pUnit->ioPort, pUnit->level);
	return true;
}

/******************************************************************************
 * Function: tlkmdi_led_off
 * Descript: Turn off a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkmdi_led_off(uint08 ledID)
{
	tlkmdi_led_nor_unit_t *pUnit;
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	pUnit->isCtrl = false;
	pUnit->isOpen = false;
	gpio_set_level(pUnit->ioPort, !pUnit->level);
	return true;
}

/******************************************************************************
 * Function: tlkmdi_led_auto
 * Descript: Control led flash,the led flash frequency can be controlled by
 * 			 'onTimerMs' and 'offTimerMs', can also choose to keep the led on
 * 			 or off after the flashing is over.
 * Params: @ledID[IN]--The ledID.
 * 		   @firstOn[IN]--The state of the first work, true is on, false is off.
 * 		   @count[IN]--led flash count.
 * 		   @onTimerMs[IN]--on time.
 * 		   @offTimerMs[IN]--off time.
 * 		   @isKeepOn[IN]--The status after the flash is over, true is on, false is off.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkmdi_led_auto(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs, bool isKeepOn) 
{
	tlkmdi_led_nor_unit_t *pUnit;
	
	if(onTimerMs == 0) return false;
	
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	
	if(!firstOn && offTimerMs == 0) firstOn = true;
	if(!firstOn){
		pUnit->isOpen = false;
		gpio_set_level(pUnit->ioPort, !pUnit->level);
	}else{
		pUnit->isOpen = true;
		gpio_set_level(pUnit->ioPort, pUnit->level);
	}
	
	pUnit->count = count;
	pUnit->isCtrl = true;
	pUnit->isKeep = isKeepOn;
	pUnit->onTimer = onTimerMs*1000;
	pUnit->offTimer = offTimerMs*1000;
	pUnit->runTimer = clock_time();
	tlkapi_timer_insertNode(&sTlkMdiLedCtrl.norTimer);
	
	return true;
}
static bool tlkmdi_led_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	bool isBusy;
	uint08 index;
	tlkmdi_led_nor_unit_t *pUnit;
	isBusy = false;
	for(index=0; index<TLKMDI_LED_NOR_MAX_NUMB; index++){
		pUnit = &sTlkMdiLedCtrl.norUnit[index];
		if(pUnit->ledID == 0 || !pUnit->isCtrl) continue;
		tlkmdi_led_handler(pUnit);
		if(!isBusy && pUnit->isCtrl) isBusy = true;
	}
	return isBusy;
}
static void tlkmdi_led_handler(tlkmdi_led_nor_unit_t *pUnit)
{
	if(!pUnit->isCtrl) return;
	if(pUnit->isOpen && clock_time_exceed(pUnit->runTimer, pUnit->onTimer)){
		pUnit->runTimer = clock_time();
		if(pUnit->offTimer != 0){
			pUnit->isOpen = false;
			gpio_set_level(pUnit->ioPort, !pUnit->level);
		}
		if(pUnit->count != 0){
			pUnit->count--;
			if(pUnit->count != 0) return;
			if(!pUnit->isKeep){
				pUnit->isOpen = false;
				gpio_set_level(pUnit->ioPort, !pUnit->level);
			}else{
				pUnit->isOpen = true;
				gpio_set_level(pUnit->ioPort, pUnit->level);
			}
			pUnit->isCtrl = false;
			return;
		}
	}
	if(!pUnit->isOpen && pUnit->onTimer != 0 && clock_time_exceed(pUnit->runTimer, pUnit->offTimer)){
		pUnit->isOpen = true;
		pUnit->runTimer = clock_time();
		gpio_set_level(pUnit->ioPort, pUnit->level);
	}
}
static tlkmdi_led_nor_unit_t *tlkmdi_led_getIdleUnit(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_LED_NOR_MAX_NUMB; index++){
		if(sTlkMdiLedCtrl.norUnit[index].ledID == 0) break;
	}
	if(index == TLKMDI_LED_NOR_MAX_NUMB) return nullptr;
	return &sTlkMdiLedCtrl.norUnit[index];
}

static tlkmdi_led_nor_unit_t *tlkmdi_led_getUsedUnit(uint08 ledID)
{
	uint08 index;
	if(ledID == 0) return nullptr;
	for(index=0; index<TLKMDI_LED_NOR_MAX_NUMB; index++){
		if(sTlkMdiLedCtrl.norUnit[index].ledID == ledID) break;
	}
	if(index == TLKMDI_LED_NOR_MAX_NUMB) return nullptr;
	return &sTlkMdiLedCtrl.norUnit[index];
}
#endif //#if (TLKMDI_LED_NOR_MAX_NUMB != 0)




#if (TLKMDI_LED_PWM_MAX_NUMB != 0)

static void tlkmdi_led_pwmStart(uint08 pwmID)
{
	if(pwmID == 0) pwm_start(FLD_PWM0_EN);
	else pwm_start(BIT(pwmID));
}
static void tlkmdi_led_pwmStop(uint08 pwmID)
{
	if(pwmID == 0) pwm_stop(FLD_PWM0_EN);
	else pwm_stop(BIT(pwmID));
}
static void tlkmdi_pwmled_innerOn(tlkmdi_led_pwm_unit_t *pUnit, uint16 pwmDutyUs)
{
	pUnit->isOpen = true;
	pwm_set_tcmp(pUnit->pwmID-1, pwmDutyUs * TLKMDI_LED_PWM_CLOCK_1US);
	tlkmdi_led_pwmStart(pUnit->pwmID-1);
}
static void tlkmdi_pwmled_innerOff(tlkmdi_led_pwm_unit_t *pUnit)
{
	pUnit->isOpen = false;
	pwm_set_tcmp(pUnit->pwmID-1, 0);
	tlkmdi_led_pwmStop(pUnit->pwmID-1);
}

/******************************************************************************
 * Function: tlkmdi_pwmled_insert
 * Descript: Insert an led with a PWM function, enable the PWM function of the LED.
 * Params: @ledID[IN]--The ledID.
 * 		   @ioPort[IN]--ioPort
 * 		   @pwmID[IN]--refer to 'pwm_id_e'.
 * 		   @isInvert[IN]--default true.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int tlkmdi_pwmled_insert(uint08 ledID, uint32 ioPort, uint08 pwmID, bool isInvert)
{
	tlkmdi_led_pwm_unit_t *pUnit;
	
	if(ledID == 0 || ioPort == 0 || pwmID > PWM5_ID){
		tlkapi_error(TLKMDI_LED_DBG_FLAG, TLKMDI_LED_DBG_SIGN, "tlkmdi_led_insertPwm: failure - error param");
		return -TLK_EPARAM;
	}
	
	pUnit = tlkmdi_pwmled_getUsedUnit(ledID);
	if(pUnit != nullptr) return -TLK_EREPEAT;
	
	pUnit = tlkmdi_pwmled_getIdleUnit();
	if(pUnit == nullptr) return -TLK_EQUOTA;
	
	pUnit->ledID = ledID;
	pUnit->pwmID = pwmID + 1;
	pUnit->ioPort = ioPort;
	
	pwm_set_pin(pUnit->ioPort);
	if(isInvert) pwm_n_invert_en(pUnit->pwmID-1);
	pwm_set_clk((uint08)(sys_clk.pclk*1000*1000/TLKMDI_LED_PWM_PCLK_SPEED-1));
	pwm_set_tcmp(pUnit->pwmID-1, TLKMDI_LED_PWM_CLOCK_10US);
    pwm_set_tmax(pUnit->pwmID-1, TLKMDI_LED_PWM_CLOCK_1MS);
	tlkmdi_led_pwmStop(pUnit->pwmID-1);

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_pwmled_remove
 * Descript: Disable the gpio for the led, remove an led with a PWM function.
 * Params: @ledID[IN]--The ledID.
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @enOutput[IN]--enOutput, true enable output, false disable output.
 * Return: TLK_ENONE is success,other value is failure.
*******************************************************************************/
int tlkmdi_pwmled_remove(uint08 ledID, uint08 upDown, bool enOutput)
{
	uint08 index;
	tlkmdi_led_pwm_unit_t *pUnit;

	if(ledID == 0) return -TLK_EPARAM;
	
	pUnit = tlkmdi_pwmled_getUsedUnit(ledID);
	if(pUnit == nullptr) return -TLK_ENOOBJECT;

	tlkmdi_led_pwmStop(pUnit->pwmID-1);
	
	gpio_function_en(pUnit->ioPort);
	gpio_set_input_en(pUnit->ioPort, 0);
    gpio_set_output_en(pUnit->ioPort, enOutput);
	gpio_setup_up_down_resistor(pUnit->ioPort, upDown);
	tmemcpy(pUnit, 0, sizeof(tlkmdi_led_nor_unit_t));
	for(index=0; index<TLKMDI_LED_NOR_MAX_NUMB; index++){
		if(sTlkMdiLedCtrl.pwmUnit[index].ledID != 0 && sTlkMdiLedCtrl.pwmUnit[index].isCtrl) break;
	}
	if(index == TLKMDI_LED_NOR_MAX_NUMB){
		tlkapi_timer_removeNode(&sTlkMdiLedCtrl.pwmTimer);
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_pwmled_on
 * Descript: Turn on an led with a PWM function.
 * Params: @keledIDyID[IN]--The ledID.
 * Return: true is on, false is off.
*******************************************************************************/
bool tlkmdi_pwmled_on(uint08 ledID, uint16 pwmPeriodUs, uint16 pwmDutyUs)
{
	tlkmdi_led_pwm_unit_t *pUnit;
	
	pUnit = tlkmdi_pwmled_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;

	pUnit->isCtrl = false;
	pwm_set_tmax(pUnit->pwmID-1, pwmPeriodUs * TLKMDI_LED_PWM_CLOCK_1US);
	tlkmdi_pwmled_innerOn(pUnit, pwmDutyUs);
	return true;
}

/******************************************************************************
 * Function: tlkmdi_pwmled_off
 * Descript: Turn off a led with a PWM function.
 * Params: @ledID[IN]--The ledID.
 * Return: true is on, false is off.
*******************************************************************************/
bool tlkmdi_pwmled_off(uint08 ledID)
{
	tlkmdi_led_pwm_unit_t *pUnit;
	
	pUnit = tlkmdi_pwmled_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	
	pUnit->isCtrl = false;
	tlkmdi_pwmled_innerOff(pUnit);
	return true;
}

/******************************************************************************
 * Function: tlkmdi_pwmled_auto
 * Descript: Use PWM to control led breathing,can choose to keep the led on
 * 			 or off after the breathing is over.
 * Params: @ledID[IN]--The ledID.
 * 		   @firstOn[IN]--The state of the first work, true is on, false is off.
 * 		   @count[IN]--breath count.
 * 		   @onTimerMs[IN]--on time.
 * 		   @offTimerMs[IN]--off time.
 * 		   @pwmPeriodUs[IN]--One PWM cycle.
 * 		   @pwmDutyUs[IN]--Duty cycle.
 * 		   @dutyFlushUs[IN]--Duty cycle(pwmDutyUs) changes time.
 * 		   @stepUs[IN]--breath step.
 * 		   @isKeepOn[IN]--The status after the breath is over, true is on,
 * 		   				false is off.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkmdi_pwmled_auto(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs,
	uint16 pwmPeriodUs, uint16 pwmDutyUs, uint16 dutyFlushUs, uint08 stepUs, uint08 isKeepOn)
{
	tlkmdi_led_pwm_unit_t *pUnit;
	
	if(onTimerMs == 0) return false; //Param Error
	if(dutyFlushUs != 0 && (stepUs == 0 || stepUs >= pwmPeriodUs)) return false; //Param Error
	
	pUnit = tlkmdi_pwmled_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
			
	pUnit->count = count;
	pUnit->isCtrl = true;
	pUnit->isInvt = false;
	pUnit->isKeep = isKeepOn;
	pUnit->stepChg = stepUs * TLKMDI_LED_PWM_CLOCK_1US;
	pUnit->onTimer = onTimerMs*1000;
	pUnit->offTimer = offTimerMs*1000;
	pUnit->runTimer = clock_time()|1;
	pUnit->flushTmo = dutyFlushUs;
	if(pUnit->flushTmo != 0) pUnit->flushTmr = clock_time()|1;
	
	pUnit->dutyMax = pwmDutyUs*TLKMDI_LED_PWM_CLOCK_1US;
	if(pUnit->flushTmo != 0) pUnit->curDuty = pUnit->stepChg;
	else pUnit->curDuty = pUnit->dutyMax;
	pwm_set_tcmp(pUnit->pwmID-1, pUnit->curDuty);
    pwm_set_tmax(pUnit->pwmID-1, pUnit->dutyMax);
	if(!firstOn && offTimerMs == 0) firstOn = true;
	if(!firstOn){
		tlkmdi_pwmled_innerOff(pUnit);
	}else{
		tlkmdi_pwmled_innerOn(pUnit, pUnit->curDuty);
	}
	
	tlkapi_timer_insertNode(&sTlkMdiLedCtrl.pwmTimer);
	
	return true;
}
static bool tlkmdi_pwmled_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	bool isBusy;
	uint08 index;
	tlkmdi_led_pwm_unit_t *pUnit;
	isBusy = false;
	for(index=0; index<TLKMDI_LED_PWM_MAX_NUMB; index++){
		pUnit = &sTlkMdiLedCtrl.pwmUnit[index];
		if(pUnit->ledID == 0 || !pUnit->isCtrl) continue;
		tlkmdi_pwmled_handler(pUnit);
		if(!isBusy && pUnit->isCtrl) isBusy = true;
	}
	return isBusy;
}
static void tlkmdi_pwmled_handler(tlkmdi_led_pwm_unit_t *pUnit)
{
	if(!pUnit->isCtrl) return;
	if(pUnit->isOpen && pUnit->flushTmo != 0 && clock_time_exceed(pUnit->flushTmr, pUnit->flushTmo)){
		if(pUnit->isInvt){
			if(pUnit->curDuty >= pUnit->stepChg) pUnit->curDuty -= pUnit->stepChg;
			else pUnit->curDuty = 0;
			if(pUnit->curDuty == 0) pUnit->isInvt = false;
		}else{
			if(pUnit->curDuty+pUnit->stepChg <= pUnit->dutyMax) pUnit->curDuty += pUnit->stepChg;
			else pUnit->curDuty = pUnit->dutyMax;
			if(pUnit->curDuty == pUnit->dutyMax) pUnit->isInvt = true;
		}
		pwm_set_tcmp(pUnit->pwmID-1, pUnit->curDuty);
	}
	if(pUnit->isOpen && clock_time_exceed(pUnit->runTimer, pUnit->onTimer)){
		pUnit->runTimer = clock_time();
		if(pUnit->offTimer != 0){
			tlkmdi_pwmled_innerOff(pUnit);
		}
		if(pUnit->count != 0){
			pUnit->count--;
			if(pUnit->count != 0) return;
			if(!pUnit->isKeep){
				tlkmdi_pwmled_innerOff(pUnit);
			}else{
				tlkmdi_pwmled_innerOn(pUnit, pUnit->dutyMax);
			}
			pUnit->isCtrl = false;
			return;
		}
	}
	if(!pUnit->isOpen && pUnit->onTimer != 0 && clock_time_exceed(pUnit->runTimer, pUnit->offTimer)){
		pUnit->runTimer = clock_time();
		if(pUnit->flushTmo != 0) pUnit->curDuty = pUnit->stepChg;
		else pUnit->curDuty = pUnit->dutyMax;
		tlkmdi_pwmled_innerOn(pUnit, pUnit->curDuty);
	}
}
static tlkmdi_led_pwm_unit_t *tlkmdi_pwmled_getIdleUnit(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_LED_PWM_MAX_NUMB; index++){
		if(sTlkMdiLedCtrl.pwmUnit[index].ledID == 0) break;
	}
	if(index == TLKMDI_LED_PWM_MAX_NUMB) return nullptr;
	return &sTlkMdiLedCtrl.pwmUnit[index];
}

static tlkmdi_led_pwm_unit_t *tlkmdi_pwmled_getUsedUnit(uint08 ledID)
{
	uint08 index;
	if(ledID == 0) return nullptr;
	for(index=0; index<TLKMDI_LED_PWM_MAX_NUMB; index++){
		if(sTlkMdiLedCtrl.pwmUnit[index].ledID == ledID) break;
	}
	if(index == TLKMDI_LED_PWM_MAX_NUMB) return nullptr;
	return &sTlkMdiLedCtrl.pwmUnit[index];
}

#endif //(TLKMDI_LED_PWM_MAX_NUMB != 0)





#endif //#if (TLK_MDI_LED_ENABLE)

