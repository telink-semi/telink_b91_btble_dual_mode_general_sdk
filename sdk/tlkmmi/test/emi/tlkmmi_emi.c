/********************************************************************************************************
 * @file     tlkmmi_emi.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../tlkmmi_testStdio.h"
#if (TLK_TEST_EMI_ENABLE)
#include "tlkmmi_emi.h"
#include "drivers.h"
#include "tlkstk/tlkstk_stdio.h"
#include "EMI/bt_emi.h"
#include "BT/rf_bt.h"
/**
 * @brief   Init the emi reference parameter
 */
unsigned char  g_power_level = 4;  // 0
unsigned char  g_chn = 2;          // 2
unsigned char  g_cmd_now = 1;      // 1
unsigned char  g_run = 1;          // 1
unsigned char  g_hop = 0;          // 0
unsigned char  g_tx_cnt = 0;       // 0
unsigned short g_pa_setting = 0;
emi_RF_MODE_BT_e  g_mode = BR_DH3; //1
extern unsigned char mode_set_nslot;
void emi_init(void);
void emicarrieronly(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_prbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emirx(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitxprbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_tx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_tx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_loop_server();
/**
 * @brief   Init the structure of the emi test command and function
 */
test_list_t  ate_list[] = {
		{0x01,emicarrieronly},
		{0x02,emi_con_prbs9},
		{0x03,emirx},
		{0x04,emitxprbs9},
		{0x05,emitx55},
		{0x06,emitx0f},
		{0x07,emi_con_tx55},
		{0x08,emi_con_tx0f}
};

extern void tlkbt_regPlicIrqClaim(plic_interrupt_claim_callback_t cb);
extern void tlkbt_set_workMode(u8 workMode);
extern void btc_enterTestMode(void);
extern int  tlkusb_init(uint16 usbID);
extern void tlkdbg_init(void);
extern void tlkusb_handler(void);

static int tlkmmi_emi_init(void);
static int tlkmmi_emi_start(void);
static int tlkmmi_emi_pause(void);
static int tlkmmi_emi_close(void);
static int tlkmmi_emi_input(uint08 msgID, uint08 *pData, uint16 dataLen);
static void tlkmmi_emi_handler(void);


const tlkmmi_testModinf_t gTlkMmiEmiModinf = 
{
	tlkmmi_emi_init, //.Init
	tlkmmi_emi_start, //.Start
	tlkmmi_emi_pause, //.Pause
	tlkmmi_emi_close, //.Close
	tlkmmi_emi_input, //.Input
	tlkmmi_emi_handler, //Handler
};



static int tlkmmi_emi_init(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_start(void)
{
//	gpio_function_en(GPIO_PE7);
//	gpio_output_en(GPIO_PE7);
//	gpio_input_dis(GPIO_PE7);
//	gpio_set_level(GPIO_PE7,0);
//	gpio_function_en(GPIO_PD0);
//	gpio_output_en(GPIO_PD0);
//	gpio_input_dis(GPIO_PD0);
//	gpio_set_level(GPIO_PD0,0);
	emi_init();
	bt_emi_init();
	return TLK_ENONE;
}
static int tlkmmi_emi_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_close(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static void tlkmmi_emi_handler(void)
{
	emi_loop_server();
}
/////////////////////////////////////////////////////

/**
 * @brief		This function serves to EMI Init
 * @return 		none
 */

void emi_init(void)
{
//	rf_access_code_comm(EMI_ACCESS_CODE);             // access code

    write_sram8(TX_PACKET_MODE_ADDR,g_tx_cnt);        // tx_cnt
    write_sram8(RUN_STATUE_ADDR,g_run);               // run
    write_sram8(TEST_COMMAND_ADDR,g_cmd_now);         // cmd
    write_sram8(POWER_ADDR,g_power_level);            // power
    write_sram8(CHANNEL_ADDR,g_chn);                  // chn
    write_sram8(RF_MODE_ADDR,g_mode);                 // mode
    write_sram8(CD_MODE_HOPPING_CHN,g_hop);           // hop
    write_sram16(PA_SETTING_ADDR, g_pa_setting);
    write_sram8(RSSI_ADDR,0);                         // rssi
    write_sram32(RX_PACKET_NUM_ADDR,0);               // rx_packet_num

}
/**
 * @brief		This function serves to EMI CarryOnly
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emicarrieronly(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	/* core_140c20 default value 0xc4, BIT(2) is 1, BIT(4) is 0, same as tbl_rf_1m
	 * for BLE: 0xc4, for BT: 0xd0
	 * core_140c21 default value 0x00 */
	REG_ADDR16(MODEM_MODE_CFG_RX1_0) |= BIT(2);
	/* default value 0x00
	 * for BLE: 0x00, for BT: 0xff */
	REG_ADDR32(BB_PDZB) = 0;

	rf_emi_stop();
	dma_reset();

	rf_emi_reset_baseband();
	(void)(rf_mode);
	bt_emi_single_tone(rf_chn, pwr);
//	gpio_set_level(GPIO_PD0,0);
//	gpio_set_level(GPIO_PE7,0);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
//		if((read_sram8(RUN_STATUE_ADDR))==0){
//			gpio_set_level(GPIO_PE7,1);
//		}else{
//			gpio_set_level(GPIO_PE7,0);
//		}
	}
	rf_emi_stop();
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI ConPrbs9
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr 	- power level of RF.
 * @param[in]   rf_chn 	- channel of RF.
 * @return 		none
 */
//volatile unsigned int debug;
volatile int BT_error_0 = 0;
void emi_con_prbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
#if 1
	unsigned int tx_cnt = 0;
//	unsigned int t0 = reg_system_tick, chnidx = 1;
//	if(g_hop)
//	{
		bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_Prbs9);
//	}
//	else
//	{
//		bt_emi_tx_continue_mode_run(rf_mode,rf_chn,pwr,BT_PKT_Data_Prbs9);
//	}
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
//		if(g_hop)
			bt_emi_tx_burst_mode_loop_hoop(&tx_cnt,mode_set_nslot);
	}
	bt_emi_stop();
#else
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_Prbs9);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
		if(g_hop)
			bt_emi_tx_burst_mode_loop_hoop(&tx_cnt,mode_set_nslot);
	}
	bt_emi_stop();
#endif
}

/**
 * @brief		This function serves to EMI Rx
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr	    - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emirx(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	(void)(pwr);
	unsigned int rx_cnt = 0;
	bt_emi_rx_mode_init(rf_mode, rf_chn);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)  && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
		bt_emi_rx_mode_loop(&rx_cnt);
		if(rx_cnt != read_sram32(RX_PACKET_NUM_ADDR))
		{
		    write_sram8(RSSI_ADDR,(unsigned char)bt_get_rssi(0));
		    write_sram32(RX_PACKET_NUM_ADDR,rx_cnt);
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI TxPrbs
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emitxprbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_Prbs9);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
		bt_emi_tx_burst_mode_loop(&tx_cnt, mode_set_nslot);
		if(g_tx_cnt)
		{
			if(tx_cnt >= 1000)
				break;
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI Tx55
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emitx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_0x55);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
		bt_emi_tx_burst_mode_loop(&tx_cnt, mode_set_nslot);
		if(g_tx_cnt)
		{
			if(tx_cnt >= 1000)
				break;
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI Tx0f
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emitx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_0x0f);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting))
	{
		bt_emi_tx_burst_mode_loop(&tx_cnt, mode_set_nslot);
		if(g_tx_cnt)
		{

			if(tx_cnt >= 1000)
				break;
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI_CON_TX55
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emi_con_tx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	bt_emi_tx_continue_mode_run(rf_mode,rf_chn,pwr,BT_PKT_Data_0x55);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting));
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI_CON_TX0f
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emi_con_tx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	bt_emi_tx_continue_mode_run(rf_mode,rf_chn,pwr,BT_PKT_Data_0x0f);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)  && ((read_sram16(PA_SETTING_ADDR)) == g_pa_setting));
	bt_emi_stop();
}
void emi_loop_server(){
	   g_run = read_sram8(RUN_STATUE_ADDR);  // get the run state!
	   if(g_run != 0)
	   {
		   g_power_level = read_sram8(POWER_ADDR);
		   g_chn = read_sram8(CHANNEL_ADDR);
		   g_mode = read_sram8(RF_MODE_ADDR);
		   g_cmd_now = read_sram8(TEST_COMMAND_ADDR);  // get the command!
		   g_tx_cnt = read_sram8(TX_PACKET_MODE_ADDR);
		   g_hop = read_sram8(CD_MODE_HOPPING_CHN);
		   g_pa_setting = read_sram16(PA_SETTING_ADDR);
		   for (int i = 0;i < 8;i++)
		   {
				if(g_cmd_now == ate_list[i].cmd_id)
				{
					switch(g_mode)
					{
					  case BR_DH1:
						  ate_list[i].func(BT_MODE_BR_DH1,g_power_level,g_chn);
						  break;
					  case EDR2_DH1:
						  ate_list[i].func(BT_MODE_EDR2_DH1,g_power_level,g_chn);
						  break;
					  case EDR3_DH1:
						  ate_list[i].func(BT_MODE_EDR3_DH1,g_power_level,g_chn);
						  break;
					  case BR_DH3:
						  ate_list[i].func(BT_MODE_BR_DH3,g_power_level,g_chn);
						  break;
					  case EDR2_DH3:
						  ate_list[i].func(BT_MODE_EDR2_DH3,g_power_level,g_chn);
						  break;
					  case EDR3_DH3:
						  ate_list[i].func(BT_MODE_EDR3_DH3,g_power_level,g_chn);
						  break;
					  case BR_DH5:
						  ate_list[i].func(BT_MODE_BR_DH5,g_power_level,g_chn);
						  break;
					  case EDR2_DH5:
						  ate_list[i].func(BT_MODE_EDR2_DH5,g_power_level,g_chn);
						  break;
					  case EDR3_DH5:
						  ate_list[i].func(BT_MODE_EDR3_DH5,g_power_level,g_chn);
						  break;

					  default:break;
					}
				}
		   }
		   g_run = 0;
		   write_sram8(RUN_STATUE_ADDR,g_run);
	   }
}

#endif //#if (TLK_TEST_EMI_ENABLE)

#endif //#if (TLKMMI_TEST_ENABLE)

