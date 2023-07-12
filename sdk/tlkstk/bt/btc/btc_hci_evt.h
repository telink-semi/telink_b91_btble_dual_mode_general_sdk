/********************************************************************************************************
 * @file	btc_hci_evt.h
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
#ifndef BTC_HCI_EVT_H
#define BTC_HCI_EVT_H



bool btc_hci_evtMaskIsSet(uint32 h);
void btc_hci_setEventMaskBit(u8 evt_code, bool bit_v);
void btc_hci_setEventMask(uint08 *event_mask, uint08 page, uint08 *pStatus);

int  btc_hci_sendEvent(uint08 evtID, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);

int btc_hci_sendHardwareErrorEvt(uint08 error_code);

int btc_hci_sendEnhancedFlushCompleteEvt(uint16 connection_handle);

int btc_hci_sendInquiryCompleteEvt(uint08 status);
int btc_hci_sendReadClockOffsetCompleteEvt(uint08 status, uint16 connection_handle,
        uint08 *p, uint08 len);
int btc_hci_sendReadRemoteVersionInfoCompleteEvt(uint08 status, uint16 connection_handle,
        uint08 *p, uint08 len);
int btc_hci_sendConnectCompleteEvt(uint08 status, uint16 connection_handle, uint08 *bd_addr,
		uint08 link_type, uint08 encryption_enabled);

int btc_hci_sendConnectPacketTypeChangedEvt(uint08 status, uint16 con_hdl,uint16 Packet_Type );
int btc_hci_sendNumOfCompleteEvt(uint08 num_handle, uint16 connection_handle,uint16 num_complete_pkt);
int btc_hci_sendFlushOccurredEvt(uint16 connection_handle);
int btc_hci_sendCommandCompleteEvt(uint08 num_hci_cmd, uint08* op_code, uint08 *ret_para, uint08 ret_para_len);
int btc_hci_sendQosSetupCmopleteEvt(uint08 status, uint16 con_hdl, uint08 srv_type, uint32 tkn_rate, uint32 pk_bw, uint32 latency, uint32 dly_var);
int btc_hci_sendAclEstablishEvt(uint08 status, uint16 con_hdl, uint08 *bd_addr, uint08 role, uint08 *remote_cod);
int btc_hci_sendMaxSlotsChangeEvt(uint16 con_hdl,uint08 max_slots);
int btc_hci_sendConnectRequestEvt(uint08 *bd_addr,uint08*class_of_device,uint08 link_type);
int btc_hci_sendCommandStatusEvt(uint08 status, uint08 num_hci_cmd, uint08* op_code);
int btc_hci_sendLinkSupervisonTimeoutChangeEvt(uint16 connection_handle, uint16 ls_to);
int btc_hci_sendReadRemoteExtFeaturesCompleteEvt(uint08 status, uint16 connection_handle,
        uint08 *p, uint08 len);
int btc_hci_sendReadRemoteFeaturesCompleteEvt(uint08 status, uint16 connection_handle,
        uint08 *p, uint08 len)  ;
int btc_hci_sendReadHostSuppFeaturesNotifyEvt(uint08 status, uint08 *bd_addr,
        uint08 *ext_feature);
int btc_hci_sendRoleChangeEvt(uint08 status, uint08 *bd_addr, uint08 new_role);
int btc_hci_sendSyncConnChangedEvt(uint08 status, uint16 connection_handle,
		uint08 transmission_interval, uint08 retransmission_window, uint08 rx_packet_length,
		uint08 tx_packet_length, uint08 sync_linkid);

int btc_hci_sendSyncConnCompleteEvt(uint08 status, uint16 connection_handle,
		uint08 *bd_addr, uint08 link_type, uint08 transmission_interval, uint08 retransmission_window,
		uint16 rx_packet_length, uint16 tx_packet_length, uint08 air_mode);

int btc_hci_sendLinkkeyRequestEvt(uint08 *bd_addr);

int btc_hci_sendLinkKeyNotifyEvt(uint08 *bd_addr, uint08 *key, uint08 keytype);
int btc_hci_sendAuthenCompleteEvt(uint08 status, uint16 con_hdl);
int btc_hci_sendDisconnCompleteEvt(uint08 status, uint16 connection_handle, uint08 reason);
int btc_hci_sendKeyRefreshCompleteEvt(uint08 status, uint16 con_hdl);
int btc_hci_sendEncrypChangeEvt(uint08 status, uint16 con_hdl, uint08 encrypt_enabled);

int btc_hci_sendNameReqCompleteEvt(uint08 status, uint08* bd_addr, uint08* name);

int btc_hci_sendPinCodeRequestEvt(uint08* bd_addr);
int btc_hci_sendChangeConnectLinkKeyCompleteEvt(uint08 status, uint16 con_hdl);

int btc_hci_sendIoCapabilityReqEvt(uint08 *bd_addr);
int btc_hci_sendIoCapabilityRspEvt(uint08 * bd_addr, uint08 io_cap, uint08 oob_data_present, uint08 authen_req);
int btc_hci_sendUserComfirmReqEvt(uint08 * bd_addr, uint08 * user_cfm);
int btc_hci_sendUserPasskeyNotifyEvt(uint08 * bd_addr, uint32_t passkey);
int btc_hci_sendSimplePairingCompleteEvt(uint08 status, uint08 *bd_addr);
int btc_hci_sendUserPasskeyRequestEvt(uint08 *bd_addr);
int btc_hci_sendRemoteOobDataRequestEvt(uint08 *bd_addr);

int btc_hci_sendModeChangeEvt(uint08 status, uint16 connection_handle, uint08 cur_mode, uint16 interval);





#endif // BTC_HCI_EVT_H

