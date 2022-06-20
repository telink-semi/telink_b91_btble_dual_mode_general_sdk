/********************************************************************************************************
 * @file     tlkapi_flash.h
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

#ifndef TLKAPI_FLASH_H
#define TLKAPI_FLASH_H


/******************************************************************************
 * Function: tlkapi_flash_read
 * Descript: read data from flash.
 * Params: 
 *        @addr[IN]--The data address in flash.
 *        @pBuff[OUT]--The buffer stored the data to read.
 *        @buffLen[OUT]--The read data length.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
void tlkapi_flash_read(uint32 addr, uint08 *pBuff, uint32 buffLen);

/******************************************************************************
 * Function: tlkapi_flash_write
 * Descript: write data to flash.
 * Params: 
 *        @addr[IN]--The data address in flash.
 *        @pData[IN]--The data to write.
 *        @buffLen[IN]--The write data length.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
void tlkapi_flash_write(uint32 addr, uint08 *pData, uint32 dataLen);

/******************************************************************************
 * Function: tlkapi_flash_writeByte
 * Descript: write data to flash.
 * Params: 
 *        @addr[IN]--The data address in flash.
 *        @byte[OUT]--The bytes data to write.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
void tlkapi_flash_writeByte(uint32 addr, uint08 byte);

/******************************************************************************
 * Function: tlkapi_flash_eraseSector
 * Descript: erase data from addr in flash.
 * Params: 
 *        @addr[IN]--The data address in flash.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_flash_eraseSector(uint32 addr);



#endif //TLKAPI_FLASH_H

