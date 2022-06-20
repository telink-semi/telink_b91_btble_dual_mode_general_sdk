/********************************************************************************************************
 * @file     tlkdev_xt2602e.c
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
#include "tlkdev/tlkdev_xt2602e.h"

#if (TLK_DEV_XT2602E_ENABLE)



#include <stdint.h>
#include <stdlib.h>




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
#define SPI_CLK									8000000


static int tlkdev_xt26g02e_config(const tlkdev_xt2602e_config_t *config);
static int tlkdev_xt26g02e_writeRead(tlkdev_xt2602e_buffer_t * buffer, uint16 tx_len, uint16 rx_len);
static int tlkdev_xt26g02e_writeReadDma(tlkdev_xt2602e_buffer_t * buffer, uint16 tx_len, uint16 rx_len);


// config instance 
static tlkdev_xt2602e_config_t m_nsf_config;

// spi read/write buffer
static tlkdev_xt2602e_buffer_t m_nsf_buffer;

static uint16 sTlkDevXt26g02ePageSize   = 0; //Unit: Bytes
static uint16 sTlkDevXt26g02ePageCount  = 0;  // Pages Count In Memory
static uint16 sTlkDevXt26g02eBlockPages = 0; // pages count in one block
static uint16 sTlkDevXt26g02eBlockCount = 0;  // Blocks Count In Memory

volatile uint08 gTlkDevPspiEndIrqFlag = 1;

// udisk write flag
extern volatile unsigned char g_write_flag;

// udisk read flag
extern volatile unsigned int g_cbw_flag;

// nand flash conditions without dma
tlkdev_xt2602e_config_t sTlkDevXt26g02eCfg = {
	.spi_transfer = tlkdev_xt26g02e_writeRead,
	.delay_us = delay_us,
};
// nand flash conditions with dma
tlkdev_xt2602e_config_t sTlkDevXt26g02eDmaCfg = {
	.spi_transfer = tlkdev_xt26g02e_writeReadDma,
	.delay_us = delay_us,
};


// nand flash spi pin select
pspi_pin_config_t sTlkDevXt26g02ePspiPinCfg = {
	.pspi_clk_pin 		= PSPI_CLK_PC5,
	.pspi_csn_pin		= PSPI_CSN_PC4,
	.pspi_mosi_io0_pin	= PSPI_MOSI_IO0_PC7,
	.pspi_miso_io1_pin	= PSPI_MISO_IO1_PC6,
};


/******************************************************************************
 * Function: tlkdev_xt2602e_init
 * Descript: This function serves to initialization nand spi flash.
 * Params: None.
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
int tlkdev_xt2602e_init(void)
{
	spi_master_init(PSPI_MODULE, sys_clk.pclk * 1000000 / (2 * SPI_CLK) - 1, SPI_MODE0);
	pspi_set_pin(&sTlkDevXt26g02ePspiPinCfg);
	gpio_input_dis(GPIO_PC4|GPIO_PC7);
	gpio_set_up_down_res(GPIO_PC6,GPIO_PIN_PULLUP_1M);
	spi_master_config(PSPI_MODULE, SPI_NOMAL);

	#if NAND_FLASH_DMA_SUPPORT
	spi_clr_irq_status(PSPI_MODULE, SPI_END_INT);
	spi_set_irq_mask(PSPI_MODULE, SPI_END_INT_EN);//endint_en
	plic_interrupt_enable(IRQ23_SPI_APB);

	pspi_set_tx_dma_config(DMA2);
	pspi_set_rx_dma_config(DMA3);
	tlkdev_xt26g02e_config(&sTlkDevXt26g02eDmaCfg);
	#else
	tlkdev_xt26g02e_config(&sTlkDevXt26g02eCfg);
	#endif

	tlkdev_xt2602e_resetUnlock();
	return 0;
}

/******************************************************************************
 * Function: tlkdev_xt2602e_powerOn, tlkdev_xt2602e_powerOff.
 * Descript: .
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkdev_xt2602e_powerOn(void)
{
	gpio_function_en(TLKDEV_XT2602E_POWER_PIN);
	gpio_output_en(TLKDEV_XT2602E_POWER_PIN);
	gpio_set_high_level(TLKDEV_XT2602E_POWER_PIN);	//high active
	gpio_set_up_down_res(TLKDEV_XT2602E_POWER_PIN, GPIO_PIN_PULLUP_1M);
}
void tlkdev_xt2602e_powerOff(void)
{
	gpio_function_en(TLKDEV_XT2602E_POWER_PIN);
	gpio_output_en(TLKDEV_XT2602E_POWER_PIN);
	gpio_set_low_level(TLKDEV_XT2602E_POWER_PIN);		//low active
	gpio_set_up_down_res(TLKDEV_XT2602E_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
}

/******************************************************************************
 * Function: tlkdev_xt2602e_read
 * Descript: This function serves to read spi flash.
 * Params: 
 *    @adr[in]--the address need to be read.
 *    @buff[in]--the data buffer.
 *    @len[in]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
int tlkdev_xt2602e_read(uint32 adr, uint08 *pBuff, int readLen)
{
	int ra = adr>>11;
	int ca = (adr & 0x7ff)|(((adr>>17)& 0x01)<<12);
	return tlkdev_xt2602e_pageRead(ra, ca, pBuff, readLen);
}
/******************************************************************************
 * Function: tlkdev_xt2602e_read
 * Descript: This function serves to write spi flash.
 * Params: 
 *    @adr[in]--the address need to be write.
 *    @buff[in]--the data buffer.
 *    @len[in]--the the data length need to be write.
 * Return: None.
*******************************************************************************/
int tlkdev_xt2602e_write(uint32 adr, uint08 *pData, int dataLen)
{
	int ra = adr>>11;
	int ca = (adr & 0x7ff)|(((adr>>17)& 0x01)<<12);
	return tlkdev_xt2602e_pageWrite(ra, ca, pData, dataLen);
}
/******************************************************************************
 * Function: tlkdev_xt2602e_erase
 * Descript: This function serves to erase a block in nand flash.
 * Params: 
 *    @row_address[in]--the new block_address + page_address
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
int tlkdev_xt2602e_erase(uint32 row_address)
{
	// enable write
	m_nsf_buffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 1, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}
	
	// erase block
	m_nsf_buffer.cmd[0] = NSF_CMD_BLOCK_ERASE;
	m_nsf_buffer.cmd[1] = (row_address & 0xff0000) >> 16;
	m_nsf_buffer.cmd[2] = (row_address & 0xff00) >> 8;
	m_nsf_buffer.cmd[3] = row_address; // & 0xff;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 4, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}

	return (tlkdev_xt2602e_readStatus() & NSF_ERS_F_MASK) ? TLKDEV_XT2602E_ERR_ERASE : TLKDEV_XT2602E_ERR_NONE;
}


/******************************************************************************
 * Function: tlkdev_xt2602e_pageSize
 * Descript: This function serves to get nand spi flash page byte size.
 * Params: None.
 * Return: the nand spi flash page byte size.
*******************************************************************************/
uint16 tlkdev_xt2602e_pageSize(void)
{
	return sTlkDevXt26g02ePageSize;
}
/******************************************************************************
 * Function: tlkdev_xt2602e_pageCount
 * Descript: This function serves to get nand spi flash block number.
 * Params: None.
 * Return: the nand spi flash block number.
*******************************************************************************/
uint16 tlkdev_xt2602e_pageCount(void)
{
	return sTlkDevXt26g02ePageCount;
}
/******************************************************************************
 * Function: tlkdev_xt2602e_blockPages
 * Descript: This function serves to get nand spi flash pages number in one
 *           block.
 * Params: None.
 * Return: the nand spi flash pages number in one block.
*******************************************************************************/
uint16 tlkdev_xt2602e_blockPages(void)
{
	return sTlkDevXt26g02eBlockPages;
}
/******************************************************************************
 * Function: tlkdev_xt2602e_blockCount
 * Descript: This function serves to get nand spi flash block number.
 * Params: None.
 * Return: the nand spi flash block number.
*******************************************************************************/
uint16 tlkdev_xt2602e_blockCount(void)
{
	return sTlkDevXt26g02eBlockCount;
}



/******************************************************************************
 * Function: tlkdev_xt2602e_readFeatureB0
 * Descript: This function serves to read B0 feature in nand flash.
 * Params: None.
 * Return: the B0 feature.
*******************************************************************************/
uint08 tlkdev_xt2602e_readFeatureB0(void)
{
	m_nsf_config.delay_us(NSF_PAGE_READ_TIME_US);
	m_nsf_buffer.cmd[0] = NSF_CMD_GET_FEATURE;
	m_nsf_buffer.cmd[1] = 0xb0;
	m_nsf_buffer.data = m_nsf_buffer.temp;
	m_nsf_config.spi_transfer(&m_nsf_buffer, 2, 1);
	return m_nsf_buffer.temp[0];
}
/******************************************************************************
 * Function: tlkdev_xt2602e_readFeatureC0
 * Descript: This function serves to read C0 feature in nand flash.
 * Params: None.
 * Return: the C0 feature.
*******************************************************************************/
uint08 tlkdev_xt2602e_readFeatureC0(void)
{
	m_nsf_config.delay_us(NSF_PAGE_READ_TIME_US);
	m_nsf_buffer.cmd[0] = NSF_CMD_GET_FEATURE;
	m_nsf_buffer.cmd[1] = 0xC0;
	m_nsf_buffer.data = m_nsf_buffer.temp;
	m_nsf_config.spi_transfer(&m_nsf_buffer, 2, 1);
	return m_nsf_buffer.temp[0];
}

/******************************************************************************
 * Function: tlkdev_xt2602e_readStatus
 * Descript: This function serves to read nand flash status byte.
 * Params: None.
 * Return: nand flash status byte.
*******************************************************************************/
uint08 tlkdev_xt2602e_readStatus(void)
{
	int i=0;

	m_nsf_buffer.cmd[0] = NSF_CMD_GET_FEATURE;
	m_nsf_buffer.cmd[1] = NSF_CMD_FEATURE_STATUS;
	m_nsf_buffer.data = m_nsf_buffer.temp;

	m_nsf_buffer.temp[0] = NSF_OIP_MASK;
	while(m_nsf_buffer.temp[0] & NSF_OIP_MASK) {
	m_nsf_config.delay_us(NSF_PAGE_READ_TIME_US);
	m_nsf_config.spi_transfer(&m_nsf_buffer, 2, 1);
	i++;

	if(i>100)
		break;
	}
	return	m_nsf_buffer.temp[0];
}

/******************************************************************************
 * Function: tlkdev_xt2602e_resetUnlock
 * Descript: This function serves to reset and unlock flash device.
 * Params: 
 *    @buffer[in]--the data buffer.
 *    @tx_len[in]--the data length need to be write.
 *    @rx_len[in]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
int tlkdev_xt2602e_resetUnlock(void)
{
	// reset device
	m_nsf_buffer.cmd[0] = NSF_CMD_RESET;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 1, 0) != 0) {
	return TLKDEV_XT2602E_ERR_SPI;
	}
	tlkdev_xt2602e_readStatus();
	// unlock blocks for write
	m_nsf_buffer.cmd[0] = NSF_CMD_SET_FEATURE;
	m_nsf_buffer.cmd[1] = NSF_CMD_FEATURE_LOCK;
	m_nsf_buffer.cmd[2] = 0x00;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 3, 0) != 0) {
	return TLKDEV_XT2602E_ERR_SPI;
	}

	return TLKDEV_XT2602E_ERR_NONE;
}

/******************************************************************************
 * Function: tlkdev_xt2602e_pageRead
 * Descript: This function serves to read nand flash by a page.
 * Params: 
 *    @row_address[in]--block_address + page_address.
 *    @col_address[in]--the byte address in the page.
 *    @buffer[in]--the data buffer.
 *    @read_len[in]--the data length need to be read.
 * Return: number of bytes read or negative error code on error.
*******************************************************************************/
int tlkdev_xt2602e_pageRead(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 read_len)
{
	// check data len
	if (read_len > sTlkDevXt26g02ePageSize) {
		return TLKDEV_XT2602E_ERR_DATA_TOO_BIG;
	}
	
	// read page to nand cache buffer 
	m_nsf_buffer.cmd[0] = NSF_CMD_READ_CELL_TO_CACHE;
	m_nsf_buffer.cmd[1] = (row_address & 0xff0000) >> 16;
	m_nsf_buffer.cmd[2] = (row_address & 0xff00) >> 8;
	m_nsf_buffer.cmd[3] = row_address; // & 0xff;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 4, 0) != 0) {
	return TLKDEV_XT2602E_ERR_SPI;
	}

	// check status
	if ((tlkdev_xt2602e_readStatus() & NSF_ECC_MASK) == NSF_ECC_MASK) {
	return TLKDEV_XT2602E_ERR_BAD_BLOCK;
	}

	if (read_len > 0) {
		m_nsf_buffer.cmd[0] = 0x03;
		m_nsf_buffer.cmd[1] = (col_address & 0xff00) >> 8;
		m_nsf_buffer.cmd[2] = col_address; // & 0xff;
		m_nsf_buffer.cmd[3] = 0x00;
		m_nsf_buffer.data = buffer;
		if (m_nsf_config.spi_transfer(&m_nsf_buffer, 4, read_len) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
		}
	}
	// read buffer from cache

	return read_len;
}
/******************************************************************************
 * Function: tlkdev_xt2602e_pageWrite
 * Descript: This function serves to write nand flash by a page.
 * Params: 
 *    @row_address[in]--block_address + page_address.
 *    @col_address[in]--the byte address in the page.
 *    @buffer[in]--the data buffer.
 *    @read_len[in]--the data length need to be write.
 * Return: number of bytes written or negative error code on error.
*******************************************************************************/
int tlkdev_xt2602e_pageWrite(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 data_len)
{
	if (data_len + (col_address&0x7ff) > sTlkDevXt26g02ePageSize) {
		return TLKDEV_XT2602E_ERR_DATA_TOO_BIG;
	}

	// write enable
	m_nsf_buffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 1, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}
	
	if (data_len > 0) {
		(*(buffer-3)) = NSF_CMD_PROGRAM_LOAD;
		(*(buffer-2)) = (col_address & 0xff00) >> 8;
		(*(buffer-1)) = col_address; //;
		m_nsf_buffer.data =	(buffer-3);
		if (m_nsf_config.spi_transfer(&m_nsf_buffer, data_len + 3, 0) != 0) {
			return TLKDEV_XT2602E_ERR_SPI;
		}
	}

	m_nsf_buffer.cmd[0] = NSF_CMD_PROGRAM_EXECUTE;
	m_nsf_buffer.cmd[1] = (row_address & 0xff0000) >> 16;
	m_nsf_buffer.cmd[2] = (row_address & 0xff00) >> 8;
	m_nsf_buffer.cmd[3] = row_address; //;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 4, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}

	return (tlkdev_xt2602e_readStatus() & NSF_PRG_F_MASK) 
		? TLKDEV_XT2602E_ERR_ERASE : data_len;
}
/******************************************************************************
 * Function: tlkdev_xt26g02e_pageErase
 * Descript: This function serves to erase a block in nand flash.
 * Params: 
 *    @row_address[in]--the new block_address + page_address.
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
int tlkdev_xt26g02e_pageErase(unsigned int row_address)
{
	// enable write
	m_nsf_buffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 1, 0) != 0) {
	return TLKDEV_XT2602E_ERR_SPI;
	}
	
	// erase block
	m_nsf_buffer.cmd[0] = NSF_CMD_BLOCK_ERASE;
	m_nsf_buffer.cmd[1] = (row_address & 0xff0000) >> 16;
	m_nsf_buffer.cmd[2] = (row_address & 0xff00) >> 8;
	m_nsf_buffer.cmd[3] = row_address; // & 0xff;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 4, 0) != 0) {
	return TLKDEV_XT2602E_ERR_SPI;
	}

	return (tlkdev_xt2602e_readStatus() & NSF_ERS_F_MASK) 
		?	TLKDEV_XT2602E_ERR_ERASE : TLKDEV_XT2602E_ERR_NONE;
}
/******************************************************************************
 * Function: tlkdev_xt26g02e_pageReadWrite
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
int tlkdev_xt26g02e_pageReadWrite(unsigned int row_address_old, unsigned int row_address, unsigned short col_address, unsigned char * buffer, unsigned short data_len)
{
	if(data_len + col_address > sTlkDevXt26g02ePageSize) {
		return TLKDEV_XT2602E_ERR_DATA_TOO_BIG;
	}

	// read page to nand cache buffer
	m_nsf_buffer.cmd[0] = NSF_CMD_READ_CELL_TO_CACHE;
	m_nsf_buffer.cmd[1] = (row_address_old & 0xff0000) >> 16;
	m_nsf_buffer.cmd[2] = (row_address_old & 0xff00) >> 8;
	m_nsf_buffer.cmd[3] = row_address_old; // & 0xff;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if(m_nsf_config.spi_transfer(&m_nsf_buffer, 4, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}

	if((tlkdev_xt2602e_readStatus() & NSF_ECC_MASK) == NSF_ECC_BIT_ERROR ) {
		return TLKDEV_XT2602E_ERR_BAD_BLOCK;
	}

	// write enable
	if(data_len != 0)
	{
		(*(buffer-3)) = NSF_CMD_PROGRAM_LOAD_RAND;
		(*(buffer-2)) = ((col_address & 0xff00) >> 8)|0x40;
		(*(buffer-1)) = col_address; // & 0xff;
		m_nsf_buffer.data = (buffer-3);
		if (m_nsf_config.spi_transfer(&m_nsf_buffer, data_len + 3, 0) != 0) {
			return TLKDEV_XT2602E_ERR_SPI;
		}
	}

	m_nsf_buffer.cmd[0] = NSF_CMD_WRITE_ENABLE;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 1, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}

	// program execute 0x10
	m_nsf_buffer.cmd[0] = NSF_CMD_PROGRAM_EXECUTE;
	m_nsf_buffer.cmd[1] = (row_address & 0xff0000) >> 16;
	m_nsf_buffer.cmd[2] = (row_address & 0xff00) >> 8;
	m_nsf_buffer.cmd[3] = row_address; // & 0xff;
	m_nsf_buffer.data = m_nsf_buffer.cmd;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 4, 0) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}

	return (tlkdev_xt2602e_readStatus() & NSF_PRG_F_MASK) ? TLKDEV_XT2602E_ERR_ERASE : data_len;
}


/******************************************************************************
 * Function: tlkdev_xt26g02e_config
 * Descript: This function serves to configuration nand spi flash.
 * Params: None.
 * Return: 0 on success, negative error code on error.
*******************************************************************************/
static int tlkdev_xt26g02e_config(const tlkdev_xt2602e_config_t *config)
{
	// check and copy config
	if(m_nsf_config.spi_transfer == (*config).spi_transfer){
		return TLKDEV_XT2602E_ERR_ALREADY_INITED;
	}
	
	m_nsf_config = *config;
	m_nsf_buffer.cmd[0] = NSF_CMD_READ_ID;
	m_nsf_buffer.cmd[1] = 0;
	m_nsf_buffer.data = m_nsf_buffer.temp;
	if (m_nsf_config.spi_transfer(&m_nsf_buffer, 2, 2) != 0) {
		return TLKDEV_XT2602E_ERR_SPI;
	}

	if(m_nsf_buffer.temp[0] == 0x0b)// XTX spi 4Gbit flash
	{
		if(m_nsf_buffer.temp[1] == 0xe3)// XT26G04A
		{
			sTlkDevXt26g02ePageSize	= 2048;
			sTlkDevXt26g02eBlockPages = 128;
			sTlkDevXt26g02ePageCount = 2048; // 4G bit
			sTlkDevXt26g02eBlockCount = 16;
		}
		else
		{
			return TLKDEV_XT2602E_ERR_UNKNOWN_DEVICE;
		}
	}
	else if(m_nsf_buffer.temp[0] == 0x2c)// XTX spi 2Gbit flash
	{
		if(m_nsf_buffer.temp[1] == 0x24)// XT26G02E
		{
			sTlkDevXt26g02ePageSize	= 2048;
			sTlkDevXt26g02eBlockPages = 64;
			sTlkDevXt26g02ePageCount	= 2048; // 2G bit
			sTlkDevXt26g02eBlockCount = 8;
		}
		else
		{
			return TLKDEV_XT2602E_ERR_UNKNOWN_DEVICE;
		}
	}
	else
	{
		return TLKDEV_XT2602E_ERR_UNKNOWN_DEVICE;
	}
	return TLKDEV_XT2602E_ERR_NONE;
}

/******************************************************************************
 * Function: tlkdev_xt26g02e_writeRead
 * Descript: This function serves to write read spi flash by dma.
 * Params: 
 *     @buffer[IN]--the data buffer.
 *     @tx_len[IN]--the data length need to be write.
 *     @rx_len[IN]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
static int tlkdev_xt26g02e_writeRead(tlkdev_xt2602e_buffer_t * buffer, uint16 tx_len, uint16 rx_len)
{
	if(rx_len>0)
		spi_master_write_read(PSPI_MODULE, buffer->cmd, tx_len, buffer->data, rx_len);
	else
		spi_master_write(PSPI_MODULE, buffer->data, tx_len);
	return 0; // always return ok.
}
/******************************************************************************
 * Function: tlkdev_xt26g02e_writeReadDma
 * Descript: This function serves to write read spi flash by dma.
 * Params: 
 *     @buffer[IN]--the data buffer.
 *     @tx_len[IN]--the data length need to be write.
 *     @rx_len[IN]--the data length need to be read.
 * Return: 0 on success.
*******************************************************************************/
static int tlkdev_xt26g02e_writeReadDma(tlkdev_xt2602e_buffer_t * buffer, uint16 tx_len, uint16 rx_len)
{
	if(rx_len>0)
	{
		if(g_cbw_flag > 0)
		{
			gTlkDevPspiEndIrqFlag = 0;
			spi_master_write_read_dma(PSPI_MODULE, buffer->cmd, tx_len, buffer->data, rx_len);
			while(!gTlkDevPspiEndIrqFlag);
		}
		else
		{
			spi_tx_dma_dis(PSPI_MODULE);
			spi_rx_dma_dis(PSPI_MODULE);
			spi_master_write_read(PSPI_MODULE, buffer->cmd, tx_len, buffer->data, rx_len);
		}
	}
	else if(tx_len>0)
	{
		if(g_write_flag==1)
		{
			gTlkDevPspiEndIrqFlag = 0;
			spi_master_write_dma(PSPI_MODULE, buffer->data, tx_len);

			while(!gTlkDevPspiEndIrqFlag);
		}
		else
		{
			spi_tx_dma_dis(PSPI_MODULE);
			spi_master_write(PSPI_MODULE, buffer->data, tx_len);
		}
	}
	while (spi_is_busy(PSPI_MODULE));
	return 0; // always return ok.
}





#endif //#if (TLK_DEV_XT2602E_ENABLE)

