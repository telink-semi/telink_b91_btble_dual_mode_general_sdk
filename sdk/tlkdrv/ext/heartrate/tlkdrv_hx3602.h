/********************************************************************************************************
 * @file	tlkdrv_hx3602.h
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
#ifndef TLKDRV_HR_HX3602_H
#define TLKDRV_HR_HX3602_H

#if (TLKDRV_HEARTRATE_HX3602_ENABLE)


#define TLKDRV_HR_HX3602_I2C_SLAVE_ADDR			0x88
#define TLKDRV_HR_HX3602_CLOCK_SPEED            (1000*1000) //1M

#define TLKDRV_HR_HX3602_SCL_PIN                GPIO_PE1
#define TLKDRV_HR_HX3602_SDA_PIN                GPIO_PE3

typedef struct{
	uint08 isInit;
	uint08 isOpen;
	bool   irqRdy;
}tlkdrv_hr_hx3602_t;


#endif //#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#endif //TLKDRV_HR_HX3602_H

