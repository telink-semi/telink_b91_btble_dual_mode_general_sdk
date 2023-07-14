/********************************************************************************************************
 * @file	tlkdrv_hrs3602.h
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
#ifndef _HRS3600_H_
#define _HRS3600_H_

#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#include <stdint.h>
#include <stdbool.h>

//#include "opr_oled.h"


#define TIMER_READ_MODE

#define HRS_ALG_LIB

#define IR_CHECK_TOUCH

#define CHECK_TOUCH_DIFF

//#define TESTVEC

//#define TYHX_DEMO

#define HRS_BLE_APP

#define RTT_DEBUG

//#define GSENSER_DATA

#define NEW_GSEN_SCHME 


#define	 DEBUG_PRINTF(...)


typedef struct {
   int16_t AXIS_X;
   int16_t AXIS_Y;
   int16_t AXIS_Z;
} GsensorRaw_t;


void Hrs3602_chip_disable(void);
void Hrs3602_chip_enable(void);

bool Hrs3602_chip_init(void);
void Hrs3602_chip_reset(void);
void heart_rate_meas_timeout_handler(void * p_context);
void Hrs3602_INT_init(void);
void Hrs3602_INT_enable(void);
void Hrs3602_INT_disable(void);

bool    Hrs3602_write_reg(uint8_t addr, uint8_t data);
uint8_t Hrs3602_read_reg(uint8_t addr);
bool Hrs3602_brust_read_reg(uint8_t addr , uint8_t *buf, uint8_t length);

void    Hrs3602_timeout_handler(void);
void    Hrs3602_init_touch_mode(void);
void    Hrs3602_Int_handle(void);
void    Hrs3602_blood_presure_Int_handle(void);
void    Hrs3602_chip_disable(void);
void    Hrs3602_alg_config(void);
extern void Hrs3602_set_first_hr_mode(uint8_t mode);
extern void Hrs3602_set_dc_thres_low(int32_t value);

void Hrs3602_bp_alg_config(void); // add ericy 20180625

bool Hrs3602_read_hrs(int32_t *hrm_data, int32_t *als_data);
bool Hrs3602_read_ps1(int32_t *infrared_data);
void Hrs3602_set_motion_status(uint16_t motion_status);

uint32_t hrs3602_timers_start(void);
uint32_t hrs3602_timers_stop(void);
void Hrs3300_button_handler(void);
void read_data_packet(int32_t *ps_data);


#endif //#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#endif // _HRS3600_H_


