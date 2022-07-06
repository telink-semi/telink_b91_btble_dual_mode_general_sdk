/********************************************************************************************************
 * @file     tlkdev_bat.c
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
#include "tlkdev/tlkdev_stdio.h"
#if (TLKDEV_CFG_BAT_ENABLE)
#include "tlkdev/sys/tlkdev_bat.h"
#include "drivers.h"


extern unsigned short g_adc_vref;
extern unsigned char g_adc_pre_scale;
extern unsigned char g_adc_vbat_divider;



int tlkdev_bat_init(uint32 gpio, uint32 adcGpio)
{
	if(gpio != 0 && adcGpio == 0) return -TLK_EPARAM;
	
	if(gpio != 0){
		gpio_function_en(gpio);
		gpio_output_dis(gpio);
		gpio_input_dis(gpio);
	}

	/******power off sar adc********/
	adc_power_off();
	
	//reset whole digital adc module
	adc_reset();
	
	/******set adc sample clk as 4MHz******/
	adc_clk_en();//enable signal of 24M clock to sar adc
	adc_set_clk(5);//default adc_clk 4M = 24M/(1+div),
	
	//set misc channel vref 1.2V
	analog_write_reg8(areg_adc_vref, ADC_VREF_1P2V);
	analog_write_reg8(areg_ain_scale, (analog_read_reg8(areg_ain_scale)&(0xC0)) | 0x3d);
	g_adc_vref = 1175;

	if(gpio == 0){
		//set Analog input pre-scaling,ADC_PRESCALE_1
		analog_write_reg8(areg_ain_scale  , (analog_read_reg8(areg_ain_scale)&(~FLD_SEL_AIN_SCALE)) | (ADC_PRESCALE_1<<6));
		g_adc_pre_scale = 1<<(unsigned char)ADC_PRESCALE_1;
	}else{
		//set Analog input pre-scaling,ADC_PRESCALE_1F4
		analog_write_reg8(areg_ain_scale  , (analog_read_reg8(areg_ain_scale)&(~FLD_SEL_AIN_SCALE)) | (ADC_PRESCALE_1F4<<6));
		g_adc_pre_scale = 1<<(unsigned char)ADC_PRESCALE_1F4;
	}
	
	//set sample frequency.96k
	adc_set_state_length(240, 10);

	//default adc_resolution set as 14bit ,BIT(13) is sign bit
	adc_set_resolution(ADC_RES14);

	//set misc t_sample 6 cycle of adc clock:  6 * 1/4M
	adc_set_tsample_cycle(ADC_SAMPLE_CYC_6);

	//enable adc channel.
	adc_set_m_chn_en();
	
	if(gpio == 0){ ////vbat mode, vbat channel
		//set vbat divider : ADC_VBAT_DIV_1F3
		analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (ADC_VBAT_DIV_1F3<<2));
		g_adc_vbat_divider = 5-ADC_VBAT_DIV_1F3;
		adc_set_diff_input(ADC_VBAT, GND);
	}else{ ////base mode, gpio channel
		//set vbat divider : ADC_VBAT_DIV_OFF
		analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (ADC_VBAT_DIV_OFF<<2));
		g_adc_vbat_divider = 1;
		adc_set_diff_input(adcGpio>>12, GND);
	}
	
	/******power on sar adc********/
	//note: this setting must be set after all other settings
	adc_power_on();
	
	//wait at least 2 sample cycle(f = 96K, T = 10.4us)
	sleep_us(25);

	return TLK_ENONE;
}

int tlkdev_bat_getVoltage(uint16 *pVoltage)
{
	uint16 adc_misc_data;
	uint32 adc_result;
	uint32 adc_average;
	uint08 ana_read_f3;

	if(pVoltage == nullptr) return -TLK_EPARAM;
	
	//when MCU powered up or wakeup from deep/deep with retention, adc need be initialized
	
	//Note:25us should be reserved between each reading(wait at least 2 sample cycle(f = 96K, T = 10.4us)).
	//The sdk is only sampled once, and the user can redesign the filtering algorithm according to the actual application.
	ana_read_f3 = analog_read_reg8(areg_adc_data_sample_control);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 | FLD_NOT_SAMPLE_ADC_DATA);
	adc_misc_data = analog_read_reg16(areg_adc_misc_l);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 & (~FLD_NOT_SAMPLE_ADC_DATA));


	if(adc_misc_data & BIT(13)){
		adc_misc_data=0;
		return -TLK_EFAIL;
	}
	
	adc_average = adc_misc_data;
	adc_result = adc_average;
	
	/**
	 * move the "2 sample cycle" wait operation before adc_get_code(),
	 * otherwise may have data lose due to no waiting when adc_power_on.
	 * changed by chaofan.20201230.
	 */
	sleep_us(21);//wait at least 2 sample cycle(f = 96K, T = 10.4us)
	
	*pVoltage = adc_calculate_voltage(adc_result);
	return TLK_ENONE;
}


#if 0

_attribute_ram_code_ void adc_bat_detect_init(void)
{

	/******power off sar adc********/
	adc_power_off();

	//reset whole digital adc module
	adc_reset();

	/******set adc sample clk as 4MHz******/
	adc_clk_en();//enable signal of 24M clock to sar adc
	adc_set_clk(5);//default adc_clk 4M = 24M/(1+div),

	//set misc channel vref 1.2V
	analog_write_reg8(areg_adc_vref, ADC_VREF_1P2V);
	analog_write_reg8(areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(0xC0)) | 0x3d );
	g_adc_vref = 1175;

#if VBAT_CHANNEL_EN//vbat mode, vbat channel
	//set Analog input pre-scaling,ADC_PRESCALE_1F4
	analog_write_reg8( areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(~FLD_SEL_AIN_SCALE)) | (ADC_PRESCALE_1<<6) );
	g_adc_pre_scale = 1<<(unsigned char)ADC_PRESCALE_1;
#else
	//set Analog input pre-scaling,ADC_PRESCALE_1F4
	analog_write_reg8( areg_ain_scale  , (analog_read_reg8( areg_ain_scale  )&(~FLD_SEL_AIN_SCALE)) | (ADC_PRESCALE_1F4<<6) );
	g_adc_pre_scale = 1<<(unsigned char)ADC_PRESCALE_1F4;
#endif

	//set sample frequency.96k
	adc_set_state_length(240, 10);

	//default adc_resolution set as 14bit ,BIT(13) is sign bit
	adc_set_resolution(ADC_RES14);

	//set misc t_sample 6 cycle of adc clock:  6 * 1/4M
	adc_set_tsample_cycle(ADC_SAMPLE_CYC_6);

	//enable adc channel.
	adc_set_m_chn_en();

#if VBAT_CHANNEL_EN//vbat mode, vbat channel
	//set vbat divider : ADC_VBAT_DIV_1F3
	analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (ADC_VBAT_DIV_1F3<<2) );
	g_adc_vbat_divider = 5-ADC_VBAT_DIV_1F3;

	adc_set_diff_input(ADC_VBAT, GND);
#else//base mode, gpio channel
	//set vbat divider : ADC_VBAT_DIV_OFF
	analog_write_reg8(areg_adc_vref_vbat_div, (analog_read_reg8(areg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (ADC_VBAT_DIV_OFF<<2) );
	g_adc_vbat_divider = 1;

	adc_set_diff_input(ADC_INPUT_PIN_CHN>>12, GND);
#endif
	/******power on sar adc********/
	//note: this setting must be set after all other settings
	adc_power_on();

	//wait at least 2 sample cycle(f = 96K, T = 10.4us)
	sleep_us(25);
}


_attribute_data_retention_  u16     batt_vol_mv;


#define USED_DEEP_ANA_REG                   DEEP_ANA_REG1


#if (BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation

_attribute_data_retention_	u32	lowBattDet_tick   = 0;

_attribute_ram_code_ void user_init_battery_power_check(void)
{
	/*****************************************************************************************
	 Note: battery check must do before any flash write/erase operation, cause flash write/erase
		   under a low or unstable power supply will lead to error flash operation

		   Some module initialization may involve flash write/erase, include: OTA initialization,
				SMP initialization, ..
				So these initialization must be done after  battery check
	*****************************************************************************************/

		#if(BAT_LEAKAGE_PROTECT_EN)
		do{
			u8 analog_deep = analog_read_reg8(USED_DEEP_ANA_REG);
			u16 bat_deep_thres = BAT_DEEP_THRES_MV;
			u16 bat_suspend_thres = BAT_SUSPEND_THRES_MV;
			if(analog_deep & LOW_BATT_FLG){
				if(analog_deep & LOW_BATT_SUSPEND_FLG){//<1.8v
					bat_deep_thres += 200;
					bat_suspend_thres += 100;
				}
				else{//1.8--2.0v
					bat_deep_thres += 200;
				}
			}
			app_battery_power_check(bat_deep_thres,bat_suspend_thres);

			wd_clear(); //clear watch dog

			if(analog_deep & LOW_BATT_SUSPEND_FLG){
				sleep_us(100000);
			}
		}while(analog_read_reg8(USED_DEEP_ANA_REG) & LOW_BATT_SUSPEND_FLG);
		#else
			if(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG){
				app_battery_power_check(BAT_DEEP_THRES_MV + 200);
			}
			else{
				app_battery_power_check(BAT_DEEP_THRES_MV);
			}
		#endif


}

#endif


#if (BAT_LEAKAGE_PROTECT_EN)
_attribute_ram_code_ void app_battery_power_check(u16 threshold_deep_vol_mv, u16 threshold_suspend_vol_mv)
{


	//when MCU powered up or wakeup from deep/deep with retention, adc need be initialized
	if(!adc_hw_initialized){
		adc_hw_initialized = 1;
		adc_bat_detect_init();
	}
	//Note:25us should be reserved between each reading(wait at least 2 sample cycle(f = 96K, T = 10.4us)).
	//The sdk is only sampled once, and the user can redesign the filtering algorithm according to the actual application.
	unsigned short adc_misc_data;
	u32 adc_result;
	u32 adc_average;
	u8 ana_read_f3 = analog_read_reg8(areg_adc_data_sample_control);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 | FLD_NOT_SAMPLE_ADC_DATA);
	adc_misc_data = analog_read_reg16(areg_adc_misc_l);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 & (~FLD_NOT_SAMPLE_ADC_DATA));


	if(adc_misc_data & BIT(13)){
		adc_misc_data=0;
		return;
	}

	adc_average = adc_misc_data;
	adc_result = adc_average;
////////////////// adc sample data convert to voltage(mv) ////////////////
	//(adc_result * g_adc_vref * g_adc_vbat_divider * g_adc_pre_scale) >> 13
#if VBAT_CHANNEL_EN
//	batt_vol_mv  = (adc_result * 1175 * 3)>>13;
	batt_vol_mv  = adc_calculate_voltage(adc_result);
#else
//	batt_vol_mv  = (adc_result * 1175)>>10;
	batt_vol_mv  = adc_calculate_voltage(adc_result);
#endif

	u8 analog_used_deep_reg = analog_read_reg8(USED_DEEP_ANA_REG);
	if(batt_vol_mv < threshold_deep_vol_mv){
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High, 1);  //drive pin pad high wakeup deepsleep
		}

		if(batt_vol_mv <= threshold_suspend_vol_mv)//  bat_vol<1.8v  enter suspend
		{
			analog_write_reg8(USED_DEEP_ANA_REG,  analog_used_deep_reg | LOW_BATT_FLG | LOW_BATT_SUSPEND_FLG);  //mark
			cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_PAD, 0);  //suspend
		}
		else  //1.8v<bat_vol<2.0v  enter deep sleep
		{
			analog_write_reg8(USED_DEEP_ANA_REG,  analog_used_deep_reg | (LOW_BATT_FLG & (~LOW_BATT_SUSPEND_FLG)));  //mark
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
		}
	}
	else
	{
		analog_write_reg8(USED_DEEP_ANA_REG,  analog_used_deep_reg & (~(LOW_BATT_FLG | LOW_BATT_SUSPEND_FLG)));  //mark
	}

}
#else
_attribute_ram_code_ void app_battery_power_check(u16 alram_vol_mv)
{
	u16 temp;
	int i,j;
	
	//Note:25us should be reserved between each reading(wait at least 2 sample cycle(f = 96K, T = 10.4us)).
	//The sdk is only sampled once, and the user can redesign the filtering algorithm according to the actual application.
	unsigned short adc_misc_data;
	u32 adc_result;
	u32 adc_average;
	u8 ana_read_f3 = analog_read_reg8(areg_adc_data_sample_control);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 | FLD_NOT_SAMPLE_ADC_DATA);
	adc_misc_data = analog_read_reg16(areg_adc_misc_l);
	analog_write_reg8(areg_adc_data_sample_control, ana_read_f3 & (~FLD_NOT_SAMPLE_ADC_DATA));


	if(adc_misc_data & BIT(13)){
		adc_misc_data=0;
		return;
	}

	adc_average = adc_misc_data;
	adc_result = adc_average;
////////////////// adc sample data convert to voltage(mv) ////////////////
	//(adc_result * g_adc_vref * g_adc_vbat_divider * g_adc_pre_scale) >> 13
#if VBAT_CHANNEL_EN
	batt_vol_mv  = (adc_result * 1175 * 3)>>13;
#else
	batt_vol_mv  = (adc_result * 1175)>>10;
#endif

	if(batt_vol_mv < alram_vol_mv){

		analog_write_reg8(USED_DEEP_ANA_REG,  analog_read_reg8(USED_DEEP_ANA_REG) | LOW_BATT_FLG);  //mark

		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High, 1);  //drive pin pad high wakeup deepsleep
		}

		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
	}

}
#endif //#if (BAT_LEAKAGE_PROTECT_EN)

#endif


#endif //TLKDEV_CFG_BAT_ENABLE

