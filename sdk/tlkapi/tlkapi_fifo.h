/********************************************************************************************************
 * @file     tlkapi_fifo.h
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

#ifndef TLKAPI_FIFO_H
#define TLKAPI_FIFO_H


#define TLKAPI_FIFO_MIN_SIZE      64


typedef struct{
	uint08 isCover; //Whether overwrite is allowed
	uint08 isParty; //Whether partial writing is allowed
	uint16 more75L;
	uint16 more90L;
	uint16 woffset;
	uint16 roffset;
	uint16 buffLen;
	uint08 *pBuffer;
}tlkapi_fifo_t;

/******************************************************************************
 * Function: tlkapi_fifo_init
 * Descript: Initialize the FIFO manager.
 * Params:
 *     @isCover[IN]--Whether to use overlay reads and writes. 
 *                   If an overlay is used, the old FIFO will be automatically
 *                   overwritten when the FIFO is full.
 *     @buffLen[IN]--The size of fifo to be managed.
 *     @pBuffer[IN]--Fifo to be managed.
 * Return: Fifo management handle, and zero means failture.
 * Others: None.
*******************************************************************************/
int  tlkapi_fifo_init(tlkapi_fifo_t *pFifo, uint08 isCover, uint08 isPartly, uint08 *pBuffer, uint16 buffLen);

/******************************************************************************
 * Function: tlkapi_fifo_deinit
 * Descript: Uninitialize the FIFO manager.
 * Params:
 *     @fifo[IN]--Fifo management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_fifo_deinit(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_reset
 * Descript: Empty saved data in FIFO.
 * Params:
 *     @fifo[]--Fifo management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_fifo_reset(tlkapi_fifo_t *pFifo);


/******************************************************************************
 * Function: tlkapi_fifo_clear
 * Descript: Empty saved data in FIFO.
 * Params:
 *     @fifo[]--Fifo management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_fifo_clear(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_isEmpty
 * Descript: verify FIFO is empty.
 * Params:
 *     @fifo[]--Fifo management handle.
 * Return: ture is null, other means have data in fifo.
 * Others: None.
*******************************************************************************/
bool tlkapi_fifo_isEmpty(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_idleLen
 * Descript: Get FIFO the rest of length which not fill data.
 * Params:
 *     @fifo[]--Fifo management handle.
 * Return: quota of the fifo.
 * Others: None.
*******************************************************************************/
uint16 tlkapi_fifo_idleLen(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_dataLen
 * Descript: Get FIFO the length of which fill data.
 * Params:
 *     @fifo[]--Fifo management handle.
 * Return: The length of which fill data.
 * Others: None.
*******************************************************************************/
uint16 tlkapi_fifo_dataLen(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_buffLen
 * Descript: Get The buffer length which specify by the FIFO.
 * Params:
 *     @fifo[]--Fifo management handle.
 * Return: The length of buffer.
 * Others: None.
*******************************************************************************/
uint16 tlkapi_fifo_buffLen(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_isMore75,tlkapi_fifo_isMore90
 * Descript: Verify current fifo(fill data) length add the data length will 
 *           be insert is big than 75% or 90% of total fifo length.
 * Params:
 *     @fifo[IN]--Fifo management handle.
 *     @dataLen[IN]--The length of data will be insert.
 * Return: true is over the 75% or 90%, false is not.
 * Others: None.
*******************************************************************************/
bool tlkapi_fifo_isMore75(tlkapi_fifo_t *pFifo, uint16 dataLen);
bool tlkapi_fifo_isMore90(tlkapi_fifo_t *pFifo, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_fifo_read
 * Descript: Read written data from FIFO.
 * Params:
 *     @fifo[IN]--Fifo management handle.
 *     @pBuff[OUT]--A buffer that holds read data.
 *     @readLen[IN]--The maximum length of the data to be read.
 * Return: The actual length of the data read. A negative value means 
 *         that the read failed.
 * Others: None.
*******************************************************************************/
int tlkapi_fifo_read(tlkapi_fifo_t *pFifo, uint08 *pBuff, uint16 readLen);
int tlkapi_fifo_write(tlkapi_fifo_t *pFifo, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_fifo_readByte
 * Descript: 
 * Params:
 *     @fifo[IN]--Fifo management handle.
 *     @pByte[OUT]--A buffer that holds read data.
 * Return: A negative value means that the read failed.
 * Others: None.
*******************************************************************************/
int tlkapi_fifo_readByte(tlkapi_fifo_t *pFifo, uint08 *pByte);
int tlkapi_fifo_writeByte(tlkapi_fifo_t *pFifo, uint08 obyte);



#endif //TLKAPI_FIFO_H

