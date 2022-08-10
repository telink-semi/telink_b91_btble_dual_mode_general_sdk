/********************************************************************************************************
 * @file     tlkdev_xtsd04g.c
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
#if (TLK_DEV_XTSD01G_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xtsd01g.h"

#include "drivers.h"



#define TLKDEV_XTSD01G_DEBUG_ENABLE      (1 && TLKDEV_CFG_DBG_ENABLE)

#define TLKDEV_XTSD01G_DBG_FLAG         (TLKDEV_XTSD01G_DEBUG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKDEV_XTSD01G_DBG_SIGN         "[XTSD04G]"


#define TLKDEV_XTSD01G_IRQ_DISABLE()     uint32 rie = core_disable_interrupt()
#define TLKDEV_XTSD01G_IRQ_RESTORE()     core_restore_interrupt(rie)

extern void pspi_set_pin_mux(pspi_pin_def_e pin);


static void tlkdev_xtsd04g_gpioConfig(void);

static void tlkdev_xtsd01g_spiCsLow(void);
static void tlkdev_xtsd01g_spiCsHigh(void);

static uint08 tlkdev_xtsd04g_spiSelect(void);
static void   tlkdev_xtsd04g_spiDisSelect(void);

static uint08 tlkdev_xtsd04g_spiGetResponse(uint08 Response);
static uint08 tlkdev_xtsd04g_spiRecvData(uint08 *pBuff, uint16 buffLen);
static uint08 tlkdev_xtsd04g_spiSendData(uint08 *pData, uint08 dataCmd);

//static void tlkdev_xtsd01g_switchLowSpeed(void);
static void tlkdev_xtsd01g_switchHighSpeed(void);

static uint08 tlkdev_xtsd04g_sendCmd(uint08 cmd, uint32 arg, uint08 crc);


static uint08 tlkdev_xtsd01g_waitReady(void);

static void tlkdev_xtsd04g_writeByte(uint08 TxData);
static uint08 tlkdev_xtsd04g_readByte(void);

static void tlkdev_xtsd04g_spiRead(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen);
static void tlkdev_xtsd04g_spiReadDma(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen);

static void tlkdev_xtsd04g_readData(uint08 *pBuff, uint32 buffLen);
static void tlkdev_xtsd04g_writeData(uint08 *pData, uint32 dataLen);



extern unsigned char s_hspi_tx_dma_chn;
extern unsigned char s_hspi_rx_dma_chn;
extern unsigned char s_pspi_tx_dma_chn;
extern unsigned char s_pspi_rx_dma_chn;

#if (TLKDEV_XTSD01G_SPI_MODULE == TLKDEV_XTSD01G_SPI_PSPI)
static pspi_pin_config_t sTlkDevXtsd04gPspiPinCfg = {
	.pspi_clk_pin		= PSPI_CLK_PC5,
	.pspi_csn_pin 		= PSPI_NONE_PIN,
	.pspi_mosi_io0_pin  = PSPI_MOSI_IO0_PC7,
	.pspi_miso_io1_pin  = PSPI_MISO_IO1_PC6,//3line mode set none
};
#elif (TLKDEV_XTSD01G_SPI_MODULE == TLKDEV_XTSD01G_SPI_HSPI)
static hspi_pin_config_t sTlkDevXtsd04gHspiPinCfg = {
	.hspi_clk_pin		= HSPI_CLK_PB4,
	.hspi_csn_pin 		= HSPI_NONE_PIN,
	.hspi_mosi_io0_pin  = HSPI_MOSI_IO0_PB3,
	.hspi_miso_io1_pin  = HSPI_MISO_IO1_PB2,//3line mode set none
	.hspi_wp_io2_pin    = HSPI_NONE_PIN,//set quad mode otherwise set none
	.hspi_hold_io3_pin  = HSPI_NONE_PIN,//set quad mode otherwise set none
};
#endif

static uint08 sTlkDevXtsd01gType=0;
static uint08 sTlkDevXtsd01gIsEnter = 0;
static uint08 sTlkDevXtsd01gIsReady = false;
static uint08 sTlkDevXtsd01gPowerIsOn = false;



int tlkdev_xtsd01g_init(void)
{
	uint08 r1;
    uint16 retry;
    uint08 buf[10];
	uint16 i;
//	uint08 buffer[32];
	
	sTlkDevXtsd01gIsEnter = 1;
	
	tlkdev_xtsd01g_powerOn();
	delay_ms(10); // delay_ms(50); delay 250ms for voltage setup.
	
	tlkdev_xtsd04g_gpioConfig();

	//74 clock
 	for(i=0; i<10; i++){
 		tlkdev_xtsd04g_writeByte(0xFF);
 	}
	
	retry=0X10;
	//enable spi mode
	while(--retry){
		r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_00, 0, 0x95);
		if(r1 == 0x01) break;
		delay_us(100);
	}
	if(r1 != 0x01){
		#if(TLKDEV_XTSD01G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "sd_nand_flash_init");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "enable spi mode fail");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return -TLK_EFAIL;
	}
	
	//wait for busy
	sTlkDevXtsd01gType=0;
	retry=500;
	do{
		tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_55,0,1);
		delay_us(10);
		r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_41, 0x40000000, 0X01);
		delay_ms(1);
	}while(r1 && retry--);
	
	if(retry == 0){
		#if(TLKDEV_XTSD01G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "sd_nand_flash_init");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "wait for busy fail");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return -TLK_EFAIL;
	}
	
	r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_58,0,0X01);
	if(r1==0){
		for(i=0;i<4;i++)buf[i]=tlkdev_xtsd04g_readByte();	//80 FF 80 00
		if(buf[0]&0x40)sTlkDevXtsd01gType=TLKDEV_XTSD01G_TYPE_V2HC;
		else sTlkDevXtsd01gType=TLKDEV_XTSD01G_TYPE_V2;
	}else{
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return -TLK_EFAIL;
	}
	
	#if 0//get Capacity
	uint08 csd[16] = {0};
	uint32 Capacity;
	uint08 n;
	uint16 csize;

	memset(csd, 0, 16);

	r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_09,0,0x01);
	if(r1 == 0){
		r1=tlkdev_xtsd04g_spiRecvData(buffer,16);
		if(r1==0x00)tmemcpy(csd,buffer,16);//in order to align(4) for dma
	}

	if(r1 != 0){
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return 0xdd;
	}
	
	if((csd[0]&0xC0)==0x40){
		csize = csd[9] + ((uint16)csd[8] << 8) + 1;
		Capacity = (uint32)csize << 10;
	}else{
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16)csd[7] << 2) + ((uint16)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32)csize << (n - 9);
	}
	#endif
	tlkdev_xtsd04g_spiDisSelect();
	tlkdev_xtsd01g_switchHighSpeed();
	
	sTlkDevXtsd01gIsEnter = 0;
	sTlkDevXtsd01gIsReady = true;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkdev_xtsd01g_powerOn, tlkdev_xtsd01g_powerOff.
 * Descript: .
 * Params: None.
 * Return: None.
*******************************************************************************/
bool tlkdev_xtsd01g_powerIsOn(void)
{
	return sTlkDevXtsd01gPowerIsOn;
}
void tlkdev_xtsd01g_powerOn(void)
{
	if(sTlkDevXtsd01gPowerIsOn) return;
	
	gpio_function_en(TLKDEV_XTSD01G_POWER_PIN);
	gpio_output_en(TLKDEV_XTSD01G_POWER_PIN);
	
	gpio_set_low_level(TLKDEV_XTSD01G_POWER_PIN);		//low active
	gpio_set_up_down_res(TLKDEV_XTSD01G_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
	
	sTlkDevXtsd01gPowerIsOn = true;
}
void tlkdev_xtsd01g_powerOff(void)
{
	if(!sTlkDevXtsd01gPowerIsOn) return;
	
	gpio_function_en(TLKDEV_XTSD01G_POWER_PIN);
	gpio_output_en(TLKDEV_XTSD01G_POWER_PIN);
	
	gpio_set_high_level(TLKDEV_XTSD01G_POWER_PIN);	//high active
	gpio_set_up_down_res(TLKDEV_XTSD01G_POWER_PIN, GPIO_PIN_PULLUP_1M);

	sTlkDevXtsd01gPowerIsOn = false;
}
void tlkdev_xtsd01g_shutDown(void)
{
	gpio_shutdown(GPIO_PC4|GPIO_PC5|GPIO_PC6|GPIO_PC7);	
	tlkdev_xtsd01g_powerOff();
}


int tlkdev_xtsd01g_read(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	int ret;

	if(!sTlkDevXtsd01gIsReady) return -TLK_EFAIL;
	if(sTlkDevXtsd01gIsEnter != 0){
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "tlkdev_xtsd01g_read Reentry");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		return -TLK_EREPEAT;
	}
	if(!sTlkDevXtsd01gPowerIsOn){
		tlkdev_xtsd01g_powerOn();
		tlkdev_xtsd01g_init();
		if(!sTlkDevXtsd01gIsReady){
			tlkdev_xtsd01g_powerOff();
			return -TLK_EFAIL;
		}
	}

	sTlkDevXtsd01gIsEnter = 1;
		
	do{
		ret = tlkdev_xtsd01g_readBlock(pBuff, blkOffs);
		pBuff += 512;
		blkOffs ++;
	}while(--blkNumb && ret == TLK_ENONE);
	
	sTlkDevXtsd01gIsEnter = 0;
	
	return ret;
}
int tlkdev_xtsd01g_write(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	int ret;
	
	if(!sTlkDevXtsd01gIsReady) return -TLK_EFAIL;
	
	if(sTlkDevXtsd01gIsEnter != 0){
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "tlkdev_xtsd01g_write Reentry");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		return -TLK_EREPEAT;
	}
	if(!sTlkDevXtsd01gPowerIsOn){
		tlkdev_xtsd01g_powerOn();
		tlkdev_xtsd01g_init();
		if(!sTlkDevXtsd01gIsReady){
			tlkdev_xtsd01g_powerOff();
			return -TLK_EFAIL;
		}
	}
	
	sTlkDevXtsd01gIsEnter = 1;
	
	do{
		ret = tlkdev_xtsd01g_writeBlock(pData, blkOffs);
		pData += 512;
		blkOffs ++;
	}while(--blkNumb && ret == TLK_ENONE);
	sTlkDevXtsd01gIsEnter = 0;
	
	return ret;
}
int tlkdev_xtsd01g_readBlock(uint08 *pBuff, uint32 blkOffs)
{
	uint08 r1;
	uint08 buffer[512] = {0};
	if(sTlkDevXtsd01gType!=TLKDEV_XTSD01G_TYPE_V2HC) blkOffs <<= 9;

	r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_17, blkOffs, 0X01);
	if(r1==0){
		r1 = tlkdev_xtsd04g_spiRecvData(buffer, 512);
		if(r1==0x00) tmemcpy(pBuff, buffer, 512);//in order to align(4) for dma
	}
	tlkdev_xtsd04g_spiDisSelect();
	if(r1 != 0) return -TLK_EFAIL;	
	return TLK_ENONE;
}
int tlkdev_xtsd01g_writeBlock(uint08 *pData, uint32 blkOffs)
{
	uint08 r1;
	uint08 buffer[512] = {0};
	if(sTlkDevXtsd01gType!=TLKDEV_XTSD01G_TYPE_V2HC) blkOffs *= 512;

	r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_24, blkOffs, 0X01);
	if(r1 == 0){
		tmemcpy(buffer, pData, 512);//in order to align(4) for dma
		r1 = tlkdev_xtsd04g_spiSendData(buffer, 0xFE);
	}
	tlkdev_xtsd04g_spiDisSelect();
	if(r1 != 0) return -TLK_EFAIL;	
	return TLK_ENONE;
}






bool tlkdev_xtsd01g_getCID(uint08 *cid_data)
{
    uint08 r1;
	uint08 buffer[32];

    r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_10,0,0x01);
    if(r1==0x00){
		r1 = tlkdev_xtsd04g_spiRecvData(buffer,16);
		if(r1==0x00) tmemcpy(cid_data,buffer,16);//in order to align(4) for dma
    }
	tlkdev_xtsd04g_spiDisSelect();
	if(r1) return false;
	else return true;
}

bool tlkdev_xtsd01g_getCSD(uint08 *csd_data)
{
    uint08 r1;
	uint08 buffer[32];
    r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_09,0,0x01);
    if(r1==0){
		r1 = tlkdev_xtsd04g_spiRecvData(buffer,16);
		if(r1==0x00) tmemcpy(csd_data,buffer,16);//in order to align(4) for dma
    }
	tlkdev_xtsd04g_spiDisSelect();
	if(r1) return false;
	else return true;
}


uint tlkdev_xtsd01g_getBlockCount(void)
{
	if(sTlkDevXtsd01gIsEnter != 0)
	{
		#if(TLKDEV_XTSD01G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "tlkdev_xtsd01g_getBlockCount Reentry");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		return 0x11;
	}
	
	sTlkDevXtsd01gIsEnter = 1;
	
	uint08 csd[16] = {0};

    uint32 Capacity;
    uint08 n;
	uint16 csize;

	memset(csd, 0, 16);

    if(tlkdev_xtsd01g_getCSD(csd)!=0) return 0;

    if((csd[0]&0xC0)==0x40)
    {
		csize = csd[9] + ((uint16)csd[8] << 8) + 1;
		Capacity = (uint32)csize << 10;
    }else{
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16)csd[7] << 2) + ((uint16)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32)csize << (n - 9);
    }
    sTlkDevXtsd01gIsEnter = 0;
    return Capacity;
}


void tlkdev_xtsd01g_eraseBlocks(uint32 blkOffs, uint32 blkNumb)
{
	uint08 resp;
	uint32 start_addr;
	uint32 end_addr;

	start_addr = blkOffs;
	end_addr = (blkOffs+blkNumb-1);
	if(sTlkDevXtsd01gType  != TLKDEV_XTSD01G_TYPE_V2HC){
		start_addr <<= 9;
		end_addr <<= 9;
	}
	tlkdev_xtsd01g_spiCsLow();
	resp=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_32, start_addr, 0x01);
	if(resp != 0){
		tlkdev_xtsd01g_spiCsHigh();
		return;
	}
	
	resp=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_33, end_addr, 0x01);
	if(resp != 0){
		tlkdev_xtsd01g_spiCsHigh();
		return;
	}
		
	tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD01G_CMD_38, 0x0, 0x01);	//resp = 0xC0
	for(uint08 i=0;i<10;i++){
		tlkdev_xtsd04g_writeByte(0xFF);
	}
	
	uint16 cnt=0;
	uint16 total=15000; //wait 15000 * 200 us = 3 s for check ready.
	do{
		if(tlkdev_xtsd04g_readByte() == 0xFF){
			tlkdev_xtsd01g_spiCsHigh();
			return;
		}
		cnt++;
		delay_us(200);
	}while(cnt < total);
	tlkdev_xtsd01g_spiCsHigh();
}



//static void tlkdev_xtsd01g_switchLowSpeed(void)
//{
//	spi_master_init(TLKDEV_XTSD01G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XTSD01G_FOD_CLK) - 1, SPI_MODE0);
//}
static void tlkdev_xtsd01g_switchHighSpeed(void)
{
	spi_master_init(TLKDEV_XTSD01G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XTSD01G_FPP_CLK) - 1, SPI_MODE0);
}

static void tlkdev_xtsd04g_gpioConfig(void)
{
	gpio_output_en(TLKDEV_XTSD01G_SPI_CS_PIN);
	gpio_input_dis(TLKDEV_XTSD01G_SPI_CS_PIN);
	gpio_set_high_level(TLKDEV_XTSD01G_SPI_CS_PIN);
	gpio_function_en(TLKDEV_XTSD01G_SPI_CS_PIN);
	
	
	#if(TLKDEV_XTSD01G_SPI_MODULE == TLKDEV_XTSD01G_SPI_PSPI)
	pspi_set_pin(&sTlkDevXtsd04gPspiPinCfg);
	gpio_input_dis(GPIO_PC7);
	gpio_set_up_down_res(GPIO_PC6,GPIO_PIN_PULLUP_1M);
	spi_master_init(TLKDEV_XTSD01G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XTSD01G_FOD_CLK) - 1, SPI_MODE0);
	#else
	hspi_set_pin(&sTlkDevXtsd04gHspiPinCfg);
	spi_master_init(TLKDEV_XTSD01G_SPI_MODULE, sys_clk.hclk * 1000000 / (2 * TLKDEV_XTSD01G_FOD_CLK) - 1, SPI_MODE0);
	#endif
	
	spi_master_config(TLKDEV_XTSD01G_SPI_MODULE, SPI_NOMAL);
}

static void tlkdev_xtsd01g_spiCsLow(void)
{
	gpio_set_low_level(TLKDEV_XTSD01G_SPI_CS_PIN);
}
static void tlkdev_xtsd01g_spiCsHigh(void)
{
	gpio_set_high_level(TLKDEV_XTSD01G_SPI_CS_PIN);
}
static uint08 tlkdev_xtsd04g_spiSelect(void)
{
	tlkdev_xtsd01g_spiCsLow();
	if(tlkdev_xtsd01g_waitReady()==0)return 0;
	tlkdev_xtsd04g_spiDisSelect();
	return 1;
}
static void tlkdev_xtsd04g_spiDisSelect(void)
{
	tlkdev_xtsd01g_spiCsHigh();
	tlkdev_xtsd04g_writeByte(0xff);
}

static uint08 tlkdev_xtsd04g_spiGetResponse(uint08 Response)
{
	uint32 t=0;
	uint32 Count = 0x3F;
	do{
		if(tlkdev_xtsd04g_readByte() == Response) return TLKDEV_XTSD01G_MSD_RESPONSE_NO_ERR;
		delay_us(100);
		t ++;
	}while(t < Count);
	#if(TLKDEV_XTSD01G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "tlkdev_xtsd04g_spiGetResponse");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
	}
	#endif
	return TLKDEV_XTSD01G_MSD_RESPONSE_FAILURE;
}
static uint08 tlkdev_xtsd04g_spiRecvData(uint08 *pBuff, uint16 buffLen)
{
	if(tlkdev_xtsd04g_spiGetResponse(0xFE)) return 1;
	
	tlkdev_xtsd04g_readData(pBuff, buffLen);
    tlkdev_xtsd04g_readByte();
    tlkdev_xtsd04g_readByte();
	
    return 0;
}
static uint08 tlkdev_xtsd04g_spiSendData(uint08 *pData, uint08 dataCmd)
{
	uint16 t;
	if(tlkdev_xtsd01g_waitReady()) return 1;
	tlkdev_xtsd04g_writeByte(dataCmd);
	if(dataCmd !=0XFD )
	{
		tlkdev_xtsd04g_writeData(pData, 512);
		tlkdev_xtsd04g_writeByte(0xFF);	//CRC
		tlkdev_xtsd04g_writeByte(0xFF);	//CRC
		t=tlkdev_xtsd04g_readByte();
		if((t&0x1F)!=0x05)return 2;

		uint16 cnt=0;
		uint16 total=2500;	//wait 2500x100 us = 250 ms for check ready.
		do
		{
			if(tlkdev_xtsd04g_readByte() == 0xFF)
				return 0;
			cnt++;
			delay_us(100);
		}while(cnt<total);
	}
    return 1;
}


static uint08 tlkdev_xtsd04g_sendCmd(uint08 cmd, uint32 arg, uint08 crc)
{
	uint08 r1;
	uint32 t=0;
	uint32 Count=0XFF;
	
	tlkdev_xtsd04g_spiDisSelect();
	if(tlkdev_xtsd04g_spiSelect()) return 0XFF;

	tlkdev_xtsd04g_writeByte(cmd | 0x40);
	tlkdev_xtsd04g_writeByte(arg >> 24);
	tlkdev_xtsd04g_writeByte(arg >> 16);
	tlkdev_xtsd04g_writeByte(arg >> 8);
	tlkdev_xtsd04g_writeByte(arg);
	tlkdev_xtsd04g_writeByte(crc);
	if(cmd==TLKDEV_XTSD01G_CMD_12) tlkdev_xtsd04g_writeByte(0xff);
		
	do{
		r1 = tlkdev_xtsd04g_readByte();
		if(r1 != 0xff) break;
		t ++;
		delay_us(10);
	}while(t<Count);
	#if(TLKDEV_XTSD01G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "tlkdev_xtsd04g_sendCmd");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
	}
	#endif
	return r1;
}


static void tlkdev_xtsd04g_spiRead(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen)
{
	spi_rx_dma_dis(spi_sel);
	spi_rx_fifo_clr(spi_sel);
	spi_set_transmode(spi_sel, SPI_MODE_READ_ONLY);
	spi_rx_cnt(spi_sel, buffLen);
	spi_set_cmd(spi_sel, 0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
	spi_read(spi_sel, (unsigned char*)pBuff, buffLen);
	while(spi_is_busy(spi_sel));
}
static void tlkdev_xtsd04g_spiReadDma(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen)
{
	unsigned char  rx_dma_chn;
	spi_rx_fifo_clr(spi_sel);
	spi_rx_dma_en(spi_sel);
	spi_rx_cnt(spi_sel, buffLen);
	spi_set_transmode(spi_sel, SPI_MODE_READ_ONLY);
	if(HSPI_MODULE == spi_sel) rx_dma_chn = s_hspi_rx_dma_chn;
	else rx_dma_chn = s_pspi_rx_dma_chn;
	spi_set_dma(rx_dma_chn, reg_spi_data_buf_adr(spi_sel), (unsigned int)convert_ram_addr_cpu2bus(pBuff), buffLen);
	spi_set_cmd(spi_sel, 0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
}


static uint08 tlkdev_xtsd01g_waitReady(void)
{
	uint32 t=0;
	uint32 Count=0x2FF;
	do{
		if(tlkdev_xtsd04g_readByte()==0XFF)return 0;
		delay_us(100);
		t++;
	}while(t<Count);
	#if(TLKDEV_XTSD01G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "tlkdev_xtsd01g_waitReady");
		tlkapi_trace(TLKDEV_XTSD01G_DBG_FLAG, TLKDEV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
	}
	#endif
	return 1;
}



static void tlkdev_xtsd04g_writeByte(uint08 TxData)
{
	tlkdev_xtsd04g_writeData(&TxData, 1);
}
static uint08 tlkdev_xtsd04g_readByte(void)
{
	uint08 temp[1];
	tlkdev_xtsd04g_readData(temp,1);
	return temp[0];
}

static void tlkdev_xtsd04g_readData(uint08 *pBuff, uint32 buffLen)
{
	while(spi_is_busy(TLKDEV_XTSD01G_SPI_MODULE));
	#if TLKDEV_XTSD01G_DMA_ENABLE
	if(((buffLen % 4) == 0) && (buffLen>=4))
	{
		spi_tx_dma_dis(TLKDEV_XTSD01G_SPI_MODULE);
		pspi_set_rx_dma_config(TLKDEV_XTSD01G_DMA_CHN);
		tlkdev_xtsd04g_spiReadDma(TLKDEV_XTSD01G_SPI_MODULE, pBuff, buffLen);
	}
	else
	#endif
	{
		TLKDEV_XTSD01G_IRQ_DISABLE();
		tlkdev_xtsd04g_spiRead(TLKDEV_XTSD01G_SPI_MODULE, pBuff, buffLen);
		TLKDEV_XTSD01G_IRQ_RESTORE();
	}
	while (spi_is_busy(TLKDEV_XTSD01G_SPI_MODULE));
}
static void tlkdev_xtsd04g_writeData(uint08 *pData, uint32 dataLen)
{
	pspi_set_pin_mux(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
	
	while(spi_is_busy(TLKDEV_XTSD01G_SPI_MODULE));

	#if TLKDEV_XTSD01G_DMA_ENABLE
	if(((dataLen % 4) == 0) && (dataLen>=4))
	{
		spi_rx_dma_dis(TLKDEV_XTSD01G_SPI_MODULE);
		pspi_set_tx_dma_config(TLKDEV_XTSD01G_DMA_CHN);
		spi_master_write_dma(TLKDEV_XTSD01G_SPI_MODULE, pData, dataLen);
	}
	else
	#endif
	{
		spi_tx_dma_dis(TLKDEV_XTSD01G_SPI_MODULE);
		TLKDEV_XTSD01G_IRQ_DISABLE();
		spi_master_write(TLKDEV_XTSD01G_SPI_MODULE, pData, dataLen);
		TLKDEV_XTSD01G_IRQ_RESTORE();
	}
	while(spi_is_busy(TLKDEV_XTSD01G_SPI_MODULE));
	gpio_set_high_level(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
	gpio_function_en(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
}





///////////////////////////////////////////////////////////////////////////////////////////////////

/********** The time format (16bits) is:**************
Bits15 ~ 11 represents hours, which can be 0 ~ 23; 
bits10 ~ 5 represents minutes, which can be 0 ~ 59; 
bits4 ~ 0 represents seconds, which can be 0 ~ 29, and each unit is 2 seconds, which means that the actual second value is 2 times of the value.
*/

/********* The date format (16bits) is:*************
 Bits15 ~ 9 represents the year, which can be 0 ~ 127. It represents the difference from 1980,
 That is to say, the actual year is this value plus 1980, which can be expressed up to 2107;

 Bits8 ~ 5 represents the month, which can be 1 ~ 12;
 Bits4 ~ 0 represents the number and can be 1 ~ 31.
*/

//Find the high byte of 16 bit time format
#define TLKDEV_XTSD01G_DISK_TIME_HB(H,M,S)    (unsigned char)(((((H)<<11))|((M)<<5)|(S))>>8)
//Find the low byte of 16 bit time format
#define TLKDEV_XTSD01G_DISK_TIME_LB(H,M,S)    (unsigned char)((((H)<<11))|((M)<<5)|(S))
//Find the high byte of 16 bit date format
#define TLKDEV_XTSD01G_DISK_DATE_HB(Y,M,D)    (unsigned char)(((((Y)-1980)<<9)|((M)<<5)|(D))>>8)
//Find the low byte of 16 bit date format
#define TLKDEV_XTSD01G_DISK_DATE_LB(Y,M,D)    (unsigned char)((((Y)-1980)<<9)|((M)<<5)|(D))

static const char scTlkDevXtsd01gDiskReadMe[] = "This is DualMode SDK Fat16 demo\r\n";

static const uint08 scTlkDevXtsd01gDiskMBR[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff, (TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff000000)>>24, 0x00, 0x00, 0x00, 0x00,
};
static const uint08 scTlkDevXtsd01gDiskDBR[64] =
{
	0xEB, 0x3C, 0x90, //Jump instruction, cannot be changed to 0, otherwise it prompts that it is not formatted (0~2)
	'M','S','D','O','S','5','.','0', //File system and version information "MSDOS5.0" (3~10)
	TLKDEV_XTSD01G_DISK_BLOCK_SIZE & 0xFF, (TLKDEV_XTSD01G_DISK_BLOCK_SIZE & 0xFF00) >> 8, //Number of bytes per sector, 512 bytes (11~12)
	TLKDEV_XTSD01G_DISK_CLUSTER_NUMB, //Number of sectors per cluster, 64 sectors (13)
	TLKDEV_XTSD01G_DISK_RSV_NUMB & 0xFF, (TLKDEV_XTSD01G_DISK_RSV_NUMB & 0xFF00) >> 8, //Number of reserved sectors, 8 (14~15)
	TLKDEV_XTSD01G_DISK_FAT_COPIES, //The number of FAT copies of this partition, which is 2 (16)
	(TLKDEV_XTSD01G_DISK_CLUSTER_SIZE >> 5) & 0xFF, (TLKDEV_XTSD01G_DISK_CLUSTER_SIZE >> 13) & 0xFF, //Number of root directory entries, 1024(00 04) entries (17~18). Unit:32Bytes
	0x00, 0x00, //The number of small sectors (<=32M), here is 0, which means to read the value from the large sector field (19~20)
	0xF8, //Media descriptor, 0xF8 means hard disk (21)
	(TLKDEV_XTSD01G_DISK_FAT_NUMB & 0xFF), (TLKDEV_XTSD01G_DISK_FAT_NUMB & 0xFF00)>>8, //Number of sectors per FAT, 64 (22~23)
	(TLKDEV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF), (TLKDEV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF00)>>8, //Number of sectors per track, 64 (24~25)
	(TLKDEV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF), (TLKDEV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF00)>>8, //The number of heads is 256M/32K-0x200 (16M) (reserve 512 32k sectors for program writing) (26~27)
	(TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff), (TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDEV_XTSD01G_DISK_DBR_OFFSET & 0xff000000)>>24, //Number of hidden sectors There is no hidden sector here, it is 0 (28~31)
	(TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff), (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff00)>>8, (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff0000)>>16, (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff000000)>>24,  //Number of large sectors, the total number of sectors, 256k is 0x200 (32~35)
	0x80, //Disk drive parameters, 80 means hard disk (36)
	0x00, //Reserved (37)
	0x29, //Extended boot tag, 0x29 means the next three fields are available (38)
	0x34, 0x12, 0x00, 0x00, //Label serial number (39~42)
	'T', 'L', 'K', '-', 'F', 'S', '-', 'D', 'E', 'M', 'O', //Disk label volume (43~53)
	'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20, 0x00, 0x00, //File system type information, the string "FAT16" (54~63)
};
static const uint08 scTlkDevXtsd01gDiskFat[6] =
{
	//------------- Block1: FAT16 Table -------------//
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF// // first 2 entries must be F8FF, third entry is cluster end of readme file };
};
static const uint08 scTlkDevXtsd01gDiskRoot[64] = //------------- Block2: Root Directory -------------//
{
	// first entry is volume label
	'T' , 'L' , 'K' , '-' , 'F' , 'S' , '-' , 'D' , 'E' , 'M' , 'O',
	0x08, //File property, indicating the label volume of the disk
	0x00, //Reserved
	0x00, //Create time millisecond timestamp

	//File creation time, 15:27:35
	TLKDEV_XTSD01G_DISK_TIME_LB(15,27,35), TLKDEV_XTSD01G_DISK_TIME_HB(15,27,35), //0x00, 0x00,
	//File creation date, August 19, 2008
	TLKDEV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDEV_XTSD01G_DISK_DATE_HB(2008,8,19), //0x00, 0x00,
	//Last visit date, August 20, 2008
	TLKDEV_XTSD01G_DISK_DATE_LB(2008,8,20), TLKDEV_XTSD01G_DISK_DATE_HB(2008,8,20), //0x00, 0x00,
	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:36:47
	TLKDEV_XTSD01G_DISK_TIME_LB(15,36,47), TLKDEV_XTSD01G_DISK_TIME_HB(15,36,47), //0x4F, 0x6D,
	//Last revision date, August 19, 2008
	TLKDEV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDEV_XTSD01G_DISK_DATE_HB(2008,8,19), //0x65, 0x43,

	0x00, 0x00,  //Start cluster low word
	0x00, 0x00, 0x00, 0x00, //file length
	
	// second entry is readme file
	'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 
	0x00, //File properties, representing read-write files
	0x00, //Reserved
	0x00, //0xC6, //0x00-Create time millisecond timestamp
	
	//File creation time, 15:48:26
	TLKDEV_XTSD01G_DISK_TIME_LB(15,48,26), TLKDEV_XTSD01G_DISK_TIME_HB(15,48,26),
	//File creation date, August 19, 2008
	TLKDEV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDEV_XTSD01G_DISK_DATE_HB(2008,8,19),
	//Last visit date
	TLKDEV_XTSD01G_DISK_DATE_LB(2008,8,20), TLKDEV_XTSD01G_DISK_DATE_HB(2008,8,20),

	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:50:33
	TLKDEV_XTSD01G_DISK_TIME_LB(15,50,33), TLKDEV_XTSD01G_DISK_TIME_HB(15,50,33), //0x88, 0x6D,
	//Last revision date, August 19, 2008
	TLKDEV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDEV_XTSD01G_DISK_DATE_HB(2008,8,19), //0x65, 0x43, 
	
	0x02, 0x00, //Start cluster low word, cluster 2.
	sizeof(scTlkDevXtsd01gDiskReadMe)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
};

int tlkdev_xtsd01g_diskInit(void)
{
	int ret;
	uint32 blkCount;
	uint08 temp[512];

	if(!sTlkDevXtsd01gIsReady) return -TLK_ENOREADY;
	
	ret = tlkdev_xtsd01g_diskRead(temp, 0, 1);
	if(ret != TLK_ENONE) return TLK_ENONE;
	if((temp[510] != 0x55) || (temp[511] != 0xaa)) return -TLK_EFAIL;
	
	blkCount = (temp[461]<<24) + (temp[460]<<16) + (temp[459]<<8) + temp[458];
	if(blkCount != TLKDEV_XTSD01G_DISK_BLOCK_NUMB){
		ret = tlkdev_xtsd01g_diskFormat();
	}
	return ret;
}
int tlkdev_xtsd01g_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	return tlkdev_xtsd01g_read(pBuff, blkOffs, blkNumb);
}
int tlkdev_xtsd01g_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	return tlkdev_xtsd01g_write(pData, blkOffs, blkNumb);
}
int tlkdev_xtsd01g_diskFormat(void)
{
	uint08 index;
	uint08 fatNum;
	uint32 offset;
	uint08 block[TLKDEV_XTSD01G_DISK_BLOCK_SIZE];

	if(!sTlkDevXtsd01gIsReady) return -TLK_ENOREADY;

	offset = TLKDEV_XTSD01G_DISK_DBR_OFFSET;
	
	//write DBR
	tmemset(block, 0, TLKDEV_XTSD01G_DISK_BLOCK_SIZE);
	tmemcpy(block, scTlkDevXtsd01gDiskDBR, sizeof(scTlkDevXtsd01gDiskDBR));
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdev_xtsd01g_diskWrite(block, offset, 1);
	tmemset(block, 0, TLKDEV_XTSD01G_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDEV_XTSD01G_DISK_RSV_NUMB; index ++){
		tlkdev_xtsd01g_diskWrite(block, offset+index, 1);
	}
	offset += index;

	//write FAT
	for(fatNum = 0; fatNum < TLKDEV_XTSD01G_DISK_FAT_COPIES; fatNum++){
		tmemcpy(block, scTlkDevXtsd01gDiskFat, sizeof(scTlkDevXtsd01gDiskFat));
		tlkdev_xtsd01g_diskWrite(block, offset, 1);
		tmemset(block, 0, TLKDEV_XTSD01G_DISK_BLOCK_SIZE);
		for(index = 1; index < TLKDEV_XTSD01G_DISK_FAT_NUMB; index ++){
			tlkdev_xtsd01g_diskWrite(block, offset+index, 1);
		}
		offset += index;
	}
	
	//write root dir
	tmemcpy(block, scTlkDevXtsd01gDiskRoot, sizeof(scTlkDevXtsd01gDiskRoot));
	tlkdev_xtsd01g_diskWrite(block, offset, 1);
	tmemset(block, 0, TLKDEV_XTSD01G_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDEV_XTSD01G_DISK_CLUSTER_NUMB; index ++){
		tlkdev_xtsd01g_diskWrite(block, offset+index, 1);
	}
	offset += index;
	
	//write file
	tmemcpy(block, scTlkDevXtsd01gDiskReadMe, sizeof(scTlkDevXtsd01gDiskReadMe));
	tlkdev_xtsd01g_diskWrite(block, offset, 1);
	offset ++;

	//write MBR
	tmemset(block, 0, TLKDEV_XTSD01G_DISK_BLOCK_SIZE);
	tmemcpy(&block[446], scTlkDevXtsd01gDiskMBR, sizeof(scTlkDevXtsd01gDiskMBR));
	block[461] = (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff000000)>>24;
	block[460] = (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff0000)>>16;
	block[459] = (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff00)>>8;
	block[458] = (TLKDEV_XTSD01G_DISK_BLOCK_NUMB & 0xff);
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdev_xtsd01g_diskWrite(block, 0, 1);
	
	return TLK_ENONE;
}


#endif

