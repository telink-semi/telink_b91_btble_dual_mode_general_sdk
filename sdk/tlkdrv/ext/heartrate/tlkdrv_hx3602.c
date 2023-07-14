/********************************************************************************************************
 * @file	tlkdrv_hx3602.c
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
#include "tlkdrv_heartrate.h"
#if (TLKDRV_HEARTRATE_HX3602_ENABLE)
#include "tlkdrv_hx3602.h"
#include "drivers.h"
#include "hx3602/tlkdrv_hrs3602.h"
#include "hx3602/tlkdrv_hrs3602HrsDriv.h"
#include "hx3602/tlkdrv_tyhxHrsAlg.h"

#define TLKDRV_HR_HX3602_DBG_FLAG     TLKDRV_HEARTRATE_DBG_FLAG//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_HR_HX3602_DBG_SIGN     TLKDRV_HEARTRATE_DBG_SIGN//"[HEARTRATE]"


static int tlkdrv_hx3602_powerOn(void);


static bool tlkdrv_hx3602_isOpen(void);
static int tlkdrv_hx3602_init(void);
static int tlkdrv_hx3602_open(void);
static int tlkdrv_hx3602_close(void);
static int tlkdrv_hx3602_reset(void);
static int tlkdrv_hx3602_handler(uint16 opcode, uint32 param0, uint32 param1);
const tlkdrv_heartRate_modinf_t gcTlkDrvHx3602Infs = {
	tlkdrv_hx3602_isOpen, //IsOpen
	tlkdrv_hx3602_init,  //Init
	tlkdrv_hx3602_open,  //Open
	tlkdrv_hx3602_close, //Close
	tlkdrv_hx3602_reset,  //Reset
	tlkdrv_hx3602_handler, //Handler
};

static tlkdrv_hr_hx3602_t sTlkDrvHx3602Ctrl = {0};

static bool tlkdrv_hx3602_isOpen(void)
{
	return sTlkDrvHx3602Ctrl.isOpen;
}
static int tlkdrv_hx3602_init(void)
{
	tmemset(&sTlkDrvHx3602Ctrl, 0, sizeof(sTlkDrvHx3602Ctrl));

	//i2c
    i2c_master_init();
	i2c_set_master_clk((unsigned char)(sys_clk.pclk*1000*1000/(4*TLKDRV_HR_HX3602_CLOCK_SPEED)));
	i2c_set_pin(TLKDRV_HR_HX3602_SDA_PIN, TLKDRV_HR_HX3602_SCL_PIN);

	//hr init
	delay_ms(5);
	tlkdrv_hx3602_powerOn();

	u8 addr = 0;
	u8 chipID = 0;
	i2c_master_send_stop(0);
	i2c_master_write_read(0x88, &addr, 1, &chipID, 1);
	tlkapi_trace(TLKDRV_HR_HX3602_DBG_FLAG, TLKDRV_HR_HX3602_DBG_SIGN, "wbwb:hx3602 -- id = %x", chipID);

	Hrs3602_chip_disable();

	sTlkDrvHx3602Ctrl.isInit = true;

	return TLK_ENONE;
}
static int tlkdrv_hx3602_open(void)
{
	if(!sTlkDrvHx3602Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvHx3602Ctrl.isOpen) return -TLK_EREPEAT;

	tlkdrv_hx3602_powerOn();
	sTlkDrvHx3602Ctrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_hx3602_close(void)
{
	if(!sTlkDrvHx3602Ctrl.isOpen) return TLK_ENONE;

	Hrs3602_chip_disable();
	sTlkDrvHx3602Ctrl.isOpen = false;

	return TLK_ENONE;
}
static int tlkdrv_hx3602_reset(void)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_hx3602_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	Hrs3602_Int_handle(); //40ms timer call this function
	//tlkapi_ext_trace(TLKDRV_HR_HX3602_DBG_FLAG, TLKDRV_HR_HX3602_DBG_SIGN, "sensor -- hr_hx3602 handler, step = %d", step);
	return TLK_ENONE;
}

static int tlkdrv_hx3602_powerOn(void)
{
	if(Hrs3602_chip_init()){
		Hrs3602_alg_config();
		Hrs3602_driv_init();
		tyhx_hrs_alg_open();
	}else{
		tlkapi_error(TLKDRV_HR_HX3602_DBG_FLAG, TLKDRV_HR_HX3602_DBG_SIGN, "hr read id err");
	}

	return TLK_ENONE;
}

unsigned char tlkdrv_hx3602_i2cWrite(unsigned char reg, unsigned char dat)
{
	u8 buf[2];

	buf[0] = reg;
	buf[1] = dat;
	i2c_master_send_stop(1);
	i2c_master_write(TLKDRV_HR_HX3602_I2C_SLAVE_ADDR, buf, 2);
	return TLK_ENONE;
}

unsigned char tlkdrv_hx3602_i2cRead(unsigned char reg, unsigned char len, unsigned char *buf)
{
	i2c_master_send_stop(0);
	i2c_master_write_read(TLKDRV_HR_HX3602_I2C_SLAVE_ADDR, &reg, 1, buf, len);
	return TLK_ENONE;
}

#endif //#if (TLKDRV_GSENSOR_DSDA217_ENABLE)

