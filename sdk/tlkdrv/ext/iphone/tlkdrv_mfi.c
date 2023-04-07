/********************************************************************************************************
 * @file     tlkdrv_mfi.c
 *
 * @brief    This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_mfi.h"
#include "drivers.h"


static void tlkdrv_mfi_i2cStart(void);
static void tlkdrv_mfi_i2cStop(void);
static int  tlkdrv_mfi_i2cReadBit(void);
static void tlkdrv_mfi_i2cWriteBit(int bit);
static uint tlkdrv_mfi_i2cReadByte(int last);
static int  tlkdrv_mfi_i2cWriteByte(uint08 dat);
static bool tlkdrv_mfi_i2cWriteByteWaitAck(uint08 id);



static tlkdrv_mfi_t sTlkdrvMfiCtrl;



int tlkdrv_mfi_init(void)
{
	
	tmemset(&sTlkdrvMfiCtrl, 0, sizeof(tlkdrv_mfi_t));
	
	sTlkdrvMfiCtrl.isInit = true;
	
    return TLK_ENONE;
}

int tlkdrv_mfi_open(void)
{
	if(!sTlkdrvMfiCtrl.isInit) return -TLK_ENOREADY;
	if(sTlkdrvMfiCtrl.isOpen) return -TLK_EREPEAT;
	sTlkdrvMfiCtrl.isOpen = true;

	uint32 chipID;
	uint08 buffer[4];
	
	tmemset(buffer, 0, sizeof(uint08)*4);
	
	tlkdrv_mfi_read(kHAPMFiHWAuthRegister_DeviceID, buffer, 4);
	ARRAY_TO_UINT32H(buffer, 0, chipID);

	if(chipID != TLKDRV_MFI_CHIP_ID){
		tlkapi_trace(TLKDRV_MFI_DBG_FLAG, TLKDRV_MFI_DBG_SIGN, "tlkdrv_mfi_open check failure: %d - %d", TLKDRV_MFI_CHIP_ID, chipID);
		return -TLK_EFAIL;
	}
	return TLK_ENONE;
}
int tlkdrv_mfi_close(void)
{
	if(!sTlkdrvMfiCtrl.isOpen) return TLK_ENONE;
	sTlkdrvMfiCtrl.isOpen = false;
	return TLK_ENONE;
}


int tlkdrv_mfi_read(uint08 reg, uint08 *pBuff, uint08 readLen)
{
	int index;

	if(!sTlkdrvMfiCtrl.isOpen) return -TLK_ENOREADY;
	
	tlkdrv_mfi_i2cStart();
	if(tlkdrv_mfi_i2cWriteByteWaitAck(TLKDRV_MFI_I2C_ADDR)){
		tlkdrv_mfi_i2cStop();
		return -TLK_EFAIL;
	}
	tlkdrv_mfi_i2cWriteByte(reg);
	tlkdrv_mfi_i2cStop();

	tlkdrv_mfi_i2cStart();
	if(tlkdrv_mfi_i2cWriteByteWaitAck(TLKDRV_MFI_I2C_ADDR | 1)){
		tlkdrv_mfi_i2cStop();
		return -TLK_EFAIL;
	}
	for(index = 0; index < readLen; index++){
		*pBuff++ = tlkdrv_mfi_i2cReadByte(index == (readLen-1));
	}
	tlkdrv_mfi_i2cStop();

	return readLen;
}

int tlkdrv_mfi_write(uint08 reg, uint08 *pData, uint08 dataLen)
{
	int index;

	if(!sTlkdrvMfiCtrl.isOpen) return -TLK_ENOREADY;
	
	tlkdrv_mfi_i2cStart();
	if(tlkdrv_mfi_i2cWriteByteWaitAck(TLKDRV_MFI_I2C_ADDR)){
		tlkdrv_mfi_i2cStop();
		return -TLK_EFAIL;
	}
	tlkdrv_mfi_i2cWriteByte(reg);
	for(index=0; index<dataLen; index++ ){
		tlkdrv_mfi_i2cWriteByte(*pData++);
	}
	tlkdrv_mfi_i2cStop();
	return dataLen;
}





static inline void tlkdrv_mfi_i2cWait(void)
{
	
}

static inline void tlkdrv_mfi_i2cLongWait(void)
{
	delay_us(1);
}

// Pulling the line to ground is considered a logical zero while letting the line float is a logical one.   http://en.wikipedia.org/wiki/I%C2%B2C
static inline void tlkdrv_mfi_i2cSclOut(int v)
{
	gpio_set_output_en(TLKDRV_MFI_I2C_SCL_PIN,(!v));
}

static inline int tlkdrv_mfi_i2cSclIn(void)
{
	return gpio_read(TLKDRV_MFI_I2C_SCL_PIN);
}

// Pulling the line to ground is considered a logical zero while letting the line float is a logical one.   http://en.wikipedia.org/wiki/I%C2%B2C
static inline void tlkdrv_mfi_i2cSdaOut(int v)
{
	gpio_set_output_en(TLKDRV_MFI_I2C_SDA_PIN,(!v));
}

static inline int tlkdrv_mfi_i2cSdaIn(void)
{
	return gpio_read(TLKDRV_MFI_I2C_SDA_PIN);
}

static inline void tlkdrv_mfi_i2cSclInit(void)
{
	//gpio_set_func(TLKDRV_MFI_I2C_SCL_PIN, AS_GPIO);
	gpio_set_gpio_en(TLKDRV_MFI_I2C_SCL_PIN);
}

static inline void tlkdrv_mfi_i2cSdaInit(void)
{
	//gpio_set_func(TLKDRV_MFI_I2C_SDA_PIN, AS_GPIO);
	gpio_set_gpio_en(TLKDRV_MFI_I2C_SDA_PIN);
	gpio_set_input_en(TLKDRV_MFI_I2C_SDA_PIN, 1);
}

static inline void tlkdrv_mfi_i2cSclIdle(void)
{
	gpio_set_output_en(TLKDRV_MFI_I2C_SCL_PIN, 0);
	gpio_write(TLKDRV_MFI_I2C_SCL_PIN, 0);
}

static inline void tlkdrv_mfi_i2cSdaIdle(void)
{
	gpio_set_output_en(TLKDRV_MFI_I2C_SDA_PIN, 0);
	gpio_write(TLKDRV_MFI_I2C_SDA_PIN, 0);
}


/******************************************************************************
 * Function: tlkdrv_mfi_i2cStart
 * Descript: Sets clock high, then data high. This will do a stop if data was 
 *           low. Then sets data low, which should be a start condition.  
 *           After executing, data is left low, while clock is left high.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_mfi_i2cStart(void)
{
	tlkdrv_mfi_i2cSclInit();
	tlkdrv_mfi_i2cSdaInit();
	tlkdrv_mfi_i2cSdaIdle();
	tlkdrv_mfi_i2cSclIdle();
	tlkdrv_mfi_i2cSdaOut(0);		//sda: 0
	tlkdrv_mfi_i2cWait();
}
/******************************************************************************
 * Function: tlkdrv_mfi_i2cStop
 * Descript: puts data low, then clock low, then clock high, then data high.
 *           This should cause a stop, which should idle the bus, I.E. both clk
 *           and data are high.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_mfi_i2cStop(void)
{
	tlkdrv_mfi_i2cSdaOut(0);
	tlkdrv_mfi_i2cWait();
	tlkdrv_mfi_i2cSclOut(0);
	tlkdrv_mfi_i2cWait();
	tlkdrv_mfi_i2cSclOut(1);
	tlkdrv_mfi_i2cWait();
	tlkdrv_mfi_i2cSdaOut(1);
}


static int tlkdrv_mfi_i2cReadBit(void)
{
	tlkdrv_mfi_i2cWriteBit(1);
	return tlkdrv_mfi_i2cSdaIn();
}
static void tlkdrv_mfi_i2cWriteBit(int bit)
{
	tlkdrv_mfi_i2cSclOut(0);
	tlkdrv_mfi_i2cSdaOut(bit);
	tlkdrv_mfi_i2cLongWait();
	tlkdrv_mfi_i2cSclOut(1);
}

static uint tlkdrv_mfi_i2cReadByte(int last)
{
	int index;
	uint08 dat = 0;

	for(index=0; index<8; index++){
		tlkdrv_mfi_i2cWriteBit(1);
		if(tlkdrv_mfi_i2cSdaIn()){
			dat =(dat << 1) | 0x01;
		}else{
			dat = dat << 1;
		}
	}
	tlkdrv_mfi_i2cWriteBit(last);
	return dat;
}
static int tlkdrv_mfi_i2cWriteByte(uint08 dat)
{
	int i = 0x80;
	while(i){
		tlkdrv_mfi_i2cWriteBit((dat & i));
		i = i >> 1;
	}
	return tlkdrv_mfi_i2cReadBit();
}

static bool tlkdrv_mfi_i2cWriteByteWaitAck(uint08 id)
{
	int num = 0;
	int nak = 1;
	while(nak && num++ < 2000){
		nak = tlkdrv_mfi_i2cWriteByte (id);
		if(!nak){
			return false;
		}
		delay_us(500);
		//i2c_sim_stop();
		tlkdrv_mfi_i2cStart();
	}
	tlkdrv_mfi_i2cStop();
	return true;
}

