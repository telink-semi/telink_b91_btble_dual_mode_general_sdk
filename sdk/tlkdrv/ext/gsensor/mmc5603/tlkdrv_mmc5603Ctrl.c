/********************************************************************************************************
 * @file	tlkdrv_mmc5603Ctrl.c
 *
 * @brief	This is the source file for BTBLE SDK
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
/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright, and the information and source code
 *  contained herein is confidential. The software including the source code
 *  may not be copied and the information contained herein may not be used or
 *  disclosed except with the written permission of MEMSIC Inc. (C) 2019
 *****************************************************************************/

/**
 * @brief
 * This file implement magnetic sensor driver APIs.
 * Modified history:
 * V1.5: Update to 1 step factory selftest on 20190619
 */

#include "tlkapi/tlkapi_stdio.h"
#include "../tlkdrv_gsensor.h"
#if TLKDRV_GSENSOR_MMC5603_ENABLE
#include "tlkdrv_mmc5603Ctrl.h"
#include "math.h"
#include "drivers.h"


extern unsigned char tlkdrv_mmc5603_i2cWrite(unsigned char reg, unsigned char dat);
extern unsigned char tlkdrv_mmc5603_i2cRead(unsigned char reg, unsigned char len, unsigned char *buf);

/* Function declaration */

/**
 * @brief SET operation
 */
void MMC5603_SET(void);

/**
 * @brief RESET operation
 */
void MMC5603_RESET(void);

/**
 * @brief OTP read done check
 */
int MMC5603_Check_OTP(void);

/**
 * @brief Check Product ID
 */
int MMC5603_CheckID(void);

/*********************************************************************************
 * decription: SET operation
 *********************************************************************************/
void MMC5603_SET(void)
{
	/* Write 0x08 to register 0x1B, set SET bit high */
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_CTRL0, MMC5603_CMD_SET);

	/* Delay to finish the SET operation */
	Delay_Ms(1);

	return;
}

/*********************************************************************************
 * decription: RESET operation
 *********************************************************************************/
void MMC5603_RESET(void)
{
	/* Write 0x10 to register 0x1B, set RESET bit high */
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_CTRL0, MMC5603_CMD_RESET);

	/* Delay to finish the RESET operation */
	Delay_Ms(1);

	return;
}


extern int mm5603_main(void);

int mmc5603_dev_init(void *arg)
{
	int ret = 0;
	/* Check product ID */
	ret = MMC5603_CheckID();
	if (ret < 0)
		return -1;
	MMC5603_Enable();
	mm5603_main();
	MMC5603_Disable();

	return ret;
}



void Delay_Ms(int cnt)
{
	delay_ms(cnt);
}
void Delay_Us(int cnt)
{
	delay_us(cnt);
	return;
}
int I2C_Write_Reg(unsigned char i2c_add, unsigned char reg_add, unsigned char cmd)
{
	/* i2c_add is the 7-bit i2c address of the sensor
	 * reg_add is the register address to wtite
	 * cmd is the value that need to be written to the register
	 * I2C operating successfully, return 1, otherwise return 0;
	 */

	/*
	.
	. Need to be implemented by user.
	.
	*/
	tlkdrv_mmc5603_i2cWrite(reg_add, cmd);
	return 1;
}
int I2C_Read_Reg(unsigned char i2c_add, unsigned char reg_add, unsigned char *data)
{
	/* i2c_add is the 7-bit i2c address of the sensor
	 * reg_add is the register address to read
	 * data is the first address of the buffer that need to store the register value
	 * I2C operating successfully, return 1, otherwise return 0;
	 */

	/*
	.
	. Need to be implemented by user.
	.
	*/
	tlkdrv_mmc5603_i2cRead(reg_add, 1, data);
	return 1;
}
int I2C_MultiRead_Reg(unsigned char i2c_add, unsigned char reg_add, int num, unsigned char *data)
{
	/* i2c_add is the 7-bit i2c address of the sensor
	 * reg_add is the first register address to read
	 * num is the number of the register to read
	 * data is the first address of the buffer that need to store the register value
	 * I2C operating successfully, return 1, otherwise return 0;
	 */

	/*
	.
	. Need to be implemented by user.
	.
	*/
	tlkdrv_mmc5603_i2cRead(reg_add, num, data);
	return 1;
}





/*********************************************************************************
 * decription: Product ID check
 *********************************************************************************/
int MMC5603_CheckID(void)
{
	unsigned char pro_id = 0;

	/* Read register 0x39, check product ID */
	I2C_Read_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_PRODUCTID1, &pro_id);
	//printf("****************** pro_id = 0x%x ",pro_id);
	if (pro_id != MMC5603_PRODUCT_ID)
		return -1;

	return 1;
}

/*********************************************************************************
 * decription: Continuous mode configuration with auto set and reset
 *********************************************************************************/
void MMC5603_Continuous_Mode_With_Auto_SR(uint8_t bandwith, uint8_t sampling_rate)
{
	/* Write reg 0x1C, Set BW<1:0> = bandwith */
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_CTRL1, bandwith);

	/* Write reg 0x1A, set ODR<7:0> = sampling_rate */
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_ODR, sampling_rate);

	/* Write reg 0x1B */
	/* Set Auto_SR_en bit '1', Enable the function of automatic set/reset */
	/* Set Cmm_freq_en bit '1', Start the calculation of the measurement period according to the ODR*/
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_CTRL0, MMC5603_CMD_CMM_FREQ_EN | MMC5603_CMD_AUTO_SR_EN);

	/* Write reg 0x1D */
	/* Set Cmm_en bit '1', Enter continuous mode */
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_CTRL2, MMC5603_CMD_CMM_EN);

	return;
}

/*********************************************************************************
 * decription: Disable sensor continuous mode
 *********************************************************************************/
void MMC5603_Disable(void)
{
	/* Write reg 0x1D */
	/* Set Cmm_en bit '0', Disable continuous mode */
	//printf(">>>>>> MMC5603_Disable");
	I2C_Write_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_CTRL2, 0x00);
	return;
}

/*********************************************************************************
 * decription: Enable sensor
 *********************************************************************************/
void MMC5603_Enable(void)
{
	/* Work mode setting */
	//MMC5603_Continuous_Mode_With_Auto_SR(MMC5603_CMD_BW00, 50); // 50HZ
	MMC5603_Continuous_Mode_With_Auto_SR(MMC5603_CMD_BW01, 50); // 100HZ
	return;
}

/*********************************************************************************
 * decription: Read the data register and convert to magnetic field
 *********************************************************************************/
void MMC5603_GetData(float *mag_out)
{
	uint8_t data_reg[6] = {0};
	uint16_t data_temp[3] = {0};

	/* Read register data */
	I2C_MultiRead_Reg(MMC5603_7BITI2C_ADDRESS, MMC5603_REG_DATA, 6, data_reg);

	/* Get high 16bits data */
	data_temp[0] = (uint16_t)(data_reg[0] << 8 | data_reg[1]);
	data_temp[1] = (uint16_t)(data_reg[2] << 8 | data_reg[3]);
	data_temp[2] = (uint16_t)(data_reg[4] << 8 | data_reg[5]);

	/* Transform to unit Gauss */
	mag_out[0] = ((float)data_temp[0] - MMC5603_16BIT_OFFSET) / MMC5603_16BIT_SENSITIVITY;
	mag_out[1] = ((float)data_temp[1] - MMC5603_16BIT_OFFSET) / MMC5603_16BIT_SENSITIVITY;
	mag_out[2] = ((float)data_temp[2] - MMC5603_16BIT_OFFSET) / MMC5603_16BIT_SENSITIVITY;
	//printf("mag raw data :%f %f %f \r\n", mag_out[0], mag_out[1], mag_out[2]);

	return;
}



#endif
