/********************************************************************************************************
 * @file     tlkdev_xtsd01g.h
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

#ifndef TLKDEV_XTSD01G_H
#define TLKDEV_XTSD01G_H


#if (TLK_DEV_XTSD01G_ENABLE)


#define TLKDEV_XTSD01G_POWER_PIN        GPIO_PB6

#define TLKDEV_XTSD01G_DMA_ENABLE       1
#if (TLKDEV_XTSD01G_DMA_ENABLE)
	#define TLKDEV_XTSD01G_DMA_CHN      DMA6
#endif

#define TLKDEV_XTSD01G_SPI_HSPI         1
#define TLKDEV_XTSD01G_SPI_PSPI         0
#define TLKDEV_XTSD01G_SPI_MODULE       TLKDEV_XTSD01G_SPI_PSPI
#define TLKDEV_XTSD01G_SPI_CS_PIN       GPIO_PC4


#define TLKDEV_XTSD01G_FPP_CLK          24000000
#define TLKDEV_XTSD01G_FOD_CLK          240000


#define TLKDEV_XTSD01G_TYPE_NONE     0X00
#define TLKDEV_XTSD01G_TYPE_MMC      0X01
#define TLKDEV_XTSD01G_TYPE_V1       0X02
#define TLKDEV_XTSD01G_TYPE_V2       0X04
#define TLKDEV_XTSD01G_TYPE_V2HC     0X06

#define TLKDEV_XTSD01G_CMD_00        0
#define TLKDEV_XTSD01G_CMD_01        1
#define TLKDEV_XTSD01G_CMD_08        8
#define TLKDEV_XTSD01G_CMD_09        9
#define TLKDEV_XTSD01G_CMD_10        10
#define TLKDEV_XTSD01G_CMD_12        12
#define TLKDEV_XTSD01G_CMD_16        16
#define TLKDEV_XTSD01G_CMD_17        17
#define TLKDEV_XTSD01G_CMD_18        18
#define TLKDEV_XTSD01G_CMD_23        23
#define TLKDEV_XTSD01G_CMD_24        24
#define TLKDEV_XTSD01G_CMD_25        25
#define TLKDEV_XTSD01G_CMD_32        32
#define TLKDEV_XTSD01G_CMD_33        33
#define TLKDEV_XTSD01G_CMD_38        38
#define TLKDEV_XTSD01G_CMD_41        41
#define TLKDEV_XTSD01G_CMD_55        55
#define TLKDEV_XTSD01G_CMD_58        58
#define TLKDEV_XTSD01G_CMD_59        59

#define TLKDEV_XTSD01G_MSD_DATA_OK              0x05
#define TLKDEV_XTSD01G_MSD_DATA_CRC_ERR         0x0B
#define TLKDEV_XTSD01G_MSD_DATA_WRITE_ERR       0x0D
#define TLKDEV_XTSD01G_MSD_DATA_OTHER_ERR       0xFF

#define TLKDEV_XTSD01G_MSD_RESPONSE_NO_ERR      0x00
#define TLKDEV_XTSD01G_MSD_IN_IDLE_STATE        0x01
#define TLKDEV_XTSD01G_MSD_ERASE_RESET          0x02
#define TLKDEV_XTSD01G_MSD_ILLEGAL_COMMAND      0x04
#define TLKDEV_XTSD01G_MSD_COM_CRC_ERR          0x08
#define TLKDEV_XTSD01G_MSD_ERASE_SEQUENCE_ERR   0x10
#define TLKDEV_XTSD01G_MSD_ADDRESS_ERR          0x20
#define TLKDEV_XTSD01G_MSD_PARAMETER_ERR        0x40
#define TLKDEV_XTSD01G_MSD_RESPONSE_FAILURE     0xFF






int tlkdev_xtsd01g_init(void);

bool tlkdev_xtsd01g_powerIsOn(void);
void tlkdev_xtsd01g_powerOn(void);
void tlkdev_xtsd01g_powerOff(void);
void tlkdev_xtsd01g_shutDown(void);

int tlkdev_xtsd01g_read(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
int tlkdev_xtsd01g_write(uint08 *pData, uint32 blkOffs, uint16 blkNumb);

int tlkdev_xtsd01g_readBlock(uint08 *pBuff, uint32 blkOffs); //512
int tlkdev_xtsd01g_writeBlock(uint08 *pData, uint32 blkOffs);

uint tlkdev_xtsd01g_getBlockCount(void);
void tlkdev_xtsd01g_eraseBlocks(uint32 blkOffs, uint32 blkNumb);

bool tlkdev_xtsd01g_getCID(uint08 *cid_data);
bool tlkdev_xtsd01g_getCSD(uint08 *csd_data);





#define TLKDEV_XTSD01G_DISK_BLOCK_SIZE       512
#define TLKDEV_XTSD01G_DISK_BLOCK_NUMB       0x0003E000 //128M-2M=126M
#define TLKDEV_XTSD01G_DISK_CLUSTER_SIZE     (32*1024)
#define TLKDEV_XTSD01G_DISK_CLUSTER_NUMB     (TLKDEV_XTSD01G_DISK_CLUSTER_SIZE/TLKDEV_XTSD01G_DISK_BLOCK_SIZE)

#define TLKDEV_XTSD01G_DISK_DBR_OFFSET       (4096) //(2*1024*1024)/512
#define TLKDEV_XTSD01G_DISK_FAT_COPIES       2

#define TLKDEV_XTSD01G_DISK_RSV_NUMB         8
#define TLKDEV_XTSD01G_DISK_FAT_NUMB         TLKDEV_XTSD01G_DISK_CLUSTER_NUMB

int tlkdev_xtsd01g_diskInit(void);
int tlkdev_xtsd01g_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
int tlkdev_xtsd01g_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb);
int tlkdev_xtsd01g_diskFormat(void);




#endif

#endif //TLKDEV_XTSD01G_H

