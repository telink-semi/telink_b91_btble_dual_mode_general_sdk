/********************************************************************************************************
 * @file	tlkdrv_keyboard.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#if (TLKDEV_KEYBOARD_ENABLE)
#include "tlkdrv_keyboard.h"
#include "drivers.h"



static const uint16 sTlkDrvKeyboardColPins[] = TLKDRV_KEYBOARD_COL_PINS;
static const uint16 sTlkDrvKeyboardRowPins[] = TLKDRV_KEYBOARD_ROW_PINS;

static tlkdrv_keyboard_t sTlkDrvKeyboardCtrl;
static TlkDrvKeyboardStatusChangedCB sTlkDrvKeyboardStatusChangedCB = nullptr;


int tlkdrv_keyboard_init(void)
{
	tmemset(&sTlkDrvKeyboardCtrl, 0, sizeof(tlkdrv_keyboard_t));
	sTlkDrvKeyboardCtrl.colNumb = sizeof(sTlkDrvKeyboardColPins)/sizeof(sTlkDrvKeyboardColPins[0]);
	sTlkDrvKeyboardCtrl.rowNumb = sizeof(sTlkDrvKeyboardRowPins)/sizeof(sTlkDrvKeyboardRowPins[0]);

	return TLK_ENONE;
}


void tlkdrv_keyboard_wakeup(void)
{
	
}
void tlkdrv_keyboard_suspend(void)
{
	
}


int tlkdrv_keyboard_open(uint16 *pColPin, uint16 *pRowPin, uint08 colNumb, uint08 rowNumb,
	uint08 colWrite, uint08 rowValid, uint08 colPullup, uint08 rowPullup)
{
	uint08 index;
	
	if(pColPin == nullptr || pRowPin == nullptr || colNumb == 0 || rowNumb == 0){
		tlkapi_error(TLKDRV_KB_DBG_FLAG, TLKDRV_KB_DBG_SIGN,
			"tlkdrv_keyboard_open: error param - colNumb[%d], rowNumb[%d]", colNumb, rowNumb);
		return -TLK_EPARAM;
	}
	if(colNumb > TLKDRV_KEYBOARD_COLPIN_MAX_NUMB || rowNumb > TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB){
		tlkapi_error(TLKDRV_KB_DBG_FLAG, TLKDRV_KB_DBG_SIGN,
			"tlkdrv_keyboard_open: colNumb[%d-%d], rowNumb[%d-%d]",
			colNumb, TLKDRV_KEYBOARD_COLPIN_MAX_NUMB, rowNumb, TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB);
		return -TLK_EOVERFLOW;
	}

	if(colWrite != 0) colWrite = 1;
	if(rowValid != 0) rowValid = 1;

	sTlkDrvKeyboardCtrl.colNumb = colNumb;
	sTlkDrvKeyboardCtrl.rowNumb = rowNumb;
	sTlkDrvKeyboardCtrl.colWrite = colWrite;
	sTlkDrvKeyboardCtrl.rowValid = rowValid;
	sTlkDrvKeyboardCtrl.colPullup = colPullup;
	sTlkDrvKeyboardCtrl.rowPullup = rowPullup;
	tmemcpy(sTlkDrvKeyboardCtrl.colPin, pColPin, colNumb);
	tmemcpy(sTlkDrvKeyboardCtrl.rowPin, pRowPin, rowNumb);
	for(index=0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		gpio_function_en(sTlkDrvKeyboardCtrl.colPin[index]);
		gpio_set_output_en(sTlkDrvKeyboardCtrl.colPin[index], 1);
		gpio_set_input_en(sTlkDrvKeyboardCtrl.colPin[index], 0);
		gpio_setup_up_down_resistor(sTlkDrvKeyboardCtrl.colPin[index], sTlkDrvKeyboardCtrl.colPullup);
		gpio_set_level(sTlkDrvKeyboardCtrl.colPin[index], !sTlkDrvKeyboardCtrl.colWrite);
	}
	for(index=0; index<sTlkDrvKeyboardCtrl.rowNumb; index++){
		gpio_function_en(sTlkDrvKeyboardCtrl.rowPin[index]);
		gpio_set_output_en(sTlkDrvKeyboardCtrl.rowPin[index], 0);
		gpio_set_input_en(sTlkDrvKeyboardCtrl.rowPin[index], 1);
		gpio_setup_up_down_resistor(sTlkDrvKeyboardCtrl.rowPin[index], sTlkDrvKeyboardCtrl.rowPullup);
	}
	sTlkDrvKeyboardCtrl.isOpen = true;
	
	return TLK_ENONE;
}
int tlkdrv_keyboard_close(void)
{
	tmemset(&sTlkDrvKeyboardCtrl, 0, sizeof(tlkdrv_keyboard_t));
	return TLK_ENONE;
}



void tlkdrv_keyboard_scan(void)
{
	bool isReleased;
	uint08 iIndex;
	uint08 jIndex;
	uint08 rLevel;
	uint08 oldNumb;
	uint08 keyNumb;
	uint08 keyRcd[TLKDRV_KEYBOARD_KEY_MAX_NUMB];
	uint08 oldKey[TLKDRV_KEYBOARD_KEY_MAX_NUMB];
		
	if(!sTlkDrvKeyboardCtrl.isOpen) return;
	
	if(sTlkDrvKeyboardCtrl.keyNumb == 0 && !tlkdrv_keyboard_isHasPressed()){
		return;
	}
	//Scan existing keys
	keyNumb = 0;
	for(iIndex=0; iIndex<sTlkDrvKeyboardCtrl.colNumb; iIndex++){
		gpio_set_level(sTlkDrvKeyboardCtrl.colPin[iIndex], sTlkDrvKeyboardCtrl.colWrite);
		delay_us(20);
		for(jIndex=0; jIndex<sTlkDrvKeyboardCtrl.rowNumb; jIndex++){
			if(gpio_get_level(sTlkDrvKeyboardCtrl.rowPin[jIndex])) rLevel = 1;
			else rLevel = 0;
			if(rLevel == sTlkDrvKeyboardCtrl.rowValid){
				keyRcd[keyNumb++] = jIndex*sTlkDrvKeyboardCtrl.rowNumb+iIndex+1;
				if(keyNumb >= TLKDRV_KEYBOARD_KEY_MAX_NUMB) break;
			}
		}
		gpio_set_level(sTlkDrvKeyboardCtrl.colPin[iIndex], !sTlkDrvKeyboardCtrl.colWrite);
	}
	/* Detect whether the legacy keys are completely consistent with the new trigger keys. 
	If they are completely consistent, they are considered as repeated keys */
	if(sTlkDrvKeyboardCtrl.keyNumb == keyNumb && (keyNumb == 0 ||
		tmemcmp(sTlkDrvKeyboardCtrl.keyRcd, keyRcd, keyNumb) == 0)){
		return; //Repeat Or None Key
	}
	/* Detects whether a legacy key is released and whether a new key is pressed */
	oldNumb = 0;
	for(iIndex=0; iIndex<sTlkDrvKeyboardCtrl.keyNumb; iIndex++){
		isReleased = false;
		if(keyNumb == 0){
			isReleased = true;
		}else{
			for(jIndex=0; jIndex<keyNumb; jIndex++){
				if(keyRcd[jIndex] == sTlkDrvKeyboardCtrl.keyRcd[iIndex]) break;
			}
			if(jIndex != keyNumb){
				isReleased = true;
				if(jIndex+1 == keyNumb){
					keyRcd[jIndex] = 0;
				}else{
					tmemcpy(keyRcd[jIndex], keyRcd[jIndex+1], keyNumb-1-jIndex);
				}
				keyNumb --;
			}
		}
		if(isReleased){
			if(iIndex+1 == keyNumb){
				oldKey[oldNumb++] = sTlkDrvKeyboardCtrl.keyRcd[iIndex];
				sTlkDrvKeyboardCtrl.keyRcd[iIndex] = 0;
			}else{
				tmemcpy(sTlkDrvKeyboardCtrl.keyRcd[iIndex], sTlkDrvKeyboardCtrl.keyRcd[iIndex+1], sTlkDrvKeyboardCtrl.keyNumb-1-iIndex);
			}
			sTlkDrvKeyboardCtrl.keyNumb --;
		}
	}
	/* Merge the new key into the system key list */
	if(sTlkDrvKeyboardCtrl.keyNumb+keyNumb > TLKDRV_KEYBOARD_KEY_MAX_NUMB){
		keyNumb = TLKDRV_KEYBOARD_KEY_MAX_NUMB-sTlkDrvKeyboardCtrl.keyNumb;
	}
	if(keyNumb != 0){
		tmemcpy(&sTlkDrvKeyboardCtrl.keyRcd[sTlkDrvKeyboardCtrl.keyNumb], keyRcd, keyNumb);
		sTlkDrvKeyboardCtrl.keyNumb += keyNumb;
	}
	/* Notifies the  status of key changed to user */
	if(sTlkDrvKeyboardStatusChangedCB != nullptr){
		sTlkDrvKeyboardStatusChangedCB(oldKey, oldNumb, keyRcd, keyNumb);
	}
}


bool tlkdrv_keyboard_isHasPressed(void)
{
	bool isHave;
	uint08 index;
	uint08 rLevel;
	for(index=0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		gpio_set_level(sTlkDrvKeyboardCtrl.colPin[index], sTlkDrvKeyboardCtrl.colWrite);
	}
	delay_us(20);
	for(index=0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		if(gpio_get_level(sTlkDrvKeyboardCtrl.rowPin[index])) rLevel = 1;
		else rLevel = 0;
		if(rLevel == sTlkDrvKeyboardCtrl.rowValid) break;
	}
	if(index != sTlkDrvKeyboardCtrl.colNumb) isHave = true;
	else return false;
	for(index=0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		gpio_set_level(sTlkDrvKeyboardCtrl.colPin[index], !sTlkDrvKeyboardCtrl.colWrite);
	}
	return isHave;
}



#endif //#if (TLKDEV_KEYBOARD_ENABLE)

