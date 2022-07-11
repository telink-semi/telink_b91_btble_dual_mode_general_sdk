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
#if (TLK_DEV_XTSD04G_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xtsd04g.h"

#include "drivers.h"


#define TLKDEV_XTSD04G_DEBUG_ENABLE      (1 && TLKDEV_CFG_DBG_ENABLE)

#define TLKDEV_XTSD04G_DBG_FLAG         (TLKDEV_XTSD04G_DEBUG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKDEV_XTSD04G_DBG_SIGN         "[XTSD04G]"


#define TLKDEV_XTSD04G_IRQ_DISABLE()     uint32 rie = core_disable_interrupt()
#define TLKDEV_XTSD04G_IRQ_RESTORE()     core_restore_interrupt(rie)

extern void pspi_set_pin_mux(pspi_pin_def_e pin);


static void tlkdev_xtsd04g_gpioConfig(void);

void tlkdev_xtsd04g_spiCsLow(void);
void tlkdev_xtsd04g_spiCsHigh(void);

static uint08 tlkdev_xtsd04g_spiSelect(void);
static void   tlkdev_xtsd04g_spiDisSelect(void);

static uint08 tlkdev_xtsd04g_spiGetResponse(uint08 Response);
static uint08 tlkdev_xtsd04g_spiRecvData(uint08 *pBuff, uint16 buffLen);
static uint08 tlkdev_xtsd04g_spiSendData(uint08 *pData, uint08 dataCmd);



static uint08 tlkdev_xtsd04g_sendCmd(uint08 cmd, uint32 arg, uint08 crc);


uint08 tlkdev_xtsd04g_waitReady(void);

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

#if (TLKDEV_XTSD04G_SPI_MODULE == TLKDEV_XTSD04G_SPI_PSPI)
static pspi_pin_config_t sTlkDevXtsd04gPspiPinCfg = {
	.pspi_clk_pin		= PSPI_CLK_PC5,
	.pspi_csn_pin 		= PSPI_NONE_PIN,
	.pspi_mosi_io0_pin  = PSPI_MOSI_IO0_PC7,
	.pspi_miso_io1_pin  = PSPI_MISO_IO1_PC6,//3line mode set none
};
#elif (TLKDEV_XTSD04G_SPI_MODULE == TLKDEV_XTSD04G_SPI_HSPI)
static hspi_pin_config_t sTlkDevXtsd04gHspiPinCfg = {
	.hspi_clk_pin		= HSPI_CLK_PB4,
	.hspi_csn_pin 		= HSPI_NONE_PIN,
	.hspi_mosi_io0_pin  = HSPI_MOSI_IO0_PB3,
	.hspi_miso_io1_pin  = HSPI_MISO_IO1_PB2,//3line mode set none
	.hspi_wp_io2_pin    = HSPI_NONE_PIN,//set quad mode otherwise set none
	.hspi_hold_io3_pin  = HSPI_NONE_PIN,//set quad mode otherwise set none
};
#endif

uint32 sdcardblocknum;
volatile char sTlkDevXtsd04gEnterFlag = 0;

static uint08 sTlkDevXtsd04gSDType=0;



const uint08 sdmsc_disk_mbr[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, SDDISK_SECTOR_OFFSET & 0xff, (SDDISK_SECTOR_OFFSET & 0xff00)>>8, (SDDISK_SECTOR_OFFSET & 0xff0000)>>16, (SDDISK_SECTOR_OFFSET & 0xff000000)>>24, 0x00, 0x00, 0x00, 0x00,
};
const uint08 sdmsc_disk_dbr[64] =
{
  //------------- Block0: Boot Sector -------------//
  // byte_per_sector    = DISK_BLOCK_SIZE; fat12_sector_num_16  = DISK_BLOCK_NUM;
  // sector_per_cluster = 1; reserved_sectors = 1;
  // fat_num            = 1; fat12_root_entry_num = 16;
  // sector_per_fat     = 1; sector_per_track = 1; head_num = 1; hidden_sectors = 0;
  // drive_number       = 0x80; media_type = 0xf8; extended_boot_signature = 0x29;
  // filesystem_type    = "FAT12   "; volume_serial_number = 0x1234; volume_label = "TinyUSB 0  ";
  // FAT magic code at offset 510-511
  0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, SDDISK_SECTOR_SIZE/256, SDDISK_CLUSTER_NUM, 0x08, 0x00,
  0x01, 0x00, SDDISK_CLUSTER_SIZE/(32*256), 0x00, 0x00, 0xF8, SDDISK_CLUSTER_NUM, 0x00, SDDISK_CLUSTER_NUM, 0x00, SDDISK_CLUSTER_NUM, 0x00,  SDDISK_SECTOR_OFFSET & 0xff, (SDDISK_SECTOR_OFFSET & 0xff00)>>8, (SDDISK_SECTOR_OFFSET & 0xff0000)>>16, (SDDISK_SECTOR_OFFSET & 0xff000000)>>24,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x29, 0x34, 0x12, 0x00, 0x00, 'T' , 'i' , 'n' , 'y' , 'U' ,
  'S' , 'B' , ' ' , '0' , ' ' , ' ' , 0x46, 0x41, 0x54, 0x31, 0x36, 0x20, 0x20, 0x20, 0x00, 0x00,
};
const uint08 sdmsc_disk_fat_table[6] =
{
	//------------- Block1: FAT16 Table -------------//
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF// // first 2 entries must be F8FF, third entry is cluster end of readme file };
};


const char sdmsc_disk_readme[] = "This is smart watch MassStorage Class demo\r\n";
const uint08 sdmsc_disk_root_dir[64] = //------------- Block2: Root Directory -------------//
{
     // first entry is volume label
     'T' , 'e' , 'l' , 'i' , 'n' , 'k' , 'U' , ' ' , ' ' , ' ' , ' ' , 0x08, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x6D, 0x65, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     // second entry is readme file
	 'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 0x20, 0x00, 0xC6, 0x52, 0x6D,
     0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 0x02, 0x00,
     sizeof(sdmsc_disk_readme)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
};




int tlkdev_xtsd04g_init(void)
{
	uint08 r1;
    uint16 retry;
    uint08 buf[10];
	uint16 i;
	uint08 buffer[32];

	if(sTlkDevXtsd04gEnterFlag != 0){
		#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "sd_nand_flash_init Reentry");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		return 0x11;
	}
	
	sTlkDevXtsd04gEnterFlag = 1;
	delay_ms(10); // delay_ms(50); delay 250ms for voltage setup.
	
	tlkdev_xtsd04g_gpioConfig();

	//74 clock
 	for(i=0;i<10;i++){
 		tlkdev_xtsd04g_writeByte(0xFF);
 	}
	
	retry=0X10;
	//enable spi mode
	while(--retry){
		r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_00, 0, 0x95);
		if(r1 == 0x01) break;
		delay_us(100);
	}
	if(r1 != 0x01){
		#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "sd_nand_flash_init");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "enable spi mode fail");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd04gEnterFlag = 0;
		return 0xaa;
	}

	//wait for busy
	sTlkDevXtsd04gSDType=0;
	retry=500;
	do
	{
		tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_55,0,1);
		delay_us(10);
		r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_41,0x40000000,0X01);
		delay_ms(1);
	}while(r1&&retry--);

	if(retry == 0){
		#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "sd_nand_flash_init");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "wait for busy fail");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd04gEnterFlag = 0;
		return 0xbb;
	}

	r1 = tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_58,0,0X01);
	if(r1==0)
	{
		for(i=0;i<4;i++)buf[i]=tlkdev_xtsd04g_readByte();	//80 FF 80 00
		if(buf[0]&0x40)sTlkDevXtsd04gSDType=TLKDEV_XTSD04G_TYPE_V2HC;
		else sTlkDevXtsd04gSDType=TLKDEV_XTSD04G_TYPE_V2;
	}
	else
	{
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd04gEnterFlag = 0;
		return 0xcc;
	}

	//get Capacity
	uint08 csd[16] = {0};
	uint32 Capacity;
	uint08 n;
	uint16 csize;

	memset(csd, 0, 16);

	r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_09,0,0x01);
	if(r1==0)
	{
		r1=tlkdev_xtsd04g_spiRecvData(buffer,16);
		if(r1==0x00)tmemcpy(csd,buffer,16);//in order to align(4) for dma
	}

	if(r1!=0)
	{
		tlkdev_xtsd04g_spiDisSelect();
		sTlkDevXtsd04gEnterFlag = 0;
		return 0xdd;
	}
	
	if((csd[0]&0xC0)==0x40)
	{
		csize = csd[9] + ((uint16)csd[8] << 8) + 1;
		Capacity = (uint32)csize << 10;
	}else
	{
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16)csd[7] << 2) + ((uint16)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32)csize << (n - 9);
	}
	
	sdcardblocknum = Capacity;
	
	tlkdev_xtsd04g_spiDisSelect();
	tlkdev_xtsd04g_switchHighSpeed();
	
	sTlkDevXtsd04gEnterFlag = 0;
	
	return TLK_ENONE;
}


int tlkdev_xtsd04g_format(void)
{
	uint32 lba = 0;
	uint08 fat16block[512] = {0};
	uint32 sdcardfatblocknum = sdcardblocknum - SDDISK_SECTOR_OFFSET;
	
	//write DBR
	memcpy(fat16block, sdmsc_disk_dbr, sizeof(sdmsc_disk_dbr));
	fat16block[35] = (sdcardfatblocknum & 0xff000000)>>24;
	fat16block[34] = (sdcardfatblocknum & 0xff0000)>>16;
	fat16block[33] = (sdcardfatblocknum & 0xff00)>>8;
	fat16block[32] = sdcardfatblocknum & 0xff;
	fat16block[510] = 0x55;
	fat16block[511] = 0xaa;

	lba = SDDISK_SECTOR_OFFSET;//2M offset
	tlkdev_xtsd04g_write(fat16block, lba, 1);
	memset(fat16block, 0, 512);
	for(lba = SDDISK_SECTOR_OFFSET + 1; lba < SDDISK_SECTOR_OFFSET + 8; lba ++){
		tlkdev_xtsd04g_write(fat16block, lba, 1);
	}
	//write reserved_sectors
	memset(fat16block, 0, 512);

	//write FAT
	memcpy(fat16block, sdmsc_disk_fat_table, sizeof(sdmsc_disk_fat_table));
	tlkdev_xtsd04g_write(fat16block, lba, 1);

	memset(fat16block, 0, 512);

	for(lba = SDDISK_SECTOR_OFFSET + 8 + 1; lba < SDDISK_SECTOR_OFFSET + 8 + 64; lba ++)
	{
		tlkdev_xtsd04g_write(fat16block, lba, 1);
	}

	//write root dir
	memcpy(fat16block, sdmsc_disk_root_dir, sizeof(sdmsc_disk_root_dir));
	tlkdev_xtsd04g_write(fat16block, lba, 1);

	memset(fat16block, 0, 512);
	for(lba =  SDDISK_SECTOR_OFFSET + 8 + 64 + 1; lba <  SDDISK_SECTOR_OFFSET + 8 + 64 + 64; lba ++)
	{
		tlkdev_xtsd04g_write(fat16block, lba, 1);
	}
	memset(fat16block, 0, 512);
	//write file
	memcpy(fat16block, sdmsc_disk_readme, sizeof(sdmsc_disk_readme));
	tlkdev_xtsd04g_write(fat16block, lba, 1);

	//write MBR
	memset(fat16block, 0, 512);
	memcpy(&fat16block[446], sdmsc_disk_mbr, sizeof(sdmsc_disk_mbr));
	fat16block[461] = (sdcardfatblocknum & 0xff000000)>>24;
	fat16block[460] = (sdcardfatblocknum & 0xff0000)>>16;
	fat16block[459] = (sdcardfatblocknum & 0xff00)>>8;
	fat16block[458] = sdcardfatblocknum & 0xff;
	fat16block[510] = 0x55;
	fat16block[511] = 0xaa;
	lba = 0;
	tlkdev_xtsd04g_write(fat16block, lba, 1);
	
	return TLK_ENONE;
}






void tlkdev_xtsd04g_switchLowSpeed(void)
{
	spi_master_init(TLKDEV_XTSD04G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XTSD04G_FOD_CLK) - 1, SPI_MODE0);
}
void tlkdev_xtsd04g_switchHighSpeed(void)
{
	spi_master_init(TLKDEV_XTSD04G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XTSD04G_FPP_CLK) - 1, SPI_MODE0);
}


int tlkdev_xtsd04g_read(uint08 *pBuff, uint32 sector, uint08 sectCnt)
{
	if(sTlkDevXtsd04gEnterFlag != 0){
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "tlkdev_xtsd04g_read Reentry");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		return 0x11;
	}
	
	sTlkDevXtsd04gEnterFlag = 1;
	
	uint08 r1;
	do{
		r1=tlkdev_xtsd04g_readBa512(pBuff, sector);
		pBuff += 512;
		sector ++;
	}while(--sectCnt && r1==0);
	
	sTlkDevXtsd04gEnterFlag = 0;
	return r1;
}
int tlkdev_xtsd04g_write(uint08 *pData, uint32 sector, uint08 sectCnt)
{
	if(sTlkDevXtsd04gEnterFlag != 0){
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "tlkdev_xtsd04g_write Reentry");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		return 0x11;
	}
	
	sTlkDevXtsd04gEnterFlag = 1;
	
	uint08 r1;
	do{
		r1=tlkdev_xtsd04g_writeBa512(pData, sector);
		pData += 512;
		sector ++;
	}while(--sectCnt && r1==0);
	sTlkDevXtsd04gEnterFlag = 0;
	
	return r1;
}
uint08 tlkdev_xtsd04g_readBa512(uint08 *buf, uint32 sector)
{
	uint08 r1;
	uint08 buffer[512] = {0};
	if(sTlkDevXtsd04gSDType!=TLKDEV_XTSD04G_TYPE_V2HC)sector <<= 9;

	r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_17,sector,0X01);
	if(r1==0)
	{
		r1=tlkdev_xtsd04g_spiRecvData(buffer,512);
		if(r1==0x00)tmemcpy(buf,buffer,512);//in order to align(4) for dma
	}
	tlkdev_xtsd04g_spiDisSelect();
	return r1;
}
uint08 tlkdev_xtsd04g_writeBa512(uint08 *buf, uint32 sector)
{
	uint08 r1;
	uint08 buffer[512] = {0};
	if(sTlkDevXtsd04gSDType!=TLKDEV_XTSD04G_TYPE_V2HC)sector *= 512;

	r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_24,sector,0X01);
	if(r1==0)
	{
		tmemcpy(buffer,buf,512);//in order to align(4) for dma
		r1=tlkdev_xtsd04g_spiSendData(buffer,0xFE);
	}
	tlkdev_xtsd04g_spiDisSelect();
	return r1;
}


/******************************************************************************
 * Function: tlkdev_xtsd04g_powerOn, tlkdev_xtsd04g_powerOff.
 * Descript: .
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkdev_xtsd04g_powerOn(void)
{
	gpio_function_en(TLKDEV_XTSD04G_POWER_PIN);
	gpio_output_en(TLKDEV_XTSD04G_POWER_PIN);
	
	gpio_set_low_level(TLKDEV_XTSD04G_POWER_PIN);		//low active
	gpio_set_up_down_res(TLKDEV_XTSD04G_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
}
void tlkdev_xtsd04g_powerOff(void)
{
	gpio_function_en(TLKDEV_XTSD04G_POWER_PIN);
	gpio_output_en(TLKDEV_XTSD04G_POWER_PIN);
	
	gpio_set_high_level(TLKDEV_XTSD04G_POWER_PIN);	//high active
	gpio_set_up_down_res(TLKDEV_XTSD04G_POWER_PIN, GPIO_PIN_PULLUP_1M);
}



uint08 tlkdev_xtsd04g_getCID(uint08 *cid_data)
{
    uint08 r1;
	uint08 buffer[32];

    r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_10,0,0x01);
    if(r1==0x00)
	{
		r1=tlkdev_xtsd04g_spiRecvData(buffer,16);
		if(r1==0x00)tmemcpy(cid_data,buffer,16);//in order to align(4) for dma
    }
	tlkdev_xtsd04g_spiDisSelect();
	if(r1)return 1;
	else return 0;
}

uint08 tlkdev_xtsd04g_getCSD(uint08 *csd_data)
{
    uint08 r1;
	uint08 buffer[32];
    r1=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_09,0,0x01);
    if(r1==0)
	{
		r1=tlkdev_xtsd04g_spiRecvData(buffer,16);
		if(r1==0x00)tmemcpy(csd_data,buffer,16);//in order to align(4) for dma
    }
	tlkdev_xtsd04g_spiDisSelect();
	if(r1)return 1;
	else return 0;
}


uint32 tlkdev_xtsd04g_getSectorCount(void)
{
	if(sTlkDevXtsd04gEnterFlag != 0)
	{
		#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "tlkdev_xtsd04g_getSectorCount Reentry");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
		#endif
		return 0x11;
	}
	
	sTlkDevXtsd04gEnterFlag = 1;
	

	uint08 csd[16] = {0};

    uint32 Capacity;
    uint08 n;
	uint16 csize;

	memset(csd, 0, 16);

    if(tlkdev_xtsd04g_getCSD(csd)!=0) return 0;

    if((csd[0]&0xC0)==0x40)
    {
		csize = csd[9] + ((uint16)csd[8] << 8) + 1;
		Capacity = (uint32)csize << 10;
    }else
    {
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16)csd[7] << 2) + ((uint16)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32)csize << (n - 9);
    }
    sTlkDevXtsd04gEnterFlag = 0;
    return Capacity;
}


uint08 tlkdev_xtsd04g_eraseSector(uint32 start_sector, uint32 total_count)
{
	uint08 resp;
	uint32 start_addr;
	uint32 end_addr;

	start_addr = start_sector;
	end_addr = (start_sector+total_count-1);
	if( sTlkDevXtsd04gSDType  != TLKDEV_XTSD04G_TYPE_V2HC )
	{
		start_addr <<= 9;
		end_addr <<= 9;
	}
	tlkdev_xtsd04g_spiCsLow();
	/* ????TLKDEV_XTSD04G_CMD_32+????????????????, R1??????? */
	resp=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_32, start_addr, 0x01);
	if( 0 == resp )
	{
		/* ????TLKDEV_XTSD04G_CMD_33+????????????????, R1??????? */
		resp=tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_33, end_addr, 0x01);
		if( 0 == resp )
		{
			/* ????TLKDEV_XTSD04G_CMD_38??????????څ???, R1b???????  */	
			tlkdev_xtsd04g_sendCmd(TLKDEV_XTSD04G_CMD_38, 0x0, 0x01);	//resp = 0xC0

			for(uint08 i=0;i<10;i++)
			{
				tlkdev_xtsd04g_writeByte(0xFF);
			}

			uint16 cnt=0;
			uint16 total=65534; //wait 65534 * 200 us = 13 s for check ready.
			do
			{
				if(tlkdev_xtsd04g_readByte() == 0xFF)
				{
					tlkdev_xtsd04g_spiCsHigh();
					return 0;
				}
				cnt++;
				delay_us(200);
			}while(cnt<total);
		}
	}
	
	tlkdev_xtsd04g_spiCsHigh();
	return 1;
}






static void tlkdev_xtsd04g_gpioConfig(void)
{
	gpio_output_en(TLKDEV_XTSD04G_SPI_CS_PIN);
	gpio_input_dis(TLKDEV_XTSD04G_SPI_CS_PIN);
	gpio_set_high_level(TLKDEV_XTSD04G_SPI_CS_PIN);
	gpio_function_en(TLKDEV_XTSD04G_SPI_CS_PIN);


	#if(TLKDEV_XTSD04G_SPI_MODULE == TLKDEV_XTSD04G_SPI_PSPI)
	pspi_set_pin(&sTlkDevXtsd04gPspiPinCfg);
	gpio_input_dis(GPIO_PC7);
	gpio_set_up_down_res(GPIO_PC6,GPIO_PIN_PULLUP_1M);
	spi_master_init(TLKDEV_XTSD04G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XTSD04G_FOD_CLK) - 1, SPI_MODE0);
	#else
	hspi_set_pin(&sTlkDevXtsd04gHspiPinCfg);
	spi_master_init(TLKDEV_XTSD04G_SPI_MODULE, sys_clk.hclk * 1000000 / (2 * TLKDEV_XTSD04G_FOD_CLK) - 1, SPI_MODE0);
	#endif

	spi_master_config(TLKDEV_XTSD04G_SPI_MODULE, SPI_NOMAL);
}

void tlkdev_xtsd04g_spiCsLow(void)
{
	gpio_set_low_level(TLKDEV_XTSD04G_SPI_CS_PIN);
}
void tlkdev_xtsd04g_spiCsHigh(void)
{
	gpio_set_high_level(TLKDEV_XTSD04G_SPI_CS_PIN);
}
static uint08 tlkdev_xtsd04g_spiSelect(void)
{
	tlkdev_xtsd04g_spiCsLow();
	if(tlkdev_xtsd04g_waitReady()==0)return 0;
	tlkdev_xtsd04g_spiDisSelect();
	return 1;
}
static void tlkdev_xtsd04g_spiDisSelect(void)
{
	tlkdev_xtsd04g_spiCsHigh();
	tlkdev_xtsd04g_writeByte(0xff);
}

static uint08 tlkdev_xtsd04g_spiGetResponse(uint08 Response)
{
	uint32 t=0;
	uint32 Count=0x3F;
	do{
		if(tlkdev_xtsd04g_readByte()==Response) return TLKDEV_XTSD04G_MSD_RESPONSE_NO_ERR;
		delay_us(100);
		t++;
	}while(t<Count);
	#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "tlkdev_xtsd04g_spiGetResponse");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_process(); /* open this for usb log printf*/
	}
	#endif
	return TLKDEV_XTSD04G_MSD_RESPONSE_FAILURE;
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
	if(tlkdev_xtsd04g_waitReady()) return 1;
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
	if(cmd==TLKDEV_XTSD04G_CMD_12) tlkdev_xtsd04g_writeByte(0xff);
		
	do
	{
		r1 = tlkdev_xtsd04g_readByte();
		if(r1 != 0xff) break;
		t ++;
		delay_us(10);
	}while(t<Count);
	#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "tlkdev_xtsd04g_sendCmd");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
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


uint08 tlkdev_xtsd04g_waitReady(void)
{
	uint32 t=0;
	uint32 Count=0x2FF;
	do{
		if(tlkdev_xtsd04g_readByte()==0XFF)return 0;
		delay_us(100);
		t++;
	}while(t<Count);
	#if(TLKDEV_XTSD04G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "tlkdev_xtsd04g_waitReady");
		tlkapi_trace(TLKDEV_XTSD04G_DBG_FLAG, TLKDEV_XTSD04G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
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
	while(spi_is_busy(TLKDEV_XTSD04G_SPI_MODULE));
	#if TLKDEV_XTSD04G_DMA_ENABLE
	if(((buffLen % 4) == 0) && (buffLen>=4))
	{
		spi_tx_dma_dis(TLKDEV_XTSD04G_SPI_MODULE);
		pspi_set_rx_dma_config(TLKDEV_XTSD04G_DMA_CHN);
		tlkdev_xtsd04g_spiReadDma(TLKDEV_XTSD04G_SPI_MODULE, pBuff, buffLen);
	}
	else
	#endif
	{
		TLKDEV_XTSD04G_IRQ_DISABLE();
		tlkdev_xtsd04g_spiRead(TLKDEV_XTSD04G_SPI_MODULE, pBuff, buffLen);
		TLKDEV_XTSD04G_IRQ_RESTORE();
	}
	while (spi_is_busy(TLKDEV_XTSD04G_SPI_MODULE));
}
static void tlkdev_xtsd04g_writeData(uint08 *pData, uint32 dataLen)
{
	pspi_set_pin_mux(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
	
	while(spi_is_busy(TLKDEV_XTSD04G_SPI_MODULE));

	#if TLKDEV_XTSD04G_DMA_ENABLE
	if(((dataLen % 4) == 0) && (dataLen>=4))
	{
		spi_rx_dma_dis(TLKDEV_XTSD04G_SPI_MODULE);
		pspi_set_tx_dma_config(TLKDEV_XTSD04G_DMA_CHN);
		spi_master_write_dma(TLKDEV_XTSD04G_SPI_MODULE, pData, dataLen);
	}
	else
	#endif
	{
		spi_tx_dma_dis(TLKDEV_XTSD04G_SPI_MODULE);
		TLKDEV_XTSD04G_IRQ_DISABLE();
		spi_master_write(TLKDEV_XTSD04G_SPI_MODULE, pData, dataLen);
		TLKDEV_XTSD04G_IRQ_RESTORE();
	}
	while(spi_is_busy(TLKDEV_XTSD04G_SPI_MODULE));
	gpio_set_high_level(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
	gpio_function_en(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
}


#endif

