/********************************************************************************************************
 * @file	tlkdrv_heartrate.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKDRV_HEARTRATE_H
#define TLKDRV_HEARTRATE_H


#define TLKDRV_HEARTRATE_DBG_FLAG     0//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_HEARTRATE_DBG_SIGN     0//"[HEARTRATE]"

#define TLKDRV_HEARTRATE_HX3602_ENABLE      0

typedef enum{
	TLKDRV_HEARTRATE_DEV_HX3602 = 0,
	TLKDRV_HEARTRATE_DEV_MAX,
}TLKDRV_HEARTRATE_DEV_ENUM;
typedef enum{
	TLKDRV_HEARTRATE_OPCODE_NONE = 0,
}TLKDRV_HEARTRATE_OPCODE_ENUM;


typedef struct{
	bool(*IsOpen)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*Reset)(void);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
}tlkdrv_heartRate_modinf_t;


bool tlkdrv_heartRate_isOpen(TLKDRV_HEARTRATE_DEV_ENUM dev);

int tlkdrv_heartRate_init(TLKDRV_HEARTRATE_DEV_ENUM dev);
int tlkdrv_heartRate_open(TLKDRV_HEARTRATE_DEV_ENUM dev);
int tlkdrv_heartRate_close(TLKDRV_HEARTRATE_DEV_ENUM dev);
int tlkdrv_heartRate_reset(TLKDRV_HEARTRATE_DEV_ENUM dev);
int tlkdrv_heartRate_handler(TLKDRV_HEARTRATE_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);





#endif //TLKDRV_HEARTRATE_H

