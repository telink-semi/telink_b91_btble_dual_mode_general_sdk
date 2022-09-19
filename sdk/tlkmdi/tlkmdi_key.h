/********************************************************************************************************
 * @file     tlkmdi_key.h
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
#ifndef TLKMDI_KEY_H
#define TLKMDI_KEY_H

#if (TLK_MDI_KEY_ENABLE)



#define TLKMDI_KEY_TIMEOUT0              100000 //100ms
#define TLKMDI_KEY_TIMEOUT1              10000 //100ms


#define TLKMDI_KEY_MAX_NUMB              2

#define TLKMDI_KEY_CLICK_TIME_MIN        50000 //50ms
#define TLKMDI_KEY_CLICK_TIME_MAX        500000
#define TLKMDI_KEY_SHORT_TIME_MIN        500000 //100ms
#define TLKMDI_KEY_SHORT_TIME_MAX        2500000
#define TLKMDI_KEY_LONG_TIME_MIN         2500000 //100ms
#define TLKMDI_KEY_LONG_TIME_MAX         5000000
#define TLKMDI_KEY_DCLICK_INTV_MAX       500000

typedef enum{
	TLKMDI_KEY_EVTID_NONE    = 0x00,
	TLKMDI_KEY_EVTID_PRESS   = 0x01,
	TLKMDI_KEY_EVTID_RELEASE = 0x02,
	TLKMDI_KEY_EVTID_CLICK   = 0x03,
	TLKMDI_KEY_EVTID_SHORT   = 0x04,
	TLKMDI_KEY_EVTID_LONG    = 0x05,
	TLKMDI_KEY_EVTID_DCLICK  = 0x06,
}TLKMDI_KEY_EVTID_ENUM;

typedef enum{
	TLKMDI_KEY_EVTMSK_NONE    = 0x00,
	TLKMDI_KEY_EVTMSK_PRESS   = 0x01,
	TLKMDI_KEY_EVTMSK_RELEASE = 0x02,
	TLKMDI_KEY_EVTMSK_CLICK   = 0x04,
	TLKMDI_KEY_EVTMSK_SHORT   = 0x08,
	TLKMDI_KEY_EVTMSK_LONG    = 0x10,
	TLKMDI_KEY_EVTMSK_DCLICK  = 0x20,
	TLKMDI_KEY_EVTMSK_DEFAULT = TLKMDI_KEY_EVTMSK_CLICK | TLKMDI_KEY_EVTMSK_SHORT 
		| TLKMDI_KEY_EVTMSK_LONG | TLKMDI_KEY_EVTMSK_DCLICK,
	TLKMDI_KEY_EVTMSK_ALL     = TLKMDI_KEY_EVTMSK_CLICK | TLKMDI_KEY_EVTMSK_SHORT 
		| TLKMDI_KEY_EVTMSK_LONG | TLKMDI_KEY_EVTMSK_DCLICK
		| TLKMDI_KEY_EVTMSK_PRESS | TLKMDI_KEY_EVTMSK_RELEASE,
}TLKMDI_KEY_EVTMSK_ENUM;

typedef enum{
	TLKMDI_KEY_FLAG_NONE = 0x00,
	TLKMDI_KEY_FLAG_PRESS = 0x01,
	TLKMDI_KEY_FLAG_RELEASE = 0x02,
}TLKMDI_KEY_FLAGS_ENUM;

typedef void(*TlkMdiKeyEventCB)(uint08 keyID, uint08 evtID, uint08 isPress);
typedef struct{
	uint08 keyID;
	uint08 flags;
	uint08 level;
	uint08 evtMsk;
	uint32 ioPort;
	uint32 preTimer;
	uint32 curTimer;
	TlkMdiKeyEventCB evtCB;
}tlkmdi_key_unit_t;
typedef struct{
	tlkapi_timer_t timer;
	tlkmdi_key_unit_t unit[TLKMDI_KEY_MAX_NUMB];
}tlkmdi_key_ctrl_t;




int tlkmdi_key_init(void);

int tlkmdi_key_insert(uint08 keyID, uint08 evtMsk, uint32 ioPort, uint08 level, uint08 upDown, TlkMdiKeyEventCB evtCB);
int tlkmdi_key_remove(uint08 keyID, uint08 upDown, bool enInput);




#endif //#if (TLK_MDI_KEY_ENABLE)

#endif //#ifndef TLKMDI_KEY_H

