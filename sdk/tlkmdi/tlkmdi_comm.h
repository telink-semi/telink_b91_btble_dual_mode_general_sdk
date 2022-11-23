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

typedef void(*tlkmdi_comm_cmdCB)(uint08 msgID, uint08 *pData, uint08 dataLen);
typedef void(*tlkmdi_comm_datCB)(uint08 datID, uint16 number, uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmdi_comm_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: None.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_init(void);

void tlkmdi_comm_input(uint08 *pData, uint16 dataLen);

void tlkmdi_comm_handler(void);

bool tlkmdi_comm_pmIsBusy(void);

int tlkmdi_comm_getValidDatID(uint08 *pDatID);

void tlkmdi_comm_incSendNumb(void);
void tlkmdi_comm_setSendNumb(uint08 value);
void tlkmdi_comm_getSendNumb(uint08 *pValue);


/******************************************************************************
 * Function: tlkmdi_comm_regDatCB.
 * Descript: Register the data callback.
 * Params:
 *        @datID[IN]--The data channel Id.
 *        @datCB[IN]--The data callback.
 *        @isForce[IN]--is force to process.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_comm_regDatCB(uint08 datID, tlkmdi_comm_datCB datCB, bool isForce);

/******************************************************************************
 * Function: tlkmdi_comm_regSysCB.
 * Descript: Register the data callback.
 * Params:
 *        @cmdCB[IN]--Handle System command callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regSysCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_regBtCB.
 * Descript: Register the Bt callback.
 * Params:
 *        @cmdCB[IN]--Handle BT message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regBtCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_regLeCB.
 * Descript: Register the BLE callback.
 * Params:
 *        @cmdCB[IN]--Handle BLE message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regLeCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_regDbgCB.
 * Descript: Register the data callback.
 * Params:
 *        @cmdCB[IN]--Handle Debug Message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regDbgCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_regFileCB.
 * Descript: Register the File operation callback.
 * Params:
 *        @cmdCB[IN]--Handle FILE message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regFileCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_regCallCB.
 * Descript: Register the Phone Call callback.
 * Params:
 *        @cmdCB[IN]--Handle phone call message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regCallCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_regAudioCB.
 * Descript: Register the Audio callback.
 * Params:
 *        @cmdCB[IN]--Handle Audio message callback.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_comm_regAudioCB(tlkmdi_comm_cmdCB cmdCB);

/******************************************************************************
 * Function: tlkmdi_comm_SendSysXXX.
 * Descript: send the sys command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendSysCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendSysRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendSysEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendBtXXX.
 * Descript: send the BT command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendBtCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendBtRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendBtEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendLeXXX.
 * Descript: send the BLE command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendLeCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendLeRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendLeEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendFileXXX.
 * Descript: send the File command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendFileCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendFileRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendFileEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendCallXXX.
 * Descript: send the Call command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendCallCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendCallRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendCallEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendAudioXXX.
 * Descript: send the Audio command or Response or Event.
 * Params:
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendAudioCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendAudioRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendAudioEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendXXX.
 * Descript: send the command or Response or Event or data.
 * Params:
 *        @mType[IN]--The message type.
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_sendCmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendRsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendEvt(uint08 mType, uint08 msgID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen);

int tlkmdi_comm_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);



#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKMDI_COMM_H

