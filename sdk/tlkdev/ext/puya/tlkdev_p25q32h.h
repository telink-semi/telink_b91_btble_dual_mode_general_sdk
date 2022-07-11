/********************************************************************************************************
 * @file     tlkdev_p25q32h.h
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
#ifndef TLKDEV_P25Q32H_H
#define TLKDEV_P25Q32H_H


#if (TLKDEV_EXT_PUYA_P25Q32H_ENABLE)


#define TLKDEV_P25Q32H_SPI_CS_GPIO            GPIO_PD6

#define TLKDEV_P25Q32H_PAGE_SIZE              256
#define TLKDEV_P25Q32H_SECTOR_SIZE            4096
#define TLKDEV_P25Q32H_SECTOR_COUNT           (TLKDEV_P25Q32H_TOTAL_SIZE / TLKDEV_P25Q32H_SECTOR_SIZE)
#define TLKDEV_P25Q32H_TOTAL_SIZE             (16 * 1024 * 1024)
#define TLKDEV_P25Q32H_BUSY_TIMEOUT_VALUE     (0x50000)


#define FATFS_FLASH_SECTOR_SIZE       512
#define FATFS_FLASH_SECTOR_COUNT      ((4*1024*1024)/FATFS_FLASH_SECTOR_SIZE)
#define FATFS_FLASH_BLOCK_SIZE        4096
#define FATFS_FLASH_BLOCK_COUNT       ((4*1024*1024)/FATFS_FLASH_BLOCK_SIZE)
#define FATFS_FLASH_SECTOR_IN_BLOCK   (FATFS_FLASH_BLOCK_SIZE/FATFS_FLASH_SECTOR_SIZE)



typedef enum{
	TLKDEV_P25Q32H_CMDID_WRITE_STATUS       = 0x01,
	TLKDEV_P25Q32H_CMDID_PAGE_PROGRAM       = 0x02,
	TLKDEV_P25Q32H_CMDID_READ_DATA          = 0x03,
	TLKDEV_P25Q32H_CMDID_WRITE_DISABLE      = 0x04,
	TLKDEV_P25Q32H_CMDID_READ_STATUS        = 0x05,
	TLKDEV_P25Q32H_CMDID_WRITE_ENABLE       = 0x06,
	TLKDEV_P25Q32H_CMDID_FAST_READ_DATA     = 0x0B,
	TLKDEV_P25Q32H_CMDID_SECTOR_ERASE       = 0x20,
	TLKDEV_P25Q32H_CMDID_BLOCK32K_ERASE     = 0x52,
	TLKDEV_P25Q32H_CMDID_BLOCK64K_ERASE     = 0xD8,
	TLKDEV_P25Q32H_CMDID_PAGE_ERASE         = 0x81,
	TLKDEV_P25Q32H_CMDID_MANUFACT_DEVICE_ID = 0x90,
	TLKDEV_P25Q32H_CMDID_JEDEC_DEVICE_ID    = 0x9F,
	TLKDEV_P25Q32H_CMDID_RELEASE_POWER_DOWN = 0xAB,
	TLKDEV_P25Q32H_CMDID_POWER_DOWN         = 0xB9,
	TLKDEV_P25Q32H_CMDID_CHIP_ERASE         = 0xC7,
}TLKDEV_P25Q32H_CMDID_ENUM;




int tlkdev_p25q32h_init(void);

uint32 tlkdev_p25q32h_read(uint32 addr, uint08 *pdata, uint32 length);
uint32 tlkdev_p25q32h_write(uint32 addr, uint08 *pdata, uint32 length);
bool tlkdev_p25q32h_writePage(uint32 addr, uint08 *pdata, uint32 length);

void tlkdev_p25q32h_erasePage(uint32 addr);
void tlkdev_p25q32h_eraseSector(uint32 addr);
void tlkdev_p25q32h_eraseBlock32K(uint32 addr);
void tlkdev_p25q32h_eraseBlock64K(uint32 addr);
void tlkdev_p25q32h_eraseChip(void);


void sys_norflash_write_lba(uint08 * buffer, uint32 lba, uint32 count);
void sys_norflash_erase_lba(uint32 lba, uint32 total_bytes);
void sys_norflash_read_lba(uint08 * buffer, uint32 lba, uint32 count);


void tlkdev_p25q32h_fatfs_read(uint08 *pBuffer, uint32 sector, uint08 count);
void tlkdev_p25q32h_fatfs_write(uint08 *pBuffer, uint32 sector, uint08 count);
void tlkdev_p25q32h_fatfs_earse(uint32 sector, uint08 count);







#endif //#if (TLKDEV_EXT_PUYA_P25Q32H_ENABLE)

#endif //TLKDEV_P25Q32H_H

