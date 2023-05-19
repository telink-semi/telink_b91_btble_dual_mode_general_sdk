/********************************************************************************************************
 * @file     tlkdrv_keyboard.h
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
#ifndef TLKDRV_KEYBOARD_H
#define TLKDRV_KEYBOARD_H

#if (TLKDEV_KEYBOARD_ENABLE)


#define TLKDRV_KB_DBG_FLAG               0//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_KB_DBG_SIGN               "[KB]"


#define TLKDRV_KEYBOARD_COL_PINS      {GPIO_PB6, GPIO_PA6, GPIO_PB4, GPIO_PB2, GPIO_PB3, GPIO_PC6}
#define TLKDRV_KEYBOARD_ROW_PINS      {GPIO_PD0, GPIO_PD1, GPIO_PC5, GPIO_PD3, GPIO_PD4, GPIO_PB5, GPIO_PC0}

#define TLKDRV_KEYBOARD_COL_PULLUP          GPIO_PIN_PULLUP_10K
#define TLKDRV_KEYBOARD_ROW_PULLUP          GPIO_PIN_PULLDOWN_100K
#define TLKDRV_KEYBOARD_COL_WRITE_LEVEL     1
#define TLKDRV_KEYBOARD_ROW_VALID_LEVEL     1


#define TLKDRV_KEYBOARD_KEY_MAX_NUMB        8

#define TLKDRV_KEYBOARD_COLPIN_MAX_NUMB     8
#define TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB     8

typedef void(*TlkDrvKeyboardStatusChangedCB)(uint08 *pReleasedKey, uint08 releaseNumb, uint08 *pPressedKey, uint08 pressedNumb);

typedef struct{
	uint08 isOpen;
	uint08 keyNumb;
	uint08 colNumb; //Colume Pin Number
	uint08 rowNumb; //Row Pin Number
	uint08 colWrite; //
	uint08 rowValid; //Row Valid Level: 0 or 1
	uint08 colPullup;
	uint08 rowPullup;
	uint16 colPin[TLKDRV_KEYBOARD_COLPIN_MAX_NUMB];
	uint16 rowPin[TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB];
	uint08 keyRcd[TLKDRV_KEYBOARD_KEY_MAX_NUMB];
}tlkdrv_keyboard_t;


int  tlkdrv_keyboard_init(void);

void tlkdrv_keyboard_scan(void);


int tlkdrv_keyboard_open(uint16 *pColPin, uint16 *pRowPin, uint08 colNumb, uint08 rowNumb,
	uint08 colWrite, uint08 rowValid, uint08 colPullup, uint08 rowPullup);
int tlkdrv_keyboard_close(void);


bool tlkdrv_keyboard_isHasPressed(void);



#endif //#if (TLKDEV_KEYBOARD_ENABLE)

#endif //TLKDRV_KEYBOARD_H

