/********************************************************************************************************
 * @file     tlkdrv_rtl2108.c
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
#include "tlkdrv_codec.h"
#if (TLKDRV_CODEC_RTL2108_ENABLE)
#include "tlkdrv_rtl2108.h"
#include "drivers.h"


#define TLKDRV_RTL2108_SERIAL_PORT               UART0
#define TLKDRV_RTL2108_SERIAL_TX_PIN             UART0_TX_PD2
#define TLKDRV_RTL2108_SERIAL_RX_PIN             UART0_RX_PD3

extern void audio_codec_adc_power_down(void);
extern void audio_i2s_set_pin(void);
extern void aduio_set_chn_wl(audio_channel_wl_mode_e chn_wl);
extern void audio_tx_dma_chain_init (dma_chn_e chn,unsigned short * out_buff,unsigned int buff_size);
extern void audio_tx_dma_chain_init_abort (dma_chn_e chn,unsigned short * out_buff,unsigned int buff_size);
extern void audio_rx_dma_chain_init_abort (dma_chn_e chn,unsigned short * in_buff, unsigned int buff_size);

static uint16 tlkdrv_rtl2108_calcCrc16(uint08 *pData, uint16 dataLen);

static bool tlkdrv_rtl2108_isOpen(uint08 subDev);
static int tlkdrv_rtl2108_init(uint08 subDev);
static int tlkdrv_rtl2108_open(uint08 subDev);
static int tlkdrv_rtl2108_pause(uint08 subDev);
static int tlkdrv_rtl2108_close(uint08 subDev);
static int tlkdrv_rtl2108_reset(uint08 subDev);
static int tlkdrv_rtl2108_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1);

static int tlkdrv_rtl2108_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_rtl2108_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);


static void tlkdrv_rtl2108_gpioInit(void);
static void tlkdrv_rtl2108_uartInit(void);
static int  tlkdrv_rtl2108_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk);
static void tlkdrv_rtl2108_disable(void);


extern uint16  gTlkDrvCodecSpkBuffLen;
extern uint16  gTlkDrvCodecMicBuffLen;
extern uint08 *gpTlkDrvCodecSpkBuffer;
extern uint08 *gpTlkDrvCodecMicBuffer;
extern aduio_i2s_codec_config_t audio_i2s_codec_config;


const tlkdrv_codec_modinf_t gcTlkDrvRtl2108Inf = {
	tlkdrv_rtl2108_isOpen, //IsOpen
	tlkdrv_rtl2108_init, //Init
	tlkdrv_rtl2108_open, //Open
	tlkdrv_rtl2108_pause, //Pause
	tlkdrv_rtl2108_close, //Close
	tlkdrv_rtl2108_reset, //Reset
	tlkdrv_rtl2108_config, //Config
};
static tlkdrv_rtl2108_t sTlkDrvRtl2108Ctrl = {0};

static bool tlkdrv_rtl2108_isOpen(uint08 subDev)
{
	if(!sTlkDrvRtl2108Ctrl.isInit || !sTlkDrvRtl2108Ctrl.isOpen) return false;
	if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		if(sTlkDrvRtl2108Ctrl.micIsEn) return true;
		else return false;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		if(sTlkDrvRtl2108Ctrl.spkIsEn) return true;
		else return false;
	}else{
		return true;
	}
}
static int tlkdrv_rtl2108_init(uint08 subDev)
{
	tmemset(&sTlkDrvRtl2108Ctrl, 0, sizeof(tlkdrv_rtl2108_t));

	sTlkDrvRtl2108Ctrl.isInit = true;
	sTlkDrvRtl2108Ctrl.isOpen = false;
	sTlkDrvRtl2108Ctrl.isMute = false;
	sTlkDrvRtl2108Ctrl.micVol = 60;
	sTlkDrvRtl2108Ctrl.bitDepth = 16;
	sTlkDrvRtl2108Ctrl.sampleRate = 48000;
	
	tlkdrv_rtl2108_gpioInit();
	delay_ms(1000);
	tlkdrv_rtl2108_uartInit();
	
	tlkdrv_rtl2108_setMicEnable();
	
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_open(uint08 subDev)
{
	if(!sTlkDrvRtl2108Ctrl.isInit) return -TLK_ESTATUS;
	if(sTlkDrvRtl2108Ctrl.isOpen) return -TLK_EREPEAT;

	tlkdrv_rtl2108_setMicMuteStatus(sTlkDrvRtl2108Ctrl.isMute);
	tlkdrv_rtl2108_setMicGain(sTlkDrvRtl2108Ctrl.micVol);
	tlkdrv_rtl2108_setMicEnable();
	
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0) sTlkDrvRtl2108Ctrl.micIsEn = true;
	else sTlkDrvRtl2108Ctrl.micIsEn = false;
	if((subDev & TLKDRV_CODEC_SUBDEV_SPK) != 0) sTlkDrvRtl2108Ctrl.spkIsEn = true;
	else sTlkDrvRtl2108Ctrl.spkIsEn = false;
	tlkdrv_rtl2108_enable(sTlkDrvRtl2108Ctrl.bitDepth, sTlkDrvRtl2108Ctrl.channel,
		sTlkDrvRtl2108Ctrl.sampleRate, sTlkDrvRtl2108Ctrl.micIsEn, sTlkDrvRtl2108Ctrl.spkIsEn);
	
	sTlkDrvRtl2108Ctrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_pause(uint08 subDev)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_rtl2108_close(uint08 subDev)
{
	if(!sTlkDrvRtl2108Ctrl.isInit || !sTlkDrvRtl2108Ctrl.isOpen) return -TLK_ESTATUS;
	
	tlkdrv_rtl2108_disable();
	
	sTlkDrvRtl2108Ctrl.isOpen = false;
	sTlkDrvRtl2108Ctrl.micIsEn = false;
	sTlkDrvRtl2108Ctrl.spkIsEn = false;
	
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_reset(uint08 subDev)
{
	bool isOpen = sTlkDrvRtl2108Ctrl.isOpen;
	
	tlkdrv_rtl2108_factoryReset();
	delay_ms(200);
	
	if(isOpen) tlkdrv_rtl2108_close(subDev);
	if(isOpen) tlkdrv_rtl2108_open(subDev);
	
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	int ret = -TLK_ENOSUPPORT;
	switch(opcode){
		case TLKDRV_CODEC_OPCODE_SET_MUTE:  //param:[uint08]isMute
			ret = tlkdrv_rtl2108_setMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_MUTE:
			ret = tlkdrv_rtl2108_getMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_VOLUME: //param:[uint08]volume
			ret = tlkdrv_rtl2108_setVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_VOLUME:
			ret = tlkdrv_rtl2108_getVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_CHANNEL: //param:[uint08]chnCnt-1/2/3
			ret = tlkdrv_rtl2108_setChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_CHANNEL:
			ret = tlkdrv_rtl2108_getChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH: //param:[uint08]bitDepth-8,16,20,24,32
			ret = tlkdrv_rtl2108_setBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_BIT_DEPTH:
			ret = tlkdrv_rtl2108_getBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE: //param:[uint32]bitDepth-8,16,20,24,32
			ret = tlkdrv_rtl2108_setSampleRateDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE:
			ret = tlkdrv_rtl2108_getSampleRateDeal(subDev, param0, param1);
			break;
	}
	return ret;
}

static int tlkdrv_rtl2108_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 mute = param0 & 0xFF;
	if(mute != 0x00 && mute != 0x01) return -TLK_EPARAM;
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	tlkdrv_rtl2108_setMicMuteStatus(mute);
	sTlkDrvRtl2108Ctrl.isMute = mute;
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	return sTlkDrvRtl2108Ctrl.isMute;
}
static int tlkdrv_rtl2108_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 micVol = param0 & 0xFF;
	if(micVol > 100) return -TLK_EPARAM;
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	tlkdrv_rtl2108_setMicGain(micVol);
	sTlkDrvRtl2108Ctrl.micVol = micVol;
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	return sTlkDrvRtl2108Ctrl.micVol;
}
static int tlkdrv_rtl2108_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 channel = param0 & 0xFF;
	if(channel != 0x01 && channel != 0x02 && channel != 0x03) return -TLK_EPARAM;
	sTlkDrvRtl2108Ctrl.channel = channel;
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvRtl2108Ctrl.channel;
}
static int tlkdrv_rtl2108_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 bitDapth = param0 & 0xFF;
	if(bitDapth != 8 && bitDapth != 16 && bitDapth != 20 && bitDapth != 24 && bitDapth != 32){
		return -TLK_EPARAM;
	}
	if(bitDapth != 16) return -TLK_ENOSUPPORT;
	sTlkDrvRtl2108Ctrl.bitDepth = 16;
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvRtl2108Ctrl.bitDepth;
}
static int tlkdrv_rtl2108_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint32 sampleRate = param0;
	if(sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000 && sampleRate != 44100 && sampleRate != 48000){
		return -TLK_EPARAM;
	}
	if(sampleRate != 48000) return -TLK_ENOSUPPORT;
	sTlkDrvRtl2108Ctrl.sampleRate = 48000;
	return TLK_ENONE;
}
static int tlkdrv_rtl2108_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvRtl2108Ctrl.sampleRate;
}


static int tlkdrv_rtl2108_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk)
{
	int rateIndex;

	if(bitDepth != 16 && bitDepth != 24 && channel != 0x01 && channel != 0x02 && channel != 0x03){
		return -TLK_ENOSUPPORT;
	}
	if(bitDepth == 24){
		if(channel == 0x03) bitDepth = STEREO_BIT_24;
		else bitDepth = MONO_BIT_24;
	}else{
		if(channel == 0x03) bitDepth = STEREO_BIT_16;
		else bitDepth = MONO_BIT_16;
	}
	
	audio_reset();
	codec_reset();

	audio_tx_dma_dis();
	audio_rx_dma_dis();

	audio_set_codec_supply(CODEC_2P8V);
	
	rateIndex = audio_sample_rate_to_index(sampleRate);

	if(enSpk && gpTlkDrvCodecSpkBuffer != nullptr && gTlkDrvCodecSpkBuffLen != 0){
		audio_tx_dma_chain_init_abort(TLKDRV_CODEC_SPK_DMA, (uint16*)gpTlkDrvCodecSpkBuffer, gTlkDrvCodecSpkBuffLen);
	}
	if(enMic && gpTlkDrvCodecMicBuffer != nullptr && gTlkDrvCodecMicBuffLen != 0){
		audio_rx_dma_chain_init_abort(TLKDRV_CODEC_MIC_DMA, (uint16*)gpTlkDrvCodecMicBuffer, gTlkDrvCodecMicBuffLen);
	}

	audio_i2s_set_pin();
    aduio_set_chn_wl(bitDepth);
    
    audio_mux_config(IO_I2S, audio_i2s_codec_config.audio_in_mode, audio_i2s_codec_config.audio_in_mode, audio_i2s_codec_config.audio_out_mode);
    audio_i2s_config(I2S_I2S_MODE,I2S_BIT_16_DATA, I2S_M_CODEC_S, audio_i2s_codec_config.i2s_data_invert_select);
    audio_set_i2s_clock(rateIndex, AUDIO_RATE_EQUAL,0);
    audio_clk_en(1, 1);
	
	audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO, I2S_OUT);
	
	audio_set_codec_adc_wnf(CODEC_ADC_WNF_INACTIVE);
	if(enMic){
		audio_codec_adc_enable(1);
	}else{
		audio_codec_adc_enable(0);
	}
	
	return TLK_ENONE;
}
static void tlkdrv_rtl2108_disable(void)
{
	audio_codec_pwr_down();
	audio_codec_adc_power_down();
	audio_codec_dac_power_down();
	audio_codec_adc_enable(0);

	audio_reset();
	codec_reset();

	audio_tx_dma_dis();
	audio_rx_dma_dis();
	
	audio_set_codec_supply(CODEC_1P8V);
}




static void tlkdrv_rtl2108_gpioInit(void)
{
	gpio_function_en(TLKDRV_RTL2108_GPIO_SYS);
	gpio_output_en(TLKDRV_RTL2108_GPIO_SYS);
	gpio_set_output_en(TLKDRV_RTL2108_GPIO_SYS , 1);
	gpio_write(TLKDRV_RTL2108_GPIO_SYS, 1);

	gpio_function_en(TLKDRV_RTL2108_GPIO_RESET);
	gpio_output_en(TLKDRV_RTL2108_GPIO_RESET);
	gpio_set_output_en(TLKDRV_RTL2108_GPIO_RESET, 0);
	gpio_write(TLKDRV_RTL2108_GPIO_RESET, 0);

	gpio_function_en(TLKDRV_RTL2108_GPIO_I2S_READY);
	gpio_output_en(TLKDRV_RTL2108_GPIO_I2S_READY);
	gpio_set_output_en(TLKDRV_RTL2108_GPIO_I2S_READY, 1);
	gpio_write(TLKDRV_RTL2108_GPIO_I2S_READY, 1);
	
	gpio_set_input_en(TLKDRV_RTL2108_GPIO_INIT_READY, 1);
	gpio_output_dis(TLKDRV_RTL2108_GPIO_INIT_READY);
	gpio_setup_up_down_resistor(TLKDRV_RTL2108_GPIO_INIT_READY, PM_PIN_UP_DOWN_FLOAT);
	
	gpio_set_input_en(TLKDRV_RTL2108_GPIO_POWER_SWITCH, 1);
	gpio_output_dis(TLKDRV_RTL2108_GPIO_POWER_SWITCH);
	gpio_setup_up_down_resistor(TLKDRV_RTL2108_GPIO_POWER_SWITCH, PM_PIN_UP_DOWN_FLOAT);
}
static void tlkdrv_rtl2108_uartInit(void)
{
	unsigned short div;
	unsigned char bwpc;
	
	uart_reset(TLKDRV_RTL2108_SERIAL_PORT);
	uart_set_pin(TLKDRV_RTL2108_SERIAL_TX_PIN, TLKDRV_RTL2108_SERIAL_RX_PIN);
	uart_cal_div_and_bwpc(921600, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(TLKDRV_RTL2108_SERIAL_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);
	
	/* UART RX enable */
//	uart_rx_irq_trig_level(TLKDRV_RTL2108_SERIAL_PORT, 1);
//	plic_interrupt_enable(IRQ19_UART0);
//	uart_set_irq_mask(TLKDRV_RTL2108_SERIAL_PORT, UART_RX_IRQ_MASK | UART_ERR_IRQ_MASK);
//	plic_set_priority(IRQ19_UART0, IRQ_PRI_LEV3);
}


//_attribute_ram_code_sec_ 
//void uart0_irq_handler(void)
//{
//	if(uart_get_irq_status(TLKDRV_RTL2108_SERIAL_PORT, UART_RXBUF_IRQ_STATUS)){
//		while(uart_get_rxfifo_num(TLKDRV_RTL2108_SERIAL_PORT) > 0){
//			uint08 rxData = reg_uart_data_buf(TLKDRV_RTL2108_SERIAL_PORT, uart_rx_byte_index[TLKDRV_RTL2108_SERIAL_PORT]) ;
//			uart_rx_byte_index[TLKDRV_RTL2108_SERIAL_PORT] ++;
//			uart_rx_byte_index[TLKDRV_RTL2108_SERIAL_PORT] &= 0x03;
//			tlkapi_fifo_writeByte(&sTlkApiRecvFifo, rxData);
//			tlkapi_sendData(1, "rv:", &rxData, 1);
//		}
//	}
//	if(uart_get_irq_status(TLKDRV_RTL2108_SERIAL_PORT, UART_RX_ERR)){
//		uart_clr_irq_status(TLKDRV_RTL2108_SERIAL_PORT, UART_CLR_RX);
//		// it will clear rx_fifo and rx_err_irq ,rx_buff_irq,so it won't enter rx_buff interrupt.
//		uart_reset(TLKDRV_RTL2108_SERIAL_PORT); //clear hardware pointer
//		uart_clr_rx_index(TLKDRV_RTL2108_SERIAL_PORT); //clear software pointer
//	}
//}






void tlkdrv_rtl2108_getFwVersion(void)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_GET_VERSION; //Command ID
	buffer[dataLen++] = 0x00; //Payload Length
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);	
}
void tlkdrv_rtl2108_setCurrentState(uint08 state)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_GET_CURRENT_STATE; //Command ID
	buffer[dataLen++] = 0x01; //Payload Length
	buffer[dataLen++] = state;
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}
void tlkdrv_rtl2108_setMicGain(uint08 volume)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_SET_MICGAIN; //Command ID
	buffer[dataLen++] = 0x01; //Payload Length
	buffer[dataLen++] = volume; 
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);	
}
void tlkdrv_rtl2108_getMicGain(void)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_GET_MICGAIN; //Command ID
	buffer[dataLen++] = 0x00; //Payload Length
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}
void tlkdrv_rtl2108_setMicMuteStatus(uint08 status)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_SET_MICMUTE; //Command ID
	buffer[dataLen++] = 0x01; //Payload Length
	buffer[dataLen++] = status;
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}
void tlkdrv_rtl2108_getMicMuteStatus(void)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_GET_MICMUTE; //Command ID
	buffer[dataLen++] = 0x00; //Payload Length
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}

void tlkdrv_rtl2108_setMusicEq(uint08 eqType)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_SET_MUSICEQ; //Command ID
	buffer[dataLen++] = 0x01; //Payload Length
	buffer[dataLen++] = eqType;
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}
void tlkdrv_rtl2108_getMusicEq(void)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_GET_MUSICEQ; //Command ID
	buffer[dataLen++] = 0x00; //Payload Length
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}

void tlkdrv_rtl2108_setCallStatus(uint08 status)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_SET_CALL_STATE; //Command ID
	buffer[dataLen++] = 0x01; //Payload Length
	buffer[dataLen++] = status;
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}
void tlkdrv_rtl2108_setMicEnable(void)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_SET_MIC_ENABLE; //Command ID
	buffer[dataLen++] = 0x01; //Payload Length
	buffer[dataLen++] = 0x01;
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}

void tlkdrv_rtl2108_factoryReset(void)
{
	uint16 crcVal;
	uint08 dataLen;
	uint08 buffer[16];
	
	dataLen = 0;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER1; //Header
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_HEADER2;
	buffer[dataLen++] = TLKDRV_RTL2108_CMD_FACTORY_RESET; //Command ID
	buffer[dataLen++] = 0x00; //Payload Length
	crcVal = tlkdrv_rtl2108_calcCrc16(buffer, dataLen);
	buffer[dataLen++] = (crcVal >> 0) & 0xff; //CRC
	buffer[dataLen++] = (crcVal >> 8) & 0xff;
	uart_send(TLKDRV_RTL2108_SERIAL_PORT, buffer, dataLen);
}

static uint16 tlkdrv_rtl2108_calcCrc16(uint08 *pData, uint16 dataLen)
{
	uint16 crc = 0xFFFF;
	if(pData != nullptr){
		uint16 index;
		for(index = 0; index < dataLen; index++){
			crc = (uint8_t)(crc >> 8) | (crc << 8);
			crc ^= pData[index];
			crc ^= (uint8_t)(crc & 0xFF) >> 4;
			crc ^= (crc << 8) << 4;
			crc ^= ((crc & 0xFF) << 4) << 1;
		}
	}
	return crc;
}



#endif //#if (TLKDRV_CODEC_RTL2108_ENABLE)

