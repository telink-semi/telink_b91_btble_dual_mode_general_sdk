/********************************************************************************************************
 * @file	tlkdrv_dsda217.h
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
#ifndef TLKDRV_DSDA217_H
#define TLKDRV_DSDA217_H

#if (TLKDRV_GSENSOR_DSDA217_ENABLE)


#define TLKDRV_DSDA217_CLOCK_SPEED             200000 //Max 400kHz
#define TLKDRV_DSDA217_INT_PIN                 GPIO_PD2
#define TLKDRV_DSDA217_SCL_PIN                 GPIO_PD0
#define TLKDRV_DSDA217_SDA_PIN                 GPIO_PD1
#define TLKDRV_DSDA217_POWER_PIN               GPIO_PD3

#define TLKDRV_DSDA217_I2C_READ_ADDR           0x4D //0x4F
#define TLKDRV_DSDA217_I2C_WRITE_ADDR          0x4C //0x4E

/*******************************************************************************
Macro definitions - Register define for Gsensor asic
********************************************************************************/
#define TLKDRV_DSDA217_REG_SPI_I2C                 0x00
#define TLKDRV_DSDA217_REG_WHO_AM_I                0x01
#define TLKDRV_DSDA217_REG_ACC_X_LSB               0x02
#define TLKDRV_DSDA217_REG_ACC_X_MSB               0x03
#define TLKDRV_DSDA217_REG_ACC_Y_LSB               0x04
#define TLKDRV_DSDA217_REG_ACC_Y_MSB               0x05
#define TLKDRV_DSDA217_REG_ACC_Z_LSB               0x06
#define TLKDRV_DSDA217_REG_ACC_Z_MSB               0x07 
#define TLKDRV_DSDA217_REG_MOTION_FLAG             0x09
#define TLKDRV_DSDA217_REG_STEPS_MSB               0x0D
#define TLKDRV_DSDA217_REG_STEPS_LSB               0x0E
#define TLKDRV_DSDA217_REG_G_RANGE                 0x0F
#define TLKDRV_DSDA217_REG_ODR_AXIS_DISABLE        0x10
#define TLKDRV_DSDA217_REG_POWERMODE_BW            0x11
#define TLKDRV_DSDA217_REG_SWAP_POLARITY           0x12
#define TLKDRV_DSDA217_REG_FIFO_CTRL               0x14
#define TLKDRV_DSDA217_REG_INT_SET0                0x15
#define TLKDRV_DSDA217_REG_INTERRUPT_SETTINGS1     0x16
#define TLKDRV_DSDA217_REG_INTERRUPT_SETTINGS2     0x17
#define TLKDRV_DSDA217_REG_INTERRUPT_MAPPING1      0x19
#define TLKDRV_DSDA217_REG_INTERRUPT_MAPPING2      0x1a
#define TLKDRV_DSDA217_REG_INTERRUPT_MAPPING3      0x1b
#define TLKDRV_DSDA217_REG_INT_PIN_CONFIG          0x20
#define TLKDRV_DSDA217_REG_INT_LATCH               0x21
#define TLKDRV_DSDA217_REG_ACTIVE_DURATION         0x27
#define TLKDRV_DSDA217_REG_ACTIVE_THRESHOLD        0x28
#define TLKDRV_DSDA217_REG_TAP_DURATION            0x2A
#define TLKDRV_DSDA217_REG_TAP_THRESHOLD           0x2B
#define TLKDRV_DSDA217_REG_STEP_CONFIG1            0x2F
#define TLKDRV_DSDA217_REG_STEP_CONFIG2            0x30
#define TLKDRV_DSDA217_REG_STEP_CONFIG3            0x31
#define TLKDRV_DSDA217_REG_STEP_CONFIG4            0x32
#define TLKDRV_DSDA217_REG_STEP_FILTER             0x33
#define TLKDRV_DSDA217_REG_SM_THRESHOLD            0x34
#define TLKDRV_DSDA217_REG_CUSTOM_OFFSET_X         0x38
#define TLKDRV_DSDA217_REG_CUSTOM_OFFSET_Y         0x39
#define TLKDRV_DSDA217_REG_CUSTOM_OFFSET_Z         0x3a
#define TLKDRV_DSDA217_REG_ENGINEERING_MODE        0x7f
#define TLKDRV_DSDA217_REG_SENSITIVITY_TRIM_X      0x80
#define TLKDRV_DSDA217_REG_SENSITIVITY_TRIM_Y      0x81
#define TLKDRV_DSDA217_REG_SENSITIVITY_TRIM_Z      0x82
#define TLKDRV_DSDA217_REG_COARSE_OFFSET_TRIM_X    0x83
#define TLKDRV_DSDA217_REG_COARSE_OFFSET_TRIM_Y    0x84
#define TLKDRV_DSDA217_REG_COARSE_OFFSET_TRIM_Z    0x85
#define TLKDRV_DSDA217_REG_FINE_OFFSET_TRIM_X      0x86
#define TLKDRV_DSDA217_REG_FINE_OFFSET_TRIM_Y      0x87
#define TLKDRV_DSDA217_REG_FINE_OFFSET_TRIM_Z      0x88
#define TLKDRV_DSDA217_REG_SENS_COMP               0x8c
#define TLKDRV_DSDA217_REG_MEMS_OPTION             0x8f
#define TLKDRV_DSDA217_REG_CHIP_INFO               0xc0
#define TLKDRV_DSDA217_REG_CHIP_INFO_SECOND        0xc1
#define TLKDRV_DSDA217_REG_MEMS_OPTION_SECOND      0xc7
#define TLKDRV_DSDA217_REG_SENS_COARSE_TRIM        0xd1
#define TLKDRV_DSDA217_REG_OSC_TRIM                0x8e



typedef struct AccData_tag{
   sint16 ax;                                   //加速度计原始数据结构体  数据格式 0 0 1024
   sint16 ay;
   sint16 az;
}AccData;

#define mir3da_abs(x)          (((x) > 0) ? (x) : (-(x)))


typedef struct{
	uint08 isInit;
	uint08 isOpen;
}tlkdrv_dsda217_t;







#endif //#if (TLKDRV_GSENSOR_DSDA217_ENABLE)

#endif //TLKDRV_DSDA217_H

