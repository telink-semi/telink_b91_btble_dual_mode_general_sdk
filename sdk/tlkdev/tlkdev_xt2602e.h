/********************************************************************************************************
 * @file     tlkdev_xt2602e.h
 *
 * @brief    This is the header file for BTBLE SDK
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

#ifndef TLKDEV_XT2602E_H
#define TLKDEV_XT2602E_H


#if (TLK_DEV_XT2602E_ENABLE)


// Note: The driver has no bad block check and management.

#define TLKDEV_XT2602E_POWER_PIN	GPIO_PD4

// Nand Flash Cmd Size
#define TLKDEV_XT2602E_BYTES 		4

// Possible error codes, these are negative to allow
// valid positive return values
typedef enum{
	TLKDEV_XT2602E_ERR_NONE            = 0, // No error
	TLKDEV_XT2602E_ERR_NOT_INITED     = -1, // Nand SPI Flash driver not inited
	TLKDEV_XT2602E_ERR_ALREADY_INITED = -2, // Nand SPI Flash driver already inited
	TLKDEV_XT2602E_ERR_UNKNOWN_DEVICE = -3, // Nand SPI Flash unsupported device
	TLKDEV_XT2602E_ERR_READ_ONLY      = -4, // Device read-only
	TLKDEV_XT2602E_ERR_BAD_BLOCK      = -5, // Device read-only
	TLKDEV_XT2602E_ERR_DATA_TOO_BIG   = -6, // Data to write/read is greater than page
	TLKDEV_XT2602E_ERR_ERASE          = -7, // Block erase hardware error
	TLKDEV_XT2602E_ERR_PROGRAM        = -8, // Block write hardware error
	TLKDEV_XT2602E_ERR_SPI            = -100 // Error 
}TLKDEV_XT2602E_ERR_ENUM;


// Nand Flash data struct
typedef struct{
	__attribute__((aligned(4))) unsigned char cmd[TLKDEV_XT2602E_BYTES];
	__attribute__((aligned(4))) unsigned char *data;
	__attribute__((aligned(4))) unsigned char temp[8];
}tlkdev_xt2602e_buffer_t;

// NAND SPI Flash configuration 
typedef struct{
	// SPI transfer function
	int (*spi_transfer)(tlkdev_xt2602e_buffer_t *buffer, uint16 tx_len, uint16 rx_len);
	// delay microseconds function
	void(*delay_us)(unsigned int delay);
}tlkdev_xt2602e_config_t;




int tlkdev_xt2602e_init(void);
int tlkdev_xt2602e_read(uint32 adr, uint08 *pBuff, int readLen);
int tlkdev_xt2602e_write(uint32 adr, uint08 *pData, int dataLen);
int tlkdev_xt2602e_erase(uint32 row_address);


void tlkdev_xt2602e_powerOn(void);
void tlkdev_xt2602e_powerOff(void);

uint16 tlkdev_xt2602e_pageSize(void);
uint16 tlkdev_xt2602e_pageCount(void);
uint16 tlkdev_xt2602e_blockPages(void);
uint16 tlkdev_xt2602e_blockCount(void);

uint08 tlkdev_xt2602e_readFeatureB0(void);
uint08 tlkdev_xt2602e_readFeatureC0(void);

uint08 tlkdev_xt2602e_readStatus(void);

int tlkdev_xt2602e_resetUnlock(void);

int tlkdev_xt2602e_pageRead(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 read_len);
int tlkdev_xt2602e_pageWrite(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 data_len);



#endif //#if (TLK_DEV_XT2602E_ENABLE)

#endif //TLKDEV_XT2602E_H

