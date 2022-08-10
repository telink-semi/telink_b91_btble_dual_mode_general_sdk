/********************************************************************************************************
 * @file     tlkdev_xt26g0x.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#if (TLK_DEV_XT26G0X_ENABLE)
#include "tlkdev/ext/xtx/tlkdev_xt26g0x.h"



#include <stdint.h>
#include <stdlib.h>


// Note: tlkdev_xt26g0x_pageWrite has problem.


// Device Codes
#define NSF_DEVICE_TOSHIBA_TC58CVx             0x98
#define NSF_DEVICE_TC58CVG2S0HxAIx             0xCD // 4Gb 
#define NSF_DEVICE_GIGADEVICE_GD5FxGQ4x        0xC8
#define NSF_DEVICE_GD5F1GQ4R                   0xA1		// 1Gb 1.8v
#define NSF_DEVICE_GD5F2GQ4R                   0xA2		// 2Gb 1.8v
#define NSF_DEVICE_GD5F1GQ4U                   0xB1		// 1Gb 3.3v
#define NSF_DEVICE_GD5F2GQ4U                   0xB2		// 2Gb 3.3v

// Nand Flash Page Size
#define NSF_PAGE_SIZE_BYTES                    2048

// Nand Flash Commands
#define NSF_CMD_READ_ID                        0x9F
#define NSF_CMD_READ_CELL_TO_CACHE             0x13
#define NSF_CMD_GET_FEATURE                    0x0F
#define NSF_CMD_SET_FEATURE                    0x1F
#define NSF_CMD_FEATURE_STATUS                 0xC0
#define NSF_CMD_FEATURE_LOCK                   0xA0
#define NSF_CMD_RESET                          0xFF
#define NSF_CMD_WRITE_ENABLE                   0x06
#define NSF_CMD_BLOCK_ERASE                    0xD8
#define NSF_CMD_PROGRAM_LOAD                   0x02
#define NSF_CMD_PROGRAM_LOAD_RAND              0x84
#define NSF_CMD_PROGRAM_EXECUTE                0x10

// Nand Flash Status Bits
#define NSF_OIP_MASK                           0x01
#define NSF_PRG_F_MASK                         0b00001000
#define NSF_ERS_F_MASK                         0b00000100
#define NSF_ECC_MASK                           0b00110000
#define NSF_ECC_BIT_ERROR                      0x20
#define NSF_ECC_BITS                           4

// Timings
#define NSF_PAGE_READ_TIME_US                  115
#define NSF_RESET_TIME_MS                      7

// pspi clock define
#define TLKDEV_XT26G0X_SPI_CLOCK               8000000


static int tlkdev_xt26g0x_config(const tlkdev_xt26g0x_config_t *config);
static int tlkdev_xt26g0x_writeRead(tlkdev_xt26g0x_buffer_t * buffer, uint16 tx_len, uint16 rx_len);
//static int tlkdev_xt26g0x_writeReadDma(tlkdev_xt26g0x_buffer_t * buffer, uint16 tx_len, uint16 rx_len);


// config instance 
static tlkdev_xt26g0x_config_t sTlkDevXt26g0xConfig;
// spi read/write buffer
static tlkdev_xt26g0x_buffer_t sTlkDevXt26g0xBuffer;



// nand flash conditions without dma
tlkdev_xt26g0x_config_t sTlkDevXt26g02eCfg = {
	.spi_transfer = tlkdev_xt26g0x_writeRead,
	.delay_us = delay_us,
};

// nand flash spi pin select
pspi_pin_config_t sTlkDevXt26g0xPspiPinCfg = {
	.pspi_clk_pin 		= PSPI_CLK_PC5,
	.pspi_csn_pin		= PSPI_CSN_PC4,
	.pspi_mosi_io0_pin	= PSPI_MOSI_IO0_PC7,
	.pspi_miso_io1_pin	= PSPI_MISO_IO1_PC6,
};


static uint08 sTlkDevXt26g0xIsEnter = 0;
static uint08 sTlkDevXt26g0xIsReady = false;
static uint08 sTlkDevXt26g0xPowerIsOn = false;
static uint32 sTlkDevXt26g0xBlockCount = 0;  // Blocks Count In Memory


/******************************************************************************
 * Function: tlkdev_xt26g0x_init
 * Descript: This function serves to initialization nand spi flash.
 * Params: None.
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
int tlkdev_xt26g0x_init(void)
{
	tlkdev_xt26g0x_powerOn();

	if(sTlkDevXt26g0xIsEnter) return TLK_EREPEAT;
	sTlkDevXt26g0xIsEnter = true;
	sTlkDevXt26g0xIsReady = false;
	
	spi_master_init(PSPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDEV_XT26G0X_SPI_CLOCK) - 1, SPI_MODE0);
	pspi_set_pin(&sTlkDevXt26g0xPspiPinCfg);
	gpio_input_dis(GPIO_PC4|GPIO_PC7);
	gpio_set_up_down_res(GPIO_PC6,GPIO_PIN_PULLUP_1M);
	spi_master_config(PSPI_MODULE, SPI_NOMAL);
	
	if(tlkdev_xt26g0x_config(&sTlkDevXt26g02eCfg) != TLK_ENONE){
		tlkdev_xt26g0x_shutDown();
		sTlkDevXt26g0xIsEnter = false;
		return -TLK_EFAIL;
	}
	
	tlkdev_xt26g0x_resetUnlock();

	sTlkDevXt26g0xIsEnter = false;
	sTlkDevXt26g0xIsReady = true;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkdev_xt26g0x_powerOn, tlkdev_xt26g0x_powerOff.
 * Descript: .
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkdev_xt26g0x_powerOn(void)
{
	if(sTlkDevXt26g0xPowerIsOn) return;
	gpio_function_en(TLKDEV_XT26G0X_POWER_PIN);
	gpio_output_en(TLKDEV_XT26G0X_POWER_PIN);
	gpio_set_high_level(TLKDEV_XT26G0X_POWER_PIN);	//high active
	gpio_set_up_down_res(TLKDEV_XT26G0X_POWER_PIN, GPIO_PIN_PULLUP_1M);
	sTlkDevXt26g0xPowerIsOn = true;
}
void tlkdev_xt26g0x_powerOff(void)
{
	if(!sTlkDevXt26g0xPowerIsOn) return;
	gpio_function_en(TLKDEV_XT26G0X_POWER_PIN);
	gpio_output_en(TLKDEV_XT26G0X_POWER_PIN);
	gpio_set_low_level(TLKDEV_XT26G0X_POWER_PIN);		//low active
	gpio_set_up_down_res(TLKDEV_XT26G0X_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
	sTlkDevXt26g0xPowerIsOn = false;
}
void tlkdev_xt26g0x_shutDown(void)
{
	gpio_shutdown(GPIO_PC4|GPIO_PC5|GPIO_PC6|GPIO_PC7);	
	tlkdev_xt26g0x_powerOff();
}


/******************************************************************************
 * Function: tlkdev_xt26g0x_read
 * Descript: This function serves to read spi flash.
 * Params: 
 *    @adr[in]--the address need to be read.
 *    @buff[in]--the data buffer.
 *    @len[in]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
int tlkdev_xt26g0x_read(uint32 addr, uint08 *pBuff, int readLen)
{
	int ret;
	int ra = addr>>11;
	int ca = (addr & 0x7ff)|(((addr>>17)& 0x01)<<12);
	if(!sTlkDevXt26g0xIsReady) return -TLK_EFAIL;
	if(sTlkDevXt26g0xIsEnter) return -TLK_EREPEAT;

	if(!sTlkDevXt26g0xPowerIsOn){
		tlkdev_xt26g0x_powerOn();
		tlkdev_xt26g0x_init();
		if(!sTlkDevXt26g0xIsReady){
			tlkdev_xt26g0x_powerOff();
			return -TLK_EFAIL;
		}
	}
	sTlkDevXt26g0xIsEnter = true;
	ret = tlkdev_xt26g0x_pageRead(ra, ca, pBuff, readLen);
	sTlkDevXt26g0xIsEnter = false;
	return ret;
}
/******************************************************************************
 * Function: tlkdev_xt26g0x_read
 * Descript: This function serves to write spi flash.
 * Params: 
 *    @adr[in]--the address need to be write.
 *    @buff[in]--the data buffer.
 *    @len[in]--the the data length need to be write.
 * Return: None.
*******************************************************************************/
int tlkdev_xt26g0x_write(uint32 addr, uint08 *pData, int dataLen)
{
	int ret;
	int ra = addr>>11;
	int ca = (addr & 0x7ff)|(((addr>>17)& 0x01)<<12);
	if(!sTlkDevXt26g0xIsReady) return -TLK_EFAIL;
	if(sTlkDevXt26g0xIsEnter) return -TLK_EREPEAT;
	if(!sTlkDevXt26g0xPowerIsOn){
		tlkdev_xt26g0x_powerOn();
		tlkdev_xt26g0x_init();
		if(!sTlkDevXt26g0xIsReady){
			tlkdev_xt26g0x_powerOff();
			return -TLK_EFAIL;
		}
	}
	sTlkDevXt26g0xIsEnter = true;
	ret = tlkdev_xt26g0x_pageWrite(ra, ca, pData, dataLen);
	sTlkDevXt26g0xIsEnter = false;
	return ret;
}
/******************************************************************************
 * Function: tlkdev_xt26g0x_erase
 * Descript: This function serves to erase a block in nand flash.
 * Params: 
 *    @row_address[in]--the new block_address + page_address
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
int tlkdev_xt26g0x_erase(uint32 row_address)
{
	int ret;
	if(!sTlkDevXt26g0xIsReady) return -TLK_EFAIL;
	if(sTlkDevXt26g0xIsEnter) return -TLK_EREPEAT;
	sTlkDevXt26g0xIsEnter = true;
	// enable write
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 1, 0) != 0){
		sTlkDevXt26g0xIsEnter = false;
		return -TLK_EFAIL;
	}
	// erase block
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_BLOCK_ERASE;
	sTlkDevXt26g0xBuffer.cmd[1] = (row_address & 0xff0000) >> 16;
	sTlkDevXt26g0xBuffer.cmd[2] = (row_address & 0xff00) >> 8;
	sTlkDevXt26g0xBuffer.cmd[3] = row_address; // & 0xff;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, 0) != 0){
		sTlkDevXt26g0xIsEnter = false;
		return -TLK_EFAIL;
	}
	if((tlkdev_xt26g0x_readStatus() & NSF_ERS_F_MASK) != 0) ret = -TLK_EFAIL;
	else ret = TLK_ENONE;
	sTlkDevXt26g0xIsEnter = false;
	return ret;
}

uint tlkdev_xt26g0x_blockCount(void)
{
	return sTlkDevXt26g0xBlockCount;
}


/******************************************************************************
 * Function: tlkdev_xt26g0x_readFeatureB0
 * Descript: This function serves to read B0 feature in nand flash.
 * Params: None.
 * Return: the B0 feature.
*******************************************************************************/
uint08 tlkdev_xt26g0x_readFeatureB0(void)
{
	sTlkDevXt26g0xConfig.delay_us(NSF_PAGE_READ_TIME_US);
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_GET_FEATURE;
	sTlkDevXt26g0xBuffer.cmd[1] = 0xb0;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.temp;
	sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 2, 1);
	return sTlkDevXt26g0xBuffer.temp[0];
}
/******************************************************************************
 * Function: tlkdev_xt26g0x_readFeatureC0
 * Descript: This function serves to read C0 feature in nand flash.
 * Params: None.
 * Return: the C0 feature.
*******************************************************************************/
uint08 tlkdev_xt26g0x_readFeatureC0(void)
{
	sTlkDevXt26g0xConfig.delay_us(NSF_PAGE_READ_TIME_US);
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_GET_FEATURE;
	sTlkDevXt26g0xBuffer.cmd[1] = 0xC0;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.temp;
	sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 2, 1);
	return sTlkDevXt26g0xBuffer.temp[0];
}

/******************************************************************************
 * Function: tlkdev_xt26g0x_readStatus
 * Descript: This function serves to read nand flash status byte.
 * Params: None.
 * Return: nand flash status byte.
*******************************************************************************/
uint08 tlkdev_xt26g0x_readStatus(void)
{
	int i=0;
	
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_GET_FEATURE;
	sTlkDevXt26g0xBuffer.cmd[1] = NSF_CMD_FEATURE_STATUS;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.temp;
	
	sTlkDevXt26g0xBuffer.temp[0] = NSF_OIP_MASK;
	while(sTlkDevXt26g0xBuffer.temp[0] & NSF_OIP_MASK) {
		sTlkDevXt26g0xConfig.delay_us(NSF_PAGE_READ_TIME_US);
		sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 2, 1);
		i++;
		if(i > 100) break;
	}
	return sTlkDevXt26g0xBuffer.temp[0];
}

/******************************************************************************
 * Function: tlkdev_xt26g0x_resetUnlock
 * Descript: This function serves to reset and unlock flash device.
 * Params: 
 *    @buffer[in]--the data buffer.
 *    @tx_len[in]--the data length need to be write.
 *    @rx_len[in]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
int tlkdev_xt26g0x_resetUnlock(void)
{
	// reset device
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_RESET;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 1, 0) != 0){
		return -TLK_EFAIL;
	}
	tlkdev_xt26g0x_readStatus();
	// unlock blocks for write
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_SET_FEATURE;
	sTlkDevXt26g0xBuffer.cmd[1] = NSF_CMD_FEATURE_LOCK;
	sTlkDevXt26g0xBuffer.cmd[2] = 0x00;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 3, 0) != 0){
		return -TLK_EFAIL;
	}

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkdev_xt26g0x_pageRead
 * Descript: This function serves to read nand flash by a page.
 * Params: 
 *    @row_address[in]--block_address + page_address.
 *    @col_address[in]--the byte address in the page.
 *    @buffer[in]--the data buffer.
 *    @read_len[in]--the data length need to be read.
 * Return: number of bytes read or negative error code on error.
*******************************************************************************/
int tlkdev_xt26g0x_pageRead(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 read_len)
{
	// read page to nand cache buffer 
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_READ_CELL_TO_CACHE;
	sTlkDevXt26g0xBuffer.cmd[1] = (row_address & 0xff0000) >> 16;
	sTlkDevXt26g0xBuffer.cmd[2] = (row_address & 0xff00) >> 8;
	sTlkDevXt26g0xBuffer.cmd[3] = row_address; // & 0xff;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, 0) != 0){
		return -TLK_EFAIL;
	}

	// check status
	if((tlkdev_xt26g0x_readStatus() & NSF_ECC_MASK) == NSF_ECC_MASK){
		return -TLK_EFAIL;
	}

	if(read_len > 0){
		sTlkDevXt26g0xBuffer.cmd[0] = 0x03;
		sTlkDevXt26g0xBuffer.cmd[1] = (col_address & 0xff00) >> 8;
		sTlkDevXt26g0xBuffer.cmd[2] = col_address; // & 0xff;
		sTlkDevXt26g0xBuffer.cmd[3] = 0x00;
		sTlkDevXt26g0xBuffer.data = buffer;
		if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, read_len) != 0){
			return -TLK_EFAIL;
		}
	}
	// read buffer from cache

	return TLK_ENONE;
}
/******************************************************************************
 * Function: tlkdev_xt26g0x_pageWrite
 * Descript: This function serves to write nand flash by a page.
 * Params: 
 *    @row_address[in]--block_address + page_address.
 *    @col_address[in]--the byte address in the page.
 *    @buffer[in]--the data buffer.
 *    @read_len[in]--the data length need to be write.
 * Return: number of bytes written or negative error code on error.
*******************************************************************************/
int tlkdev_xt26g0x_pageWrite(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 data_len)
{
	// write enable
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 1, 0) != 0){
		return -TLK_EFAIL;
	}
	
	if(data_len > 0){
		(*(buffer-3)) = NSF_CMD_PROGRAM_LOAD;
		(*(buffer-2)) = (col_address & 0xff00) >> 8;
		(*(buffer-1)) = col_address; //;
		sTlkDevXt26g0xBuffer.data =	(buffer-3);
		if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, data_len + 3, 0) != 0){
			return -TLK_EFAIL;
		}
	}
	
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_PROGRAM_EXECUTE;
	sTlkDevXt26g0xBuffer.cmd[1] = (row_address & 0xff0000) >> 16;
	sTlkDevXt26g0xBuffer.cmd[2] = (row_address & 0xff00) >> 8;
	sTlkDevXt26g0xBuffer.cmd[3] = row_address; //;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, 0) != 0){
		return -TLK_EFAIL;
	}
	if((tlkdev_xt26g0x_readStatus() & NSF_PRG_F_MASK) != 0) return -TLK_EFAIL;
	return TLK_ENONE;
}
/******************************************************************************
 * Function: tlkdev_xt26g0x_pageErase
 * Descript: This function serves to erase a block in nand flash.
 * Params: 
 *    @row_address[in]--the new block_address + page_address.
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
int tlkdev_xt26g0x_pageErase(unsigned int row_address)
{
	// enable write
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 1, 0) != 0){
		return -TLK_EFAIL;
	}
	
	// erase block
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_BLOCK_ERASE;
	sTlkDevXt26g0xBuffer.cmd[1] = (row_address & 0xff0000) >> 16;
	sTlkDevXt26g0xBuffer.cmd[2] = (row_address & 0xff00) >> 8;
	sTlkDevXt26g0xBuffer.cmd[3] = row_address; // & 0xff;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, 0) != 0){
		return -TLK_EFAIL;
	}

	if((tlkdev_xt26g0x_readStatus() & NSF_PRG_F_MASK) != 0) return -TLK_EFAIL;
	return TLK_ENONE;
}
/******************************************************************************
 * Function: tlkdev_xt26g0x_pageReadWrite
 * Descript: This function serves to read a page from old address and write to
 *           new address in nand flash.
 * Params: 
 *    @row_address_old[in]--the old block_address + page_address
 *    @row_address[in]--the new block_address + page_address
 *    @col_address[in]--the byte address in the page
 *    @buffer[in]--the data buffer.
 *    @data_len[in]--the data length need to be write.
 * Return: data_len or negative error code on error.
*******************************************************************************/
int tlkdev_xt26g0x_pageReadWrite(unsigned int row_address_old, unsigned int row_address, unsigned short col_address, unsigned char * buffer, unsigned short data_len)
{
	// read page to nand cache buffer
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_READ_CELL_TO_CACHE;
	sTlkDevXt26g0xBuffer.cmd[1] = (row_address_old & 0xff0000) >> 16;
	sTlkDevXt26g0xBuffer.cmd[2] = (row_address_old & 0xff00) >> 8;
	sTlkDevXt26g0xBuffer.cmd[3] = row_address_old; // & 0xff;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, 0) != 0) {
		return -TLK_EFAIL;
	}

	if((tlkdev_xt26g0x_readStatus() & NSF_ECC_MASK) == NSF_ECC_BIT_ERROR){
		return -TLK_EFAIL;
	}

	// write enable
	if(data_len != 0){
		(*(buffer-3)) = NSF_CMD_PROGRAM_LOAD_RAND;
		(*(buffer-2)) = ((col_address & 0xff00) >> 8)|0x40;
		(*(buffer-1)) = col_address; // & 0xff;
		sTlkDevXt26g0xBuffer.data = (buffer-3);
		if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, data_len + 3, 0) != 0){
			return -TLK_EFAIL;
		}
	}

	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 1, 0) != 0){
		return -TLK_EFAIL;
	}

	// program execute 0x10
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_PROGRAM_EXECUTE;
	sTlkDevXt26g0xBuffer.cmd[1] = (row_address & 0xff0000) >> 16;
	sTlkDevXt26g0xBuffer.cmd[2] = (row_address & 0xff00) >> 8;
	sTlkDevXt26g0xBuffer.cmd[3] = row_address; // & 0xff;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.cmd;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 4, 0) != 0){
		return -TLK_EFAIL;
	}
	
	if((tlkdev_xt26g0x_readStatus() & NSF_PRG_F_MASK) != 0) return -TLK_EFAIL;
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkdev_xt26g0x_config
 * Descript: This function serves to configuration nand spi flash.
 * Params: None.
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
static int tlkdev_xt26g0x_config(const tlkdev_xt26g0x_config_t *config)
{
	// check and copy config
	if(sTlkDevXt26g0xConfig.spi_transfer == (*config).spi_transfer){
		return -TLK_EREPEAT;
	}
	
	sTlkDevXt26g0xConfig = *config;
	sTlkDevXt26g0xBuffer.cmd[0] = NSF_CMD_READ_ID;
	sTlkDevXt26g0xBuffer.cmd[1] = 0;
	sTlkDevXt26g0xBuffer.data = sTlkDevXt26g0xBuffer.temp;
	if(sTlkDevXt26g0xConfig.spi_transfer(&sTlkDevXt26g0xBuffer, 2, 2) != 0){
		return -TLK_EFAIL;
	}

	if(sTlkDevXt26g0xBuffer.temp[0] == 0x0b){ // XTX spi 4Gbit flash
		if(sTlkDevXt26g0xBuffer.temp[1] == 0xe3){ // XT26G0XA
			sTlkDevXt26g0xBlockCount = TLKDEV_XT26G0X_DISK_BLOCK_NUMB0;
		}else{
			return -TLK_EUNKNOWN;
		}
	}else if(sTlkDevXt26g0xBuffer.temp[0] == 0x2c){ // XTX spi 2Gbit flash
		if(sTlkDevXt26g0xBuffer.temp[1] == 0x24){ // XT26G02E
			sTlkDevXt26g0xBlockCount = TLKDEV_XT26G0X_DISK_BLOCK_NUMB1;
		}else{
			return -TLK_EUNKNOWN;
		}
	}else{
		return -TLK_EUNKNOWN;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkdev_xt26g0x_writeRead
 * Descript: This function serves to write read spi flash by dma.
 * Params: 
 *     @buffer[IN]--the data buffer.
 *     @tx_len[IN]--the data length need to be write.
 *     @rx_len[IN]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
static int tlkdev_xt26g0x_writeRead(tlkdev_xt26g0x_buffer_t * buffer, uint16 tx_len, uint16 rx_len)
{
	if(rx_len>0)
		spi_master_write_read(PSPI_MODULE, buffer->cmd, tx_len, buffer->data, rx_len);
	else
		spi_master_write(PSPI_MODULE, buffer->data, tx_len);
	return 0; // always return ok.
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
#define TLKDEV_XT26G0X_DISK_TIME_HB(H,M,S)    (unsigned char)(((((H)<<11))|((M)<<5)|(S))>>8)
//Find the low byte of 16 bit time format
#define TLKDEV_XT26G0X_DISK_TIME_LB(H,M,S)    (unsigned char)((((H)<<11))|((M)<<5)|(S))
//Find the high byte of 16 bit date format
#define TLKDEV_XT26G0X_DISK_DATE_HB(Y,M,D)    (unsigned char)(((((Y)-1980)<<9)|((M)<<5)|(D))>>8)
//Find the low byte of 16 bit date format
#define TLKDEV_XT26G0X_DISK_DATE_LB(Y,M,D)    (unsigned char)((((Y)-1980)<<9)|((M)<<5)|(D))

static const char scTlkDevXt26g04DiskReadMe[] = "This is DualMode SDK Fat16 demo\r\n";

static const uint08 scTlkDevXt26g04DiskMBR[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff, (TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff000000)>>24, 0x00, 0x00, 0x00, 0x00,
};
static const uint08 scTlkDevXt26g04DiskDBR[64] =
{
	0xEB, 0x3C, 0x90, //Jump instruction, cannot be changed to 0, otherwise it prompts that it is not formatted
	'M','S','D','O','S','5','.','0', //File system and version information "MSDOS5.0"
	TLKDEV_XT26G0X_DISK_BLOCK_SIZE & 0xFF, (TLKDEV_XT26G0X_DISK_BLOCK_SIZE & 0xFF00) >> 8, //Number of bytes per sector, 512 bytes (11~12)
	TLKDEV_XT26G0X_DISK_CLUSTER_NUMB, //Number of sectors per cluster, 64 sectors (13)
	TLKDEV_XT26G0X_DISK_RSV_NUMB & 0xFF, (TLKDEV_XT26G0X_DISK_RSV_NUMB >> 8) & 0xFF, //Number of reserved sectors, 8 (14~15)
	TLKDEV_XT26G0X_DISK_FAT_COPIES, //The number of FAT copies of this partition, which is 2 (16)
	(TLKDEV_XT26G0X_DISK_CLUSTER_SIZE >> 5) & 0xFF, (TLKDEV_XT26G0X_DISK_CLUSTER_SIZE >> 13) & 0xFF, //Number of root directory entries, 1024(00 04) entries (17~18). Unit:32Bytes
	0x00, 0x00, //The number of small sectors (<=32M), here is 0, which means to read the value from the large sector field (19~20)
	0xF8, //Media descriptor, 0xF8 means hard disk (21)
	(TLKDEV_XT26G0X_DISK_FAT_NUMB & 0xFF), (TLKDEV_XT26G0X_DISK_FAT_NUMB & 0xFF00)>>8, //Number of sectors per FAT, 64 (22~23)
	(TLKDEV_XT26G0X_DISK_CLUSTER_NUMB & 0xFF), (TLKDEV_XT26G0X_DISK_CLUSTER_NUMB & 0xFF00)>>8, //Number of sectors per track, 64 (24~25)
	(TLKDEV_XT26G0X_DISK_CLUSTER_NUMB & 0xFF), (TLKDEV_XT26G0X_DISK_CLUSTER_NUMB & 0xFF00)>>8, //The number of heads is 256M/32K-0x200 (16M) (reserve 512 32k sectors for program writing) (26~27)
	TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff, (TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDEV_XT26G0X_DISK_DBR_OFFSET & 0xff000000)>>24, //Number of hidden sectors There is no hidden sector here, it is 0 (28~31)
	0x00, 0x00, 0x00, 0x00, //Number of large sectors, the total number of sectors, 256k is 0x200 (32~35)
	0x80, //Disk drive parameters, 80 means hard disk (34)
	0x00, //Reserved (35)
	0x29, //Extended boot tag, 0x29 means the next three fields are available (36)
	0x34, 0x12, 0x00, 0x00, //Label serial number (37~40)
	'T' , 'L' , 'K' , '-' , 'F' , 'S' , '-' , 'D' , 'E' , 'M' , 'O', //Disk label volume
	0x46, 0x41, 0x54, 0x31, 0x36, 0x20, 0x20, 0x20, 0x00, 0x00, //File system type information, the string "FAT16"
};
static const uint08 scTlkDevXt26g04DiskFat[6] =
{
	//------------- Block1: FAT16 Table -------------//
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF// // first 2 entries must be F8FF, third entry is cluster end of readme file };
};
static const uint08 scTlkDevXt26g04DiskRoot[64] = //------------- Block2: Root Directory -------------//
{
	// first entry is volume label
	'T' , 'L' , 'K' , '-' , 'F' , 'S' , '-' , 'D' , 'E' , 'M' , 'O',
	0x08, //File property, indicating the label volume of the disk
	0x00, //Reserved
	0x00, //Create time millisecond timestamp

	//File creation time, 15:27:35
	TLKDEV_XT26G0X_DISK_TIME_LB(15,27,35), TLKDEV_XT26G0X_DISK_TIME_HB(15,27,35), //0x00, 0x00,
	//File creation date, August 19, 2008
	TLKDEV_XT26G0X_DISK_DATE_LB(2008,8,19), TLKDEV_XT26G0X_DISK_DATE_HB(2008,8,19), //0x00, 0x00,
	//Last visit date, August 20, 2008
	TLKDEV_XT26G0X_DISK_DATE_LB(2008,8,20), TLKDEV_XT26G0X_DISK_DATE_HB(2008,8,20), //0x00, 0x00,
	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:36:47
	TLKDEV_XT26G0X_DISK_TIME_LB(15,36,47), TLKDEV_XT26G0X_DISK_TIME_HB(15,36,47), //0x4F, 0x6D,
	//Last revision date, August 19, 2008
	TLKDEV_XT26G0X_DISK_DATE_LB(2008,8,19), TLKDEV_XT26G0X_DISK_DATE_HB(2008,8,19), //0x65, 0x43,

	0x00, 0x00,  //Start cluster low word
	0x00, 0x00, 0x00, 0x00, //file length
	
	// second entry is readme file
	'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 
	0x00, //File properties, representing read-write files
	0x00, //Reserved
	0x00, //0xC6, //0x00-Create time millisecond timestamp
	
	//File creation time, 15:48:26
	TLKDEV_XT26G0X_DISK_TIME_LB(15,48,26), TLKDEV_XT26G0X_DISK_TIME_HB(15,48,26),
	//File creation date, August 19, 2008
	TLKDEV_XT26G0X_DISK_DATE_LB(2008,8,19), TLKDEV_XT26G0X_DISK_DATE_HB(2008,8,19),
	//Last visit date
	TLKDEV_XT26G0X_DISK_DATE_LB(2008,8,20), TLKDEV_XT26G0X_DISK_DATE_HB(2008,8,20),

	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:50:33
	TLKDEV_XT26G0X_DISK_TIME_LB(15,50,33), TLKDEV_XT26G0X_DISK_TIME_HB(15,50,33), //0x88, 0x6D,
	//Last revision date, August 19, 2008
	TLKDEV_XT26G0X_DISK_DATE_LB(2008,8,19), TLKDEV_XT26G0X_DISK_DATE_HB(2008,8,19), //0x65, 0x43, 
	
	0x02, 0x00, //Start cluster low word, cluster 2.
	sizeof(scTlkDevXt26g04DiskReadMe)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
};


int tlkdev_xt26g0x_diskInit(void)
{
	int ret;
	uint32 count;
	uint32 blkCount;
	uint08 temp[512];

	if(!sTlkDevXt26g0xIsReady) return -TLK_ENOREADY;
	
	ret = tlkdev_xt26g0x_diskRead(temp, 0, 1);
	if(ret != TLK_ENONE) return TLK_ENONE;
	if((temp[510] != 0x55) || (temp[511] != 0xaa)) return -TLK_EFAIL;

	count = tlkdev_xt26g0x_blockCount() - TLKDEV_XT26G0X_DISK_DBR_OFFSET;
	blkCount = (temp[461]<<24) + (temp[460]<<16) + (temp[459]<<8) + temp[458];
	if(blkCount != count){
		ret = tlkdev_xt26g0x_diskFormat();
	}
	return ret;
}
int tlkdev_xt26g0x_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	tlkdev_xt26g0x_resetUnlock();
	return tlkdev_xt26g0x_read(blkOffs*TLKDEV_XT26G0X_DISK_BLOCK_SIZE, pBuff, 
		blkNumb*TLKDEV_XT26G0X_DISK_BLOCK_SIZE);
}
int tlkdev_xt26g0x_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	uint08 buffer[4+TLKDEV_XT26G0X_DISK_BLOCK_SIZE];
	tmemcpy(buffer+4, pData, TLKDEV_XT26G0X_DISK_BLOCK_SIZE);
	return tlkdev_xt26g0x_write(blkOffs*TLKDEV_XT26G0X_DISK_BLOCK_SIZE, buffer, 
		TLKDEV_XT26G0X_DISK_BLOCK_SIZE);
}

int tlkdev_xt26g0x_diskFormat(void)
{
	uint08 index;
	uint32 count;
	uint08 fatNum;
	uint32 offset;
	uint08 block[TLKDEV_XT26G0X_DISK_BLOCK_SIZE] = {0};

	if(!sTlkDevXt26g0xIsReady) return -TLK_ENOREADY;

	offset = TLKDEV_XT26G0X_DISK_DBR_OFFSET;
	count = tlkdev_xt26g0x_blockCount() - TLKDEV_XT26G0X_DISK_DBR_OFFSET;
	
	//write DBR
	tmemcpy(block, scTlkDevXt26g04DiskDBR, sizeof(scTlkDevXt26g04DiskDBR));
	block[35] = (count & 0xff000000)>>24;
	block[34] = (count & 0xff0000)>>16;
	block[33] = (count & 0xff00)>>8;
	block[32] = (count & 0xff);
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdev_xt26g0x_diskWrite(block, offset, 1);
	tmemset(block, 0, TLKDEV_XT26G0X_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDEV_XT26G0X_DISK_RSV_NUMB; index ++){
		tlkdev_xt26g0x_diskWrite(block, offset+index, 1);
	}
	offset += index;
	
	//write FAT
	for(fatNum = 0; fatNum < TLKDEV_XT26G0X_DISK_FAT_COPIES; fatNum++){
		tmemcpy(block, scTlkDevXt26g04DiskFat, sizeof(scTlkDevXt26g04DiskFat));
		tlkdev_xt26g0x_diskWrite(block, offset, 1);
		tmemset(block, 0, TLKDEV_XT26G0X_DISK_BLOCK_SIZE);
		for(index = 1; index < TLKDEV_XT26G0X_DISK_FAT_NUMB; index ++){
			tlkdev_xt26g0x_diskWrite(block, offset+index, 1);
		}
		offset += index;
	}
	
	//write root dir
	tmemcpy(block, scTlkDevXt26g04DiskRoot, sizeof(scTlkDevXt26g04DiskRoot));
	tlkdev_xt26g0x_diskWrite(block, offset, 1);
	tmemset(block, 0, TLKDEV_XT26G0X_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDEV_XT26G0X_DISK_CLUSTER_NUMB; index ++){
		tlkdev_xt26g0x_diskWrite(block, offset+index, 1);
	}
	offset += index;

	//write file
	tmemcpy(block, scTlkDevXt26g04DiskReadMe, sizeof(scTlkDevXt26g04DiskReadMe));
	tlkdev_xt26g0x_diskWrite(block, offset, 1);
	offset ++;
	
	//write MBR
	tmemset(block, 0, 512);
	tmemcpy(&block[446], scTlkDevXt26g04DiskMBR, sizeof(scTlkDevXt26g04DiskMBR));
	block[461] = (count & 0xff000000)>>24;
	block[460] = (count & 0xff0000)>>16;
	block[459] = (count & 0xff00)>>8;
	block[458] = (count & 0xff);
	block[510] = 0x55;
	block[511] = 0xaa;
	offset = 0;
	tlkdev_xt26g0x_diskWrite(block, 1, 1);
	
	return TLK_ENONE;
}




#endif //#if (TLK_DEV_XT26G0X_ENABLE)

