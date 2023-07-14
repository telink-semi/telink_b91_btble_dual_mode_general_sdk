/********************************************************************************************************
 * @file	tlkdrv_mmc5603.c
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_gsensor.h"
#if (TLKDRV_GSENSOR_MMC5603_ENABLE)
#include "tlkdrv_mmc5603.h"
#include "./mmc5603/tlkdrv_mmc5603Ctrl.h"
#include "drivers.h"

extern void mmc5603_arithmetic(void);

static bool tlkdrv_mmc5603_isOpen(void);
static int tlkdrv_mmc5603_init(void);
static int tlkdrv_mmc5603_open(void);
static int tlkdrv_mmc5603_close(void);
static int tlkdrv_mmc5603_reset(void);
static int tlkdrv_mmc5603_handler(uint16 opcode, uint32 param0, uint32 param1);
const tlkdrv_gsensor_modinf_t gcTlkDrvMMC5603Infs = {
	tlkdrv_mmc5603_isOpen, //IsOpen
	tlkdrv_mmc5603_init,  //Init
	tlkdrv_mmc5603_open,  //Open
	tlkdrv_mmc5603_close, //Close
	tlkdrv_mmc5603_reset,  //Reset
	tlkdrv_mmc5603_handler, //Handler
};

static tlkdrv_mmc5603_t sTlkDrvMMC5603Ctrl = {0};


static bool tlkdrv_mmc5603_isOpen(void)
{
	return sTlkDrvMMC5603Ctrl.isOpen;
}
static int tlkdrv_mmc5603_init(void)
{
	tmemset(&sTlkDrvMMC5603Ctrl, 0, sizeof(sTlkDrvMMC5603Ctrl));

	//i2c
    i2c_master_init();
	i2c_set_master_clk((unsigned char)(sys_clk.pclk*1000*1000/(4*TLKDRV_MMC5603_CLOCK_SPEED)));
	i2c_set_pin(TLKDRV_MMC5603_SDA_PIN, TLKDRV_MMC5603_SCL_PIN);
	
	//geo init
	if(mmc5603_dev_init(NULL)){

	}else{
		tlkapi_error(TLKDRV_MMC5603_DBG_FLAG, TLKDRV_MMC5603_DBG_SIGN, "wbwb:mmc6503 read id err");
	}

	uint08 addr = 0x39;
	uint08 chipID = 0;
	i2c_master_write_read(TLKDRV_MMC5603_I2C_SLAVE_ADDR, &addr, 1, &chipID, 1);
	tlkapi_trace(TLKDRV_MMC5603_DBG_FLAG, TLKDRV_MMC5603_DBG_SIGN, "wbwb:mmc6503 -- id = %x", chipID);

	sTlkDrvMMC5603Ctrl.isInit = true;

	return TLK_ENONE;
}
static int tlkdrv_mmc5603_open(void)
{
	if(!sTlkDrvMMC5603Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvMMC5603Ctrl.isOpen) return -TLK_EREPEAT;

	MMC5603_Enable();
	sTlkDrvMMC5603Ctrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_mmc5603_close(void)
{
	if(!sTlkDrvMMC5603Ctrl.isOpen) return TLK_ENONE;

	MMC5603_Disable();
	sTlkDrvMMC5603Ctrl.isOpen = false;
	return TLK_ENONE;
}
static int tlkdrv_mmc5603_reset(void)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_mmc5603_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	mmc5603_arithmetic();
	return -TLK_ENOSUPPORT;
}

unsigned char tlkdrv_mmc5603_i2cWrite(unsigned char reg, unsigned char dat)
{
	uint08 buffer[2];

	buffer[0] = reg;
	buffer[1] = dat;
	i2c_master_send_stop(1);
	i2c_master_write(TLKDRV_MMC5603_I2C_SLAVE_ADDR, buffer, 2);
	return TLK_ENONE;
}

unsigned char tlkdrv_mmc5603_i2cRead(unsigned char reg, unsigned char len, unsigned char *buf)
{
	i2c_master_send_stop(0);
	i2c_master_write_read(TLKDRV_MMC5603_I2C_SLAVE_ADDR, &reg, 1, buf, len);
	return TLK_ENONE;
}



#endif //#if (TLKDRV_GSENSOR_DSDA217_ENABLE)

