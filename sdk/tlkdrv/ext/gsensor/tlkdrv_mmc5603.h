/********************************************************************************************************
 * @file	tlkdrv_mmc5603.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef TLKDRV_GEO_MMC5603_H
#define TLKDRV_GEO_MMC5603_H


#if (TLKDRV_GSENSOR_MMC5603_ENABLE)


#define TLKDRV_MMC5603_DBG_FLAG     TLKDRV_GSENSOR_DBG_FLAG//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_MMC5603_DBG_SIGN     TLKDRV_GSENSOR_DBG_SIGN//"[GEO]"


#define TLKDRV_MMC5603_I2C_SLAVE_ADDR			MMC5603_7BITI2C_ADDRESS
#define TLKDRV_MMC5603_CLOCK_SPEED            	(1000*1000) //1M

#define TLKDRV_MMC5603_SCL_PIN                	GPIO_PE1
#define TLKDRV_MMC5603_SDA_PIN                	GPIO_PE3


typedef struct{
	uint08 isInit;
	uint08 isOpen;
	bool   irqRdy;
}tlkdrv_mmc5603_t;


#endif //#if (TLKDRV_GSENSOR_MMC5603_ENABLE)

#endif //TLKDRV_GEO_MMC5603_H

