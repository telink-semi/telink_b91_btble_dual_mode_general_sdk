/********************************************************************************************************
 * @file     tlkmmi_lemstCtrl.c
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
#if (TLKMMI_LEMST_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmmi/lemst/tlkmmi_lemst.h"
#include "tlkmmi/lemst/tlkmmi_lemstCtrl.h"
#include "tlkmmi/lemst/tlkmmi_lemstAcl.h"

#define TLKMMI_LEMST_NAME_DEF     "TLK-BLE"


static tlkmmi_lemst_ctrl_t gTlkMmiLemstCtrl = {0};


int tlkmmi_lemst_ctrlInit(void)
{
	uint08 index;
		
	// Read Local Name
	tlkapi_flash_read(TLK_CFG_FLASH_LE_NAME_ADDR, gTlkMmiLemstCtrl.lename, TLK_CFG_FLASH_LE_NAME_LENS-1);
	for(index=0; index<TLK_CFG_FLASH_LE_NAME_LENS-1; index++){
		if(gTlkMmiLemstCtrl.lename[index] == 0xFF || gTlkMmiLemstCtrl.lename[index] == 0x00) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_LEMST_NAME_DEF);
		tmemcpy(gTlkMmiLemstCtrl.lename, TLKMMI_LEMST_NAME_DEF, index);
	}
	gTlkMmiLemstCtrl.nameLen = index;
	gTlkMmiLemstCtrl.lename[index] = 0x00;
	
	return TLK_ENONE;
}



bool tlkmmi_lemst_volumeInc(void)
{
	return tlkmmi_lemst_aclVolumeInc();
}
bool tlkmmi_lemst_volumeDec(void)
{
	return tlkmmi_lemst_aclVolumeDec();
}

uint08 tlkmmi_lemst_getNameLen(void)
{
	return gTlkMmiLemstCtrl.nameLen;
}

/******************************************************************************
 * Function: tlkmmi_lemst_getName
 * Descript: Get BT Name.
 * Params:
 * Return: Return Bt name is success.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_lemst_getName(void)
{
	return gTlkMmiLemstCtrl.lename;
}

/******************************************************************************
 * Function: tlkmmi_lemst_getAddr
 * Descript: Get the Bt address. 
 * Params:
 * Return: Return Bt Address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_lemst_getAddr(void)
{
	return gTlkMmiLemstCtrl.leaddr;
}

int tlkmmi_lemst_setName(uint08 *pName, uint08 nameLen)
{
	uint08 btBuffer[TLK_CFG_FLASH_BT_NAME_LENS];
	uint08 leBuffer[TLK_CFG_FLASH_LE_NAME_LENS];
	
	if(pName == nullptr || nameLen == 0) return -TLK_EPARAM;
	if(nameLen > TLK_CFG_FLASH_LE_NAME_LENS-1) nameLen = TLK_CFG_FLASH_LE_NAME_LENS-1;
	
	tlkapi_flash_read(TLK_CFG_FLASH_BT_NAME_ADDR, btBuffer, TLK_CFG_FLASH_BT_NAME_LENS);
	tlkapi_flash_read(TLK_CFG_FLASH_LE_NAME_ADDR, leBuffer, TLK_CFG_FLASH_LE_NAME_LENS);
	
	tmemset(leBuffer, 0xFF, TLK_CFG_FLASH_LE_NAME_LENS);
	tmemcpy(leBuffer, pName, nameLen);
	tlkapi_flash_eraseSector(TLK_CFG_FLASH_LE_NAME_ADDR & 0xFFFFF000);
	tlkapi_flash_write(TLK_CFG_FLASH_BT_NAME_ADDR, btBuffer, TLK_CFG_FLASH_BT_NAME_LENS);
	tlkapi_flash_write(TLK_CFG_FLASH_LE_NAME_ADDR, leBuffer, TLK_CFG_FLASH_LE_NAME_LENS);
	
	tmemcpy(gTlkMmiLemstCtrl.lename, pName, nameLen);
	gTlkMmiLemstCtrl.nameLen = nameLen;
	gTlkMmiLemstCtrl.lename[nameLen] = 0x00;

	return TLK_ENONE;
}

int tlkmmi_lemst_setAddr(uint08 *pAddr)
{
	uint08 btBuffer[6];
	uint08 leBuffer[12];
	
	if(pAddr == nullptr) return -TLK_EPARAM;
	
	tlkapi_flash_read(TLK_CFG_FLASH_BT_ADDR_ADDR, btBuffer, 6);
	tlkapi_flash_read(TLK_CFG_FLASH_LE_ADDR_ADDR, leBuffer, 12);
	
	tmemcpy(leBuffer, pAddr, 6);
	tlkapi_flash_eraseSector(TLK_CFG_FLASH_LE_ADDR_ADDR & 0xFFFFF000);
	tlkapi_flash_write(TLK_CFG_FLASH_BT_ADDR_ADDR, btBuffer, 6);
	tlkapi_flash_write(TLK_CFG_FLASH_LE_ADDR_ADDR, leBuffer, 12);
	
	tmemcpy(gTlkMmiLemstCtrl.leaddr, pAddr, 6);
	return TLK_ENONE;
}

#endif //TLKMMI_LEMST_ENABLE

