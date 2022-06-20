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
}BTP_HFP_CALL_STATUS_ENUM;

typedef enum{
	BTP_HFP_VOLUME_TYPE_NONE = 0,
	BTP_HFP_VOLUME_TYPE_SPK,
	BTP_HFP_VOLUME_TYPE_MIC,
}BTP_HFP_VOLUME_TYPE_ENUM;

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




#endif /* BTP_HFP_H */

