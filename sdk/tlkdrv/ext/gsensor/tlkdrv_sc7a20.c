/********************************************************************************************************
 * @file	tlkdrv_sc7a20.c
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
#if (TLKDRV_GSENSOR_SC7A20_ENABLE)
#include "tlkdrv_sc7a20.h"
#include "drivers.h"
#include "sc7a20/tlkdrv_sc7a20_alg.h"
#include "sc7a20/tlkdrv_sc7a20_app.h"


#define TLKDRV_SC7A20_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_SC7A20_DBG_SIGN     "[DRV]"


static bool tlkdrv_sc7a20_isOpen(void);
static int tlkdrv_sc7a20_init(void);
static int tlkdrv_sc7a20_open(void);
static int tlkdrv_sc7a20_close(void);
static int tlkdrv_sc7a20_reset(void);
static int tlkdrv_sc7a20_handler(uint16 opcode, uint32 param0, uint32 param1);
const tlkdrv_gsensor_modinf_t gcTlkDrvSc7a20Infs = {
	tlkdrv_sc7a20_isOpen, //IsOpen
	tlkdrv_sc7a20_init,  //Init
	tlkdrv_sc7a20_open,  //Open
	tlkdrv_sc7a20_close, //Close
	tlkdrv_sc7a20_reset,  //Reset
	tlkdrv_sc7a20_handler, //Handler
};

static tlkdrv_sc7a20_t sTlkDrvSc7a20Ctrl = {0};

#if(TLKDRV_SC7A20_INT_EN)
_attribute_ram_code_sec_noinline_
void gpio_risc0_irq_handler(void)
{
	gpio_clr_irq_status(FLD_GPIO_IRQ_GPIO2RISC0_CLR);
	sTlkDrvSc7a20Ctrl.irqRdy = true;
}
#endif

static bool tlkdrv_sc7a20_isOpen(void)
{
	return sTlkDrvSc7a20Ctrl.isOpen;
}
static int tlkdrv_sc7a20_init(void)
{
	tmemset(&sTlkDrvSc7a20Ctrl, 0, sizeof(sTlkDrvSc7a20Ctrl));

	//i2c
    i2c_master_init();
	i2c_set_master_clk((unsigned char)(sys_clk.pclk*1000*1000/(4*TLKDRV_SC7A20_CLOCK_SPEED)));
	i2c_set_pin(TLKDRV_SC7A20_SDA_PIN, TLKDRV_SC7A20_SCL_PIN);

#if(TLKDRV_SC7A20_INT_EN)
	//interrupt pin
	gpio_function_en(TLKDRV_SC7A20_INT_PIN);
	gpio_output_dis(TLKDRV_SC7A20_INT_PIN);
	gpio_input_en(TLKDRV_SC7A20_INT_PIN);
	gpio_set_up_down_res(TLKDRV_SC7A20_INT_PIN, GPIO_PIN_PULLDOWN_100K);
#endif


	//gSensor
	static u8 chipID = 0;
	chipID = SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_L_INIT();
	tlkapi_trace(TLKDRV_SC7A20_DBG_FLAG, TLKDRV_SC7A20_DBG_SIGN, "wbwb:sc7a20 -- chip id = %x", chipID);

#if(TLKDRV_SC7A20_INT_EN)
	//modify gSensor register to set the sensor trigger irq when fifo is full
	SL_SC7A20_I2c_Spi_Write(1,0x25,0x00);
	SL_SC7A20_I2c_Spi_Write(1,0x22,0x04);
#endif

	sTlkDrvSc7a20Ctrl.isInit = true;

	return TLK_ENONE;
}
static int tlkdrv_sc7a20_open(void)
{
	if(!sTlkDrvSc7a20Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvSc7a20Ctrl.isOpen) return -TLK_EREPEAT;

#if(TLKDRV_SC7A20_INT_EN)
	//enable wakeup
	tlkapi_trace(TLKDRV_SC7A20_DBG_FLAG, TLKDRV_SC7A20_DBG_SIGN, "wbwb:sc7a20 -- irq init");
	pm_set_gpio_wakeup(TLKDRV_SC7A20_INT_PIN, WAKEUP_LEVEL_HIGH, 1);

	//enable irq
	gpio_set_gpio2risc0_irq(TLKDRV_SC7A20_INT_PIN, INTR_RISING_EDGE);
	plic_set_priority(IRQ26_GPIO2RISC0, IRQ_PRI_LEV2);
	plic_interrupt_enable(IRQ26_GPIO2RISC0);
#endif
	
	sTlkDrvSc7a20Ctrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_sc7a20_close(void)
{
	if(!sTlkDrvSc7a20Ctrl.isOpen) return TLK_ENONE;

#if(TLKDRV_SC7A20_INT_EN)
	//disable wakeup
	pm_set_gpio_wakeup(TLKDRV_SC7A20_INT_PIN, WAKEUP_LEVEL_LOW, 0);
	//disable irq
	plic_interrupt_disable(IRQ26_GPIO2RISC0);
#endif

	sTlkDrvSc7a20Ctrl.isOpen = false;

	return TLK_ENONE;
}
static int tlkdrv_sc7a20_reset(void)
{
	SL_Pedo_Kcal_ResetStepCount();
	SL_Pedo_WorkMode_Reset();
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_sc7a20_handler(uint16 opcode, uint32 param0, uint32 param1)
{
#if(TLKDRV_SC7A20_INT_EN)
	if( sTlkDrvSc7a20Ctrl.irqRdy )
	{
		sTlkDrvSc7a20Ctrl.irqRdy = false;

		u32 step = SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_L_ALGO();
		tlkapi_ext_trace(TLKDRV_SC7A20_DBG_FLAG, TLKDRV_SC7A20_DBG_SIGN, "sensor -- sc7a20 handler, step = %d", step);
	}
#else
	u32 step = SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_L_ALGO();
	tlkapi_ext_trace(TLKDRV_SC7A20_DBG_FLAG, TLKDRV_SC7A20_DBG_SIGN, "sensor -- sc7a20 handler, step = %d", step);
#endif

	return -TLK_ENOSUPPORT;
}

unsigned char SL_SC7A20_I2c_Spi_Write(SL_BOOL sl_spi_iic,unsigned char reg, unsigned char dat)
{
	u8 buf[2];
	if( sl_spi_iic ) //i2c
	{
		buf[0] = reg;
		buf[1] = dat;
		i2c_master_send_stop(1);
		i2c_master_write(TLKDRV_SC7A20_I2C_SLAVE_ADDR, buf, 2);
		return TLK_ENONE;
	}

	return -TLK_ENOSUPPORT;
}

unsigned char SL_SC7A20_I2c_Spi_Read(SL_BOOL sl_spi_iic,unsigned char reg, unsigned char len, unsigned char *buf)
{
	if( sl_spi_iic ) //i2c
	{
		i2c_master_send_stop(0);
		i2c_master_write_read(TLKDRV_SC7A20_I2C_SLAVE_ADDR, &reg, 1, buf, len);
		return TLK_ENONE;
	}

	return -TLK_ENOSUPPORT;
}

#endif //#if (TLKDRV_SENSOR_DSDA217_ENABLE)

