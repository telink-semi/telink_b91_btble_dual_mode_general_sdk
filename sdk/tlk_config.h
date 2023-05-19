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
#define TLK_APP_VERSION              0x05010202 //Application, 5.1.2.2
#endif
#define TLK_LIB_VERSION              0x05010200 //Libary, 5.1.2.0
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
#define TLK_CFG_PM_ENABLE            1
#endif
#ifndef TLK_CFG_SYS_ENABLE
#define TLK_CFG_SYS_ENABLE           1
#endif
#ifndef TLK_CFG_GUI_ENABLE
#define TLK_CFG_GUI_ENABLE           0
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
#ifndef TLK_CFG_TEST_ENABLE
#define TLK_CFG_TEST_ENABLE          0
#endif
#ifndef TLK_CFG_COMM_ENABLE
#define TLK_CFG_COMM_ENABLE          1
#endif
#ifndef TLK_CFG_VCD_ENABLE
#define TLK_CFG_VCD_ENABLE           (0 && TLK_CFG_DBG_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_OS_xxx_ENABLE
 * Descr: USB.
 * Items: 
*******************************************************************************/
#ifndef TLK_OS_FREERTOS_ENABLE
#define TLK_OS_FREERTOS_ENABLE       (1 && TLK_CFG_OS_ENABLE)
#endif
#ifndef TLK_OS_HEAP_SIZE
#define TLK_OS_HEAP_SIZE             (1024*16)
#endif

/******************************************************************************
 * Macro: TLK_USB_xxx_ENABLE
 * Descr: USB.
 * Items:
 *     @TLKDEV_CFG_DBG_ENABLE--Enable or disable the function of DEBUG.
 *     @TLKDEV_CFG_VCD_ENABLE--Enable or disable the function of VCD_LOG.
*******************************************************************************/
#ifndef TLK_USB_MSC_ENABLE
#define TLK_USB_MSC_ENABLE           (1 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_USB_UAC_ENABLE
#define TLK_USB_UAC_ENABLE           (0 && TLK_CFG_USB_ENABLE)
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
#ifndef TLK_USB_VCD_ENABLE
#define TLK_USB_VCD_ENABLE           (1 && TLK_USB_UDB_ENABLE)
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
#ifndef TLK_STK_LEH_ENABLE
#define TLK_STK_LEH_ENABLE           (1 && TLK_STK_LE_ENABLE)
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
#define TLK_STK_BTPSM_NUMB           12
#endif
#ifndef TLK_STK_BTCHN_NUMB
#define TLK_STK_BTCHN_NUMB           (TLK_STK_BTACl_NUMB*8)
#endif


/******************************************************************************
 * Macro: TLK_DEV_xxxxx_ENABLE
 * Descr: Enable or disable related devices by configuration.
 * Items:
*******************************************************************************/
#ifndef TLK_DEV_SERIAL_ENABLE
#define TLK_DEV_SERIAL_ENABLE        (1 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_CODEC_ENABLE
#define TLK_DEV_CODEC_ENABLE         (1 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_STORE_ENABLE
#define TLK_DEV_STORE_ENABLE         (1 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_SRAM_ENABLE
#define TLK_DEV_SRAM_ENABLE          (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_LCD_ENABLE
#define TLK_DEV_LCD_ENABLE           (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_SENSOR_ENABLE
#define TLK_DEV_SENSOR_ENABLE        (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_TOUCH_ENABLE
#define TLK_DEV_TOUCH_ENABLE         (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_CHARGE_ENABLE
#define TLK_DEV_CHARGE_ENABLE        (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_BATTERY_ENABLE
#define TLK_DEV_BATTERY_ENABLE       (0 && TLK_CFG_DEV_ENABLE)
#endif
#ifndef TLK_DEV_MFI_ENABLE
#define TLK_DEV_MFI_ENABLE           (0 && TLK_CFG_DEV_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_FS_xxx_ENABLE
 * Descr: 
*******************************************************************************/
#ifndef TLK_FS_FAT_ENABLE
#define TLK_FS_FAT_ENABLE            (1 && TLK_CFG_FS_ENABLE)
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
#ifndef TLK_MDI_BTATT_ENABLE
#define TLK_MDI_BTATT_ENABLE         (1 && TLK_STK_BTP_ENABLE)
#endif
#ifndef TLK_MDI_BTHID_ENABLE
#define TLK_MDI_BTHID_ENABLE         (1 && TLK_STK_BTP_ENABLE)
#endif
#ifndef TLK_MDI_BTHFP_ENABLE
#define TLK_MDI_BTHFP_ENABLE         (1 && TLK_STK_BTP_ENABLE)
#endif
#ifndef TLK_MDI_BTIAP_ENABLE
#define TLK_MDI_BTIAP_ENABLE         (0 && TLK_STK_BTP_ENABLE)
#endif
#ifndef TLK_MDI_BTA2DP_ENABLE
#define TLK_MDI_BTA2DP_ENABLE        (1 && TLK_STK_BTP_ENABLE)
#endif
#ifndef TLK_MDI_AUDIO_ENABLE
#define TLK_MDI_AUDIO_ENABLE         (1 && TLK_CFG_MDI_ENABLE)
#endif
#ifndef TLK_MDI_DEBUG_ENABLE
#define TLK_MDI_DEBUG_ENABLE         (1 && TLK_CFG_DBG_ENABLE)
#endif
#ifndef TLK_MDI_FILE_ENABLE
#define TLK_MDI_FILE_ENABLE          (1 && TLK_CFG_MDI_ENABLE)
#endif
#ifndef TLK_MDI_MP3_ENABLE
#define TLK_MDI_MP3_ENABLE           (1 && TLK_CFG_FS_ENABLE)
#endif
#ifndef TLK_MDI_KEY_ENABLE
#define TLK_MDI_KEY_ENABLE           (0 && TLK_CFG_MDI_ENABLE)
#endif
#ifndef TLK_MDI_LED_ENABLE
#define TLK_MDI_LED_ENABLE           (0 && TLK_CFG_MDI_ENABLE)
#endif
#ifndef TLK_MDI_GUI_ENABLE
#define TLK_MDI_GUI_ENABLE           (0 && TLK_CFG_GUI_ENABLE)
#endif
#ifndef TLK_MDI_USB_ENABLE
#define TLK_MDI_USB_ENABLE           (1 && TLK_CFG_USB_ENABLE)
#endif
#ifndef TLK_MDI_FS_ENABLE
#define TLK_MDI_FS_ENABLE            (1 && TLK_CFG_FS_ENABLE)
#endif
#ifndef TLK_MDI_BATTERY_ENABLE
#define TLK_MDI_BATTERY_ENABLE       (1 && TLK_DEV_BATTERY_ENABLE)
#endif
#ifndef TLK_MDI_AUDPLAY_ENABLE
#define TLK_MDI_AUDPLAY_ENABLE       (1 && TLK_MDI_AUDIO_ENABLE && TLK_MDI_MP3_ENABLE)
#endif
#ifndef TLK_MDI_AUDTONE_ENABLE
#define TLK_MDI_AUDTONE_ENABLE       (1 && TLK_MDI_AUDIO_ENABLE)
#endif
#ifndef TLK_MDI_AUDSRC_ENABLE
#define TLK_MDI_AUDSRC_ENABLE        (1 && TLK_MDI_AUDIO_ENABLE && TLK_MDI_MP3_ENABLE)
#endif
#ifndef TLK_MDI_AUDSNK_ENABLE
#define TLK_MDI_AUDSNK_ENABLE        (1 && TLK_MDI_AUDIO_ENABLE && TLK_STK_BT_ENABLE)
#endif
#ifndef TLK_MDI_AUDSCO_ENABLE
#define TLK_MDI_AUDSCO_ENABLE        (1 && TLK_MDI_AUDIO_ENABLE && TLK_STK_BT_ENABLE)
#endif
#ifndef TLK_MDI_AUDHFP_ENABLE
#define TLK_MDI_AUDHFP_ENABLE        (1 && TLK_MDI_AUDIO_ENABLE && TLK_STK_BT_ENABLE)
#endif
#ifndef TLK_MDI_AUDUAC_ENABLE
#define TLK_MDI_AUDUAC_ENABLE        (1 && TLK_MDI_AUDIO_ENABLE && TLK_USB_UAC_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_MMI_xxx_ENABLE 
 * Descr: 
*******************************************************************************/
#ifndef TLKMMI_STACK_ENABLE
#define TLKMMI_STACK_ENABLE          (1 && TLK_CFG_STK_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_AUDIO_ENABLE
#define TLKMMI_AUDIO_ENABLE          (1 && TLK_MDI_AUDIO_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_BTMGR_ENABLE
#define TLKMMI_BTMGR_ENABLE          (1 && TLK_STK_BTH_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_LEMGR_ENABLE
#define TLKMMI_LEMGR_ENABLE          (1 && TLK_STK_LE_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_LEMST_ENABLE
#define TLKMMI_LEMST_ENABLE          (1 && TLK_STK_LE_ENABLE && TLK_CFG_MMI_ENABLE && TLKAPP_DUALMODE_DONGLE_ENABLE && !TLKMMI_LEMGR_ENABLE)
#endif
#ifndef TLKMMI_PHONE_ENABLE
#define TLKMMI_PHONE_ENABLE          (1 && TLK_STK_BTH_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_SYSTEM_ENABLE
#define TLKMMI_SYSTEM_ENABLE         (1 && TLK_CFG_MMI_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_FILE_ENABLE
#define TLKMMI_FILE_ENABLE           (1 && TLK_CFG_FS_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_VIEW_ENABLE
#define TLKMMI_VIEW_ENABLE           (1 && TLK_CFG_GUI_ENABLE && TLK_CFG_MMI_ENABLE)
#endif
#ifndef TLKMMI_TEST_ENABLE
#define TLKMMI_TEST_ENABLE           (1 && TLK_CFG_TEST_ENABLE && TLK_CFG_MMI_ENABLE)
#endif

/******************************************************************************
 * Macro: TLK_TEST_xxx_ENABLE 
 * Descr: 
*******************************************************************************/
#ifndef TLK_TEST_PTS_ENABLE
#define TLK_TEST_PTS_ENABLE          (1 && TLK_CFG_TEST_ENABLE)
#endif
#ifndef TLK_TEST_RDT_ENABLE
#define TLK_TEST_RDT_ENABLE          (1 && TLK_CFG_TEST_ENABLE)
#endif
#ifndef TLK_TEST_FAT_ENABLE
#define TLK_TEST_FAT_ENABLE          (1 && TLK_CFG_TEST_ENABLE)
#endif
#ifndef TLK_TEST_EMI_ENABLE
#define TLK_TEST_EMI_ENABLE          (1 && TLK_CFG_TEST_ENABLE)
#endif
#ifndef TLK_TEST_BQB_ENABLE
#define TLK_TEST_BQB_ENABLE          (1 && TLK_CFG_TEST_ENABLE)
#endif
#ifndef TLK_TEST_USR_ENABLE
#define TLK_TEST_USR_ENABLE          (0 && TLK_CFG_TEST_ENABLE)
#endif



/******************************************************************************
 * Macro: Flash
 * Descr: 
*******************************************************************************/
#define TLK_CFG_FLASH_PLAY_LIST_ADDR          (0xE6000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_PLAY_LIST_LENS          0x02000 //0xE6000~0xE7FFF
#define TLK_CFG_FLASH_PLAY_INFO_ADDR          (0xEE000+TLK_CFG_FLASH_CAP-0x100000)

#define TLK_CFG_FLASH_OTA_PARAM_ADDR          (0xC0000+TLK_CFG_FLASH_CAP-0x100000) 

#define TLK_CFG_FLASH_SYS_CONFIG_ADDR         (0xC1000+TLK_CFG_FLASH_CAP-0x100000)

#define TLK_CFG_FLASH_PBAP_LIST_ADDR          (0xC2000+TLK_CFG_FLASH_CAP-0x100000) 
#define TLK_CFG_FLASH_PBAP_LIST_LENS          0x0E000 //0xC0000~0xCFFFF

#define TLK_CFG_FLASH_FACTORY_ADDR            (0xE9000+TLK_CFG_FLASH_CAP-0x100000) //Factory Param

#define TLK_CFG_FLASH_VOLUME_ADDR             (0xEA000+TLK_CFG_FLASH_CAP-0x100000) //play,SRC,SNK,SCO,HFP-HF,HFP-AG,Tone-local,Tone-headset


//NOTE: The position of NAME and addr can not change.
#define TLK_CFG_FLASH_LE_NAME_ADDR            (0xED000+TLK_CFG_FLASH_CAP-0x100000) //NAME  -- Bind to the tool and do not change
#define TLK_CFG_FLASH_LE_NAME_LENS            32
#define TLK_CFG_FLASH_BT_NAME_ADDR            (0xED100+TLK_CFG_FLASH_CAP-0x100000) //NAME  -- Bind to the tool and do not change
#define TLK_CFG_FLASH_BT_NAME_LENS            32
#define TLK_CFG_FLASH_LE_ADDR_ADDR            (0xFF000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_BT_ADDR_ADDR            (0xFF100+TLK_CFG_FLASH_CAP-0x100000) //ADDR  -- Bind to the tool and do not change

#define TLK_CFG_FLASH_EQ_TEST_ADDR            (0xEF000+TLK_CFG_FLASH_CAP-0x100000) //music EQ and speech EQ para

#define TLK_CFG_FLASH_BT_PAIR_ADDR0           (0xEB000+TLK_CFG_FLASH_CAP-0x100000) //+0xEC000  UAER DATA , PAIRING INFO ...
#define TLK_CFG_FLASH_BT_PAIR_ADDR1           (0xEC000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_BT_PAIR_NUMB            5 //

//NOTE: TLK_CFG_FLASH_LE_CALIBEATION can not change
#define TLK_CFG_FLASH_LE_CALIBEATION_ADDR     (0xFE000+TLK_CFG_FLASH_CAP-0x100000) //can not change
/* SMP pairing and key information area */
#define TLK_CFG_FLASH_LE_SMP_PAIRING_ADDR     (0xFA000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_LE_SMP_PAIRING_SIZE     (2*4096) //normal 8K + backup 8K = 16K
/* bonding slave information for custom pair area */
#define TLK_CFG_FLASH_LE_ADR_CUSTOM_PAIRING_ADDR     (0xF8000+TLK_CFG_FLASH_CAP-0x100000)
#define TLK_CFG_FLASH_LE_ADR_CUSTOM_PAIRING_SIZE     4096
/* bonding slave GATT service critical information area */
#define TLK_CFG_FLASH_LE_SDP_ATT_ADRR         (0xF6000+TLK_CFG_FLASH_CAP-0x100000) //for master: store peer slave device's ATT handle
#define TLK_CFG_FLASH_LE_SDP_ATT_SIZE         (2*4096) //8K flash for ATT HANLDE storage






#endif //TLK_CONFIG_H

