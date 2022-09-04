/********************************************************************************************************
 * @file     tlkmdi_event.h
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

#ifndef TLKMDI_EVENT_H
#define TLKMDI_EVENT_H



typedef enum{
	TLKMDI_EVENT_MAJOR_NONE = 0,
	TLKMDI_EVENT_MAJOR_AUDIO,
	TLKMDI_EVENT_MAJOR_PHONE,
	TLKMDI_EVENT_MAJOR_MAX,
}TLKMDI_EVENT_MAJOR_ENUM;
typedef enum{
	TLKMDI_EVENT_MINOR_NONE = 0,
}TLKMDI_EVENT_MINOR_ENUM;


typedef void(*TlkMdiEventCallback)(uint08 majorID, uint08 minorID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_event_init.
 * Descript: Trigger to Initial the Event Control block an qfifo.
 * Params: None.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_event_init(void);

/******************************************************************************
 * Function: tlkmdi_event_regCB.
 * Descript: Register each Event's callback.
 * Params:
 *        @type[IN]--The event type.
 *        @cb[IN]--The event callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_event_regCB(TLKMDI_EVENT_MAJOR_ENUM type, TlkMdiEventCallback cb);

/******************************************************************************
 * Function: tlkmdi_clear_event.
 * Descript: Clear the fifo.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_clear_event(void);

/******************************************************************************
 * Function: tlkmdi_event_count.
 * Descript: Get the count of fifo which be filled.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
uint tlkmdi_event_count(void);

/******************************************************************************
 * Function: tlkmdi_event_handler.
 * Descript: handle the dequeue's event.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_event_handler(void);

/******************************************************************************
 * Function: tlkmdi_sendAudioEvent.
 * Descript: Send the audio event.
 * Params: None.
 * Return: Return TLK_ENONE is success, others failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_sendAudioEvent(uint08 minorID, void *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_sendPhoneEvent.
 * Descript: Send the phone event.
 * Params: None.
 * Return: Return TLK_ENONE is success, others failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_sendPhoneEvent(uint08 minorID, void *pData, uint08 dataLen, bool isRealDeal);




#endif //TLKMDI_EVENT_H

