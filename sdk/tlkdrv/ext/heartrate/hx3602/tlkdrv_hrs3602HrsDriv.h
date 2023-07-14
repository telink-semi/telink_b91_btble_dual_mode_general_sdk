/********************************************************************************************************
 * @file	tlkdrv_hrs3602HrsDriv.h
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
#ifndef _HRS3600_DRIV_H_
#define _HRS3600_DRIV_H_

#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#include <stdint.h>

#if 0
/* portable 8-bit unsigned integer */
typedef unsigned char           uint8_t;
/* portable 8-bit signed integer */
typedef signed char             int8_t;
/* portable 16-bit unsigned integer */
typedef unsigned short int      uint16_t;
/* portable 16-bit signed integer */
typedef signed short int        int16_t;
/* portable 32-bit unsigned integer */
typedef unsigned int            uint32_t;
/* portable 32-bit signed integer */
typedef signed int              int32_t;
#endif

#ifndef bool
#define bool unsigned char
#endif

#ifndef  true
#define  true  1
#endif
#ifndef  false
#define  false  0
#endif

typedef enum { 
	MSG_NO_TOUCH,
	MSG_TOUCH,
	MSG_CHECK_INIT
} hrs3602_wear_status_t;

typedef struct {
	uint32_t data_cnt;
	uint8_t highlight; 
	uint8_t no_touch_check_flg;
	uint32_t no_touch_check_cnt;
	uint32_t timer_notouch_check_cnt;
	uint8_t no_touch_cnt;
} agc_data_t;

void Hrs3602_driv_init(void);
void Hrs3602_low_power(void);
void Hrs3602_normal_power(void);
hrs3602_wear_status_t Hrs3602_agc(int32_t als_raw_data , int32_t infrared_data);


#endif //#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#endif
