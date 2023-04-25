/********************************************************************************************************
 * @file     btp_hfp.h
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

#ifndef BTP_HFP_H
#define BTP_HFP_H

#if (TLK_STK_BTP_ENABLE)



typedef enum{
	BTP_HFP_CALL_DIR_NONE = 0,
	BTP_HFP_CALL_DIR_INCOMING,
	BTP_HFP_CALL_DIR_OUTGOING,
}BTP_HFP_CALL_DIR_ENUM;
typedef enum{
	BTP_HFP_CALL_STATUS_NONE = 0,
	BTP_HFP_CALL_STATUS_CLOSE,
	BTP_HFP_CALL_STATUS_START,
	BTP_HFP_CALL_STATUS_ALART,
	BTP_HFP_CALL_STATUS_NORING,
	BTP_HFP_CALL_STATUS_ACTIVE,
	BTP_HFP_CALL_STATUS_PAUSED,
	BTP_HFP_CALL_STATUS_WAITING,
}BTP_HFP_CALL_STATUS_ENUM;

typedef enum{
	BTP_HFP_VOLUME_TYPE_NONE = 0,
	BTP_HFP_VOLUME_TYPE_SPK,
	BTP_HFP_VOLUME_TYPE_MIC,
}BTP_HFP_VOLUME_TYPE_ENUM;

typedef enum{
	BTP_HFP_CALL_TYPE = 0,
	BTP_HFP_CALL_SETUP_TYPE,
	BTP_HFP_CALL_HELD_TYPE,
}BTP_HFP_CALL_STATUS_TYPE;

typedef int(*BtpHfpAgRecvCmdCB)(uint16 aclHandle, uint08 *pCmd, uint08 cmdLen);
typedef int(*BtpHfpAgUnknownCmdCB)(uint16 aclHandle, uint08 *pCmd, uint08 cmdLen);
typedef int(*BtpHfpAgGetLastPhoneCB)(uint16 aclHandle, uint08 *pNumber, uint08 numbLen, uint08 *pGetLen);



/******************************************************************************
 * Function: HFP init interface
 * Descript: This interface be used by user to initial the hfp resource
 *           of client/server before create a connection between the entity.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_hfp_init(void);

/******************************************************************************
 * Function: HFP Trigger Connect interface
 * Descript: Defines trigger the hfp connect cmd to peer hfp entity and setup
 *           a connection with devices.
 * Params:
 *        @aclHandle[IN--The Acl Handle identifier.
 *        @usrId[IN]--The service user id.
 *        @channel[IN]--the rfcomm Channel
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_hfp_connect(uint16 aclHandle, uint08 usrID, uint08 channel);

/******************************************************************************
 * Function: HFP Trigger Disonnect interface
 * Descript: Defines trigger the hfp disconnect cmd to peer hfp entity and tear up
 *           a connection with devices.
 * Params:
 *        @aclHandle[IN--The Acl Handle identifier.
 *        @usrId[IN]--The service user id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_hfp_disconn(uint16 aclHandle, uint08 usrID);

void btp_hfp_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: btp_hfphf_getFeature
 * Descript: Get the feaure of suppoted by HF.
 * Params: None.
 * Return: None.
*******************************************************************************/
extern uint btp_hfphf_getFeature(void);

/******************************************************************************
 * Function: btp_hfphf_getFeature
 * Descript: Set the feaure of suppoted by HF.
 * Params: 
 *     @feature[IN]--. Refer "HF supported features bitmap" in <HFP_v1.8.pdf>.
 * Return: None.
*******************************************************************************/
extern void btp_hfphf_setFeature(uint feature);

/******************************************************************************
 * Function: btp_hfphf_enableThreeWayCall
 * Descript: Enable or disable the function of three-party conversation.
 * Params:
 *     @enable[IN]--True,enable three-way call; false-disable three-way call.
 * Return: None.
*******************************************************************************/
extern void btp_hfphf_enableThreeWayCall(bool enable);

/******************************************************************************
 * Function: HFP Trigger Get Current Handle interface
 * Descript: Defines trigger the hfp get the current Aclhandle of 
 *           a connection with devices.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern uint16 btp_hfphf_getCurHandle(void);

/******************************************************************************
 * Function: HFP Trigger Get Speaker Volume interface
 * Descript: Defines trigger the hfp get the current volume of a call.
 * Params:
 * Return: Returning value means the volume value.
*******************************************************************************/
extern uint08 btp_hfphf_getSpkVolume(void);

/******************************************************************************
 * Function: HFP Trigger Get Mic Volume interface
 * Descript: Defines trigger the hfp get the current volume of a Mic.
 * Params:
 * Return: Returning value means the volume value.
*******************************************************************************/
extern uint08 btp_hfphf_getMicVolume(void);

/******************************************************************************
 * Function: HFP Trigger Set Speaker Volume interface
 * Descript: Defines trigger the hfp set the current volume of a call.
 * Params:
 *        @spkVolume[IN]--The Speaker volume
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_setSpkVolume(uint08 spkVolume);

/******************************************************************************
 * Function: HFP Trigger Set Mic Volume interface
 * Descript: Defines trigger the hfp set the current volume of a Mic.
 * Params:
 *        @micVolume[IN]--The Mic volume
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_setMicVolume(uint08 micVolume);

/******************************************************************************
 * Function: btp_hfphf_send
 * Descript: Define a passthrough data interface.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--Length of data to be sent.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_send(uint16 aclHandle, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: HFP Trigger Dial a Call interface
 * Descript: Defines trigger the hfp Dial a Call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 *        @pNumber[IN]--The call number.
 *        @numbLen[IN]--The call number length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_dial(uint16 aclHandle, char *pNumber, uint08 numbLen);

/******************************************************************************
 * Function: HFP Trigger Re-Dial a Call interface
 * Descript: Defines trigger the hfp Re-Dial a Call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_redial(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger Answer a Call interface
 * Descript: Defines trigger the hfp answer a Call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_answer(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger hangUp a Call interface
 * Descript: Defines trigger the hfp Hangup a Call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_hungUp(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger Reject a Call interface
 * Descript: Defines trigger the hfp reject a Call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_reject(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger Reject a call in wait and active status interface
 * Descript: Defines trigger the hfp reject a Call in wait and keep active.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_rejectWaitAndKeepActive(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger accept a call in wait and active status interface
 * Descript: Defines trigger the hfp accept a Call in wait and keep active.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_acceptWaitAndHoldActive(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger hungup a call in active and restart another call in hold.
 * Descript: Defines trigger the hfp hungup a Call in active and resume a hold call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_hungUpActiveAndResumeHold(uint16 aclHandle);

/******************************************************************************
 * Function: btp_hfphf_queryCallList
 * Descript: Query the call list in the phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_queryCallList(uint16 aclHandle);

/******************************************************************************
 * Function: btp_hfphf_queryCallState
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_queryCallState(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger siri interface
 * Descript: Defines trigger the siri.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int btp_hfphf_siri_ctrl(uint16 aclHandle);


/******************************************************************************
 * Function: HFP hf verfy IOS device interface
 * Descript: This interface be used by hf to verfy peer device is IOS OS.
 * Params:
 *         @aclHandle[IN]--The acl link identifier.
 * Return: true(Yes)/false(No).
*******************************************************************************/
extern bool btp_hfphf_isIosDev(uint16 aclHandle);

/******************************************************************************
 * Function: HFP hf verfy siri interface
 * Descript: This interface be used by hf to verfy peer device is siri enable.
 * Params:
 *         @aclHandle[IN]--The acl link identifier.
 * Return: true(Yes)/false(No).
*******************************************************************************/
extern bool btp_hfphf_isSiriEnable(uint16 aclHandle);

/******************************************************************************
 * Function: HFP hf verfy support voice recog interface
 * Descript: This interface be used by hf to verfy peer device is supported Voice Recog.
 * Params:
 *         @aclHandle[IN]--The acl link identifier.
 * Return: true(Yes)/false(No).
*******************************************************************************/
extern bool btp_hfphf_isSupportVoiceRecog(uint16 aclHandle);


/******************************************************************************
 * Function: HFP hf reset the record of siri state interface
 * Descript: This interface be used by hf to reset the record of siri state.
 * Params:
 *         @aclHandle[IN]--The acl link identifier.
 * Return: true(Yes)/false(No).
*******************************************************************************/
extern void btp_hfphf_resetSiriRecordState(uint16 aclHandle);





/******************************************************************************
 * Function: btp_hfphf_getFeature
 * Descript: Get the feaure of suppoted by HF.
 * Params: None.
 * Return: None.
*******************************************************************************/
extern uint btp_hfphf_getFeature(void);

/******************************************************************************
 * Function: btp_hfpag_getFeature
 * Descript: Set the feaure of suppoted by HF.
 * Params: 
 *     @feature[IN]--. Refer "HF supported features bitmap" in <HFP_v1.8.pdf>.
 * Return: None.
*******************************************************************************/
extern void btp_hfpag_setFeature(uint feature);

/******************************************************************************
 * Function: btp_hfphf_enableThreeWayCall
 * Descript: Enable or disable the function of three-party conversation.
 * Params:
 *     @enable[IN]--True,enable three-way call; false-disable three-way call.
 * Return: None.
*******************************************************************************/
extern void btp_hfpag_enableThreeWayCall(bool enable);

/******************************************************************************
 * Function: btp_hfpag_send
 * Descript: Define a passthrough data interface.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--Length of data to be sent.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
extern int  btp_hfpag_send(uint16 aclHandle, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: btp_hfpag_regRecvCmdCB
 * Descript: Register the command processing interface.
 * Params:
 *     @cb[IN]--Callback interface.
 * Return: None.
 * Others: When the callback interface returns TLK_ENONE, this instruction is 
 *     ignored by system.
*******************************************************************************/
extern void btp_hfpag_regRecvCmdCB(BtpHfpAgRecvCmdCB cb);

/******************************************************************************
 * Function: btp_hfpag_regUnknownCmdCB
 * Descript: Register the unknown command processing interface.
 * Params:
 *     @cb[IN]--Callback interface.
 * Return: None.
 * Others: When the callback interface returns TLK_ENONE, this instruction is 
 *     ignored by system.
*******************************************************************************/
extern void btp_hfpag_regUnknownCmdCB(BtpHfpAgUnknownCmdCB cb);

/******************************************************************************
 * Function: btp_hfpag_regGetLastPhoneCB
 * Descript: Register the interface to get the last phone number.
 * Params:
 *     @cb[IN]--Callback interface.
 * Return: None.
*******************************************************************************/
extern void btp_hfpag_regGetLastPhoneCB(BtpHfpAgGetLastPhoneCB cb);

/******************************************************************************
 * Function: btp_hfpag_insertCall
 * Descript: This interface be used by ag to add a call after a connection setup
 *           or add a new call.
 * Params:
 * Return: null.
*******************************************************************************/
extern int btp_hfpag_insertCall(uint08 *pNumber, uint08 numbLen, uint08 isIncoming);

/******************************************************************************
 * Function: btp_hfpag_removeCall
 * Descript: This interface be used by ag to remove a call.
 * Params:
 * Return: null.
*******************************************************************************/
extern int btp_hfpag_removeCall(uint08 *pNumber, uint08 numbLen);

/******************************************************************************
 * Function: btp_hfpag_activeCall
 * Descript: This interface be used by ag to active a call.
 * Params:
 * Return: null.
*******************************************************************************/
extern int btp_hfpag_activeCall(uint08 *pNumber, uint08 numbLen);

/******************************************************************************
 * Function: btp_hfpag_hungupCall
 * Descript: This interface be used by ag to hungup a call.
 * Params:
 * Return: null.
*******************************************************************************/
extern int btp_hfpag_hungupCall(void);

/******************************************************************************
 * Function: btp_hfphf_sendDtmf
 * Descript: Instruct the AG to transmit a specific DTMF code to its network
 *           connection.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @dtmf[IN]--The Dtmf tone. dtmf is a single ASCII character.
 *        The value of dtmf is '0'~9,'*','#', 'A','B','C','D'.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 * Note: To send the DTMF dial tone, you must have a phone currently in use.
*******************************************************************************/
extern int btp_hfphf_sendDtmf(uint16 aclHandle, char dtmf);

/******************************************************************************
 * Function: btp_hfp_getCurCodec
 * Descript:
 * Params:
 *     @aclHandle[IN]--The acl handle.
 * Return:
*******************************************************************************/
extern uint08 btp_hfp_getCurCodec(uint16 aclHandle);



#endif //#if (TLK_STK_BTP_ENABLE)

#endif /* BTP_HFP_H */

