/********************************************************************************************************
 * @file     tlkmdi_comm.h
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

#ifndef TLKMDI_COMM_H
#define TLKMDI_COMM_H

#if (TLK_CFG_COMM_ENABLE)


#define TLKMDI_COMM_DATA_CHANNEL_MAX       8

typedef enum{
	TLKMDI_COMM_MSTATE_HEAD = 0,
	TLKMDI_COMM_MSTATE_ATTR,
	TLKMDI_COMM_MSTATE_BODY,
	TLKMDI_COMM_MSTATE_CHECK,
	TLKMDI_COMM_MSTATE_TAIL,
	TLKMDI_COMM_MSTATE_READY,
}TLKMDI_COMM_MSTATE_ENUM;

typedef void(*tlkmdi_comm_datCB)(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmdi_comm_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_init(void);

void tlkmdi_comm_clear(void);

void tlkmdi_comm_input(uint08 *pData, uint16 dataLen);

void tlkmdi_comm_handler(void);

bool tlkmdi_comm_pmIsBusy(void);

int tlkmdi_comm_getValidDatID(uint08 *pDatID);

void tlkmdi_comm_incSendNumb(void);
void tlkmdi_comm_setSendNumb(uint08 value);
void tlkmdi_comm_getSendNumb(uint08 *pValue);


/******************************************************************************
 * Function: tlkmdi_comm_regCmdCB.
 * Descript: Registers a task for the message type in the system to distribute
 *     the received message to the specified task.
 * Params:
 *     @mtype[IN]--The Type of message.
 *     @taskID[IN]--The ID of the task.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_comm_regCmdCB(uint08 mtype, uint08 taskID);

/******************************************************************************
 * Function: tlkmdi_comm_regDatCB.
 * Descript: Register the data callback.
 * Params:
 *     @datID[IN]--The data channel Id.
 *     @datCB[IN]--The data callback.
 *     @isForce[IN]--is force to process.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_comm_regDatCB(uint08 datID, tlkmdi_comm_datCB datCB, bool isForce);

/******************************************************************************
 * Function: tlkmdi_comm_sendXXX.
 * Descript: send the command or Response or Event or data.
 * Params:
 *     @mType[IN]--The message type.
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendCmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendRsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendEvt(uint08 mType, uint08 msgID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen);

int tlkmdi_comm_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);

uint tlkmdi_comm_getSingleDatPktMaxLen(void);
uint tlkmdi_comm_getSingleDatPktUnitLen(void);


#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKMDI_COMM_H

