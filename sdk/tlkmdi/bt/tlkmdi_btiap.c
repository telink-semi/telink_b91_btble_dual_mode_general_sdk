/********************************************************************************************************
 * @file	tlkmdi_btiap.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_MDI_BTIAP_ENABLE)
#include <stdint.h>
#include "tlkmdi/bt/tlkmdi_btiap.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/iap/btp_iap.h"
#include "tlkdev/ext/tlkdev_mfi.h"
#include "tlklib/ios/iap2/iAP2LinkConfig.h"
#include "tlklib/ios/iap2/iAP2Defines.h"
#include "tlklib/ios/iap2/iAP2Log.h"
#include "tlklib/ios/iap2/iAP2FSM.h"
#include "tlklib/ios/iap2/iAP2Time.h"
#include "tlklib/ios/iap2/iAP2ListArray.h"
#include "tlklib/ios/iap2/iAP2BuffPool.h"
#include "tlklib/ios/iap2/iAP2Packet.h"
#include "tlklib/ios/iap2/iAP2Link.h"
#include "tlklib/ios/iap2/iAP2LinkRunLoop.h"


#define TLKMDI_BTIAP_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_IAP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTIAP_DBG_SIGN       "[MDI]"

#define IAP2_PACKET_SYNC                       0xFF
#define IAP2_PACKET_SOP                        0x5A
#define IAP2_PACKET_SOP_ORIG                   0x55
#define IAP2_PACKET_CONTROL_SYN                0x80   /* synchronization */
#define IAP2_PACKET_CONTROL_ACK                0x40   /* acknowledgement */
#define IAP2_PACKET_CONTROL_EAK                0x20   /* extended acknowledgement */
#define IAP2_PACKET_CONTROL_RST                0x10   /* reset */
#define IAP2_PACKET_CONTROL_SUS                0x08   /* suspend (sleep) */
#define IAP2_SESSION_START_MSB                 0x40
#define IAP2_SESSION_START_LSB                 0x40

#define IPA2_CONTROL_SESSION_VERSION                  1
#define IAP2_RequestAuthenticationCertificate         0xAA00
#define IAP2_RequestAuthenticationChallengeResponse   0xAA02
#define IAP2_AuthenticationFailed					  0xAA04
#define IAP2_AuthenticationSucceeded                  0xAA05

#define IAP2_StartIdentification                      0x1D00
#define IAP2_IdentificationAccepted                   0x1D02

#define IAP2_StartExternalAccessoryProtocolSession    0xEA00
#define IAP2_StopExternalAccessoryProtocolSession     0xEA01


#define IAP2_STACK_EN     1


static void tlkmdi_btiap_dataRecv(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
#if !(IAP2_STACK_EN)
static void tlkmdi_btiap_hardCodedRecvDeal(uint08 *pData, uint16 dataLen, uint08 *pSendData, uint08 srcFlag, struct iAP2Link_st* link);
static void tlkmdi_btiap_eapRecvDeal(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
#endif
static BOOL tlkmdi_btiap_eap2RecvDeal(struct iAP2Link_st *pLink, uint08 *pData, uint32 dataLen, uint08 session);
static void tlkmdi_btiap_eap2SendDeal(struct iAP2Link_st *pLink, iAP2Packet_t*packet);
#if !(IAP2_STACK_EN)
static uint08 tlkmdi_btiap_calcChecksum(uint08 *pData, uint32 dataLen);
#endif


#if IAP2_STACK_EN
static iAP2Packet_t sTlkMdiBtIapPacket;
static iAP2LinkRunLoop_t *sTlkMdiBtIapRanLoop = nullptr;
#endif    

uint16 eap_session_id=0;
uint16 eap_flag=0; 

const uint08 identification_information_part1[] = {
	0x00, 0x0C, 0x00, 0x01, 'C', 'X', 'P', 'L', '3', '0', '1', 0x00,
	0x00, 0x11, 0x00, 0x02, 'C', 'r', 'i','c', 'u','t', ',',' ', 'I', 'n', 'c', '.', 0x00,
	0x00, 0x0E, 0x00, 0x03, '1', '2', '3', '4', '5', '6', '7', '8', '9', 0x00,
	0x00, 0x0a, 0x00, 0x04, '1', '.', '0','.', '0', 0x00,
	0x00, 0x0a, 0x00, 0x05, '2', '.', '0','.', '0', 0x00,
	0x00, 0x06, 0x00, 0x06, 0xea,0x02,
	0x00, 0x08, 0x00, 0x07, 0xea,0,0xea,1,
	0x00, 0x05, 0x00, 0x08, 0x00,
	0x00, 0x06, 0x00, 0x09, 0x00, 0x00,
	0x00, 0x2e, 0x00, 0x0a, 0,5,0,0,1,  0,0x20,0,1,'c','o','m','.','c','r','i','c','u','t','.','e','x','p','l','o','r','e','.','p','r','o','t','o','c','o','l',0,  0,5,0,2,0,
	0x00, 0x25, 0x00, 0x0a, 0,5,0,0,2,  0,0x17,0,1,'c','o','m','.','c','r','i','c','u','t','.','e','x','p','l','o','r','e',0,  0,5,0,2,0,
	0x00, 0x0f, 0x00, 0x0b, '2','5','6','2','7','Z','F','V','T','7',0,
	0x00, 0x07, 0x00, 0x0C, 0x65, 0x6E, 0x00,
	0x00, 0x07, 0x00, 0x0D, 0x65, 0x6E, 0x00,
	0x00, 0x15, 0x00, 0x22, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 0
};
const uint08 identification_information_part2[] = {
	0x00, 0x38, 0x00, 0x11, 0,6,0,0,0,0, 0,0x20,0,1,'B','l','u','e','t','o','o','t','h','T','r','a','n','s','p','o','r','t','C','o','m','p','o','n','e','n','t',0,0,4,0,2,0,0x0a,0,3,0x01,0xda,0x20,0xe9,0x28,0xe3
};

static tlkmdi_btiap_t sTlkMdiBtIapCtrl;
//iAP2PacketSYNData_t test_synParam = {
//	1, //version
//	5, //maxOutstandingPackets
//	3, //maxRetransmissions
//	3, //maxCumAck
//	4096, //maxPacketSize
//	1035, //retransmitTimeout
//	0xc8, //cumAckTimeout
//	2, //numSessionInfo
//	5, //peerMaxOutstandingPackets
//	256, //peerMaxPacketSize
//	{{0xa,0,1},{0x0b,2,1}}
//};
iAP2PacketSYNData_t test_synParam = { //<Accessory Interface Specification R36.pdf> P394
	1, //version
	5, //maxOutstandingPackets
	30, //maxRetransmissions
	3, //maxCumAck
	650, //maxPacketSize
	1500, //retransmitTimeout
	73, //cumAckTimeout
	2, //numSessionInfo
	5, //peerMaxOutstandingPackets
	650, //peerMaxPacketSize
	{{0xa,0,1},{0x0b,2,1}}
};



int tlkmdi_btiap_init(void)
{
	#if (TLK_DEV_MFI_ENABLE)
	int ret;
	#endif

	tmemset(&sTlkMdiBtIapCtrl, 0, sizeof(tlkmdi_btiap_t));
	
	#if (TLK_DEV_MFI_ENABLE)
	ret = tlkdev_mfi_init();
	if(ret == TLK_ENONE){
		ret = tlkdev_mfi_open();
	}
	if(ret != TLK_ENONE){
		tlkdev_mfi_close();
		return ret;
	}
	#endif
	
	tlkmdi_btiap_setName((uint08*)"Telink-IAP", strlen("Telink-IAP"));
	#if (IPA2_CONTROL_SESSION_VERSION == 2)
	test_synParam.sessionInfo[0].version = 2;
	#endif
	
	btp_iap_regDataCB(tlkmdi_btiap_dataRecv);
	
	return TLK_ENONE;
}

void tlkmdi_btiap_setAddr(uint08 bdaddr[6])
{
	tmemcpy(sTlkMdiBtIapCtrl.bdaddr, bdaddr, 6);
}
void tlkmdi_btiap_setName(uint08 *pName, uint08 nameLen)
{
	if(pName == nullptr || nameLen == 0) return;
	if(nameLen > 32) nameLen = 32;
	sTlkMdiBtIapCtrl.nameLen = nameLen;
	tmemcpy(sTlkMdiBtIapCtrl.name, pName, nameLen);
}

extern void iap2_memInit(void);
void tlkmdi_btiap_setAclHandle(bool isConn, uint16 aclHandle)
{
	tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "tlkmdi_btiap_setAclHandle: %d 0x%x",
		isConn, aclHandle);
	tmemset(&sTlkMdiBtIapPacket, 0, sizeof(iAP2Packet_t));
	if(isConn && sTlkMdiBtIapCtrl.aclHandle == 0){
		sTlkMdiBtIapCtrl.aclHandle = aclHandle;
		sTlkMdiBtIapCtrl.detFlag = 0x55;

		iap2_memInit();
	    memset(&sTlkMdiBtIapPacket, 0, sizeof(iAP2Packet_t));
	    sTlkMdiBtIapRanLoop = iAP2LinkRunLoopCreateAccessory(&test_synParam, (void *)0, 
	    	(iAP2LinkSendPacketCB_t)tlkmdi_btiap_eap2SendDeal,
			(iAP2LinkDataReadyCB_t)tlkmdi_btiap_eap2RecvDeal,
			(void *)0, (void *)0, 0, 1, nullptr);
		if(sTlkMdiBtIapRanLoop != nullptr){
			sTlkMdiBtIapPacket.link = sTlkMdiBtIapRanLoop->link;
		}
	}else if(!isConn && sTlkMdiBtIapCtrl.aclHandle == aclHandle){
		sTlkMdiBtIapCtrl.aclHandle = 0;
		sTlkMdiBtIapCtrl.detFlag = 0xAA;
		sTlkMdiBtIapRanLoop = nullptr;
		sTlkMdiBtIapPacket.link = nullptr;
	}
}


void tlkmdi_btiap_handler(void)
{
#if IAP2_STACK_EN
	if(sTlkMdiBtIapCtrl.aclHandle == 0 || sTlkMdiBtIapRanLoop == nullptr) return;

    if(sTlkMdiBtIapCtrl.detFlag == 0x55)
    {
		uint08 kIap2PacketDetectData1[] = { 0xFF, 0x55, 0x02, 0x00, 0xEE, 0x10 };
		if(btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, kIap2PacketDetectData1, 6) == TLK_ENONE){
			tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "tlkmdi_btiap_handler: send_detect");
        	sTlkMdiBtIapCtrl.detFlag = 0;
		}
    }
    else if(sTlkMdiBtIapCtrl.detFlag == 0xaa)
    {
        iAP2LinkRunLoopDetached(sTlkMdiBtIapRanLoop);
        sTlkMdiBtIapCtrl.detFlag = 0;
    }

//    uint32 iap2_curTime = iAP2TimeGetCurTimeMs();
//    _iAP2TimeHandleExpired(sTlkMdiBtIapRanLoop->link->mainTimer, iap2_curTime);
	iAP2LinkRunLoopRunOnce(sTlkMdiBtIapRanLoop, (void *)(&sTlkMdiBtIapPacket));
#endif
}

static void tlkmdi_btiap_dataRecv(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
#if IAP2_STACK_EN
    uint08 detect;
    uint32 failedChecksums;
    uint32 sopdetect;

    if(sTlkMdiBtIapRanLoop == nullptr) return;

    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "tlkmdi_btiap_dataRecv", pData, dataLen);
    iAP2PacketParseBuffer(pData, dataLen, &sTlkMdiBtIapPacket, 650, (BOOL*)&detect, &failedChecksums, &sopdetect);

    if(detect == 1){
        tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "detect device");
        iAP2LinkRunLoopAttached(sTlkMdiBtIapRanLoop);
        tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "sTlkMdiBtIapPacket.bufferLen=%d", sTlkMdiBtIapPacket.bufferLen);
    }
#else
	tlkmdi_btiap_eapRecvDeal(aclHandle, rfcHandle, pData, dataLen);
#endif
}






#if IAP2_STACK_EN
static void LinkDataSentCB(struct iAP2Link_st* link, void* context)
{
	tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "LinkDataSentCB");
}
static void tlkmdi_btiap_eap2SendDeal(struct iAP2Link_st* link, iAP2Packet_t*packet)
{
	*(uint08 *)((uint08 *)packet->pckData + 8) = iAP2PacketCalcHeaderChecksum(packet);
	if(packet->packetLen > 9){
		*(uint08 *)((uint08 *)packet->pckData + packet->packetLen-1) = iAP2PacketCalcPayloadChecksum(packet);//packet->dataChecksum;
	}
	tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "tlkmdi_btiap_eap2SendDeal data=", (uint08 *)packet->pckData, packet->packetLen);
	btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)packet->pckData, packet->packetLen);  
}
static BOOL tlkmdi_btiap_eap2RecvDeal(struct iAP2Link_st* link, uint08* data, uint32 dataLen, uint08 session)
{
	BOOL ret;
	uint16 len;
	uint16 pos=0;
	uint08 buffer[628] = {0xff, 0x5a, 2, 0x74, 0x40, 0x2d, 6,0x0a,0x2b,0x40,0x40,0x02,0x6a,0xaa,0x1,2,0x64,0,0};
	uint08 ack[] = {0xff, 0x5a, 0, 0x9, 0x40, 0x2b, 0xdc, 0, 0x2c};
    uint16 command = (uint16)((data[4] << 8) | data[5]);
	
    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "tlkmdi_btiap_eap2RecvDeal data=", data, dataLen);
	ret = true;
	if(command == IAP2_RequestAuthenticationCertificate && data[0] == IAP2_SESSION_START_MSB)
	{
		buffer[0] = 0x40;
		buffer[1] = 0x40;
		buffer[2] = 0x02;
		buffer[3] = 0x6a;
		buffer[4] = 0xaa;
		buffer[5] = 0x01;
		buffer[6] = 0x02;
		buffer[7] = 0x64;
		buffer[8] = 0x00;
		buffer[9] = 0x00;
		#if (TLK_DEV_MFI_ENABLE)
		ret = tlkdev_mfi_loadCertificateData(&buffer[10], 608);
		#endif
		len = 10 + 608;
		ret = iAP2LinkQueueSendData(link, buffer, len, session, NULL, LinkDataSentCB);
	}
	else if(command == IAP2_RequestAuthenticationChallengeResponse && data[0] == IAP2_SESSION_START_MSB)
	{
        buffer[0] = 0x40;
        buffer[1] = 0x40;
        buffer[2] = 0x00;
        buffer[3] = 0x4a;
        buffer[4] = 0xaa;
        buffer[5] = 0x03;
        buffer[6] = 0x00;
        buffer[7] = 0x44;
        buffer[8] = 0x00;
        buffer[9] = 0x00;
		#if (TLK_DEV_MFI_ENABLE)
		tlkdev_mfi_loadChallengeData(&data[10], ((data[6]<<8)|data[7])-4, &buffer[10], 64);
		#endif
		len = 10 + 64;
		ret = iAP2LinkQueueSendData(link, buffer, len, session, NULL, LinkDataSentCB);
	}
	else if(command == IAP2_AuthenticationSucceeded && data[0] == IAP2_SESSION_START_MSB)
	{
		tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "authentication succ ");
	}
	else if(command == IAP2_StartIdentification && data[0] == IAP2_SESSION_START_MSB)
	{
	    tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StartIdentification");
		//identification
		buffer[0] = 0x40;
        buffer[1] = 0x40;
        buffer[4] = 0x1d;
        buffer[5] = 0x01;
        buffer[6] = (uint08)((sTlkMdiBtIapCtrl.nameLen+5)>>8);
        buffer[7] = (uint08)(sTlkMdiBtIapCtrl.nameLen+5);
        //param id =0 : name
        buffer[8] = 0;
        buffer[9] = 0;
        pos = 10;
		if(sTlkMdiBtIapCtrl.nameLen != 0){
			tmemcpy(&buffer[pos], sTlkMdiBtIapCtrl.name, sTlkMdiBtIapCtrl.nameLen);
		}
        pos += sTlkMdiBtIapCtrl.nameLen;
        buffer[pos] = 0;
        pos++;
        tmemcpy(&buffer[pos],identification_information_part1,sizeof(identification_information_part1));
        pos += (sizeof(identification_information_part1));
        tmemcpy(&buffer[pos],identification_information_part2,sizeof(identification_information_part2));
        pos += (sizeof(identification_information_part2));
        tmemcpy(&buffer[pos-6], sTlkMdiBtIapCtrl.bdaddr, 6);
        buffer[2] = (uint08)(pos>>8);
        buffer[3] = (uint08)pos;
        len = pos;
		ret = iAP2LinkQueueSendData(link, buffer, len, session, NULL, LinkDataSentCB);
	}
	else if(command == IAP2_IdentificationAccepted && data[0] == IAP2_SESSION_START_MSB)
	{
		tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "identification accept");
	}
    else if(command == IAP2_StartExternalAccessoryProtocolSession && data[0] == IAP2_SESSION_START_MSB)
    {
        tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StartExternalAccessoryProtocolSession", data, dataLen);
        ack[5] = link->sentSeq;
        ack[6] = link->recvSeq;
	    ack[8] = iAP2PacketCalcChecksum(ack,8);
	    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StartExternalAccessoryProtocolSession succ ack=", (uint08 *)ack, 9);
	    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
        eap_session_id = (uint16)((uint16)(data[15]<<8) | (uint16)data[16]);
        eap_flag = 0x55;
    }
    else if(command == IAP2_StopExternalAccessoryProtocolSession && data[0] == IAP2_SESSION_START_MSB)
    {
        tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StopExternalAccessoryProtocolSession", data, dataLen);
        ack[5] = link->sentSeq;
        ack[6] = link->recvSeq;
	    ack[8] = iAP2PacketCalcChecksum(ack,8);
	    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StopExternalAccessoryProtocolSession succ ack=", (uint08 *)ack, 9);
	    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
        eap_flag = 0;
    }
    //EAP
    if(data[0] == (eap_session_id>>8) && data[1] == (uint08)eap_session_id && eap_flag == 0x55)
    {
        tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "recv data", data, dataLen);
        ack[5] = link->sentSeq;
        ack[6] = link->recvSeq;
	    ack[8] = iAP2PacketCalcChecksum(ack,8);
	    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP succ ack=", (uint08 *)ack, 9);
	    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));

		#if IAP2_HARDCODED
		tlkmdi_btiap_hardCodedRecvDeal(data+2, dataLen, buffer, 1, link);
		#endif
        /*
        app_iap2_handle_flag = 0x55;
        buffer[2] = (dataLen+10)>>8;
        buffer[3] = dataLen+10;
        buffer[5] = link->sentSeq+1;
        buffer[6] = link->recvSeq;
        buffer[7] = 0x0b;
        buffer[8] = iAP2PacketCalcChecksum(buffer,8);
        tmemcpy(&buffer[9],data,dataLen+1);
	    my_dump_str_data (1, "IAP2_EAP Loopback=", (uint08 *)buffer, dataLen+10);
	    iap_tx_data_req(&app_ctl.dev_array[app_ctl.app_cur_index].remote,(uint08 *)buffer,dataLen+10);
	    */
    }

    return ret;  
}
#else 
static void tlkmdi_btiap_eapRecvDeal(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	uint16 pos=0;
    uint08 ack[9] = {0xff, 0x5a, 0, 0x9, 0x40, 0x2b, 0xdc, 0, 0x2c};
    uint08 buffer[628] = {0xff, 0x5a, 2, 0x74, 0x40, 0x2d, 6,0x0a,0x2b,0x40,0x40,0x02,0x6a,0xaa,0x1,2,0x64,0,0};
    
    if(pData[0] != IAP2_PACKET_SYNC) return;
    if(pData[1] == IAP2_PACKET_SOP_ORIG)
    {
    	uint08 detect_device[] = {0xff,0x55,0x02,0x00,0xee,0x10};
        if(tmemcmp(pData, detect_device, sizeof(detect_device)) == 0)
        {
        	#if (IPA2_CONTROL_SESSION_VERSION == 1)
		    uint08 syn[] = {0xff,0x5a, 0, 0x1a, 0x80, 0x2b, 0, 0, 0xe5, 0x01,0x04, 0x01, 0, 4, 0xc8, 0,0x86, 03, 02, 0xa, 0, 01, 0xb, 02,  01,98};
			#else
		    uint08 syn[] = {0xff,0x5a, 0, 0x1a, 0x80, 0x2b, 0, 0, 0xe5, 0x01,0x04, 0x01, 0, 4, 0xc8, 0,0x86, 03, 02, 0xa, 0, 02, 0xb, 02,  01,98};
			#endif
            //detect device
            dataLen = sizeof(syn);
            syn[8] = tlkmdi_btiap_calcChecksum(syn, 8);
            syn[dataLen-1] = tlkmdi_btiap_calcChecksum(syn, dataLen-1);
            tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "syn=", syn, dataLen); 
            btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)syn,sizeof(syn)); 
        }
    }
	if(pData[1] != IAP2_PACKET_SOP) return;
    
	if(pData[4] == (IAP2_PACKET_CONTROL_SYN|IAP2_PACKET_CONTROL_ACK))
	{
		ack[6] = pData[5];
		ack[8] = tlkmdi_btiap_calcChecksum(ack,8);
		tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "ack data=", (uint08 *)ack, 9);
		btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack, sizeof(ack));
	}
	if(pData[4] == IAP2_PACKET_CONTROL_ACK && pData[9] == IAP2_SESSION_START_MSB && pData[10] == IAP2_SESSION_START_LSB)
	{
	    uint16 command = (uint16)((pData[13] <<8) | pData[14]);
        if(command == IAP2_RequestAuthenticationCertificate)
        {
            buffer[5] = pData[6]+1;
            buffer[6] = pData[5];
            buffer[8] = tlkmdi_btiap_calcChecksum(buffer,8);			
			tlkdev_mfi_loadCertificateData(&buffer[19], 608);
            buffer[627] = tlkmdi_btiap_calcChecksum(buffer,627);
            tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "cert=", buffer,628); 
            btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)buffer,628);
        }
        else if(command == IAP2_RequestAuthenticationChallengeResponse)
        {
        	uint08 cert_response[84] = {0xff, 0x5a, 0, 0x54, 0x40, 0x2d, 6,0x0a,0x2b,0x40,0x40,0x00,0x4a,0xaa,0x3,0,0x44,0,0};
            cert_response[5] = pData[6]+1;
            cert_response[6] = pData[5];
            cert_response[8] = tlkmdi_btiap_calcChecksum(cert_response,8);
			tlkdev_mfi_loadChallengeData(&pData[19], ((pData[15]<<8)|pData[16])-4, &cert_response[19], 64);
            cert_response[83] = tlkmdi_btiap_calcChecksum(cert_response,83);
            //tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "cert_response=", cert_response,84); 
            btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)cert_response, 84);  
        }
        else if(command == IAP2_AuthenticationSucceeded)
        {
            ack[5] = pData[6];
            ack[6] = pData[5];
		    ack[8] = tlkmdi_btiap_calcChecksum(ack,8);
		    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "authentication succ ack=", (uint08 *)ack, 9);
		    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
        }
        else if(command == IAP2_StartIdentification)
        {
        	uint08 idInfoHead[]={0xFF, 0x5A, 0x01, 0x34, 0x40, 0x2E, 0x66, 0x0A, 0x13, 0x40, 0x40, 0x01, 0x2A, 0x1D, 0x01};
            //identification
            tmemcpy(buffer, idInfoHead, sizeof(idInfoHead));
    		buffer[5] = pData[6]+1;
            buffer[6] = pData[5];
            buffer[15] = (uint08)((sTlkMdiBtIapCtrl.nameLen+5)>>8);
            buffer[16] = (uint08)(sTlkMdiBtIapCtrl.nameLen+5);
            //param id =0 : name
            buffer[17] = 0;
            buffer[18] = 0;
            pos = 19;
			if(sTlkMdiBtIapCtrl.nameLen != 0){
				tmemcpy(&buffer[pos], sTlkMdiBtIapCtrl.name, sTlkMdiBtIapCtrl.nameLen);
			}
            pos += sTlkMdiBtIapCtrl.nameLen;
            buffer[pos] = 0;
            pos++;
            tmemcpy(&buffer[pos],identification_information_part1,sizeof(identification_information_part1));
            pos += (sizeof(identification_information_part1));
            tmemcpy(&buffer[pos],identification_information_part2,sizeof(identification_information_part2));
            pos += (sizeof(identification_information_part2));
            tmemcpy(&buffer[pos-6], sTlkMdiBtIapCtrl.bdaddr, 6);
       
            pos++;
            buffer[2] = (uint08)(pos>>8);
            buffer[3] = (uint08)pos;
            buffer[11] = (uint08)((pos-10)>>8);
            buffer[12] = (uint08)(pos-10);
            buffer[8] = tlkmdi_btiap_calcChecksum(buffer, 8);
    		buffer[pos-1] = tlkmdi_btiap_calcChecksum(buffer, pos-1);
    		tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IdentificationInformation=", buffer, pos);
    		btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, buffer,pos);
        }
        else if(command == IAP2_IdentificationAccepted)
        {
            ack[5] = pData[6];
            ack[6] = pData[5];
	        ack[8] = tlkmdi_btiap_calcChecksum(ack,8);
	        tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "identification accept ack=", (uint08 *)ack, 9);
	        btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
        }
        else if(command == IAP2_StartExternalAccessoryProtocolSession)
        {
            tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StartExternalAccessoryProtocolSession", pData, dataLen);
            ack[5] = pData[6];
            ack[6] = pData[5];
		    ack[8] = tlkmdi_btiap_calcChecksum(ack,8);
		    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StartExternalAccessoryProtocolSession succ ack=", (uint08 *)ack, 9);
		    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
            eap_session_id = (uint16)((uint16)(pData[24]<<8) | (uint16)pData[25]);  
        }
        else if(command == IAP2_StopExternalAccessoryProtocolSession)
        {
            tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StopExternalAccessoryProtocolSession", pData, dataLen);
            ack[5] = pData[6];
            ack[6] = pData[5];
		    ack[8] = tlkmdi_btiap_calcChecksum(ack,8);
		    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_StopExternalAccessoryProtocolSession succ ack=", (uint08 *)ack, 9);
		    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
        }
	} 
    else
    {   
        if((pData[4] == IAP2_PACKET_CONTROL_ACK) && (pData[9] == (eap_session_id>>8)) && (pData[10] == (uint08)eap_session_id))
        {
            if(dataLen >= 256){
                tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "recv data", pData, 32);
            }else{
                tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "recv data", pData, dataLen);
            }
            ack[5] = pData[6];
            ack[6] = pData[5];
		    ack[8] = tlkmdi_btiap_calcChecksum(ack,8);
		    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP succ ack=", (uint08 *)ack, 9);
		    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)ack,sizeof(ack));
			#if IAP2_HARDCODED
		    tlkmdi_btiap_hardCodedRecvDeal(pData+11, dataLen, buffer, 0, nullptr);
			#endif
            /*
            uint08 temp_seq = data[6];
            uint08 temp_ack = data[5];
            data[5] = temp_seq+1;
            data[6] = temp_ack;
            data[8] = tlkmdi_btiap_calcChecksum(data,8);
		    tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP Loopback=", (uint08 *)data, len);
		    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)data,len);
		    */
        }
    }
}

static uint08 tlkmdi_btiap_calcChecksum(uint08 *pData, uint32 dataLen)
{
    uint08 checksum = 0;
    if((dataLen > 0)){
        const uint08* bufferEnd = pData + dataLen;
        while (pData != bufferEnd){
            checksum += *pData; /* add to checksum */
            pData++;
        }
        checksum = (uint08)(0x100 - checksum); /* 2's complement negative checksum */
    }
    return checksum;
}
#endif //#if IAP2_STACK_EN






#if IAP2_HARDCODED

uint08 scTlkMdiBtIapEapAck1[] = {0xFF, 0x5A, 0x00, 0x4C, 0x40, 0x80, 0x98, 0x0B, 0xF7, 0x00, 0x05, 0x00, 0x3E, 0xE5, 0x86, 0x00, 0x00, 0x00, 0x00, \
	0xF9, 0x00, 0xBE, 0x82, 0x02, 0x00, 0x00, 0x00, 0xF0, 0xDE, 0xBE, 0x82, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x70, 0x0F, 0x83, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x6F, 0x0F, 0x83, 0x02, 0x00, \
	0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D}; 
uint08 scTlkMdiBtIapEapAck2[] = {0xFF, 0x5A, 0x00, 0x1E, 0x40, 0x81, 0x99, 0x0B, 0x23, 0x00, 0x05, 0x00, 0x10, 0x84, 0xC7, 0x5A, 0x67, 0x17, 0xA4, \
	0xFB, 0xB9, 0x96, 0xDD, 0x2A, 0xD2, 0x8C, 0xFD, 0x91, 0x64, 0x83}; 
uint08 scTlkMdiBtIapEapAck3[] = {0xFF, 0x5A, 0x00, 0x16, 0x40, 0x82, 0x9A, 0x0B, 0x29, 0x00, 0x05, 0x00, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0x4F, 0x00, 0x0C, 0x8C};
uint08 scTlkMdiBtIapEapAck4[] = {0xFF, 0x5A, 0x00, 0x13, 0x40, 0x83, 0x9B, 0x0B, 0x2A, 0x00, 0x05, 0x00, 0x05, 0x77, 0x00, 0x00, 0x02, 0xEA, 0x93};
uint08 scTlkMdiBtIapEapAck5[] = {0xFF, 0x5A, 0x00, 0x13, 0x40, 0x84, 0x9C, 0x0B, 0x28, 0x00, 0x05, 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0xF3};
uint08 scTlkMdiBtIapEapAck6[] = {0xFF, 0x5A, 0x00, 0x19, 0x40, 0x85, 0x9D, 0x0B, 0x20, 0x00, 0x05, 0x00, 0x0B, 0xA0, 0x00, 0x51, 0x00, 0x44, 0x44, 0x51, 0x00, 0x44, 0x44, 0x51, 0x4D};
uint08 scTlkMdiBtIapEapAck7[] = {0xFF, 0x5A, 0x00, 0x17, 0x40, 0x89, 0xA1, 0x0B, 0x1A, 0x00, 0x05, 0x00, 0x09, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x39, 0xAF, 0xAF, 0xC9, 0xD9};
uint08 scTlkMdiBtIapEapAck8[] = {0xFF, 0x5A, 0x00, 0x11, 0x40, 0x8C, 0xA4, 0x0B, 0x1A, 0x00, 0x05, 0x00, 0x03, 0x0A, 0x01, 0x00, 0xED};
uint08 scTlkMdiBtIapEapAck9[] = {0xFF, 0x5A, 0x00, 0x13, 0x40, 0x8D, 0xA5, 0x0B, 0x16, 0x00, 0x05, 0x00, 0x05, 0xA4, 0xDE, 0x00, 0x00, 0x00, 0x74};
uint08 scTlkMdiBtIapEapAck10[] = {0xFF, 0x5A, 0x00, 0x17, 0x40, 0x8E, 0xA6, 0x0B, 0x10, 0x00, 0x05, 0x00, 0x09, 0x60, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};
uint08 scTlkMdiBtIapEapAck11[] = {0xFF, 0x5A, 0x00, 0x3E, 0x40, 0x8F, 0xA7, 0x0B, 0xE7, 0x00, 0x05, 0x00, 0x30, 0x87, 0xA6, 0xE6, 0xC3, 0xD1, 0x41, \
	0x9C, 0xDA, 0x78, 0x97, 0xFA, 0x10, 0x1C, 0x91, 0x73, 0xE5, 0xDB, 0xDF, 0x10, 0xC2, 0x88, 0x98, 0xD7, 0x2C, 0xB7, 0x0F, 0x7D, 0x32, 0x0B,  \
	0x24, 0xFF, 0xCA, 0x40, 0xDD, 0x20, 0x74, 0x41, 0x9E, 0x61, 0x17, 0x3C, 0x88, 0xCE, 0x09, 0x20, 0xE1, 0x94, 0xD9, 0x22};
uint08 scTlkMdiBtIapEapAck12[] = {0xFF, 0x5A, 0x00, 0x1E, 0x40, 0x91, 0xA9, 0x0B, 0x03, 0x00, 0x05, 0x80, 0x10, 0x89, 0x73, 0x85, 0xAF, 0xA3, 0x0D, 0xEA, 0x38, 0xE3, 0xA6, 0xE3, 0x94, 0x39, 0xEC, 0xEC, 0x6E, 0xEA};
uint08 scTlkMdiBtIapEapAck13[] = {0xFF, 0x5A, 0x00, 0x1E, 0x40, 0x90, 0xA8, 0x0B, 0x05, 0x00, 0x05, 0x80, 0x10, 0x82, 0x99, 0xCE, 0xCD, 0x8F, 0xE4, 0xBF, 0x9B, 0x5E, 0xFB, 0x6A, 0x14, 0x49, 0xDB, 0x35, 0x30, 0x88};
uint08 scTlkMdiBtIapEapAck14[] = {0xFF, 0x5A, 0x00, 0x1E, 0x40, 0x95, 0xAD, 0x0B, 0xFB, 0x00, 0x05, 0x80, 0x10, 0x54, 0x65, 0x5E, 0x0B, 0xA6, 0x68, 0x6F, 0x33, 0x0C, 0x9E, 0xD5, 0x73, 0x71, 0x6D, 0x07, 0x47, 0x7B};

static void tlkmdi_btiap_hardCodecEapAck(uint08 *pSendData, uint08 *pRecvData, uint16 sendLen)
{
    pSendData[5] = pRecvData[6]+1;
    pSendData[6] = pRecvData[5];
    pSendData[8] = tlkmdi_btiap_calcChecksum(pSendData,8);
    pSendData[9] = pRecvData[9];
    pSendData[10] = pRecvData[10];
    pSendData[sendLen-1] = tlkmdi_btiap_calcChecksum(pSendData, sendLen-1);
    //tlkapi_array(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck1=", (uint08 *)send_data, sizeof(send_data));
    btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)pSendData, sendLen);  
}  
static void tlkmdi_btiap_hardCodecEap2Ack(struct iAP2Link_st* link, uint08 *pSendData, uint08 *pRecvData, uint16 sendLen)
{
    pSendData[5] = link->sentSeq+1;
    pSendData[6] = link->recvSeq;
    pSendData[8] = iAP2PacketCalcChecksum(pSendData,8);
    pSendData[9] = pRecvData[0];
    pSendData[10] = pRecvData[1];
    pSendData[sendLen-1] = iAP2PacketCalcChecksum(pSendData, sendLen-1);
	btp_iap_sendData(sTlkMdiBtIapCtrl.aclHandle, nullptr, 0, (uint08 *)pSendData, sendLen);
}
static void tlkmdi_btiap_hardCodedRecvDeal(uint08 *pData, uint16 dataLen, uint08 *pSendData, uint08 srcFlag, struct iAP2Link_st* link)
{
	uint08 flag = 0;
	uint16 sendLen = 0;
	
    if(pData[0] == 0x40 && pData[1] == 0x40){
        sendLen = sizeof(scTlkMdiBtIapEapAck1);
        tmemcpy(pSendData,scTlkMdiBtIapEapAck1, sendLen);
        tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck1");
    }
    else if(pData[0] == 0x00 && pData[1] == 0x04){
        if(pData[2] == 0xCF){
             sendLen = sizeof(scTlkMdiBtIapEapAck2);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck2, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck2");
        }else if(pData[2] == 0x12){
             sendLen = sizeof(scTlkMdiBtIapEapAck3);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck3, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck3");
        }else if(pData[2] == 0x60){
             sendLen = sizeof(scTlkMdiBtIapEapAck10);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck10, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck10");
        }
    }
    else if(pData[0] == 0x00 && pData[1] == 0x01){
        if(pData[2] == 0x77){
             sendLen = sizeof(scTlkMdiBtIapEapAck4);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck4, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck4");
        }else if(pData[2] == 0x03){
             sendLen = sizeof(scTlkMdiBtIapEapAck5);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck5, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck5");
        }else if(pData[2] == 0xB9){
             sendLen = sizeof(scTlkMdiBtIapEapAck7);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck7, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck7");
        }else if(pData[2] == 0x0A){
             sendLen = sizeof(scTlkMdiBtIapEapAck8);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck8, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck8");
        }else if(pData[2] == 0xA4){
             sendLen = sizeof(scTlkMdiBtIapEapAck9);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck9, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck9");
        }
    }
    else if(pData[0] == 0x0 && pData[1] == 0x03){
        if(pData[2] == 0xA0){
             sendLen = sizeof(scTlkMdiBtIapEapAck6);
             tmemcpy(pSendData,scTlkMdiBtIapEapAck6, sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck6");
        }
    }
    else if(pData[0] == 0x00 && pData[1] == 0x31)    {
        sendLen = sizeof(scTlkMdiBtIapEapAck11);
        tmemcpy(pSendData,scTlkMdiBtIapEapAck11, sendLen);
        tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP scTlkMdiBtIapEapAck11");
    }
    else if(pData[0] == 0x80 && pData[1] == 0x10){
        if(pData[2] == 0xe3){
             sendLen = sizeof(scTlkMdiBtIapEapAck12);
             tmemcpy(pSendData,(scTlkMdiBtIapEapAck12), sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP (scTlkMdiBtIapEapAck12)");
        }
        else if(pData[2] == 0x8e){
             sendLen = sizeof(scTlkMdiBtIapEapAck13);
             tmemcpy(pSendData,(scTlkMdiBtIapEapAck13), sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP (scTlkMdiBtIapEapAck13)");
        }
        else if(pData[2] == 0xE4){
             sendLen = sizeof(scTlkMdiBtIapEapAck14);
             tmemcpy(pSendData,(scTlkMdiBtIapEapAck14), sendLen);
             tlkapi_trace(TLKMDI_BTIAP_DBG_FLAG, TLKMDI_BTIAP_DBG_SIGN, "IAP2_EAP (scTlkMdiBtIapEapAck14)");
        }   
    }
    else 
    {
        flag = 1;
    }
    if(flag == 0){
		if(srcFlag == 0) tlkmdi_btiap_hardCodecEapAck(pSendData, pData, sendLen);
		else tlkmdi_btiap_hardCodecEap2Ack(link, pSendData, pData, sendLen);
	}
        
}


#endif //#if IAP2_HARDCODED


#endif //#if (TLK_MDI_BTIAP_ENABLE)

