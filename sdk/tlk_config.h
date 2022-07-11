/********************************************************************************************************
 * @file     tlk_config.h
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

#ifndef TLK_CONFIG_H
#define TLK_CONFIG_H


#include "app/app_config.h"



/******************************************************************************
 * Macro: TLKDEV_FS_ENABLE
 * Descr: 
*******************************************************************************/
#ifndef TLK_APP_VERSION
#define TLK_APP_VERSION              0x05010009 //Application, 5.1.0.0
#endif
#define TLK_LIB_VERSION              0x05010000 //Libary, 5.1.0.0
#define TLK_DRV_VERSION              0x02000000 //Driver, 2.0.0.0
#define TLK_PRT_VERSION              0x0100 //Protocol, 1.0.0

/******************************************************************************
 * Macro: 
 * Descr: 
*******************************************************************************/
#define TLK_CFG_FLASH_CAP            0x200000 //0x80000=512k, 0x100000=1024KB, 0x200000=2048KB, 0x400000=4096KB

/******************************************************************************
 * Macro: 
 * Descr: 
*******************************************************************************/
#ifndef TLK_CFG_FS_ENABLE
#define TLK_CFG_FS_ENABLE            1
#endif
#ifndef TLK_CFG_OS_ENABLE
#define TLK_CFG_OS_ENABLE            0
#endif
#ifndef TLK_CFG_PM_ENABLE
#define TLK_CFG_PM_ENABLE            0
#endif
#ifndef TLK_CFG_WDG_ENABLE
#define TLK_CFG_WDG_ENABLE           0 //WatchDog
#endif
#ifndef TLK_CFG_DEV_ENABLE
#define TLK_CFG_DEV_ENABLE           1 
#endif
#ifndef TLK_CFG_ALG_ENABLE
#define TLK_CFG_ALG_ENABLE           1
#endif
#ifndef TLK_CFG_STK_ENABLE
#define TLK_CFG_STK_ENABLE           1 
#endif
#ifndef TLK_CFG_MDI_ENABLE
#define TLK_CFG_MDI_ENABLE           1 
#endif
#ifndef TLK_CFG_MMI_ENABLE
#define TLK_CFG_MMI_ENABLE           1 
#endif
#ifndef TLK_CFG_DBG_ENABLE
#define TLK_CFG_DBG_ENABLE           1
#endif
#ifndef TLK_CFG_USB_ENABLE
#define TLK_CFG_USB_ENABLE           1
#endif

/******************************************************************************
 * Macro: TLK_USB_xxx_ENABLE
 * Descr: USB.
 * Items:
 *     @TLKDEV_CFG_DBG_ENABLE--Enable or disable the function of DEBUG.
 *     @TLKDEV_CFG_VCD_ENABLE--Enable or disable the function of VCD_LOG.
*******************************************************************************/
#ifndef TLK_USB_VCD_ENABLE
#define TLK_USB_VCD_ENABLE           (0 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_MSC_ENABLE
#define TLK_USB_MSC_ENABLE           (1 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_AUD_ENABLE
#define TLK_USB_AUD_ENABLE           (0 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_UDB_ENABLE
#define TLK_USB_UDB_ENABLE           (1 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_CDC_ENABLE
#define TLK_USB_CDC_ENABLE           (1 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_HID_ENABLE
#define TLK_USB_HID_ENABLE           (0 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_USR_ENABLE
#define TLK_USB_USR_ENABLE           (0 && TLK_CFG_USB_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_ALG_xxx_ENABLE
 * Descr: STK.
*******************************************************************************/
#ifndef TLK_ALG_EQ_ENABLE
#define TLK_ALG_EQ_ENABLE            (0 && TLK_CFG_ALG_ENABLE)
#endif
#ifndef TLK_ALG_EC_ENABLE
#define TLK_ALG_EC_ENABLE            (1 && TLK_CFG_ALG_ENABLE)
#endif
#ifndef TLK_ALG_AGC_ENABLE
#define TLK_ALG_AGC_ENABLE           (1 && TLK_CFG_ALG_ENABLE)
#endif
#ifndef TLK_ALG_AAC_ENABLE
#define TLK_ALG_AAC_ENABLE           (0 && TLK_CFG_ALG_ENABLE)
#endif
#ifndef TLK_ALG_AEC_ENABLE
#define TLK_ALG_AEC_ENABLE           (1 && TLK_CFG_ALG_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_STK_xxx_ENABLE
 * Descr: STK.
*******************************************************************************/
#ifndef TLK_STK_BT_ENABLE
#define TLK_STK_BT_ENABLE            (1 && TLK_CFG_STK_ENABLE)
#endif
#ifndef TLK_STK_LE_ENABLE
#define TLK_STK_LE_ENABLE            (1 && TLK_CFG_STK_ENABLE)
#endif
#ifndef TLK_STK_BTH_ENABLE
#define TLK_STK_BTH_ENABLE           (1 && TLK_STK_BT_ENABLE)
#endif
#ifndef TLK_STK_BTP_ENABLE
#define TLK_STK_BTP_ENABLE           (1 && TLK_STK_BTH_ENABLE)
#endif

#ifndef TLK_STK_BTACl_NUMB
#define TLK_STK_BTACl_NUMB           2
#endif
#ifndef TLK_STK_BTSCO_NUMB
#define TLK_STK_BTSCO_NUMB           1
#endif
#ifndef TLK_STK_BTPSM_NUMB
#define TLK_STK_BTPSM_NUMB           8
#endif
#ifndef TLK_STK_BTCHN_NUMB
#define TLK_STK_BTCHN_NUMB           (TLK_STK_BTACl_NUMB*6)
#endif

/******************************************************************************
 * Macro: TLK_MDI_xxx_ENABLE 
 * Descr: 
*******************************************************************************/
#ifndef TLK_MDI_BTACL_ENABLE
#define TLK_MDI_BTACL_ENABLE         (1 && TLK_STK_BTH_ENABLE)
#endif
#ifndef TLK_MDI_BTINQ_ENABLE
#define TLK_MDI_BTINQ_ENABLE         (1 && TLK_STK_BTH_ENABLE)
#endif
#ifndef TLK_MDI_BTREC_ENABLE
#define TLK_MDI_BTREC_ENABLE         (1 && TLK_MDI_BTACL_ENABLE)
#endif
#ifndef TLK_MDI_HFP_ENABLE
#define TLK_MDI_HFP_ENABLE           (1)
#endif
#ifndef TLK_MDI_FILE_ENABLE
#define TLK_MDI_FILE_ENABLE          (1)
#endif
#ifndef TLK_MDI_AUD_ENABLE
#define TLK_MDI_AUD_ENABLE           (1)
#endif
#ifndef TLK_MDI_MP3_ENABLE
#define TLK_MDI_MP3_ENABLE           (1 && TLK_CFG_FS_ENABLE)
#endif
#ifndef TLK_MDI_HID_ENABLE
#define TLK_MDI_HID_ENABLE           (1)
#endif
#ifndef TLK_MDI_USB_ENABLE
#define TLK_MDI_USB_ENABLE           (1 && TLK_CFG_USB_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_DEV_xxxxx_ENABLE
 * Descr: Enable or disable related devices by configuration.
 * Items:
 *     @TLK_DEV_XTSD04G_ENABLE--Enable or disable NAND Flash (XTSD04G).
 *     @TLK_DEV_XT2602E_ENABLE--Enable or disable NAND Flash (XT26G02E).
*******************************************************************************/
#ifndef TLK_DEV_XTSD04G_ENABLE
#define TLK_DEV_XTSD04G_ENABLE       (1 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_XT2602E_ENABLE
#define TLK_DEV_XT2602E_ENABLE       (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_SERIAL_ENABLE
#define TLK_DEV_SERIAL_ENABLE        (1)
#endif

#ifndef TLK_DEV_MIC_BUFF_SIZE
#define TLK_DEV_MIC_BUFF_SIZE        (1024*2)
#endif
#ifndef TLK_DEV_SPK_BUFF_SIZE
#define TLK_DEV_SPK_BUFF_SIZE        (1024*4) //If SRC is enabled, the value must be greater than or equal to 4K
#endif

/******************************************************************************
 * Macro: TLK_FS_xxx_ENABLE
 * Descr: 
*******************************************************************************/
#ifndef TLK_FS_FAT_ENABLE
#define TLK_FS_FAT_ENABLE            (1 && TLK_CFG_FS_ENABLE && TLK_DEV_XTSD04G_ENABLE)
#endif
#ifndef TLK_FS_PFF_ENABLE
#define TLK_FS_PFF_ENABLE            (0 && TLK_CFG_FS_ENABLE && TLK_DEV_XT2602E_ENABLE)
#endif


/******************************************************************************
 * Macro: Flash
 * Descr: 
*******************************************************************************/
#define TLK_CFG_FLASH_PLAY_LIST_ADDR          (0xE6000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_PLAY_LIST_LENS          0x02000 //0xE6000~0xE7FFF
#define TLK_CFG_FLASH_PLAY_INFO_ADDR          (0xEE000+TLK_CFG_FLASH_CAP-0x100000)

#define TLK_CFG_FLASH_OTA_PARAM_ADDR          (0xC0000+TLK_CFG_FLASH_CAP-0x100000) 


#define TLK_CFG_FLASH_PBAP_LIST_ADDR          (0xC1000+TLK_CFG_FLASH_CAP-0x100000) 
#define TLK_CFG_FLASH_PBAP_LIST_LENS          0x0F000 //0xC0000~0xCFFFF

#define TLK_CFG_FLASH_FACTORY_ADDR            (0xE9000+TLK_CFG_FLASH_CAP-0x100000) //Factory Param

#define TLK_CFG_FLASH_VOLUME_ADDR             (0xEA000+TLK_CFG_FLASH_CAP-0x100000) //play,SRC,SNK,SCO,HFP-HF,HFP-AG,Tone-local,Tone-headset

//#define TLK_CFG_FLASH_BT_NAME_ADDR            (0xED000+TLK_CFG_FLASH_CAP-0x100000) //NAME  -- Bind to the tool and do not change
//#define TLK_CFG_FLASH_BT_ADDR_ADDR            (0xFF000+TLK_CFG_FLASH_CAP-0x100000) //ADDR  -- Bind to the tool and do not change
#define TLK_CFG_FLASH_BT_NAME_ADDR            (0xED000) //NAME  -- Bind to the tool and do not change
#define TLK_CFG_FLASH_BT_ADDR_ADDR            (0xFF000) //ADDR  -- Bind to the tool and do not change


#define TLK_CFG_FLASH_EQ_TEST_ADDR            (0xEF000+TLK_CFG_FLASH_CAP-0x100000) //music EQ and speech EQ para

#define TLK_CFG_FLASH_BT_PAIR_ADDR0           (0xEB000+TLK_CFG_FLASH_CAP-0x100000) //+0xEC000  UAER DATA , PAIRING INFO ...
#define TLK_CFG_FLASH_BT_PAIR_ADDR1           (0xEC000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_BT_PAIR_NUMB            5 //

//BLE: F6000~FBFFF

/******************************************************************************
 * Macro: TLKDEV_CFG_DBG_ENABLE, TLKDEV_CFG_VCD_ENABLE
 * Descr: Enable or disable the function of DEBUG or VCD_LOG by configuration.
 * Items:
 *     @TLKDEV_CFG_DBG_ENABLE--Enable or disable the function of DEBUG.
 *     @TLKDEV_CFG_VCD_ENABLE--Enable or disable the function of VCD_LOG.
*******************************************************************************/
#ifndef TLKBTH_CFG_DBG_ENABLE
#define TLKBTH_CFG_DBG_ENABLE                 (1 && TLK_CFG_DBG_ENABLE)
#endif 
#ifndef TLKBTH_CFG_VCD_ENABLE
#define TLKBTH_CFG_VCD_ENABLE                 (0 && TLK_USB_VCD_ENABLE)
#endif
#ifndef TLKBTP_CFG_DBG_ENABLE
#define TLKBTP_CFG_DBG_ENABLE                 (1 && TLK_CFG_DBG_ENABLE)
#endif
#ifndef TLKBTP_CFG_VCD_ENABLE
#define TLKBTP_CFG_VCD_ENABLE                 (0 && TLK_USB_VCD_ENABLE)
#endif
#ifndef TLKALG_CFG_DBG_ENABLE
#define TLKALG_CFG_DBG_ENABLE                 (1 && TLK_CFG_DBG_ENABLE)
#endif
#ifndef TLKALG_CFG_VCD_ENABLE
#define TLKALG_CFG_VCD_ENABLE                 (0 && TLK_USB_VCD_ENABLE)
#endif
#ifndef TLKAPI_CFG_DBG_ENABLE
#define TLKAPI_CFG_DBG_ENABLE                 (1 && TLK_CFG_DBG_ENABLE)
#endif
#ifndef TLKDEV_CFG_DBG_ENABLE
#define TLKDEV_CFG_DBG_ENABLE                 (0 && TLK_CFG_DBG_ENABLE)
#endif
#ifndef TLKDEV_CFG_VCD_ENABLE
#define TLKDEV_CFG_VCD_ENABLE                 (0 && TLK_USB_VCD_ENABLE)
#endif
#ifndef TLKMDI_CFG_DBG_ENABLE
#define TLKMDI_CFG_DBG_ENABLE                 (1 && TLK_CFG_DBG_ENABLE)
#endif
#ifndef TLKMMI_CFG_DBG_ENABLE
#define TLKMMI_CFG_DBG_ENABLE                 (1 && TLK_CFG_DBG_ENABLE)
#endif




#endif //TLK_CONFIG_H

