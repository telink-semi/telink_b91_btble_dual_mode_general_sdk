/********************************************************************************************************
 * @file     tlkapp_dfu.c
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
#include "tlkapp_config.h"
#include "tlkapp_irq.h"
#include "tlkapp.h"

#include "drivers.h"
#include "tlkapp_dfu.h"
#include "tlkmdi/misc/tlkmdi_file.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"
#include "tlkalg/digest/sha/tlkalg_sha.h"


#define TLKAPP_DFU_SAVE_SIGN        0x3A
#define TLKAPP_DFU_SAVE_VERS        0x03
#define TLKAPP_DFU_SAVE_ADDR        TLK_CFG_FLASH_OTA_PARAM_ADDR
#define TLKAPP_DFU_SAVE_SIZE        sizeof(tlkmdi_file_saveParam_t)


typedef struct{
	uint08 saveIsOK;
	uint08 reserve0;
	uint16 reserve1;
	uint32 checkDig;
	tlkapi_save_ctrl_t saveCtrl;
	tlkmdi_file_saveParam_t saveParam;
}tlkapp_dfu_ctrl_t;


static bool tlkapp_dfu_loadData(tlkapp_dfu_ctrl_t *pCtrl);
static bool tlkapp_dfu_dataCheck(tlkapp_dfu_ctrl_t *pCtrl);
static bool tlkapp_dfu_loadCheck(tlkapp_dfu_ctrl_t *pCtrl);
static void tlkapp_dfu_earseData(tlkapp_dfu_ctrl_t *pCtrl);



void tlkapp_dfu_load(void)
{
	int ret;
	tlkapp_dfu_ctrl_t ctrl;
	tlkapp_dfu_ctrl_t *pCtrl;

	pCtrl = &ctrl;
	tmemset(pCtrl, 0, sizeof(tlkapp_dfu_ctrl_t));
	
	tlkapi_save2_init(&pCtrl->saveCtrl, TLKAPP_DFU_SAVE_SIGN, TLKAPP_DFU_SAVE_VERS,
		TLKAPP_DFU_SAVE_SIZE, TLKAPP_DFU_SAVE_ADDR);
	ret = tlkapi_save2_load(&pCtrl->saveCtrl, (uint08*)&pCtrl->saveParam, TLKAPP_DFU_SAVE_SIZE);
	if(ret > 0) pCtrl->saveIsOK = true;
	if(!pCtrl->saveIsOK) return;
	if(pCtrl->saveParam.status != 0x00 && pCtrl->saveParam.status != 0xFF){
		tlkapp_dfu_earseData(pCtrl);
		return;
	}
	if(pCtrl->saveParam.status != 0x00) return; //Continue in APP
	
	if(pCtrl->saveParam.fileSize > TLKAPP_DFU_MAX_SIZE 
		|| pCtrl->saveParam.fileSize < TLKAPP_DFU_MIN_SIZE
		|| pCtrl->saveParam.fileSize != pCtrl->saveParam.dealSize){
		tlkapp_dfu_earseData(pCtrl);
		return;
	}
	if(!tlkapp_dfu_dataCheck(pCtrl)){
		tlkapp_dfu_earseData(pCtrl);
		return;
	}
	if(tlkapp_dfu_loadData(pCtrl)){
		if(tlkapp_dfu_loadCheck(pCtrl)){
			tlkapp_dfu_earseData(pCtrl);
		}else{
			start_reboot();
		}
	}
}


static bool tlkapp_dfu_dataCheck(tlkapp_dfu_ctrl_t *pCtrl)
{
	uint32 offset;
	uint32 chkLens;
	uint32 dataAddr;
	uint32 dealSize;
	uint08 buffer[256];
	uint08 value[TLKALG_SHA1_HASH_SIZE] = {0};
	tlkalg_sha1_contex_t contex;
	tlkalg_sha1_digest_t digest;
	
	dataAddr = pCtrl->saveParam.dataAddr;
	
	dealSize = pCtrl->saveParam.dealSize-TLKALG_SHA1_HASH_SIZE;
	flash_read_page(dataAddr+dealSize, TLKALG_SHA1_HASH_SIZE, value);
	
	tlkalg_sha1_init(&contex);
	for(offset=0; offset<dealSize; offset += 256){
		flash_read_page(dataAddr+offset, 256, buffer);
		if(offset+256 <= dealSize) chkLens = 256;
		else chkLens = dealSize-offset;
		tlkalg_sha1_update(&contex, buffer, chkLens);
	}
	tlkalg_sha1_finish(&contex, &digest);
	if(tmemcmp(digest.value, value, TLKALG_SHA1_HASH_SIZE) != TLK_ENONE){
		return false;
	}
	return true;
}
static bool tlkapp_dfu_loadData(tlkapp_dfu_ctrl_t *pCtrl)
{
	uint08 flag;
	uint32 offset;
	uint32 dataAddr;
	uint32 loadAddr;
	uint08 buffer[256];
	
	tlkalg_crc32_init(&pCtrl->checkDig);
	loadAddr = TLKAPP_BOOT_START_ADDRESS;
	dataAddr = pCtrl->saveParam.dataAddr;
	
	if(pCtrl->saveParam.datPos == TLKAPP_DFU_LOAD_INNER){
		for(offset=0; offset<pCtrl->saveParam.dealSize; offset += 256){
			if((offset & 0xFFF) == 0) flash_erase_sector(loadAddr+offset);
			flash_read_page(dataAddr+offset, 256, buffer);
			if(offset == 0){
				buffer[TLKAPP_DFU_START_OFFSET] = 0xFF;
				flash_write_page(loadAddr+0x00, 128, buffer);
				flash_write_page(loadAddr+0x80, 128, buffer+128);
				buffer[TLKAPP_DFU_START_OFFSET] = TLKAPP_DFU_START_FLAG;
			}else{
				flash_write_page(loadAddr+offset, 256, buffer);
			}
			tlkalg_crc32_update(&pCtrl->checkDig, buffer, 256);
		}
		flag = TLKAPP_DFU_START_FLAG;
		flash_write_page(loadAddr+TLKAPP_DFU_START_OFFSET, 1, &flag);
		tlkalg_crc32_finish(&pCtrl->checkDig);
		return true;
	}
	else if(pCtrl->saveParam.datPos == TLKAPP_DFU_LOAD_OUTER)
	{
		
	}
	return false;
}
static bool tlkapp_dfu_loadCheck(tlkapp_dfu_ctrl_t *pCtrl)
{
	uint32 offset;
	uint32 checkDig;
	uint32 loadAddr;
	uint08 buffer[256];

	tlkalg_crc32_init(&checkDig);
	loadAddr = TLKAPP_BOOT_START_ADDRESS;
	for(offset=0; offset<pCtrl->saveParam.dealSize; offset += 256){
		flash_read_page(loadAddr+offset, 256, buffer);
		tlkalg_crc32_update(&checkDig, buffer, 256);
	}
	tlkalg_crc32_finish(&checkDig);
	if(checkDig == pCtrl->checkDig) return true;
	else return false;
}

static void tlkapp_dfu_earseData(tlkapp_dfu_ctrl_t *pCtrl)
{
	uint32 offset;
	uint32 dataAddr;

	dataAddr = pCtrl->saveParam.dataAddr;
	if(pCtrl->saveParam.datPos == TLKAPP_DFU_LOAD_INNER){
		for(offset=0; offset<pCtrl->saveParam.dealSize; offset += 4096){
			flash_erase_sector(dataAddr+offset);
		}
	}
	else if(pCtrl->saveParam.datPos == TLKAPP_DFU_LOAD_OUTER)
	{

	}
	tlkapi_save2_clean(&pCtrl->saveCtrl);
	tmemset(&pCtrl->saveParam, 0, sizeof(tlkmdi_file_saveParam_t));
}


