/********************************************************************************************************
 * @file	tlkdrv_hrs3602.c
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
#include "../tlkdrv_heartrate.h"
#if (TLKDRV_HEARTRATE_HX3602_ENABLE)
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "tlkapi/tlkapi_stdio.h"
//////////////////////////////

#include "tlkdrv_hrs3602.h"
#include "tlkdrv_hrs3600Reg.h"
#include "tlkdrv_hrs3602HrsDriv.h"
#include "tlkdrv_tyhxHrsAlg.h"

#ifdef TYHX_DEMO
#include "SEGGER_RTT.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "twi_master.h"
#include "drv_oled.h"
#include "button.h"
#include "demo_ctrl.h"
#endif


#ifdef TESTVEC    
#include "testvec.h"
#endif

#ifdef GSENSER_DATA
#include "lis3dh_drv.h"
#endif

#ifdef TYHX_DEMO
extern volatile oled_display_t oled_dis;
#endif

#define TLKDRV_HR_HX3602_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_HR_HX3602_DBG_SIGN     "[HEARTRATE]"

extern unsigned char tlkdrv_hx3602_i2cWrite(unsigned char reg, unsigned char dat);
extern unsigned char tlkdrv_hx3602_i2cRead(unsigned char reg, unsigned char len, unsigned char *buf);
// success return 0   fail  return 1
bool Hrs3602_write_reg(uint8_t addr, uint8_t data) 
{
	tlkdrv_hx3602_i2cWrite(addr, data);
    return 0;      
}

uint8_t Hrs3602_read_reg(uint8_t addr) 
{
	uint8_t data_buf = 0;
	tlkdrv_hx3602_i2cRead(addr, 1, &data_buf);
    return data_buf;      
}

bool Hrs3602_brust_read_reg(uint8_t addr , uint8_t *buf, uint8_t length) 
{
	tlkdrv_hx3602_i2cRead(addr, length, buf);
    return true;      
}
void Hrs3602_read_data_packet()
{ 
    uint8_t  databuf1[6] = {0};
    uint8_t  databuf2[6] = {0};
//    int32_t P0 = 0, P1 = 0,P2 = 0, P3 = 0;
    Hrs3602_brust_read_reg(0xa0, databuf1, 6); 
    Hrs3602_brust_read_reg(0xa6, databuf2, 6);    

//    P0= ((databuf1[0]) | (databuf1[1] << 8) | (databuf1[2] << 16));
//    P1 = ((databuf1[3]) | (databuf1[4] << 8) | (databuf1[5] << 16));
//    P2 = ((databuf2[0]) | (databuf2[1] << 8) | (databuf2[2] << 16));
//    P3  = ((databuf2[3]) | (databuf2[4] << 8) | (databuf2[5] << 16));
    Hrs3602_brust_read_reg(0xa0, databuf1, 6); 
    Hrs3602_brust_read_reg(0xa6, databuf2, 6); 
    //DEBUG_PRINTF("%d %d %d %d\r\n" ,P0,P1,P2,P3);

}

bool Hrs3602_read_hrs(int32_t *hrm_data, int32_t *als_data)
{
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;
    Hrs3602_brust_read_reg(0xa0, databuf, 6);
    
    P0 = ((databuf[0])|(databuf[1]<<8)|(databuf[2]<<16));
    P1 = ((databuf[3])|(databuf[4]<<8)|(databuf[5]<<16));
    //DEBUG_PRINTF(0," %d %d \r\n" , P0, P1);
    if (P0 > P1)
    {
        *hrm_data = P0 - P1;
    }
    else
    {
        *hrm_data = 0;
    }
    *als_data = P0;
    
    return true;    
}

bool Hrs3602_read_ps1(int32_t *infrared_data)
{
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;
    Hrs3602_brust_read_reg(0xa6, databuf, 6);
    P0 = ((databuf[0])|(databuf[1]<<8)|(databuf[2]<<16));
    P1 = ((databuf[3])|(databuf[4]<<8)|(databuf[5]<<16));
    if (P0 > P1)
    {
        *infrared_data = P0 - P1;
    }
    else
    {
        *infrared_data = 0;
    }
    return true;    
}

void Hrs3602_write_efuse()
{
    Hrs3602_write_reg( 0x85, 0x20 );   
    Hrs3602_write_reg( 0x7f, 0x10 ); 
    Hrs3602_write_reg( 0x80, 0x08 );     
    Hrs3602_write_reg( 0x81, 0x44 ); 
    Hrs3602_write_reg( 0x82, 0x40 ); 
    Hrs3602_write_reg( 0x85, 0x00 );          
}

void Hrs3602_chip_enable(void)
{
    Hrs3602_write_reg( 0x09, 0x02 );
}

void Hrs3602_chip_disable(void)
{
    Hrs3602_write_reg( 0x09, 0x03 ); 
}


bool Hrs3602_chip_init()
{
    int i =0 ;
    uint8_t chip_id =0;
    
    chip_id = Hrs3602_read_reg(0x00) ;
    if(chip_id != 0x22)
    {
        return false;
    }
    
    for(i = 0; i < INIT_ARRAY_SIZE; i++ )
    {
        if(Hrs3602_write_reg(init_register_array[i][0],init_register_array[i][1]) != 0)
        {
            return false;
        }
    }    
    
    return true;
}

void Hrs3602_alg_config(void)
{
    /*para init begin ....*/
    uint32_t prf_temp_clk_num = 0;    /*temperatrue phase clk num in each prf */
    uint32_t prf_hrs_clk_num = 0;     /*hrs phase clk num in each prf */
    uint32_t prf_ps_clk_num =0;       /*ps phase clk num in each prf */
    uint32_t prf_wait_clk_num =0;     /*wait time clk num in each prf */
    uint32_t en2rst_delay_clk_num =0; /*en signal to first reset delay time clk num in each prf */
    uint16_t rst_clk_num = 0;    
    uint16_t hrs_ckafe_clk_num = 0;
    uint16_t ps_ckafe_clk_num = 0;
    
    
    /*para init end ....*/
    
    /*chip config begin ....*/
    uint16_t sample_rate = 25;       /*config the data rate of chip frog2 ,uint is Hz*/
    uint32_t prf_clk_num = 2620000/sample_rate; /*period in clk num, num = Fclk/fs */
    
    uint8_t temperature_enable = 0;  /*temperature test function enable  , 1 mean enable ; 0 mean disable */
    uint8_t hrs_enable = 1;             /*hrs function enable  , 1 mean enable ; 0 mean disable */
    uint8_t ps_enable = 1;             /*ps function enable  , 1 mean enable ; 0 mean disable */
    
    uint8_t temperature_adc_osr = 0; /* 0 = 128 ; 1 = 256 ; 2 = 512 ; 3 = 1024 ;*/
    uint8_t hrs_adc_osr = 3;           /* 0 = 128 ; 1 = 256 ; 2 = 512 ; 3 = 1024 ;*/
    uint8_t ps_adc_osr = 3;               /* 0 = 128 ; 1 = 256 ; 2 = 512 ; 3 = 1024 ;*/
    
    uint8_t led_dr_cfg = 3 ;  /* 0 = 12.5mA ; 1 = 25mA ; 2 =50mA(default) 3 = 100mA*/
    uint8_t tia_res = 2 ;     /* 0 = 54k(default) ; 1 = 108k ; 2 =216k 3 = 432k */    
    
    en2rst_delay_clk_num = 64 ;   /* 0 ~ 127 clk num config */
    rst_clk_num = 6 ;             /* range from 0 to 7 ; 0=1 , 1=3, 2=7,....,7=255 */
    hrs_ckafe_clk_num = 128 ;     /* hrm phase led on time : 0~ 255 , 实际宽度10位，低两位默认为0*/
    ps_ckafe_clk_num = 4 ;       /* ps phase led on time : 0~ 255 , 实际宽度10位，低两位默认为0*/
    
    
    /*计算PRF_WAIT的时间*/
    if(temperature_enable == 1)
    {
        prf_temp_clk_num = (en2rst_delay_clk_num + 10) + 2*((2<<rst_clk_num) - 1) + 2*(128<<temperature_adc_osr);
    }
    
    if(hrs_enable == 1)
    {
        prf_hrs_clk_num = (en2rst_delay_clk_num + 10) + 
                           2*(2*((2<<rst_clk_num) - 1) + (hrs_ckafe_clk_num<<4) + 10 + (128<<hrs_adc_osr));
    }
    
    if(ps_enable == 1)
    {
        prf_ps_clk_num = (en2rst_delay_clk_num + 10) + 
                           2*(2*((2<<rst_clk_num) - 1) + (ps_ckafe_clk_num<<4) + 10 + (128<<ps_adc_osr));
    }
    
    prf_wait_clk_num = prf_clk_num - prf_temp_clk_num - prf_hrs_clk_num - prf_ps_clk_num ;
    /*计算PRF_WAIT的时间*/

    Hrs3602_write_reg(0xc1, 0x08);   // config external pd as input  and cap mode enable  
    Hrs3602_write_reg(0x03, 0x8f);   // all phase use same external pd 
  
    /*register configration begin ....*/
    
    Hrs3602_write_reg(0x01, (temperature_enable<<2)|temperature_adc_osr);
    Hrs3602_write_reg(0x02, (hrs_enable<<2)|(hrs_adc_osr)|(ps_adc_osr<<4)|(ps_enable<<6));
    
    //DEBUG_PRINTF(0," reg_0x02 is %x \r\n" , Hrs3602_read_reg(0x02));    
        
    
    Hrs3602_write_reg(0x04, hrs_ckafe_clk_num);
    Hrs3602_write_reg(0x05, ps_ckafe_clk_num);
    
    //DEBUG_PRINTF(0," %x %x \r\n" , Hrs3602_read_reg(0x04) , Hrs3602_read_reg(0x05));
    
    Hrs3602_write_reg(0x0a, prf_wait_clk_num);    
    Hrs3602_write_reg(0x0b, prf_wait_clk_num>>8);    
    Hrs3602_write_reg(0x0c, prf_wait_clk_num>>16);
    
    Hrs3602_write_reg(0x11, rst_clk_num);
    Hrs3602_write_reg(0x12, en2rst_delay_clk_num);
    
    //GPIO config 
    Hrs3602_write_reg(0xc0, (0x84 | led_dr_cfg)); /* led dr config , 0x84 = 12.5mA , 0x85 = 25mA ,0x86 = 50 mA ,0x87 = 100mA */
    Hrs3602_write_reg(0xc2, (0x00 | tia_res));    /* tia res config , 0x00 = 54K , 0x01 = 108K ,0x02 = 216K ,0x03 = 432K */
    /*register configration end ....*/
    Hrs3602_write_reg(0x09, 0x02 );  
    
    #ifdef IR_CHECK_TOUCH
	Hrs3602_write_reg(0X14, 0x80); 
    Hrs3602_write_reg(0X15, 0x40); 	
    #else
    Hrs3602_write_reg(0X14, 0x40); 
    Hrs3602_write_reg(0X15, 0x40); 
    #endif

    Hrs3602_write_reg( 0x08, 0x00 );  // self clear int  
    return ;        
}

void Hrs3602_int_clear()
{
	Hrs3602_write_reg(0x06, 0x7f);  
    return ;
}

void Hrs3602_Int_handle()
{        
    int32_t        hrm_raw_data;
    int32_t        als_raw_data;
    int32_t        infrared_data;
    hrs_results_t  alg_results = {MSG_HRS_ALG_NOT_OPEN,0,0,0,0};
	hrs3602_wear_status_t wear_status = MSG_NO_TOUCH;

    #ifdef BP_CUSTDOWN_ALG_LIB    
//    bp_results_t bp_alg_results;
    #endif

    Hrs3602_read_hrs(&hrm_raw_data, &als_raw_data);
    Hrs3602_read_ps1(&infrared_data);	
		

    #ifdef GSENSER_DATA
	AxesRaw_t gsen_buf = {0};
    LIS3DH_GetAccAxesRaw(&gsen_buf);
	#else
	GsensorRaw_t gsen_buf = {0};
    #endif


    #ifdef TESTVEC    
    alg_results = tyhx_hrs_alg_get_results();            
    if (alg_results.data_cnt > (sizeof(hrm_input_data)/sizeof(hrm_input_data[0])))
    {
        oled_dis.refresh_flag = 1;
        oled_dis.dis_mode = DIS_HR;
        oled_dis.dis_data = 0;
        return;
    }
    hrm_raw_data = hrm_input_data[alg_results.data_cnt];
    gsen_buf.AXIS_X = gsen_input_data_x[alg_results.data_cnt];
    gsen_buf.AXIS_Y = gsen_input_data_y[alg_results.data_cnt];
    gsen_buf.AXIS_Z = gsen_input_data_z[alg_results.data_cnt];      
    #endif    	
    wear_status = Hrs3602_agc(als_raw_data,infrared_data);
    //DEBUG_PRINTF(" gr=%d, als=%d ir=%d status=%d \r\n" ,hrm_raw_data, als_raw_data, infrared_data, wear_status);
    if(wear_status == MSG_TOUCH)
    {
        tyhx_hrs_alg_send_data(&hrm_raw_data,1,&gsen_buf.AXIS_X, &gsen_buf.AXIS_Y, &gsen_buf.AXIS_Z);	
        alg_results = tyhx_hrs_alg_get_results();	
        //DEBUG_PRINTF("%d %d %d %d\r\n" ,hrm_raw_data, als_raw_data, infrared_data, alg_results.hr_result_std);
        tlkapi_trace(TLKDRV_HR_HX3602_DBG_FLAG, TLKDRV_HR_HX3602_DBG_SIGN, "hx3602 -- hr = %d", alg_results.hr_result);
    }	
    #ifdef TYHX_DEMO	
        oled_dis.refresh_time++;
        if(oled_dis.refresh_time >= 25) //320ms*5 = 1600ms
        {
            oled_dis.refresh_flag = 1;
            oled_dis.refresh_time = 0;
            oled_dis.dis_mode = DIS_HR;         
            oled_dis.dis_data1 = alg_results.hr_result;
            oled_dis.dis_data2 = alg_results.living_status;
        }

        #ifdef HRS_BLE_APP
        rawdata_vector_t rawdata;
        rawdata.vector_flag = HRS_VECTOR_FLAG;
        rawdata.data_cnt = alg_results.data_cnt;
        rawdata.hr_result = alg_results.hr_result;           
        rawdata.red_raw_data = hrm_raw_data;
        rawdata.ir_raw_data = alg_results.hr_result_std;
        rawdata.gsensor_x = gsen_buf.AXIS_X;
        rawdata.gsensor_y = gsen_buf.AXIS_Y;
        rawdata.gsensor_z = gsen_buf.AXIS_Z;
        rawdata.red_cur = 0;
        rawdata.ir_cur = 0;
        ble_rawdata_vector_push(rawdata);   
        #endif 
    #endif 		

}/*void Hrs3602_Int_handle()*/


void heart_rate_meas_timeout_handler(void * p_context)
{ 
	Hrs3602_Int_handle();
}


#endif //#if (TLKDRV_HEARTRATE_HX3602_ENABLE)
