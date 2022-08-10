/********************************************************************************************************
 * @file     tlkdev_xt26g0x.h
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

#ifndef TLKDEV_XT26G0X_H
#define TLKDEV_XT26G0X_H


#if (TLK_DEV_XT26G0X_ENABLE)


// Note: The driver has no bad block check and management.

#define TLKDEV_XT26G0X_POWER_PIN	GPIO_PD4

// Nand Flash Cmd Size
#define TLKDEV_XT26G0X_BYTES 		4



// Nand Flash data struct
typedef struct{
	__attribute__((aligned(4))) unsigned char cmd[TLKDEV_XT26G0X_BYTES];
	__attribute__((aligned(4))) unsigned char *data;
	__attribute__((aligned(4))) unsigned char temp[8];
}tlkdev_xt26g0x_buffer_t;

// NAND SPI Flash configuration 
typedef struct{
	// SPI transfer function
	int (*spi_transfer)(tlkdev_xt26g0x_buffer_t *buffer, uint16 tx_len, uint16 rx_len);
	// delay microseconds function
	void(*delay_us)(unsigned int delay);
}tlkdev_xt26g0x_config_t;




int tlkdev_xt26g0x_init(void);
int tlkdev_xt26g0x_read(uint32 addr, uint08 *pBuff, int readLen);
int tlkdev_xt26g0x_write(uint32 addr, uint08 *pData, int dataLen);
int tlkdev_xt26g0x_erase(uint32 row_address);


void tlkdev_xt26g0x_powerOn(void);
void tlkdev_xt26g0x_powerOff(void);
void tlkdev_xt26g0x_shutDown(void);


uint tlkdev_xt26g0x_blockCount(void);


uint08 tlkdev_xt26g0x_readFeatureB0(void);
uint08 tlkdev_xt26g0x_readFeatureC0(void);

uint08 tlkdev_xt26g0x_readStatus(void);

int tlkdev_xt26g0x_resetUnlock(void);

int tlkdev_xt26g0x_pageRead(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 read_len);
int tlkdev_xt26g0x_pageWrite(uint32 row_address, uint16 col_address, uint08 *buffer, uint16 data_len);




#define TLKDEV_XT26G0X_DISK_BLOCK_SIZE       512
#define TLKDEV_XT26G0X_DISK_BLOCK_NUMB0      0x000FF000 //512M-2M=510M
#define TLKDEV_XT26G0X_DISK_BLOCK_NUMB1      0x0007F000 //256M-2M=254M
#define TLKDEV_XT26G0X_DISK_CLUSTER_SIZE     (32*1024)
#define TLKDEV_XT26G0X_DISK_CLUSTER_NUMB     (TLKDEV_XT26G0X_DISK_CLUSTER_SIZE/TLKDEV_XT26G0X_DISK_BLOCK_SIZE)

#define TLKDEV_XT26G0X_DISK_DBR_OFFSET       (4096) //(2*1024*1024)/512
#define TLKDEV_XT26G0X_DISK_FAT_COPIES       2

#define TLKDEV_XT26G0X_DISK_RSV_NUMB         8 //RSV - Reserve
#define TLKDEV_XT26G0X_DISK_FAT_NUMB         TLKDEV_XT26G0X_DISK_CLUSTER_NUMB

int tlkdev_xt26g0x_diskInit(void);
int tlkdev_xt26g0x_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
int tlkdev_xt26g0x_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb);
int tlkdev_xt26g0x_diskFormat(void);




#endif //#if (TLK_DEV_XT26G0X_ENABLE)

#endif //TLKDEV_XT26G0X_H

