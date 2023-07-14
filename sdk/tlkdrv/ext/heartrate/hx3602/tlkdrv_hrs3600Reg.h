/********************************************************************************************************
 * @file	tlkdrv_hrs3600Reg.h
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
#if (TLKDRV_HEARTRATE_HX3602_ENABLE)


#include <stdint.h>

const uint8_t init_register_array[][2] = {
	
{0x01, 0x01}, //  temper sensor enable, 0x03 dis
{0x02, 0x33}, //  (0x77,0x66,0x55) enable and osr confid. 0x77 ps/hrs 1024 osr en; 0x66 ps/hrs 512 osr en; 0x55 ps/hrs 256 osr en;
{0x03, 0x8f}, //  (0x8f,0x83) 0x8f enalbe ps/hrs led. 0x83 disable ps/hrs led
{0x04, 0x80}, //  int time for hrs, 8btis. inttime=reg_data*4
{0x05, 0x80}, //  int time for ps, 8btis. inttime=reg_data*4
{0x06, 0x50}, //  intrupt set
{0x07, 0x01}, //  intrupt enable 
{0x09, 0x02}, //  power down 0x03 ,power on 0x02
{0x11, 0x04}, //  reset time max 256; default=0x04,max 0x07
{0x12, 0x1f}, //  en2reset delay time max;  default=0x1f,max 0x7f
{0x13, 0x00}, //  PGA gain;0x00=0dB gain, 0x77=42dB,0x66=36dB，0x55=30dB,0x44=24dB,0x33=18dB,0x22=12dB,0x11=6dB,0x00=0dB
{0x14, 0x80}, //  led select for ps.  0x40 for LDR0, 0x80 for LDR1
{0x15, 0x40}, //  led select for hrs. 0x40 for LDR0, 0x80 for LDR1 
{0x16, 0x00}, //  ps idle, idle time = regdata*prf
{0xc0, 0x84}, //  (0x86) led_dr <1:0> = bit<1:0>  , 00=12.5mA ; 01=25mA ; 10=50mA ;11=100mA
{0xc1, 0x28}, //  (0xb8 0x28 0x08) 0xb8 expd diff_adc test ; 0x28 internal pd diff_adc test;  0x08 internal pd single_adc test;
              //   pgacapsel{3:0}=bit[3:0] int cap set
{0xc2, 0x00}, //  (0x20 0x40) self test config. 0x20 = adc only self test ; 0x40 = afe+adc self test

};

#define INIT_ARRAY_SIZE (sizeof(init_register_array)/sizeof(init_register_array[0]))

#endif //#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

