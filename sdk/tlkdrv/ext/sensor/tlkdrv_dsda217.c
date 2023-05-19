/********************************************************************************************************
 * @file     tlkdrv_dsda217.c
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
#include "tlkdrv_sensor.h"
#if (TLKDRV_SENSOR_DSDA217_ENABLE)
#include "tlkdrv_dsda217.h"
#include "drivers.h"


#define TLKDRV_DSDA217_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_DSDA217_DBG_SIGN     "[DRV]"


static bool tlkdrv_dsda217_isOpen(void);
static int tlkdrv_dsda217_init(void);
static int tlkdrv_dsda217_open(void);
static int tlkdrv_dsda217_close(void);
static int tlkdrv_dsda217_reset(void);
static int tlkdrv_dsda217_handler(uint16 opcode, uint32 param0, uint32 param1);
const tlkdrv_sensor_modinf_t gcTlkDrvDsda217Infs = {
	tlkdrv_dsda217_isOpen, //IsOpen
	tlkdrv_dsda217_init,  //Init
	tlkdrv_dsda217_open,  //Open
	tlkdrv_dsda217_close, //Close
	tlkdrv_dsda217_reset,  //Reset
	tlkdrv_dsda217_handler, //Handler
};
static tlkdrv_dsda217_t sTlkDrvDsda217Ctrl = {0};


#define YZ_CROSS_TALK_ENABLE                1 

#if YZ_CROSS_TALK_ENABLE 
static short yzcross=0;
#endif


uint08 i2c_addr = 0x27;



static bool tlkdrv_dsda217_isOpen(void)
{
	return sTlkDrvDsda217Ctrl.isOpen;
}
static int tlkdrv_dsda217_init(void)
{
	sTlkDrvDsda217Ctrl.isInit = true;
	sTlkDrvDsda217Ctrl.isOpen = false;

	gpio_function_en(TLKDRV_DSDA217_POWER_PIN);
	gpio_output_en(TLKDRV_DSDA217_POWER_PIN);
	gpio_set_high_level(TLKDRV_DSDA217_POWER_PIN); //Close
	gpio_set_up_down_res(TLKDRV_DSDA217_POWER_PIN, GPIO_PIN_PULLUP_1M);
		
	return TLK_ENONE;
}
static int tlkdrv_dsda217_open(void)
{
	if(!sTlkDrvDsda217Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvDsda217Ctrl.isOpen) return -TLK_EREPEAT;

	gpio_set_low_level(TLKDRV_DSDA217_POWER_PIN);
	
	i2c1_m_set_pin(TLKDRV_DSDA217_SDA_PIN, TLKDRV_DSDA217_SCL_PIN);
	i2c1_m_master_init();
	i2c1_m_set_master_clk((unsigned char)(sys_clk.pclk*1000*1000/(4*TLKDRV_DSDA217_CLOCK_SPEED)));//set i2c frequency 200K.
//	i2c_master_strech_en();

	delay_ms(10);
	i2c1_m_master_send_stop(1);
//	i2c_master_write();
	//TLKDRV_DSDA217_I2C_ADDR
	uint08 data[4] = {0};
	uint08 buffer[4] = {0};
	data[0] = TLKDRV_DSDA217_REG_WHO_AM_I;
	i2c1_m_master_send_stop(1);
	i2c1_m_master_write_read(TLKDRV_DSDA217_I2C_READ_ADDR, data, 1, buffer, 1);
	
	sTlkDrvDsda217Ctrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_dsda217_close(void)
{
	if(!sTlkDrvDsda217Ctrl.isOpen) return TLK_ENONE;

	sTlkDrvDsda217Ctrl.isOpen = false;
	gpio_set_high_level(TLKDRV_DSDA217_POWER_PIN);
	

	return TLK_ENONE;
}
static int tlkdrv_dsda217_reset(void)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_dsda217_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}



int mir3da_register_read(uint08 addr, uint08 *data_m, uint08 len)
{
	//To do i2c read api
	
	return 0;
}

int mir3da_register_write(uint08 addr, uint08 data_m)
{
	//To do i2c write api

	return 0;
}

int mir3da_register_mask_write(uint08 addr, uint08 mask, uint08 data)
{
    int res = 0;
    uint08 tmp_data;

    res = mir3da_register_read(addr, &tmp_data, 1);
    if(res) {
        return res;
    }

    tmp_data &= ~mask; 
    tmp_data |= data & mask;
    res = mir3da_register_write(addr, tmp_data);

    return res;
}


//Initialization
int mir3da_init(void)
{
	int res = 0;
	uint08 data_m = 0;

	//Retry 3 times
	res = mir3da_register_read(TLKDRV_DSDA217_REG_WHO_AM_I,&data_m,1);
    if(data_m != 0x13){
        res = mir3da_register_read(TLKDRV_DSDA217_REG_WHO_AM_I,&data_m,1);
        if(data_m != 0x13){
            res = mir3da_register_read(TLKDRV_DSDA217_REG_WHO_AM_I,&data_m,1);
            if(data_m != 0x13){
                //printf("------mir3da read chip id  error= %x-----\r\n",data_m);  
                return -1;
            }
        }
    }
    mir3da_register_mask_write(0x00, 0x24, 0x24);
	delay_ms(50); //delay 50ms
 
	//printf("------mir3da chip id = %x-----\r\n",data_m); 

	res |= mir3da_register_write(TLKDRV_DSDA217_REG_G_RANGE, 0x01);               //+/-4G,14bit
	res |= mir3da_register_write(TLKDRV_DSDA217_REG_POWERMODE_BW, 0x14);          //normal mode
	res |= mir3da_register_write(TLKDRV_DSDA217_REG_ODR_AXIS_DISABLE, 0x07);      //ODR = 125hz
	
	//Engineering mode
	res |= mir3da_register_write(TLKDRV_DSDA217_REG_ENGINEERING_MODE, 0x83);
	res |= mir3da_register_write(TLKDRV_DSDA217_REG_ENGINEERING_MODE, 0x69);
	res |= mir3da_register_write(TLKDRV_DSDA217_REG_ENGINEERING_MODE, 0xBD);
  
	//Reduce power consumption
	if(i2c_addr == 0x26){
		mir3da_register_mask_write(TLKDRV_DSDA217_REG_SENS_COMP, 0x40, 0x00);
	}

#if YZ_CROSS_TALK_ENABLE 
    res |= mir3da_register_read(TLKDRV_DSDA217_REG_CHIP_INFO_SECOND, &data_m, 1);
	           
    if(data_m & 0x10)
        yzcross = -(data_m&0x0f);
    else
		yzcross = (data_m&0x0f);
#endif	
#if 0
	mir3da_register_mask_write(0x8f, 0x02, 0x00);
#endif	
	return res;	    	
}






//enable/disable the chip
int mir3da_set_enable(uint08 enable)
{
	int res = 0;
	if(enable)
		res = mir3da_register_write(TLKDRV_DSDA217_REG_POWERMODE_BW,0x14);
	else	
		res = mir3da_register_write(TLKDRV_DSDA217_REG_POWERMODE_BW,0x80);
	
	return res;	
}

//Read three axis data, 1024 LSB = 1 g
int mir3da_read_data(sint16 *x, sint16 *y, sint16 *z)
{
    uint08    tmp_data[6] = {0};

#if 0
    if (mir3da_register_read(TLKDRV_DSDA217_REG_ACC_X_LSB, tmp_data,6) != 0) {
        return -1;
    }
#else
	mir3da_register_read(TLKDRV_DSDA217_REG_ACC_X_LSB, &tmp_data[0], 1);
	mir3da_register_read(TLKDRV_DSDA217_REG_ACC_X_MSB, &tmp_data[1], 1);
	mir3da_register_read(TLKDRV_DSDA217_REG_ACC_Y_LSB, &tmp_data[2], 1);
	mir3da_register_read(TLKDRV_DSDA217_REG_ACC_Y_MSB, &tmp_data[3], 1);
	mir3da_register_read(TLKDRV_DSDA217_REG_ACC_Z_LSB, &tmp_data[4], 1);
	mir3da_register_read(TLKDRV_DSDA217_REG_ACC_Z_MSB, &tmp_data[5], 1);
 #endif
	
    *x = ((sint16)(tmp_data[1] << 8 | tmp_data[0]))>> 3;
    *y = ((sint16)(tmp_data[3] << 8 | tmp_data[2]))>> 3;
    *z = ((sint16)(tmp_data[5] << 8 | tmp_data[4]))>> 3;	
	
#if YZ_CROSS_TALK_ENABLE
    if(yzcross)
        *y=*y-(*z)*yzcross/100;
#endif

    return 0;
}

//open active interrupt
int mir3da_enable_interrupt(uint08 th)
{
	int   res = 0;

	res = mir3da_register_write(TLKDRV_DSDA217_REG_ACTIVE_DURATION, 0x02);
	res = mir3da_register_write(TLKDRV_DSDA217_REG_ACTIVE_THRESHOLD, th);
	res = mir3da_register_write(TLKDRV_DSDA217_REG_INTERRUPT_MAPPING1, 0x04);
	res = mir3da_register_write(TLKDRV_DSDA217_REG_INT_LATCH, 0xEE);  //latch 100ms
	res = mir3da_register_write(TLKDRV_DSDA217_REG_INTERRUPT_SETTINGS1, 0x87);

	return res;
}
//close active interrupt
int mir3da_disable_interrupt(void)
{
	int   res = 0;

	res = mir3da_register_write(TLKDRV_DSDA217_REG_INTERRUPT_SETTINGS1,0x00 );
	res = mir3da_register_write(TLKDRV_DSDA217_REG_INTERRUPT_MAPPING1,0x00 );

	return res;
}




#endif //#if (TLKDRV_SENSOR_DSDA217_ENABLE)

