/********************************************************************************************************
 * @file     bth_l2cap.h
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

#ifndef BTH_L2CAP_H
#define BTH_L2CAP_H



#define L2CAP_PERFECT_ENABLE      1

#if (L2CAP_PERFECT_ENABLE)
	#define L2CAP_MTU_MAX_SIZE      696
	#define L2CAP_SBUFF_MAX_SIZE    (L2CAP_MTU_MAX_SIZE+4)
#endif









/* base frame */
#define L2CAP_SIG_CID               0x0001
#define L2CAP_CONNECTIONLESS_CID    0x0002



/* command code */
#define L2CAP_SIG_REJ           0x01
#define L2CAP_SIG_CONN_REQ      0x02
#define L2CAP_SIG_CONN_RSP      0x03
#define L2CAP_SIG_CFG_REQ       0x04
#define L2CAP_SIG_CFG_RSP       0x05
#define L2CAP_SIG_DISCONN_REQ   0x06
#define L2CAP_SIG_DISCONN_RSP   0x07
#define L2CAP_SIG_ECHO_REQ      0x08
#define L2CAP_SIG_ECHO_RSP      0x09
#define L2CAP_SIG_INFO_REQ      0x0A
#define L2CAP_SIG_INFO_RSP      0x0B


//EXTENDED FEATURE MASK <Core5.2.pdf>P1063
typedef enum{
	L2CAP_SIG_EXTFEATURE_FLOW_CTRL_MODE    = 0x0001, //Flow control mode
	L2CAP_SIG_EXTFEATURE_RTN_MODE          = 0x0002, //Retransmission mode
	L2CAP_SIG_EXTFEATURE_BI_DIR_QOS        = 0x0004, //Bi-directional QoS
	L2CAP_SIG_EXTFEATURE_ENHANCED_RTN_MODE = 0x0008, //Enhanced Retransmission Mode
	L2CAP_SIG_EXTFEATURE_STREAMING_MODE    = 0x0010, //Streaming Mode
	L2CAP_SIG_EXTFEATURE_FCS_OPTION        = 0x0020, //FCS Option
	L2CAP_SIG_EXTFEATURE_EXT_FLOW_SPEC     = 0x0040, //Extended Flow Specification for BR/EDR
	L2CAP_SIG_EXTFEATURE_FIXED_CHANNELS    = 0x0080, //Fixed Channels
	L2CAP_SIG_EXTFEATURE_EXT_WIN_SIZE      = 0x0100, //Extended Window Size
	L2CAP_SIG_EXTFEATURE_UNICAST_CONN_DATA = 0x0200, //Unicast Connectionless Data Reception
	L2CAP_SIG_EXTFEATURE_ENHANCED_CREDIT   = 0x0400, //Enhanced Credit Based Flow Control Mode

	L2CAP_SIG_EXTFEATURE_DEFAULT = 0x00000000,							
}L2CAP_SIG_EXTFEATURE_ENUM;
typedef enum{
	L2CAP_SIG_CONNRST_SUCCESS        = 0x0000, //Connection successful.
	L2CAP_SIG_CONNRST_PENDING        = 0x0001, //Connection pending
	L2CAP_SIG_CONNRST_REFUSED_NOPSM  = 0x0002, //Connection refused ?C PSM not supported
	L2CAP_SIG_CONNRST_SEC_BLOCK      = 0x0003, //Connection refused ?C security block
	L2CAP_SIG_CONNRST_NO_RESOURCES   = 0x0004, //Connection refused ?C no resources available
	L2CAP_SIG_CONNRST_INVALID_SCID   = 0x0005, //Connection refused ?C invalid Source CID
	L2CAP_SIG_CONNRST_SCID_ALREADY   = 0x0006, //Connection refused ?C Source CID already allocated
}L2CAP_SIG_CONNRST_ENUM;
typedef enum{
	L2CAP_SIG_RTNMODE_BASIC    = 0x00, //L2CAP Basic Mode
	L2CAP_SIG_RTNMODE_RTN      = 0x01, //Retransmission mode
	L2CAP_SIG_RTNMODE_FLOW     = 0x02, //Flow control mode
	L2CAP_SIG_RTNMODE_ENHANCE  = 0x03, //Enhanced Retransmission mode
	L2CAP_SIG_RTNMODE_STRAMING = 0x04, //Streaming mode
}L2CAP_SIG_RTNMODE_ENUM;
typedef enum{
	L2CAP_SIG_FCSTYPE_NOFCS  = 0x00,
	L2CAP_SIG_FCSTYPE_16bFCS = 0x01,
}L2CAP_SIG_FCSTYPE_ENUM;



#define L2CAP_CFG_TYPE_MTU            0x01
#define L2CAP_CFG_TYPE_FLUSH_TO       0x02
#define L2CAP_CFG_TYPE_QOS            0x03
#define L2CAP_CFG_TYPE_RTN            0x04
#define L2CAP_CFG_TYPE_FCS            0x05
#define L2CAP_CFG_TYPE_EXT_FLOW       0x06
#define L2CAP_CFG_TYPE_EXT_WIN_SIZE   0x07


#define L2CAP_INFOTYPE_CONNLESS_MTU         0x01
#define L2CAP_INFOTYPE_EXTENED_FEATURE      0x02
#define L2CAP_INFOTYPE_EXTENED_CHANNELS     0x03  //FIXED CHANNELS SUPPORTED   <Core5.2.pdf>P1061 and 1064






/* default value in spec */
#define L2CAP_DEFAULT_MTU 	                          676
#define L2CAP_MIN_MTU		                           48


/* used for sk_buff */
#define L2CAP_SKB_HEAD_RESERVE      4       /*len+cid+control+sdulen  2+2+2+2*/
#define L2CAP_SKB_TAIL_RESERVE	       0        /* fcs 2 */
#define L2CAP_SKB_RESERVE  (L2CAP_SKB_HEAD_RESERVE + L2CAP_SKB_TAIL_RESERVE)


/* config value */

//#define L2CAP_CFG_MTU 	                                                     (339-L2CAP_SKB_RESERVE)   /* BUF_MAX - L2CAP_SKB_RESERVE */
#define L2CAP_CFG_MTU 	                                             L2CAP_DEFAULT_MTU //676  //enlarge from 335
//#define L2CAP_CFG_MTU_INCOMING 	                                     (DH5_3_PACKET_SIZE-9)//1012
#define L2CAP_CFG_MTU_INCOMING 	                                     672

#define L2CAP_CFG_FLUSH_TO	                                               L2CAP_DEFAULT_FLUSH_TO

#define L2CAP_CFG_QOS_SERVICE_TYPE                                  L2CAP_DEFAULT_QOS_SERVICE_TYPE
#define L2CAP_CFG_QOS_TOKEN_RATE                                       0x00000000
#define L2CAP_CFG_QOS_TOKEN_BUCKET_SIZE                        0x00000000
#define L2CAP_CFG_QOS_PEEK_BANDWIDTH                            0x00000000
#define L2CAP_CFG_QOS_LATENCY                                             0xffffffff
#define L2CAP_CFG_QOS_DELAY_VARIATION                             0xffffffff


#define L2CAP_CFG_RFC_MODE                                                     L2CAP_DEFAULT_RFC_MODE                   /*L2CAP_MODE_BASE*/
/* below is only available when rfc mode is not base mode*/
#define L2CAP_CFG_RFC_TXWINDOW                                            32      /*1 to 32*/
#define L2CAP_CFG_RFC_MAXTRANSMIT                                        32
#define L2CAP_CFG_RFC_RETRANSMISSION_TIMEOUT                 1000
#define L2CAP_CFG_RFC_MONITOR_TIMEOUT                               1000
#define L2CAP_CFG_RFC_MPS                                                       0xFFFF


/* L2CAP_RTX: The Responsive Timeout eXpired timer is used to terminate
   the channel when the remote endpoint is unresponsive to signalling
   requests (min 1s, max 60s) */
#define L2CAP_CFG_RTX 60
/* L2CAP_RTX_MAXN: Maximum number of Request retransmissions before
   terminating the channel identified by the request. The decision
   should be based on the flush timeout of the signalling link. If the
   flush timeout is infinite, no retransmissions should be performed */
#define L2CAP_CFG_RTX_MAXN 0
/* L2CAP_ERTX: The Extended Response Timeout eXpired timer is used in
   place of the RTC timer when a L2CAP_ConnectRspPnd event is received
   (min 60s, max 300s) */
#define L2CAP_CFG_ERTX 300


//////////////////////// new arch  ////////////////////////




typedef enum{
	L2CAP_IFLAG_NONE = 0x00,
	L2CAP_IFLAG_RTNS = 0x01, 
	L2CAP_IFLAG_NFCS = 0x02, //No FCS
}L2CAP_IFLAG_ENUM;



#define BTH_L2CAP_CHN_TIMEOUT           (100000) // 100ms
#define BTH_L2CAP_CHN_CONN_TIMEOUT      (6000000/BTH_L2CAP_CHN_TIMEOUT)
#define BTH_L2CAP_CHN_DISC_TIMEOUT      (3000000/BTH_L2CAP_CHN_TIMEOUT)

typedef enum{
	BTH_L2CAP_EVTID_NONE = 0,
	BTH_L2CAP_EVTID_CLOSED,
	BTH_L2CAP_EVTID_REQUEST,
	BTH_L2CAP_EVTID_DISCONN,
	BTH_L2CAP_EVTID_CONNECT,
}BTH_L2CAP_EVTID_ENUM;

typedef enum{
	BTH_L2CAP_OPTION_MTU     = 0x01,
	BTH_L2CAP_OPTION_TIMEOUT = 0x02,
	BTH_L2CAP_OPTION_RTN     = 0x10,
	BTH_L2CAP_OPTION_FCS     = 0x20,
}BTH_L2CAP_OPTION_ENUM;

typedef enum{
	BTH_L2CAP_REASON_NONE = 0,
	BTH_L2CAP_REASON_DISC_BY_USER,
	BTH_L2CAP_REASON_DISC_BY_REMOTE,
	BTH_L2CAP_REASON_DISC_BY_REJECT, //The remote does not accept the given parameters. 
	BTH_L2CAP_REASON_DISC_BY_TIMEOUT,
	BTH_L2CAP_REASON_DISC_BY_ERR_PRAMA,
	BTH_L2CAP_REASON_DISC_BY_ERR_MTU,
	BTH_L2CAP_REASON_DISC_BY_ERR_STATUS,

	
}BTH_L2CAP_REASON_ENUM;


typedef struct{
	uint16 scid;
	uint16 handle;
	uint08 isActive;
}bth_l2cap_closedEvt_t;
typedef struct{
	uint16 handle;
}bth_l2cap_requestEvt_t;
typedef struct{
	uint16 scid;
	uint16 dcid;
	uint16 handle;
	uint08 userID;
	uint16 mtuSize;
	uint08 isActive; //True-The connection was initiated by us; False-The connection was initiated by peer.
}bth_l2cap_connectEvt_t;
typedef struct{
	uint16 scid;
	uint16 dcid;
	uint16 handle;
	uint08 userID;
	uint16 reason;
}bth_l2cap_disconnEvt_t;



typedef int  (*bth_l2cap_eventCallback_t)(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
typedef void (*bth_l2cap_rdataCallback_t)(uint16 psmID, uint16 chnID, uint16 handle, uint08 *pData, uint16 dataLen);




typedef struct{
    uint16 psmID;
	uint16 resv0;
    bth_l2cap_eventCallback_t eventCB;
    bth_l2cap_rdataCallback_t rdataCB;
}bth_l2cap_service_t;
typedef struct{
	uint16 scid;
    uint16 dcid;
	
	uint08 state; //TLK_STATE_CLOSED, TLK_STATE_OPENED, TLK_STATE_CONNING, TLK_STATE_CONNECT, TLK_STATE_DISCING
	uint08 busys;
	uint08 flags;
	uint08 attrs; //Refer to BTH_L2CAP_ATTRS_ENUM.
	uint08 usrID;
	
    uint16 psmID;
	uint16 mtuSize;
	uint16 timeout;
	uint16 aclHandle;

	uint08 cmdOpcode;
	uint08 rejReason; //reject reason
	uint08 disReason; //disconnect reason
	uint08 cmdIdentify;
	uint08 rejIdentify;
	uint08 rspIdentify;

	tlkapi_timer_t timer;

	//Options
	uint08 option; //Refer to BTH_L2CAP_OPTION_ENUM.
	//Flush Timeout.
	uint16 flushTimeout; 
	//RETRANSMISSION AND FLOW CONTROL OPTION
	uint16 rtnMps;
	uint08 rtnMode;//
	uint08 rtnTxWin;
	uint08 rtnMaxTx;
	uint16 rtnTimeout;
	uint16 monTimeout;
	//FCS
	uint08 fcsType;
}bth_l2cap_channel_t;

typedef struct{
	uint32 sendTimer;
	bth_l2cap_service_t service[TLK_STK_BTPSM_NUMB];
	bth_l2cap_channel_t channel[TLK_STK_BTCHN_NUMB];
}bth_l2cap_ctrl_t;


/******************************************************************************
 * Function: bth_l2cap_init
 * Descript: Initial the l2cap control block.
 * Params: None.
 * Reutrn: TLK_ENONE is success, other value if false.
*******************************************************************************/
int bth_l2cap_init(void);

/******************************************************************************
 * Function: bth_l2cap_aclDisconn
 * Descript: Send l2cap disconnect.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 * Reutrn: TLK_ENONE is success, other value if false.
*******************************************************************************/
int bth_l2cap_aclDisconn(uint16 aclHandle);

bool bth_l2cap_isBusy(void);

/******************************************************************************
 * Function: bth_l2cap_setMtuSize
 * Descript: Set the Mtu size.
 * Params:
 *     @size[IN]--The mtu size.
 * Reutrn: None.
*******************************************************************************/
void bth_l2cap_setMtuSize(uint16 size);
uint bth_l2cap_getMtuSize(void);

uint bth_l2cap_getValidCID(void);


/******************************************************************************
 * Function: bth_l2cap_regServiceCB
 * Descript: Register the callback.
 * Params:
 *     @psmID[IN]--The psm id.
 *     @usrID[IN]--The user id.
 *     @eventCB[IN]--The l2cap event callback.
 *     @rdataCB[IN]--The read data callback.
 * Reutrn: TLK_ENONE is success, other' value is failure.
*******************************************************************************/
int bth_l2cap_regServiceCB(uint16 psmID, bth_l2cap_eventCallback_t eventCB, bth_l2cap_rdataCallback_t rdataCB);

/******************************************************************************
 * Function: bth_l2cap_sendEvent
 * Descript: Set the Mtu size.
 * Params:
 *     @size[IN]--The mtu size.
 * Reutrn: None.
*******************************************************************************/
int bth_l2cap_sendEvent(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_l2cap_getIdleService
 * Descript: Get the l2cap service.
 * Params: None.
 * Reutrn: Return L2cap service.
*******************************************************************************/
bth_l2cap_service_t *bth_l2cap_getIdleService(void);

/******************************************************************************
 * Function: bth_l2cap_getUsedService
 * Descript: Get the Used l2cap service.
 * Params: @psmID[IN]--The psm id.
 * Reutrn: Return L2cap service.
*******************************************************************************/
bth_l2cap_service_t *bth_l2cap_getUsedService(uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getIdleChannel
 * Descript: Get the idle l2cap channel.
 * Params: 
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getIdleChannel(void);

/******************************************************************************
 * Function: bth_l2cap_getInitChannel
 * Descript: Get the init l2cap channel.
 * Params: 
 *     @scid[IN]--The channel id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getInitChannel(uint16 scid);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByScid
 * Descript: Get the Used l2cap channel by scid.
 * Params: 
 *     @scid[IN]--The channel id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByScid(uint16 scid);

/******************************************************************************
 * Function: bth_l2cap_getConnChannelByScid
 * Descript: Get the Connect l2cap channel by scid.
 * Params: 
 *     @scid[IN]--The channel id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByScid(uint16 scid);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByDcid
 * Descript: Get the Used l2cap channel by data channel id.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @dcid[IN]--The data channel id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByDcid(uint16 aclHandle, uint16 dcid);

/******************************************************************************
 * Function: bth_l2cap_getConnChannelByDcid
 * Descript: Get the Connect l2cap channel by data channel id.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @dcid[IN]--The data channel id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByDcid(uint16 aclHandle, uint16 dcid);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByUser
 * Descript: Get the Used l2cap channel by userid.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 *     @usrID[ID]--The user id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByUser(uint16 aclHandle, uint16 psmID, uint08 usrID);

/******************************************************************************
 * Function: bth_l2cap_getConnChannelByUser
 * Descript: Get the Connect l2cap channel by userid.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 *     @usrID[ID]--The user id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByUser(uint16 aclHandle, uint16 psmID, uint08 usrID);

/******************************************************************************
 * Function: bth_l2cap_getInitChannelByPsm
 * Descript: Get the Init l2cap channel by psm.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getInitChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByPsm
 * Descript: Get the Used l2cap channel by psm.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByPsm
 * Descript: Get the Connected l2cap channel by psm.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getNoConnChannelByPsm
 * Descript: Get the no Connected l2cap channel by psm.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getNoConnChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getNoDiscChannelByPsm
 * Descript: Get the no DisConnected l2cap channel by psm.
 * Params: 
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Reutrn: Return L2cap Channel.
*******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getNoDiscChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getSendFifoNumb
 * Descript: Gets the number of TX-FIFOs that can be used.
 * Params: None.
 * Reutrn: The number of TX-FIFOs.
*******************************************************************************/
int bth_l2cap_getValidTxFifoNumb(void);

/******************************************************************************
 * Function: bth_l2cap_sendData
 * Descript: Send the l2cap data.
 * Params: 
 *     @connHandle[IN]--The conntion handle.
 *     @pHead[IN]--The head data.
 *     @headLen[IN]--The headdata length.
 *     @pData[IN]--The payload data.
 *     @dataLen[IN]--The payload data length.
 * Reutrn: Return TLK_ENONE is success or others value is failure.
*******************************************************************************/
int	bth_l2cap_sendData(uint16 connHandle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_l2cap_sendChannelData
 * Descript: Send the l2cap data.
 * Params: 
 *     @connHandle[IN]--The conntion handle.
 *     @scid[IN]--The channel id.
 *     @pHead[IN]--The head data.
 *     @headLen[IN]--The headdata length.
 *     @pData[IN]--The payload data.
 *     @dataLen[IN]--The payload data length.
 * Reutrn: Return TLK_ENONE is success or others value is failure.
*******************************************************************************/
int	bth_l2cap_sendChannelData(uint16 connHandle, uint16 scid, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_l2cap_sendChannelDataExt
 * Descript: Send the l2cap data with user extend data.
 * Params: 
 *     @connHandle[IN]--The conntion handle.
 *     @scid[IN]--The channel id.
 *     @pUsrExt[IN]--The user extend data.
 *     @extLen[IN]--The data length.
 *     @pHead[IN]--The head data.
 *     @headLen[IN]--The headdata length.
 *     @pData[IN]--The payload data.
 *     @dataLen[IN]--The payload data length.
 * Reutrn: Return TLK_ENONE is success or others value is failure.
*******************************************************************************/
int	bth_l2cap_sendChannelDataExt(uint16 connHandle, uint16 scid, uint08 *pUsrExt, uint08 extLen, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: bth_l2cap_recvHandler
 * Descript: Receive the l2cap data.
 * Params: 
 *     @connHandle[IN]--The conntion handle.
 *     @llid[IN]--The ll id.
 *     @p[IN]--The data.
 *     @len[IN]--The data length.
 * Reutrn: Return TLK_ENONE is success or others value is failure.
*******************************************************************************/
int	bth_l2cap_recvHandler(uint16 connHandle, int llid, uint08 *p, int len);



/******************************************************************************
 * Function: l2cap_handle_data
 * Descript: Handle the l2cap data.
 * Params: 
 *     @connHandle[IN]--The conntion handle.
 *     @p[IN]--The data.
 *     @len[IN]--The data length.
 * Reutrn: Return TLK_ENONE is success or others value is failure.
*******************************************************************************/
int l2cap_handle_data(uint16 conn_handle, uint08 *p, int len);

/******************************************************************************
 * Function: l2cap_data_tx
 * Descript: send the l2cap data.
 * Params: 
 *     @conhdl[IN]--The conntion handle.
 *     @p[IN]--The data.
 *     @len[IN]--The data length.
 * Reutrn: Return TLK_ENONE is success or others value is failure.
*******************************************************************************/
int	bth_l2cap_recvHandler (unsigned short conhdl, int llid, uint08 *p, int len);

int bth_hci_pushCmdToFifo(uint16 opcode, uint08 *pData, uint08 dataLen);

#define L2CAP_HDR_LEN               4
#define L2CAP_SIG_HDR_LEN           4

#define L2CAP_CFG_REQ_LEN           4
#define L2CAP_CFG_RSP_LEN           6

#define L2CAP_CFG_OPT_HDR_LEN       2



#if 0//(L2CAP_PERFECT_ENABLE)
int l2cap_sendData(u16 connHandle, u16 channel, u8 *pHead, u16 headLen, u8 *pBody, u16 bodyLen);
int l2cap_sendSigData(u16 connHandle, u8 *pHead, u16 headLen, u8 *pBody, u16 bodyLen);
int l2cap_sendSigRejectRsp(u16 connHandle, u8 identify, u16 reason, u8 *pData, u16 dataLen);

//I-frame Standard Control Field  Core5.2:P1038
typedef union{
	uint16_t value;
	struct{
		uint16_t Type:1; //0:I-Frame, 1-S-Frame
		uint16_t TxSeq:6; //Send Sequence Number (Sequence Number Of Acknowledgement)
		uint16_t R:1;  //Final:The F-bit is set to 1 in response to an S-frame with the P bit set to 1.
		uint16_t ReqSeq:6; //Receive Sequence Number
		uint16_t SAR:2; //Segmentation and Reassembly
	}field;
}l2cap_iframeStdCtrlField_t;
//I-frame Enhanced Control Field
typedef union{
	uint16_t value;
	struct{
		uint16_t Type:1; 
		uint16_t TxSeq:6;
		uint16_t F:1;
		uint16_t ReqSeq:6;
		uint16_t SAR:2;
	}field;
}l2cap_iframeEnhCtrlField_t;
//I-frame Extended Control Field
typedef union{
	uint32_t value;
	struct{
		uint32_t Type:1;
		uint32_t F:1;
		uint32_t TxSeq:14;
		uint32_t SAR:2;
		uint32_t ReqSeq:14;
	}field;
}l2cap_iframeExtCtrlField_t;

//S-frame Standard Control Field  Core5.2:P1038
typedef union{
	uint16_t value;
	struct{
		uint16_t Type:1;   //0:I-Frame, 1-S-Frame
		uint16_t RFU0:1;   //
		uint32_t S:2;      //Supervisory function. 00-RR,Receiver Ready; 01-REJ,Reject; 10-RNR,Receiver Not Ready; 11-SREJ,Select Reject
		uint16_t RFU1:3;   //
		uint16_t R:1;      //Retransmission Disable Bit. 0-Normal operation; 1-Receiver side requests sender to postpone retransmission of I-frames.
		uint16_t ReqSeq:6; //Receive Sequence Number
		uint16_t RFU2:2;   //
	}field;
}l2cap_sframeStdCtrlField_t;
//S-frame Enhanced Control Field
typedef union{
	uint16_t value;
	struct{
		uint16_t Type:1;   //0:I-Frame, 1-S-Frame
		uint16_t RFU0:1;   //
		uint32_t S:2;      //Supervisory function. 00-RR,Receiver Ready; 01-REJ,Reject; 10-RNR,Receiver Not Ready; 11-SREJ,Select Reject
		uint16_t P:1;      //Retransmission Disable Bit. 0-Normal operation; 1-Receiver side requests sender to postpone retransmission of I-frames.
		uint16_t RFU1:2;   //Poll. The P-bit is set to 1 to solicit a response from the receiver. The receiver shallrespond immediately with a frame with the F-bit set to 1.
		uint16_t F:1;      //Final. The F-bit is set to 1 in response to an S-frame with the P bit set to 1.
		uint16_t ReqSeq:6; //Receive Sequence Number
		uint16_t RFU2:2;   //
	}field;
}l2cap_sframeEnhCtrlField_t;
//S-frame Extended Control Field
typedef union{
	uint32_t value;
	struct{
		uint32_t Type:1;
		uint32_t F:1;
		uint32_t TxSeq:14;
		uint32_t S:2;
		uint32_t P:1;
		uint32_t ReqSeq:13;
	}field;
}l2cap_sframeExtCtrlField_t;
uint8_t l2cap_sendFrame(l2cap_channel_t *pChannel, uint16_t ctrl, uint8_t *pHead, uint16_t headLen, uint8_t *pData, uint16_t dataLen, bool isAddCtr, bool isAddFcs);
uint8_t l2cap_sendIFrame(l2cap_channel_t *pChannel, uint16_t ictrl, uint8_t *pHead, uint16_t headLen, uint8_t *pData, uint16_t dataLen);
uint8_t l2cap_sendIFrameWithoutFcs(l2cap_channel_t *pChannel, uint16_t ictrl, uint8_t *pHead, uint16_t headLen, uint8_t *pData, uint16_t dataLen);
uint8_t l2cap_sendIFrameWithoutCtrFcs(l2cap_channel_t *pChannel, uint16_t ictrl, uint8_t *pHead, uint16_t headLen, uint8_t *pData, uint16_t dataLen);
uint8_t l2cap_sendSFrame(l2cap_channel_t *pChannel, uint16_t sctrl, uint8_t *pHead, uint16_t headLen, uint8_t *pData, uint16_t dataLen);
uint8_t l2cap_sendSFrameWithoutFcs(l2cap_channel_t *pChannel, uint16_t sctrl, uint8_t *pHead, uint16_t headLen, uint8_t *pData, uint16_t dataLen);

#endif





#endif //BTH_L2CAP_H

