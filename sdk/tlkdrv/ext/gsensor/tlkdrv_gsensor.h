/********************************************************************************************************
 * @file	tlkdrv_gsensor.h
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
#ifndef TLKDRV_SENSOR_H
#define TLKDRV_SENSOR_H


#define TLKDRV_GSENSOR_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_GSENSOR_DBG_SIGN     "[GSENSOR]"


#define TLKDRV_GSENSOR_SC7A20_ENABLE       0
#define TLKDRV_GSENSOR_MMC5603_ENABLE      0
#define TLKDRV_GSENSOR_DSDA217_ENABLE      0


typedef enum{
	TLKDRV_GSENSOR_DEV_SC7A20 = 0,
	TLKDRV_GSENSOR_DEV_MMC5603,
	TLKDRV_GSENSOR_DEV_DSDA217,
	TLKDRV_GSENSOR_DEV_MAX,
}TLKDRV_GSENSOR_DEV_ENUM;
typedef enum{
	TLKDRV_GSENSOR_OPCODE_NONE = 0,
}TLKDRV_SENSOR_OPCODE_ENUM;


typedef struct{
	bool(*IsOpen)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*Reset)(void);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
}tlkdrv_gsensor_modinf_t;


bool tlkdrv_gsensor_isOpen(TLKDRV_GSENSOR_DEV_ENUM dev);

int tlkdrv_gsensor_init(TLKDRV_GSENSOR_DEV_ENUM dev);
int tlkdrv_gsensor_open(TLKDRV_GSENSOR_DEV_ENUM dev);
int tlkdrv_gsensor_close(TLKDRV_GSENSOR_DEV_ENUM dev);
int tlkdrv_gsensor_reset(TLKDRV_GSENSOR_DEV_ENUM dev);
int tlkdrv_gsensor_handler(TLKDRV_GSENSOR_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);





#endif //TLKDRV_SENSOR_H

