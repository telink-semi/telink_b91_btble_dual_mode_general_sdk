/********************************************************************************************************
 * @file	tlkdrv_sc7a20.h
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
#ifndef TLKDRV_SC7A20_H
#define TLKDRV_SC7A20_H

#if (TLKDRV_GSENSOR_SC7A20_ENABLE)


#define TLKDRV_SC7A20_I2C_SLAVE_ADDR			SL_SC7A20_IIC_WRITE_ADDRESS
#define TLKDRV_SC7A20_CLOCK_SPEED             	(400*1000) //1M

#define TLKDRV_SC7A20_SCL_PIN                 	GPIO_PE1
#define TLKDRV_SC7A20_SDA_PIN                 	GPIO_PE3

#if(TLKDRV_SC7A20_INT_EN)
#define TLKDRV_SC7A20_INT_PIN                 	GPIO_PA5
#endif


typedef struct{
	uint08 isInit;
	uint08 isOpen;
	bool   irqRdy;
}tlkdrv_sc7a20_t;


#endif //#if (TLKDRV_GSENSOR_SC7A20_ENABLE)

#endif //TLKDRV_SC7A20_H

