/********************************************************************************************************
 * @file     tlkdrv_sensor.h
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

#ifndef TLKDRV_SENSOR_H
#define TLKDRV_SENSOR_H


#define TLKDRV_SENSOR_DSDA217_ENABLE      1


typedef enum{
	TLKDRV_SENSOR_DEV_DSDA217 = 0,
	TLKDRV_SENSOR_DEV_MAX,
}TLKDRV_SENSOR_DEV_ENUM;
typedef enum{
	TLKDRV_SENSOR_OPCODE_NONE = 0,
}TLKDRV_SENSOR_OPCODE_ENUM;


typedef struct{
	bool(*IsOpen)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*Reset)(void);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
}tlkdrv_sensor_modinf_t;


bool tlkdrv_sensor_isOpen(TLKDRV_SENSOR_DEV_ENUM dev);

int tlkdrv_sensor_init(TLKDRV_SENSOR_DEV_ENUM dev);
int tlkdrv_sensor_open(TLKDRV_SENSOR_DEV_ENUM dev);
int tlkdrv_sensor_close(TLKDRV_SENSOR_DEV_ENUM dev);
int tlkdrv_sensor_reset(TLKDRV_SENSOR_DEV_ENUM dev);
int tlkdrv_sensor_handler(TLKDRV_SENSOR_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);





#endif //TLKDRV_SENSOR_H

