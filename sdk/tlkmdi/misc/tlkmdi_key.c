/********************************************************************************************************
 * @file     tlkmdi_key.c
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
#if (TLK_MDI_KEY_ENABLE)
#include "tlkmdi_key.h"
#include "drivers.h"


#define TLKMDI_KEY_DBG_FLAG       ((TLK_MINOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_KEY << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_KEY_DBG_SIGN       "[MDI]"


static bool tlkmdi_key_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static bool tlkmdi_key_check(tlkmdi_key_unit_t *pUnit);
static tlkmdi_key_unit_t *tlkmdi_key_getIdleUnit(void);
static tlkmdi_key_unit_t *tlkmdi_key_getUsedUnit(uint08 keyID);



static tlkmdi_key_ctrl_t sTlkMdiKeyCtrl;

/******************************************************************************
 * Function: tlkmdi_key_init
 * Descript: Initial the timer of the key.
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkmdi_key_init(void)
{
	tmemset(&sTlkMdiKeyCtrl, 0, sizeof(tlkmdi_key_ctrl_t));
	tlkapi_timer_initNode(&sTlkMdiKeyCtrl.timer, tlkmdi_key_timer, (uint32)&sTlkMdiKeyCtrl.timer, TLKMDI_KEY_TIMEOUT0);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_key_insert
 * Descript: Insert a key,initial its GPIO and make timer working.
 * Params: @keyID[IN]--The ketID, for instance 0x01, 0x02.
 * 		   @evtMsk[IN]--A marker for key events, refer to 'TLKMDI_KEY_EVTMSK_ENUM'.
 * 		   @ioPort[IN]--ioPort
 * 		   @level[IN]--Key effective level.
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @evtCB[IN]--key callback
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int tlkmdi_key_insert(uint08 keyID, uint08 evtMsk, uint32 ioPort, uint08 level, uint08 upDown, TlkMdiKeyEventCB evtCB)
{
	tlkmdi_key_unit_t *pUnit;

	if(keyID == 0 || evtMsk == 0 || ioPort == 0 || evtCB == nullptr){
		tlkapi_error(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_insert: failure - error param");
		return -TLK_EPARAM;
	}
	
	pUnit = tlkmdi_key_getUsedUnit(keyID);
	if(pUnit != nullptr) return -TLK_EREPEAT;

	pUnit = tlkmdi_key_getIdleUnit();
	if(pUnit == nullptr) return -TLK_EQUOTA;
	
	pUnit->keyID = keyID;
	pUnit->evtMsk = evtMsk;
	pUnit->ioPort = ioPort;
	if(level != 0) pUnit->level = 1;
	else pUnit->level = 0;
	pUnit->evtCB = evtCB;

	gpio_function_en(pUnit->ioPort);
	gpio_output_dis(pUnit->ioPort);
	gpio_input_en(pUnit->ioPort);
	gpio_set_up_down_res(pUnit->ioPort, upDown);
	
	tlkapi_timer_insertNode(&sTlkMdiKeyCtrl.timer);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_key_remove
 * Descript: Disable the gpio for the key and remove a timer.
 * Params: @keyID[IN]--The keyID.
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @enInput[IN]--true enable input, false disable input.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int tlkmdi_key_remove(uint08 keyID, uint08 upDown, bool enInput)
{
	uint08 index;
	tlkmdi_key_unit_t *pUnit;
	
	pUnit = tlkmdi_key_getUsedUnit(keyID);
	if(pUnit == nullptr) return -TLK_EPARAM;
	
	if((pUnit->flags & TLKMDI_KEY_FLAG_PRESS) != 0 && (pUnit->evtMsk & TLKMDI_KEY_EVTID_RELEASE) != 0){
		pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_RELEASE, (pUnit->flags & TLKMDI_KEY_FLAG_PRESS) != 0);
	}
	gpio_function_en(pUnit->ioPort);
	gpio_set_input_en(pUnit->ioPort, enInput);
    gpio_set_output_en(pUnit->ioPort, 0);
	gpio_setup_up_down_resistor(pUnit->ioPort, upDown);
	tmemcpy(pUnit, 0, sizeof(tlkmdi_key_unit_t));

	for(index=0; index<TLKMDI_KEY_MAX_NUMB; index++){
		if(sTlkMdiKeyCtrl.unit[index].keyID != 0) break;
	}
	if(index == TLKMDI_KEY_MAX_NUMB){
		tlkapi_timer_removeNode(&sTlkMdiKeyCtrl.timer);
	}
	
	return TLK_ENONE;
}

static bool tlkmdi_key_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	uint08 index;
	uint08 isBusy0 = false;
	uint08 isBusy1 = false;
	for(index=0; index<TLKMDI_KEY_MAX_NUMB; index++){
		isBusy1 = tlkmdi_key_check(&sTlkMdiKeyCtrl.unit[index]);
		if(!isBusy0 && isBusy1) isBusy0 = true;
	}
	if(isBusy0){
		tlkapi_timer_updateNode(&sTlkMdiKeyCtrl.timer, TLKMDI_KEY_TIMEOUT1, false);
	}else{
		tlkapi_timer_updateNode(&sTlkMdiKeyCtrl.timer, TLKMDI_KEY_TIMEOUT0, false);
	}
	return true;
}
static bool tlkmdi_key_check(tlkmdi_key_unit_t *pUnit)
{
	uint08 press;
	uint08 level;
	if(pUnit->keyID == 0) return false;

	if(gpio_get_level(pUnit->ioPort) != 0) level = 1;
	else level = 0;
	if(level == pUnit->level) press = true;
	else press = false;
	if(press){
		if((pUnit->flags & TLKMDI_KEY_FLAG_PRESS) == 0){
			pUnit->curTimer = clock_time()|1;
			pUnit->flags |= TLKMDI_KEY_FLAG_PRESS;
			if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_PRESS) != 0){
				pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_PRESS, press);
			}
			tlkapi_trace(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_check: key press");
		}else if((pUnit->flags & TLKMDI_KEY_FLAG_RELEASE) == 0){
			if(clock_time_exceed(pUnit->curTimer, TLKMDI_KEY_LONG_TIME_MAX)){
				tlkapi_trace(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_check: key press timeout");
				pUnit->flags |= TLKMDI_KEY_FLAG_RELEASE;
				if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_RELEASE) != 0){
					pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_RELEASE, press);
				}
				if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_LONG) != 0){
					pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_LONG, press);
				}
				pUnit->preTimer = 0;
				pUnit->curTimer = 0;
			}
		}else{
			
		}
	}
	else{
		if((pUnit->flags & TLKMDI_KEY_FLAG_RELEASE) != 0){
			tlkapi_trace(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_check: key fault release");
			pUnit->preTimer = 0;
			pUnit->curTimer = 0;
			pUnit->flags = TLKMDI_KEY_FLAG_NONE;
		}
		else if((pUnit->flags & TLKMDI_KEY_FLAG_PRESS) != 0){
			uint32 nowTime = clock_time() | 1;
			uint32 timeDiff = (nowTime-pUnit->curTimer) >> 4;
			pUnit->flags &= ~TLKMDI_KEY_FLAG_PRESS;
			tlkapi_trace(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_check: key normal release - timeDiff[%d]", timeDiff);
			tlkapi_trace(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_check:00 nowTime[%x] - curTimer[%x]", nowTime, pUnit->curTimer);
			if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_RELEASE) != 0){
				pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_RELEASE, press);
			}
			if(timeDiff < TLKMDI_KEY_CLICK_TIME_MAX){
				if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_CLICK) != 0){
					pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_CLICK, press);
				}
				if(pUnit->preTimer != 0 && !clock_time_exceed(pUnit->preTimer, TLKMDI_KEY_DCLICK_INTV_MAX)){
					if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_DCLICK) != 0){
						pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_DCLICK, press);
					}
				}
				if(pUnit->preTimer == 0) pUnit->preTimer = nowTime;
				else pUnit->preTimer = 0;
			}else if(timeDiff < TLKMDI_KEY_SHORT_TIME_MAX){
				if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_SHORT) != 0){
					pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_SHORT, press);
				}
			}else{
				if((pUnit->evtMsk & TLKMDI_KEY_EVTMSK_LONG) != 0){
					pUnit->evtCB(pUnit->keyID, TLKMDI_KEY_EVTID_LONG, press);
				}
			}
			pUnit->flags = TLKMDI_KEY_FLAG_NONE;
		}
		else if(pUnit->preTimer != 0){
			if(clock_time_exceed(pUnit->curTimer, TLKMDI_KEY_DCLICK_INTV_MAX)){
				tlkapi_trace(TLKMDI_KEY_DBG_FLAG, TLKMDI_KEY_DBG_SIGN, "tlkmdi_key_check: release hold click");
				pUnit->preTimer = 0;
				pUnit->curTimer = 0;
				pUnit->flags = TLKMDI_KEY_FLAG_NONE;
			}
		}
	}
	
	if(pUnit->preTimer != 0 || pUnit->curTimer != 0) return true;
	else return false;
}


static tlkmdi_key_unit_t *tlkmdi_key_getIdleUnit(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_KEY_MAX_NUMB; index++){
		if(sTlkMdiKeyCtrl.unit[index].keyID == 0) break;
	}
	if(index == TLKMDI_KEY_MAX_NUMB) return nullptr;
	return &sTlkMdiKeyCtrl.unit[index];
}

static tlkmdi_key_unit_t *tlkmdi_key_getUsedUnit(uint08 keyID)
{
	uint08 index;
	if(keyID == 0) return nullptr;
	for(index=0; index<TLKMDI_KEY_MAX_NUMB; index++){
		if(sTlkMdiKeyCtrl.unit[index].keyID == keyID) break;
	}
	if(index == TLKMDI_KEY_MAX_NUMB) return nullptr;
	return &sTlkMdiKeyCtrl.unit[index];
}



#endif //#if (TLK_MDI_KEY_ENABLE)

