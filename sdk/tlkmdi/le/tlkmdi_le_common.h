/********************************************************************************************************
 * @file     blt_common.h
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

#ifndef BLT_COMMON_H_
#define BLT_COMMON_H_


#include "app/app_config.h"
#include "tlk_config.h"
#include "drivers.h"




/**
 * @brief	1 M Flash MAC address and calibration data area
 */
#define	CFG_ADR_MAC_1M_FLASH		   							0xFF000
#define	CFG_ADR_CALIBRATION_1M_FLASH							0xFE000

/**
 * @brief	2 M Flash MAC address and calibration data area
 */
#define	CFG_ADR_MAC_2M_FLASH		   							0x1FF000
#define	CFG_ADR_CALIBRATION_2M_FLASH							0x1FE000


/**
 * @brief	4 M Flash MAC address and calibration data area
 */
#define	CFG_ADR_MAC_4M_FLASH		   							0x3FF000
#define	CFG_ADR_CALIBRATION_4M_FLASH							0x3FE000





/**
 * @brief	Flash size type supported by this SDK
 */
#define		FLASH_SIZE_1M			   							0x100000
#define		FLASH_SIZE_2M			   							0x200000


/**
 * @brief	Flash size default configuration(user can change in app_config.h)
 */
#if (TLK_CFG_FLASH_CAP == 0x100000)
#define		FLASH_SIZE_CONFIG		   							FLASH_SIZE_1M
#elif (TLK_CFG_FLASH_CAP == 0x200000)
#define		FLASH_SIZE_CONFIG		   							FLASH_SIZE_2M
#elif (TLK_CFG_FLASH_CAP == 0x400000)
#define		FLASH_SIZE_CONFIG		   							FLASH_SIZE_4M
#elif (TLK_CFG_FLASH_CAP == 0x80000)
#define		FLASH_SIZE_CONFIG		   							FLASH_SIZE_512K
#else
#error "flash size not support"
#endif



/**
 * @brief	Flash using area default Configuration, user can change some of them in app_config.h
 * 			CFG_ADR_MAC:  		  BLE MAC address stored in flash, can not change this value
 * 			CFG_ADR_CALIBRATION:  some calibration data stored in flash, can not change this value
 * 			FLASH_ADR_SMP_PAIRING & FLASH_SMP_PAIRING_MAX_SIZE:
 * 									If Slave or Master SMP enable, use 16K flash for SMP pairing information storage.
 * 									First 8K is for normal use, second 8K is a backup to guarantee SMP information never lose.
 * 									use API blc_smp_configPairingSecurityInfoStorageAddressAndSize(FLASH_ADR_SMP_PAIRING, FLASH_SMP_PAIRING_MAX_SIZE)
 * 									to set the two value.
 * 			FLASH_ADR_CUSTOM_PAIRING & FLASH_CUSTOM_PAIRING_MAX_SIZE:
 * 									If master role is used but master SMP not used, use this flash area to store bonding slave information for custom pair.
 * 			FLASH_SDP_ATT_ADRRESS & FLASH_SDP_ATT_MAX_SIZE
 * 									If master role use service discovery, use this flash area to store some critical information of peer GATT server.

 */
#if(FLASH_SIZE_CONFIG == FLASH_SIZE_1M)

	/* MAC and calibration data area */
	#define	CFG_ADR_MAC		   									CFG_ADR_MAC_1M_FLASH			//can not change this value
	#define	CFG_ADR_CALIBRATION		   							CFG_ADR_CALIBRATION_1M_FLASH	//can not change this value

	/* SMP pairing and key information area */
	#ifndef FLASH_ADR_SMP_PAIRING
	#define FLASH_ADR_SMP_PAIRING         						0xFA000
	#endif

	#ifndef FLASH_SMP_PAIRING_MAX_SIZE
	#define FLASH_SMP_PAIRING_MAX_SIZE         					(2*4096)  //normal 8K + backup 8K = 16K
	#endif


	/* bonding slave information for custom pair area */
	#ifndef FLASH_ADR_CUSTOM_PAIRING
	#define FLASH_ADR_CUSTOM_PAIRING         					0xF8000
	#endif

	#ifndef FLASH_CUSTOM_PAIRING_MAX_SIZE
	#define FLASH_CUSTOM_PAIRING_MAX_SIZE     					4096
	#endif


	/* bonding slave GATT service critical information area */
	#ifndef FLASH_SDP_ATT_ADRRESS
	#define FLASH_SDP_ATT_ADRRESS         						0xF6000    //for master: store peer slave device's ATT handle
	#endif

	#ifndef FLASH_SDP_ATT_MAX_SIZE
	#define FLASH_SDP_ATT_MAX_SIZE         						(2*4096)   //8K flash for ATT HANLDE storage
	#endif

#elif(FLASH_SIZE_CONFIG == FLASH_SIZE_2M)
/* MAC and calibration data area */
	#define	CFG_ADR_MAC		   									CFG_ADR_MAC_2M_FLASH			//can not change this value
	#define	CFG_ADR_CALIBRATION		   							CFG_ADR_CALIBRATION_2M_FLASH	//can not change this value

	/* SMP pairing and key information area */
	#ifndef FLASH_ADR_SMP_PAIRING
	#define FLASH_ADR_SMP_PAIRING         						0x1FA000
	#endif

	#ifndef FLASH_SMP_PAIRING_MAX_SIZE
	#define FLASH_SMP_PAIRING_MAX_SIZE         					(2*4096)  //normal 8K + backup 8K = 16K
	#endif


	/* bonding slave information for custom pair area */
	#ifndef FLASH_ADR_CUSTOM_PAIRING
	#define FLASH_ADR_CUSTOM_PAIRING         					0x1F8000
	#endif

	#ifndef FLASH_CUSTOM_PAIRING_MAX_SIZE
	#define FLASH_CUSTOM_PAIRING_MAX_SIZE     					4096
	#endif


	/* bonding slave GATT service critical information area */
	#ifndef FLASH_SDP_ATT_ADRRESS
	#define FLASH_SDP_ATT_ADRRESS         						0x1F6000    //for master: store peer slave device's ATT handle
	#endif

	#ifndef FLASH_SDP_ATT_MAX_SIZE
	#define FLASH_SDP_ATT_MAX_SIZE         						(2*4096)   //8K flash for ATT HANLDE storage
	#endif
#endif





/** Calibration Information FLash Address Offset of  CFG_ADR_CALIBRATION_xx_FLASH ***/
#define		CALIB_OFFSET_CAP_INFO								0x00
#define		CALIB_OFFSET_TP_INFO								0x40

#define		CALIB_OFFSET_ADC_VREF								0xC0

#define		CALIB_OFFSET_FIRMWARE_SIGNKEY						0x180




extern	const u8 	telink_adv_trigger_pairing[6];
extern	const u8 	telink_adv_trigger_unpair[6];



extern u32 flash_sector_mac_address;
extern u32 flash_sector_calibration;

extern const u8 vendor_OtaUUID[];


/**
 * @brief		This function is used to enable the external crystal capacitor
 * @param[in]	en - enable the external crystal capacitor
 * @return      none
 */
static inline void blc_app_setExternalCrystalCapEnable(u8  en)
{
//	blt_miscParam.ext_cap_en = en;
	analog_write(0x8a, analog_read(0x8a) | 0x80);//disable internal cap
}


/**
 * @brief		This function is used to load customized parameters from flash sector for application
 * @param[in]	none
 * @return      none
 */
static inline void blc_app_loadCustomizedParameters(void)
{
//	 if(!blt_miscParam.ext_cap_en)
	 {
		 //customize freq_offset adjust cap value, if not customized, default ana_8A is 0x60
		 //for 1M  Flash, flash_sector_calibration equals to 0xFE000
		 if(flash_sector_calibration){
			 u8 cap_frqoft;
			 flash_read_page(flash_sector_calibration + CALIB_OFFSET_CAP_INFO, 1, &cap_frqoft);
			 if( cap_frqoft != 0xff ){
				 analog_write_reg8(0x8A, (analog_read_reg8(0x8A) & 0xc0)|(cap_frqoft & 0x3f));
			 }
		 }
	 }
}

/**
 * @brief		This function can automatically recognize the flash size,
 * 				and the system selects different customized sector according
 * 				to different sizes.
 * @param[in]	none
 * @return      none
 */
void blc_readFlashSize_autoConfigCustomFlashSector(void);

/**
 * @brief		This function is used to initialize the MAC address
 * @param[in]	flash_addr - flash address for MAC address
 * @param[in]	mac_public - public address
 * @param[in]	mac_random_static - random static MAC address
 * @return      none
 */
void blc_initMacAddress(int flash_addr, u8 *mac_public, u8 *mac_random_static);




#endif /* BLT_COMMON_H_ */
