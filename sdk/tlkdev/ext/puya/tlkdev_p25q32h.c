/********************************************************************************************************
 * @file     tlkdev_p25q32h.c
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
#if (TLKDEV_EXT_PUYA_P25Q32H_ENABLE)
#include "tlkdev/ext/puya/tlkdev_p25q32h.h"


static void p25q32s_Interface_Init(SPI_GPIO_GroupTypeDef pin);
static void p25q32s_Interface_Read(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length);
static void p25q32s_Interface_Write(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length);

void tlkdev_p25q32h_setPowerState(bool isRelease);
bool tlkdev_p25q32h_writePage(uint32 addr, uint08 *pdata, uint32 length);

uint08 tlkdev_p25q32h_get_status(void);
bool tlkdev_p25q32h_busy_wait(void);
void tlkdev_p25q32h_write_command(uint08 command);
void tlkdev_p25q32h_write_enable(void);
void tlkdev_p25q32h_write_disable(void);
uint16 tlkdev_p25q32h_getChipId(void);
uint32 tlkdev_p25q32h_getJedecId(void);



int tlkdev_p25q32h_init(void)
{
	p25q32s_Interface_Init(SPI_GPIO_GROUP_A2A3A4D6);
	tlkdev_p25q32h_setPowerState(true);
	sleep_ms(200);
	return TLK_ENONE;
}

uint32 tlkdev_p25q32h_read(uint32 addr, uint08 *pdata, uint32 length)
{
	 uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_READ_DATA, addr >> 16, addr >> 8, addr >> 0,};
	 p25q32s_Interface_Read(cmd_data, sizeof(cmd_data), pdata, length);
	 return length;
}
uint32 tlkdev_p25q32h_write(uint32 addr, uint08 *pdata, uint32 length)
{
	 uint32 feasible_size    = length;
	 uint32 currentAddress   = addr;
	 uint32 currentEndOfPage = (currentAddress / TLKDEV_P25Q32H_PAGE_SIZE + 1) * TLKDEV_P25Q32H_PAGE_SIZE - 1;

	 if(length > (TLKDEV_P25Q32H_TOTAL_SIZE - addr)) feasible_size = TLKDEV_P25Q32H_TOTAL_SIZE - addr;

	 uint32 bytes_left_to_send = feasible_size;
	 uint32 bytes_written      = 0;

	 while(bytes_written < feasible_size){
		if(currentAddress + bytes_left_to_send > currentEndOfPage) bytes_left_to_send = currentEndOfPage - currentAddress + 1;

		if(tlkdev_p25q32h_writePage(currentAddress, pdata + bytes_written, bytes_left_to_send) == false) return 0;

		bytes_written     += bytes_left_to_send;
		currentAddress     = currentEndOfPage + 1;
		currentEndOfPage  += TLKDEV_P25Q32H_PAGE_SIZE;
		bytes_left_to_send = feasible_size - bytes_written;
	 }
	 return bytes_written;
}
bool tlkdev_p25q32h_writePage(uint32 addr, uint08 *pdata, uint32 length)
{
	 tlkdev_p25q32h_write_enable();

	 uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_PAGE_PROGRAM, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), pdata, length);
	 return tlkdev_p25q32h_busy_wait();
}


void tlkdev_p25q32h_erasePage(uint32 addr)
{
	 tlkdev_p25q32h_write_enable();

	 uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_PAGE_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 tlkdev_p25q32h_busy_wait();
}
void tlkdev_p25q32h_eraseSector(uint32 addr)
{
	 tlkdev_p25q32h_write_enable();

	 uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_SECTOR_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 tlkdev_p25q32h_busy_wait();
}
void tlkdev_p25q32h_eraseBlock32K(uint32 addr)
{
	 tlkdev_p25q32h_write_enable();

	 uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_BLOCK32K_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 tlkdev_p25q32h_busy_wait();
}
void tlkdev_p25q32h_eraseBlock64K(uint32 addr)
{
	 tlkdev_p25q32h_write_enable();

	 uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_BLOCK64K_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 tlkdev_p25q32h_busy_wait();
}


void tlkdev_p25q32h_eraseChip(void)
{
	 tlkdev_p25q32h_write_enable();
	 tlkdev_p25q32h_write_command(TLKDEV_P25Q32H_CMDID_CHIP_ERASE);
	 while(tlkdev_p25q32h_get_status() & 0x01);
}



uint08 tlkdev_p25q32h_get_status(void)
{
	 uint08 status;
	 uint08 command = TLKDEV_P25Q32H_CMDID_READ_STATUS;
	 p25q32s_Interface_Read(&command, sizeof(command), &status, sizeof(status));
	 return status;
}

bool tlkdev_p25q32h_busy_wait(void)
{
	 for(int index = 0; index < TLKDEV_P25Q32H_BUSY_TIMEOUT_VALUE; index ++){
		 if(!(tlkdev_p25q32h_get_status() & 0x01)) return true;
	 }
	 return false;
}

void tlkdev_p25q32h_write_command(uint08 command)
{
	 p25q32s_Interface_Write(&command, sizeof(command), NULL, 0);
	 tlkdev_p25q32h_busy_wait();
}

void tlkdev_p25q32h_write_enable(void)
{
	 tlkdev_p25q32h_write_command(TLKDEV_P25Q32H_CMDID_WRITE_ENABLE);
}

void tlkdev_p25q32h_write_disable(void)
{
	 tlkdev_p25q32h_write_command(TLKDEV_P25Q32H_CMDID_WRITE_DISABLE);
}

uint16 tlkdev_p25q32h_getChipId(void)
{
     uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_MANUFACT_DEVICE_ID, 0x00, 0x00, 0x00,};
     uint08 chip_id[2]; // [0] manufacturer_id  [1] device_id
     p25q32s_Interface_Read(cmd_data, sizeof(cmd_data), chip_id, sizeof(chip_id));
     return (uint16)((chip_id[0] << 8) | chip_id[1]);
}

uint32 tlkdev_p25q32h_getJedecId(void)
{
     uint08 cmd_data[] = {TLKDEV_P25Q32H_CMDID_JEDEC_DEVICE_ID, 0x00, 0x00, 0x00,};
     uint08 jedec_id[3]; // [0] manufacturer_id  [1] memory_type_id  [2] capacity_id
     p25q32s_Interface_Read(cmd_data, sizeof(cmd_data), jedec_id, sizeof(jedec_id));
     return (uint32)((jedec_id[0] << 16) | (jedec_id[1] << 8) | jedec_id[2]);
}

void tlkdev_p25q32h_setPowerState(bool isRelease)
{
	 if(isRelease){
		 tlkdev_p25q32h_write_command(TLKDEV_P25Q32H_CMDID_RELEASE_POWER_DOWN);
	 }else{
		 tlkdev_p25q32h_write_command(TLKDEV_P25Q32H_CMDID_POWER_DOWN);
	 }
}








static void p25q32s_Interface_Init(SPI_GPIO_GroupTypeDef pin)
{
	spi_master_init(0x00,SPI_MODE0);
	spi_master_gpio_set(pin);
	spi_masterCSpin_select(TLKDEV_P25Q32H_SPI_CS_GPIO);
	gpio_setup_up_down_resistor(TLKDEV_P25Q32H_SPI_CS_GPIO,PM_PIN_PULLUP_1M);
}
static void p25q32s_Interface_Write(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length)
{
	spi_write(cmd_data, cmd_length, pdata, length, TLKDEV_P25Q32H_SPI_CS_GPIO);
}
static void p25q32s_Interface_Read(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length)
{
	 spi_read(cmd_data, cmd_length, pdata, length, TLKDEV_P25Q32H_SPI_CS_GPIO);
}




void sys_norflash_write_lba(uint08 * buffer, uint32 lba, uint32 count)
{
//	tlkdev_p25q32h_write(lba * FATFS_FLASH_SECTOR_SIZE, buffer, FATFS_FLASH_SECTOR_SIZE*count);
	tlkdev_p25q32h_writePage(lba * FATFS_FLASH_SECTOR_SIZE, buffer, 256);
	tlkdev_p25q32h_writePage(lba * FATFS_FLASH_SECTOR_SIZE+256, buffer+256, 256);
}

void sys_norflash_erase_lba(uint32 lba, uint32 total_bytes)
{
	uint32 erase_current = (lba* FATFS_FLASH_SECTOR_SIZE);
	uint32 erase_end = erase_current + total_bytes;
//	printf("erase_current,0x%lx,erase_end,0x%lx\n\r",erase_current,erase_end);
	while(erase_current < erase_end)
	{
		if((!(erase_current & 0xffff)) && ((erase_current + 0x10000) <= erase_end))//64k align
		{
			tlkdev_p25q32h_eraseBlock64K(erase_current);
//			printf("flash_erase_64kblock,0x%lx\n\r",erase_current);
			erase_current += 0x10000;
		}
		else if((!(erase_current & 0x7fff))&& ((erase_current + 0x8000) <= erase_end))//32k align
		{
			tlkdev_p25q32h_eraseBlock32K(erase_current);
//			printf("flash_erase_32kblock,0x%lx\n\r",erase_current);
			erase_current += 0x8000;
		}
		else if((!(erase_current & 0xfff))&& ((erase_current + 0x1000) <= erase_end))//4k align
		{
			tlkdev_p25q32h_eraseSector(erase_current);
//			printf("tlkdev_p25q32h_eraseSector,0x%lx\n\r",erase_current);
			erase_current += 0x1000;
		}
		else if((!(erase_current & 0xfff)) && ((erase_current + 0x1000) > erase_end))
		{
			uint08 temp[4096];
			tlkdev_p25q32h_read(erase_end, &(temp[erase_end & 0xfff]), (0x1000 - (erase_end & 0xfff)));
			tlkdev_p25q32h_eraseSector(erase_current & 0xfffff000);
//			printf("tlkdev_p25q32h_eraseSector,0x%lx,erase_current,0x%lx,erase_end,0x%lx\n\r",erase_current & 0xfffff000,erase_current,erase_end);
			tlkdev_p25q32h_writePage(erase_end, &(temp[erase_end & 0xfff]), (0x1000 - (erase_end & 0xfff)));
//			printf("flash_write_page,addr:0x%lx,len:0x%lx\n\r",erase_end,(0x1000 - (erase_end & 0xfff)));
			erase_current += 0x1000;
		}
		else if((erase_current & 0xfff)&&(((erase_current + 0x1000)&0xfffff000) <= erase_end))
		{
			uint08 temp[4096];
			tlkdev_p25q32h_read((erase_current & 0xfffff000), temp, erase_current & 0xfff);
			tlkdev_p25q32h_eraseSector(erase_current & 0xfffff000);
//			printf("tlkdev_p25q32h_eraseSector,0x%lx,erase_current,0x%lx,erase_end,0x%lx\n\r",erase_current & 0xfffff000,erase_current,erase_end);
			tlkdev_p25q32h_writePage(erase_current & 0xfffff000, temp, erase_current & 0xfff);
//			printf("flash_write_page,addr:0x%lx,len:0x%lx\n\r",erase_current & 0xfffff000,erase_current & 0xfff);
			erase_current = ((erase_current + 0x1000)&0xfffff000);
		}
		else if((erase_current < erase_end) && ((erase_current + 0x1000) > erase_end))
		{
			uint08 temp[4096];
			tlkdev_p25q32h_read((erase_current & 0xfffff000), temp, erase_current & 0xfff);
			tlkdev_p25q32h_read(erase_end, &(temp[erase_end & 0xfff]), (0x1000 - (erase_end & 0xfff)));
			tlkdev_p25q32h_eraseSector(erase_current);
//			printf("tlkdev_p25q32h_eraseSector,0x%lx,erase_current,0x%lx,erase_end,0x%lx\n\r",erase_current & 0xfffff000,erase_current,erase_end);
			tlkdev_p25q32h_writePage(erase_current & 0xfffff000, temp, erase_current & 0xfff);
//			printf("flash_write_page,addr:0x%lx,len:0x%lx\n\r",erase_current & 0xfffff000,erase_current & 0xfff);
			tlkdev_p25q32h_writePage(erase_end, &(temp[erase_end & 0xfff]), (0x1000 - (erase_end & 0xfff)));
			erase_current += 0x1000;
		}
	}
}
void sys_norflash_read_lba(uint08 * buffer, uint32 lba, uint32 count)
{
	tlkdev_p25q32h_read(lba * FATFS_FLASH_SECTOR_SIZE, buffer, FATFS_FLASH_SECTOR_SIZE*count);
}


void tlkdev_p25q32h_msc_write(uint08 *pBuffer, uint32 address, uint32 writeBytes)
{
	
}

 

void tlkdev_p25q32h_fatfs_read(uint08 *pBuffer, uint32 sector, uint08 count)
{
	uint08 indexI, indexJ;
	uint32 baseAddr = sector*4096;
	uint08 *pData = pBuffer;
	for(indexI=0; indexI<count; indexI++){
		tlkdev_p25q32h_read(baseAddr, pData, 4096);
		baseAddr += 4096;
		pData += 4096;
	}
}

void tlkdev_p25q32h_fatfs_write(uint08 *pBuffer, uint32 sector, uint08 count)
{ 
	uint08 indexI, indexJ;
	uint32 baseAddr = sector*4096;
	uint08 *pData = pBuffer;
	for(indexI=0; indexI<count; indexI++){
		tlkdev_p25q32h_eraseSector(baseAddr);
		for(indexJ=0; indexJ<16; indexJ++){
			tlkdev_p25q32h_writePage(baseAddr, pData, 256);
			baseAddr += 256;
			pData += 256;
		}
	}
}
void tlkdev_p25q32h_fatfs_earse(uint32 sector, uint08 count)
{
	uint08 indexI;
	uint32 baseAddr = sector*4096;
	for(indexI=0; indexI<count; indexI++){
		tlkdev_p25q32h_eraseSector(baseAddr);
		baseAddr += 4096;
	}
}


#endif //#if (TLKDEV_EXT_PUYA_TLKDEV_P25Q32H_ENABLE)

