/********************************************************************************************************
 * @file	tlkapi_save.h
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
#ifndef TLKAPI_SAVE_H
#define TLKAPI_SAVE_H



#define TLKAPI_FLASH_PAGE_SIZE        256
#define TLKAPI_FLASH_SECTOR_SIZE      4096



typedef struct{
	uint08 sign;
	uint08 vers;
	uint16 prev;
	uint16 lens; //=4+Item Length (4=sign+version+length)
	uint16 offs; //Cur Offset
	uint32 addr; //Flash Start Address
	uint32 addr0;
	uint32 addr1;
}tlkapi_save_ctrl_t;


typedef struct{
	uint32 address;
	uint16 dataLen;
	uint16 buffLen;
	uint08 *pBuffer;
}tlkapi_save_item_t;


/******************************************************************************
 * save1: Save a single item in a single sector. The length of Item can change
 *        dynamically. However, in the process of saving, the saved data will 
 *        be lost in case of abnormal power failure.
 *******************************************************************************/
int  tlkapi_save1_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version, uint32 address);
int  tlkapi_save1_load(tlkapi_save_ctrl_t *pCtrl, uint08 *pBuff, uint16 buffLen);
int  tlkapi_save1_save(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);
void tlkapi_save1_clean(tlkapi_save_ctrl_t *pCtrl);
int  tlkapi_save1_migrate(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);
int  tlkapi_save1_smartSave(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * save2: Save a single item in a single sector. The length of Item is fixed,
 *        During the saving process, the saved data will be automatically 
 *        restored in case of abnormal power failure. However, if an abnormal 
 *        power failure occurs during erasure, data will be lost.
 *******************************************************************************/
int  tlkapi_save2_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version, uint16 length, uint32 address);
int  tlkapi_save2_load(tlkapi_save_ctrl_t *pCtrl, uint08 *pBuff, uint16 buffLen);
int  tlkapi_save2_save(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);
void tlkapi_save2_clean(tlkapi_save_ctrl_t *pCtrl);
int  tlkapi_save2_migrate(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);
int  tlkapi_save2_smartSave(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * save3: Save a single item in double sectors. The length of Item is fixed,
 *        During the saving process, the saved data will be automatically 
 *        restored in case of abnormal power failure.
 *******************************************************************************/
int  tlkapi_save3_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version, uint16 length, uint32 address0, uint32 address1);
int  tlkapi_save3_load(tlkapi_save_ctrl_t *pCtrl, uint08 *pBuff, uint16 buffLen);
int  tlkapi_save3_save(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);
void tlkapi_save3_clean(tlkapi_save_ctrl_t *pCtrl);

/******************************************************************************
 * save4: Save multiple items in double sectors. The length of Item is fixed,
 *        During the saving process, the saved data will be automatically 
 *        restored in case of abnormal power failure.
 *******************************************************************************/
int  tlkapi_save4_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version, uint16 length, uint32 address0, uint32 address1);
int  tlkapi_save4_load(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems, uint16 itemCount, uint16 indexOffs);
int  tlkapi_save4_save(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems, uint16 itemCount);
void tlkapi_save4_clean(tlkapi_save_ctrl_t *pCtrl);
int  tlkapi_save4_remove(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems, uint16 itemCount);
int  tlkapi_save4_modify(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems, uint16 offset, uint16 length);
int  tlkapi_save4_migrate(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems, uint16 itemCount);


#endif //TLKAPI_SAVE_H

