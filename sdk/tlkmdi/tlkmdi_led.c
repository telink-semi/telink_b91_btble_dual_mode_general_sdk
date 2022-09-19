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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_LED_ENABLE)
#include "tlkmdi_adapt.h"
#include "tlkmdi_led.h"
#include "drivers.h"


#define TLKMDI_LED_DBG_FLAG         (TLKMDI_LED_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_LED_DBG_SIGN         TLKMDI_DBG_SIGN

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


int tlkmdi_led_init(void)
{
	tmemset(&sTlkMdiLedCtrl, 0, sizeof(tlkmdi_led_ctrl_t));
	#if (TLKMDI_LED_NOR_MAX_NUMB != 0)
	tlkmdi_adapt_initTimer(&sTlkMdiLedCtrl.norTimer, tlkmdi_led_timer, (uint32)&sTlkMdiLedCtrl, TLKMDI_LED_TIMEOUT);
	#endif
	#if (TLKMDI_LED_PWM_MAX_NUMB != 0)
	tlkmdi_adapt_initTimer(&sTlkMdiLedCtrl.pwmTimer, tlkmdi_pwmled_timer, (uint32)&sTlkMdiLedCtrl, TLKMDI_LED_TIMEOUT);
	#endif
	
	return TLK_ENONE;
}

#if (TLKMDI_LED_NOR_MAX_NUMB != 0)

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
		tlkmdi_adapt_removeTimer(&sTlkMdiLedCtrl.norTimer);
	}
	
	return TLK_ENONE;
}

bool tlkmdi_led_isOn(uint08 ledID)
{
	tlkmdi_led_nor_unit_t *pUnit;
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	return pUnit->isOpen;
}
bool tlkmdi_led_isCtrl(uint08 ledID)
{
	tlkmdi_led_nor_unit_t *pUnit;
	pUnit = tlkmdi_led_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	return pUnit->isCtrl;
}

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
bool tlkmdi_led_ctrl(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs, bool isKeepOn) 
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
	tlkmdi_adapt_insertTimer(&sTlkMdiLedCtrl.norTimer);
	
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
		tlkmdi_adapt_removeTimer(&sTlkMdiLedCtrl.pwmTimer);
	}
	
	return TLK_ENONE;
}

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
bool tlkmdi_pwmled_off(uint08 ledID)
{
	tlkmdi_led_pwm_unit_t *pUnit;
	
	pUnit = tlkmdi_pwmled_getUsedUnit(ledID);
	if(pUnit == nullptr) return false;
	
	pUnit->isCtrl = false;
	tlkmdi_pwmled_innerOff(pUnit);
	return true;
}
bool tlkmdi_pwmled_ctrl(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs,
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
	
	tlkmdi_adapt_insertTimer(&sTlkMdiLedCtrl.pwmTimer);
	
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

