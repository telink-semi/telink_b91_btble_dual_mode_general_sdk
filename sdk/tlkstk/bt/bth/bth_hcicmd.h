/********************************************************************************************************
 * @file     bth_hcicmd.h
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

#ifndef BTH_HCICMD_H
#define BTH_HCICMD_H




typedef enum{
	//Link Control Commands
	BTH_HCI_OPCODE_INQUIRY_CMD                     = 0x0401,
	BTH_HCI_OPCODE_INQUIRY_CANCEL_CMD              = 0x0402,
	BTH_HCI_OPCODE_PERIODIC_INQUIRY_MODE_CMD       = 0x0403,
	BTH_HCI_OPCODE_EXIT_PERIODIC_INQUIRY_MODE_CMD  = 0x0404,
	BTH_HCI_OPCODE_CREATE_CONNECT_CMD              = 0x0405,
	BTH_HCI_OPCODE_DISCONN_CMD                     = 0x0406,
	BTH_HCI_OPCODE_ADD_SYNC_CON_CMD                = 0x0407,
	BTH_HCI_OPCODE_CREATE_CON_CANCEL_CMD           = 0x0408,
	BTH_HCI_OPCODE_ACCEPT_CON_REQ_CMD              = 0x0409,
	BTH_HCI_OPCODE_REJECT_CON_REQ_CMD              = 0x040A,
	BTH_HCI_OPCODE_LK_REQ_REPLY_CMD                = 0x040B,
	BTH_HCI_OPCODE_LK_REQ_NEG_REPLY_CMD            = 0x040C,
	BTH_HCI_OPCODE_PIN_CODE_REQ_REPLY_CMD          = 0x040D,
	BTH_HCI_OPCODE_PIN_CODE_REQ_NEG_REPLY_CMD      = 0x040E,
	BTH_HCI_OPCODE_CHG_CON_PKT_TYPE_CMD            = 0x040F,
	BTH_HCI_OPCODE_AUTH_REQ_CMD                    = 0x0411,
	BTH_HCI_OPCODE_SET_CON_ENC_CMD                 = 0x0413,
	BTH_HCI_OPCODE_CHG_CON_LK_CMD                  = 0x0415,
	BTH_HCI_OPCODE_MASTER_LK_CMD                   = 0x0417,
	BTH_HCI_OPCODE_REM_NAME_REQ_CMD                = 0x0419,
	BTH_HCI_OPCODE_REM_NAME_REQ_CANCEL_CMD         = 0x041A,
	BTH_HCI_OPCODE_RD_REM_SUPP_FEATS_CMD           = 0x041B,
	BTH_HCI_OPCODE_RD_REM_EXT_FEATS_CMD            = 0x041C,
	BTH_HCI_OPCODE_RD_REM_VER_INFO_CMD             = 0x041D,
	BTH_HCI_OPCODE_RD_CLK_OFF_CMD                  = 0x041F,
	BTH_HCI_OPCODE_RD_LMP_HDL_CMD                  = 0x0420,
	BTH_HCI_OPCODE_SETUP_SYNC_CON_CMD              = 0x0428,
	BTH_HCI_OPCODE_ACCEPT_SYNC_CON_REQ_CMD         = 0x0429,
	BTH_HCI_OPCODE_REJECT_SYNC_CON_REQ_CMD         = 0x042A,
	BTH_HCI_OPCODE_IO_CAP_REQ_REPLY_CMD            = 0x042B,
	BTH_HCI_OPCODE_USER_CFM_REQ_REPLY_CMD          = 0x042C,
	BTH_HCI_OPCODE_USER_CFM_REQ_NEG_REPLY_CMD      = 0x042D,
	BTH_HCI_OPCODE_USER_PASSKEY_REQ_REPLY_CMD      = 0x042E,
	BTH_HCI_OPCODE_USER_PASSKEY_REQ_NEG_REPLY_CMD  = 0x042F,
	BTH_HCI_OPCODE_REM_OOB_DATA_REQ_REPLY_CMD      = 0x0430,
	BTH_HCI_OPCODE_REM_OOB_DATA_REQ_NEG_REPLY_CMD  = 0x0433,
	BTH_HCI_OPCODE_IO_CAP_REQ_NEG_REPLY_CMD        = 0x0434,
	BTH_HCI_OPCODE_ENH_SETUP_SYNC_CON_CMD          = 0x043D,
	BTH_HCI_OPCODE_ENH_ACCEPT_SYNC_CON_CMD         = 0x043E,
}BTH_HCI_CMD_ENUM;

/******************************************************************************
 * Function: bth_hci_sendResetCmd
 * Descript: Reset controller.
 * Params:

 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendResetCmd(void);


/******************************************************************************
 * Function: bth_hci_sendInquiryCmd
 * Descript: Start to Inquiry.
 * Params:
 *        @period[IN]--The inquiry time.
 *        @numRsp[IN]--The num of response.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendInquiryCmd(uint08 period, uint08 numRsp);

/******************************************************************************
 * Function: bth_hci_sendInquiryCancelCmd
 * Descript: Cancel Inquiry.
 * Params: None.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendInquiryCancelCmd(void);

/******************************************************************************
 * Function: bth_hci_sendInquiryCancelCmd
 * Descript: Cancel Inquiry.
 * Params: 
 *        @period[IN]--The inquiry time.
 *        @numRsp[IN]--The num of response.
 *        @maxPeriod[IN]--The maximum time.
 *        @minPeriod[IN]--The minmum time.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendPeriodicInquiryCmd(uint08 period, uint08 numRsp, uint16 maxPeriod, uint16 minPeriod);

/******************************************************************************
 * Function: bth_hci_sendPeriodicInquiryCancelCmd
 * Descript: Cancel periodic Inquiry.
 * Params: None.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendPeriodicInquiryCancelCmd(void);

/******************************************************************************
 * Function: bth_hci_sendCreateConnectCmd
 * Descript: Create a connection.
 * Params: 
 *        @mac[IN]--The bt address.
 *        @pktType[IN]--The packet type.
 *        @mode[IN]--The mode.
 *        @clkOffs[IN]--The clock offset.
 *        @allowRoleSwitch[IN]--allow role switch.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendCreateConnectCmd(uint08 mac[6], uint16 pktType, uint08 mode, uint16 clkOffs, uint08 allowRoleSwitch);

/******************************************************************************
 * Function: bth_hci_sendCancelConnectCmd
 * Descript: Cancel the connection.
 * Params: 
 *        @mac[IN]--The bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendCancelConnectCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendDisconnCmd
 * Descript: Disconnect a connection.
 * Params: 
 *        @handle[IN]--The connected acl handle.
 *        @reason[IN]--Disconnect reason.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendDisconnCmd(uint16 handle, uint08 reason);

/******************************************************************************
 * Function: bth_hci_sendRejectConnReqCmd
 * Descript: Reject a connection.
 * Params: 
 *        @handle[IN]--The connected acl handle.
 *        @reason[IN]--Disconnect reason.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendRejectConnReqCmd(uint08 mac[6], uint08 reason);

/******************************************************************************
 * Function: bth_hci_sendAcceptConnReqCmd
 * Descript: Accept a connection.
 * Params: 
 *        @handle[IN]--The connected acl handle.
 *        @role[IN]--The role of aclHandle.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendAcceptConnReqCmd(uint08 mac[6], uint08 role);

/******************************************************************************
 * Function: bth_hci_sendLinkkeyReqReplyCmd
 * Descript: Reply the link key request.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @pLinkkey[IN]--The link key.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendLinkkeyReqReplyCmd(uint08 mac[6], uint08 *pLinkkey);

/******************************************************************************
 * Function: bth_hci_sendLinkkeyReqNegReplyCmd
 * Descript: Reply negtive to the link key request.
 * Params: 
 *        @mac[IN]--The Bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendLinkkeyReqNegReplyCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendPinCodeReqReplyCmd
 * Descript: Start pin code request.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @pPinCode[IN]--The pin code.
 *        @pinLen[In]--The pin code length.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendPinCodeReqReplyCmd(uint08 mac[6], uint08 *pPinCode, uint08 pinlen);

/******************************************************************************
 * Function: bth_hci_sendPinCodeReqNegReplyCmd
 * Descript: Reply negtive to the pin code request.
 * Params: 
 *        @mac[IN]--The Bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendPinCodeReqNegReplyCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendAuthenticationReqCmd
 * Descript: start authenticate.
 * Params: 
 *        @aclHandle[IN]--The acl link handle.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendAuthenticationReqCmd(uint16 aclHandle);

/******************************************************************************
 * Function: bth_hci_sendSetConnectEncryptReqCmd
 * Descript: start encrypt.
 * Params: 
 *        @aclHandle[IN]--The acl link handle.
 *        @encryptEnable[IN]--is encrypt enable.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSetConnectEncryptReqCmd(uint16 aclHandle, uint08 encrytEnable);

/******************************************************************************
 * Function: bth_hci_sendRemoteNameReqCmd
 * Descript: Request peer device name.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @mode[IN]--The mode.
 *        @clockoffset[IN]--The clock offset.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendRemoteNameReqCmd(uint08 mac[6], uint08 mode, uint16 clockOffset);

/******************************************************************************
 * Function: bth_hci_sendRemoteNameReqCancelCmd
 * Descript: Cancel request peer device name.
 * Params: 
 *        @mac[IN]--The Bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendRemoteNameReqCancelCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendCreateSyncConnReqCmd
 * Descript: Create a sync connection.
 * Params: 
 *        @aclHandle[IN]--The acl link handle.
 *        @txBandwidth[IN]--The transfer bandwidth.
 *        @rxBandwidth[IN]--The receive bandwidth.
 *        @maxLatency[IN]--The maximum latency.
 *        @voiceSetting[IN]--The voice setting.
 *        @rtnEffort[IN]--Theretransmission effeort.
 *        @pktType[IN]--The packet type.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendCreateSyncConnReqCmd(uint16 aclHandle, uint32 txBandwidth, uint32 rxBandwidth, uint16 maxLatency, uint16 voiceSetting, uint08 rtnEffort, uint16 pktType);

/******************************************************************************
 * Function: bth_hci_sendAcceptSyncConnReqCmd
 * Descript: Accept a sync connection.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @txBandwidth[IN]--The transfer bandwidth.
 *        @rxBandwidth[IN]--The receive bandwidth.
 *        @maxLatency[IN]--The maximum latency.
 *        @voiceSetting[IN]--The voice setting.
 *        @rtnEffort[IN]--Theretransmission effeort.
 *        @pktType[IN]--The packet type.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendAcceptSyncConnReqCmd(uint08 mac[6], uint32 txBandwidth, uint32 rxBandwidth, uint16 maxLatency, uint16 voiceSetting, uint08 rtnEffort, uint16 pktType);

/******************************************************************************
 * Function: bth_hci_sendRejectSyncConnReqCmd
 * Descript: Reject a sync connection.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @reason[IN]--The reject reason.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendRejectSyncConnReqCmd(uint08 mac[6], uint08 reason);

/******************************************************************************
 * Function: bth_hci_sendIOCapReqReplyCmd
 * Descript: Reply a IO capability request.
 * Params: 
 *        @mac[IN]--The Bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendIOCapReqReplyCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendUsrConfirmReqReplyCmd
 * Descript: Send a reply user confirm request.
 * Params: 
 *        @mac[IN]--The Bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendUsrConfirmReqReplyCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendUsrConfirmReqNegReplyCmd
 * Descript: Send a negtive reply user confirm request.
 * Params: 
 *        @mac[IN]--The Bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendUsrConfirmReqNegReplyCmd(uint08 mac[6]);

/******************************************************************************
 * Function: bth_hci_sendSniffModeCmd
 * Descript: start enter into sniff mode request.
 * Params: 
 *        @connHandle[IN]--The connection handle.
 *        @maxInterval[IN]--The maximum interval.
 *        @minInterval[IN]--The minmum interval.
 *        @attempt[IN]--The sniff attempt times.
 *        @time[IN]--The sniff time.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSniffModeCmd(uint16 connHandle, uint16 maxInterval, uint16 minInterval, uint16 attempt, uint16 timeout);

/******************************************************************************
 * Function: bth_hci_sendExitSniffModeCmd
 * Descript: start exit sniff mode request.
 * Params: 
 *        @connHandle[IN]--The connection handle.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendExitSniffModeCmd(uint16 connHandle);

/******************************************************************************
 * Function: bth_hci_sendRoleDiscoveryCmd
 * Descript: Start role discovery.
 * Params: 
 *        @connHandle[IN]--The connection handle.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendRoleDiscoveryCmd(uint16 connHandle);

/******************************************************************************
 * Function: bth_hci_sendSwitchRoleReqCmd
 * Descript: Start role switch.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @role[IN]--The link role.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSwitchRoleReqCmd(uint08 mac[6], uint08 role);

/******************************************************************************
 * Function: bth_hci_sendWriteLinkPolicy
 * Descript: Set the link govenor policy.
 * Params: 
 *        @connHandle[IN]--The acl handle.
 *        @policySettings[IN]--The link policy.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteLinkPolicy(uint16 connHandle, uint16 policySettings);


/******************************************************************************
 * Function: bth_hci_sendReadStoredLinkkeyCmd
 * Descript: Read the link key.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @readAll[IN]--is read all.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendReadStoredLinkkeyCmd(uint08 mac[6], uint08 readAll);

/******************************************************************************
 * Function: bth_hci_sendWriteStoredLinkkeyCmd
 * Descript: Write the link key.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @pLinkkey[IN]--The link key.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteStoredLinkkeyCmd(uint08 mac[6], uint08 *pLinkkey);

/******************************************************************************
 * Function: bth_hci_sendDeleteStoredLinkkeyCmd
 * Descript: delete the link key.
 * Params: 
 *        @mac[IN]--The Bt address.
 *        @deleteAll[IN]--is delete all link key.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendDeleteStoredLinkkeyCmd(uint08 mac[6], uint08 deleteAll);

/******************************************************************************
 * Function: bth_hci_sendWriteLocalNameCmd
 * Descript: Write local name.
 * Params: 
 *        @pLocalName[IN]--The Bt device name.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteLocalNameCmd(uint08 *pLocalName);

/******************************************************************************
 * Function: bth_hci_sendWritePageTimeoutCmd
 * Descript: Set a page time to listen page timeout.
 * Params: 
 *        @timeout[IN]--The time value.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWritePageTimeoutCmd(uint16 timeout);


/******************************************************************************
 * Function: bth_hci_sendWriteScanEnableCmd
 * Descript: Start a Scan.
 * Params: 
 *        @enable[IN]--is scan enable.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteScanEnableCmd(uint08 enable);

/******************************************************************************
 * Function: bth_hci_sendWritePageScanActivityCmd
 * Descript: Set a page Scan paramter.
 * Params: 
 *        @interval[IN]--is page scan interval.
 *        @window[IN]--The page scan window.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWritePageScanActivityCmd(uint16 interval, uint16 window);

/******************************************************************************
 * Function: bth_hci_sendWriteInquiryScanActivityCmd
 * Descript: Set a inquiry Scan paramter.
 * Params: 
 *        @interval[IN]--is inquiry scan interval.
 *        @window[IN]--The inquiry scan window.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteInquiryScanActivityCmd(uint16 interval, uint16 window);

/******************************************************************************
 * Function: bth_hci_sendWriteAuthenEnableCmd
 * Descript: Start a anthentication.
 * Params: 
 *        @enable[IN]--is enable authen.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteAuthenEnableCmd(uint08 enable);

/******************************************************************************
 * Function: bth_hci_sendWriteClassOfDeviceCmd
 * Descript: Set the COD.
 * Params: 
 *        @devClass[IN]--the device class.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteClassOfDeviceCmd(uint32 devClass);

/******************************************************************************
 * Function: bth_hci_sendWriteLinkSupervisionTimeoutCmd
 * Descript: Set the link supervision time.
 * Params: 
 *        @aclHandle[IN]--the acl handle.
 *        @timeout[IN]--The timeout.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteLinkSupervisionTimeoutCmd(uint16 aclHandle, uint16 timeout);

/******************************************************************************
 * Function: bth_hci_sendWriteInquiryModeCmd
 * Descript: Set the inquiry mode
 * Params: 
 *        @mode[IN]--the inquiry mode.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteInquiryModeCmd(uint08 mode);

/******************************************************************************
 * Function: bth_hci_sendWriteExtendedInquiryRspCmd
 * Descript: Set the EIR
 * Params: 
 *        @fec[IN]--the fec.
 *        @pData[IN]--The eir data.
 *        @datalen[IN]--Eir datalen
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteExtendedInquiryRspCmd(uint08 fec, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: bth_hci_sendWriteSimplePairingModeCmd
 * Descript: Set SSP mode.
 * Params: 
 *        @spMode[IN]--the mode.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteSimplePairingModeCmd(uint08 spMode);

/******************************************************************************
 * Function: bth_hci_sendWriteLookbackModeCmd
 * Descript: Set loopback mode.
 * Params: 
 *        @mode[IN]--the mode.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteLookbackModeCmd(uint08 mode);

/******************************************************************************
 * Function: bth_hci_sendWriteSimpleDebugModeCmd
 * Descript: Set debug mode.
 * Params: 
 *        @debugMode[IN]--the mode.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendWriteSimpleDebugModeCmd(uint08 debugMode);

/******************************************************************************
 * Function: bth_hci_sendSetESCOMuteCmd
 * Descript: Set sco mute mode.
 * Params: 
 *        @escoHandle[IN]--the esco handle.
 *        @micSpk[IN]--The mic speaker
 *        @muteEn[IN]--ebable mute.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSetESCOMuteCmd(uint16 escoHandle, uint08 micSpk, uint08 muteEn);

/******************************************************************************
 * Function: bth_hci_sendSetBtAddrCmd
 * Descript: Set sco mute mode.
 * Params:
 *        @pBtAddr[IN]--the bt address.

 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSetBtAddrCmd(uint08 pBtAddr[6]);


/******************************************************************************
 * Function: bth_hci_sendSetMaxSlotCmd
 * Descript: set max slot command.
 * Params:
 *        @pBtAddr[IN]--the bt address.
 * Reutrn: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSetMaxSlotCmd(uint08 pBtAddr[6]);


#endif //BTH_HCICMD_H

