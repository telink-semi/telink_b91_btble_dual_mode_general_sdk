/********************************************************************************************************
 * @file     tlkdbg_hpudwn.c
 *
 * @brief    This is the source file for BTBLE SDK
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
#include "tlk_config.h"
#if (TLK_CFG_DBG_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_HPU_DWN_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_hpudwn.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "drivers.h"
#include "tlkdev/sys/tlkdev_serial.h"


//HPU - Hardware Protocol UART

extern int  tlkdev_serial_send1(uint08 *pData, uint16 dataLen);
extern void tlkdev_serial_setRecvFifo(tlkapi_qfifo_t *pFifo);;

static void tlkdbg_hpudwn_input(uint08 *pData, uint16 dataLen);
static void tlkdbg_hpudwn_makeRecvFrame(uint08 rbyte);
static uint16 tlkdbg_hpudwn_crc16_calc(uint08 *pData, uint16 dataLen);
static void tlkdbg_hpudwn_crc16_InvertUint08(uint08 *pDst, uint08 *pSrc);
static void tlkdbg_hpudwn_crc16_InvertUint16(uint16 *pDst, uint16 *pSrc);
static int  tlkdbg_hpudwn_makeSendFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);

static void tlkdbg_hpudbg_recvCmdDeal(uint08 *pData, uint16 dataLen);
//static void tlkdbg_hpudbg_recvDatDeal(uint16 numb, uint08 *pData, uint16 dataLen);

static void tlkdbg_hpudwn_recvStartCmdDeal(uint08 *pData, uint16 dataLen);
static void tlkdbg_hpudwn_recvFlashCmdDeal(uint08 *pData, uint16 dataLen);
static void tlkdbg_hpudwn_recvCloseCmdDeal(uint08 *pData, uint16 dataLen);

static void tlkdbg_hpudwn_sendStartRspDeal(void);
static void tlkdbg_hpudwn_sendFlashRspDeal(void);
static void tlkdbg_hpudwn_sendFSyncRspDeal(void);
static void tlkdbg_hpudwn_sendCloseRspDeal(void);

#if (TLKDBG_HPUDWN_FAST_SPEED_ENABLE)
#define TLKDBG_HPUDWN_RECV_ITEM_SIZE       2072//1100
#define TLKDBG_HPUDWN_RECV_ITEM_NUMB       2
#define TLKDBG_HPUDWN_RECV_BUFF_SIZE       2064//1040
#define TLKDBG_HPUDWN_RECV_DATA_SIZE       2048//1024
#else
#define TLKDBG_HPUDWN_RECV_BUFF_SIZE       TLKPRT_COMM_FRM_MAXLEN
#define TLKDBG_HPUDWN_RECV_DATA_SIZE       128
#endif
#define TLKDBG_HPUDWN_SEND_BUFF_SIZE       64


typedef struct{
	uint08 isStart;
	uint08 isEarse;
	uint08 sendNumb;
	uint08 reserve0;
	uint32 recvNumb;
	uint32 writeOffs;
	uint32 writeLens;
	uint32 startAddr;
	uint08 isEscape;
	uint08 makeState;
	uint16 makeLength;
	uint32 startTimer;
	uint16 recvFrameLen;
	uint16 sendFrameLen;
	uint08 recvFrame[TLKDBG_HPUDWN_RECV_BUFF_SIZE];
	uint08 sendFrame[TLKDBG_HPUDWN_SEND_BUFF_SIZE];
}tlkdbg_hpudwn_ctrl_t;

#define TLKMMI_SYS_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_SYSTEM << 24) | (TLK_MINOR_DBGID_MMI_SYSTEM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_SYS_DBG_SIGN         "[DBG]"

#if (TLKDBG_HPUDWN_FAST_SPEED_ENABLE)

__attribute__((aligned(4)))
static uint08 sTlkDbgHpuDwnRecvBuff[TLKDBG_HPUDWN_RECV_ITEM_NUMB*TLKDBG_HPUDWN_RECV_ITEM_SIZE];
static tlkapi_qfifo_t sTlkDbgHpuDwnFifo;
#endif
static tlkdbg_hpudwn_ctrl_t sTlkDbgHpuDwnCtrl;

void tlkdbg_hpudwn_init(void)
{
	tmemset(&sTlkDbgHpuDwnCtrl, 0, sizeof(tlkdbg_hpudwn_ctrl_t));
	#if (TLKDBG_HPUDWN_FAST_SPEED_ENABLE)
	tlkapi_qfifo_init(&sTlkDbgHpuDwnFifo, TLKDBG_HPUDWN_RECV_ITEM_NUMB, TLKDBG_HPUDWN_RECV_ITEM_SIZE, 
		sTlkDbgHpuDwnRecvBuff, TLKDBG_HPUDWN_RECV_ITEM_NUMB*TLKDBG_HPUDWN_RECV_ITEM_SIZE);
	#endif
}



void tlkdbg_hpudbg_recvCmd(uint08 *pData, uint16 dataLen)
{
	uint08 msgID;

	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudbg_recvCmd: %d %x", dataLen, pData[0]);
	if(dataLen == 0) return;
	
	msgID = pData[0];
	if(msgID == TLKDBG_HPUDWN_MSGID_START){
		tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudbg_recvCmd 01");
		tlkdbg_hpudwn_recvStartCmdDeal(pData+1, dataLen-1);
	}else if(msgID == TLKDBG_HPUDWN_MSGID_FLASH){
		tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudbg_recvCmd 02");
		tlkdbg_hpudwn_recvFlashCmdDeal(pData+1, dataLen-1);
	}else if(msgID == TLKDBG_HPUDWN_MSGID_CLOSE){
		tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudbg_recvCmd 03");
		tlkdbg_hpudwn_recvCloseCmdDeal(pData+1, dataLen-1);
	}
	if(sTlkDbgHpuDwnCtrl.isStart){
		while(1){
			tlkdbg_hpudwn_handler();
		}
	}
}

extern void tlkdev_serial_handler1(void);

_attribute_ram_code_sec_noinline_
void tlkdbg_hpudwn_handler(void)
{
	if(!sTlkDbgHpuDwnCtrl.isStart) return;
	if(sTlkDbgHpuDwnCtrl.isStart){
		tlkdev_serial_handler1();
	}
	if(sTlkDbgHpuDwnCtrl.isStart != 0 && clock_time_exceed(sTlkDbgHpuDwnCtrl.startTimer, 3000000)){
		
	}
}


_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_sendStartRspDeal(void)
{
	int ret;
	uint08 head[4];
	uint08 body[8];
	uint08 headLen = 0;
	uint08 bodyLen = 0;
		
	body[bodyLen++] = TLKPRT_COMM_RSP_STATUE_SUCCESS;
	body[bodyLen++] = TLKPRT_COMM_RSP_REASON_NONE;
	body[bodyLen++] = TLKDBG_HPUDWN_MSGID_START; //Msg ID
	body[bodyLen++] = (TLKDBG_HPUDWN_RECV_DATA_SIZE & 0xFF); //Maximum length of a single packet
	body[bodyLen++] = (TLKDBG_HPUDWN_RECV_DATA_SIZE & 0xFF00) >> 8;

	head[headLen++] = TLKPRT_COMM_MTYPE_DBG; //Mtype
	head[headLen++] = TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE; //MsgID
	head[headLen++] = sTlkDbgHpuDwnCtrl.sendNumb++; //Numb
	head[headLen++] = bodyLen; //Lens
	
	ret = tlkdbg_hpudwn_makeSendFrame(TLKPRT_COMM_PTYPE_RSP, head, headLen, body, bodyLen);
	if(ret == TLK_ENONE){
		tlkdev_serial_send1(sTlkDbgHpuDwnCtrl.sendFrame, sTlkDbgHpuDwnCtrl.sendFrameLen);
	}
}
_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_sendFlashRspDeal(void)
{
	int ret;
	uint08 head[4];
	uint08 body[8];
	uint08 headLen = 0;
	uint08 bodyLen = 0;
	
	body[bodyLen++] = TLKPRT_COMM_RSP_STATUE_SUCCESS;
	body[bodyLen++] = TLKPRT_COMM_RSP_REASON_NONE;
	body[bodyLen++] = TLKDBG_HPUDWN_MSGID_FLASH; //Msg ID

	head[headLen++] = TLKPRT_COMM_MTYPE_DBG; //Mtype
	head[headLen++] = TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE; //MsgID
	head[headLen++] = sTlkDbgHpuDwnCtrl.sendNumb++; //Numb
	head[headLen++] = bodyLen; //Lens	
	ret = tlkdbg_hpudwn_makeSendFrame(TLKPRT_COMM_PTYPE_RSP, head, headLen, body, bodyLen);
	if(ret == TLK_ENONE){
		tlkdev_serial_send1(sTlkDbgHpuDwnCtrl.sendFrame, sTlkDbgHpuDwnCtrl.sendFrameLen);
	}
}
_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_sendFSyncRspDeal(void)
{
	int ret;
	uint08 head[4];
	uint08 body[8];
	uint08 headLen = 0;
	uint08 bodyLen = 0;
	
	body[bodyLen++] = TLKPRT_COMM_RSP_STATUE_SUCCESS;
	body[bodyLen++] = TLKPRT_COMM_RSP_REASON_NONE;
	body[bodyLen++] = TLKDBG_HPUDWN_MSGID_FSYNC; //Msg ID
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.recvNumb & 0xFF); //Recv Number
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.recvNumb & 0xFF00) >> 8;
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.recvNumb & 0xFF0000) >> 16;
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.recvNumb & 0xFF000000) >> 24;
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.writeOffs & 0xFF); //Flash Offset
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.writeOffs & 0xFF00) >> 8;
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.writeOffs & 0xFF0000) >> 16;
	body[bodyLen++] = (sTlkDbgHpuDwnCtrl.writeOffs & 0xFF000000) >> 24;
	
	head[headLen++] = TLKPRT_COMM_MTYPE_DBG; //Mtype
	head[headLen++] = TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE; //MsgID
	head[headLen++] = sTlkDbgHpuDwnCtrl.sendNumb++; //Numb
	head[headLen++] = bodyLen; //Lens
	
	ret = tlkdbg_hpudwn_makeSendFrame(TLKPRT_COMM_PTYPE_RSP, head, headLen, body, bodyLen);
	if(ret == TLK_ENONE){
		tlkdev_serial_send1(sTlkDbgHpuDwnCtrl.sendFrame, sTlkDbgHpuDwnCtrl.sendFrameLen);
	}
}
_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_sendCloseRspDeal(void)
{
	int ret;
	uint08 head[4];
	uint08 body[8];
	uint08 headLen = 0;
	uint08 bodyLen = 0;
	
	body[bodyLen++] = TLKPRT_COMM_RSP_STATUE_SUCCESS;
	body[bodyLen++] = TLKPRT_COMM_RSP_REASON_NONE;
	body[bodyLen++] = TLKDBG_HPUDWN_MSGID_CLOSE; //Msg ID
	
	head[headLen++] = TLKPRT_COMM_MTYPE_DBG; //Mtype
	head[headLen++] = TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE; //MsgID
	head[headLen++] = sTlkDbgHpuDwnCtrl.sendNumb++; //Numb
	head[headLen++] = bodyLen; //Lens
	
	ret = tlkdbg_hpudwn_makeSendFrame(TLKPRT_COMM_PTYPE_RSP, head, headLen, body, bodyLen);
	if(ret == TLK_ENONE){
		tlkdev_serial_send1(sTlkDbgHpuDwnCtrl.sendFrame, sTlkDbgHpuDwnCtrl.sendFrameLen);
	}
}


_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudbg_recvCmdDeal(uint08 *pData, uint16 dataLen)
{
	uint08 msgID;
	
	if(dataLen == 0) return;

	msgID = pData[0];
	if(msgID == TLKDBG_HPUDWN_MSGID_START){
		tlkdbg_hpudwn_recvStartCmdDeal(pData+1, dataLen-1);
	}else if(msgID == TLKDBG_HPUDWN_MSGID_FLASH){
		tlkdbg_hpudwn_recvFlashCmdDeal(pData+1, dataLen-1);
	}else if(msgID == TLKDBG_HPUDWN_MSGID_CLOSE){
		tlkdbg_hpudwn_recvCloseCmdDeal(pData+1, dataLen-1);
	}else if(msgID == TLKDBG_HPUDWN_MSGID_REBOOT){
		core_reboot();
	}
}
_attribute_ram_code_sec_noinline_
//static 
void tlkdbg_hpudbg_recvDatDeal(uint16 numb, uint08 *pData, uint16 dataLen)
{
	if(sTlkDbgHpuDwnCtrl.isStart != 2) return;

	if(pData[0] != TLKPRT_COMM_SYS_DAT_FW || dataLen == 0){
		return;
	}else{
		pData += 1;
		dataLen -= 1;
	}
	if(((sTlkDbgHpuDwnCtrl.recvNumb+1) & 0xFFFF) != numb){
		tlkdbg_hpudwn_sendFSyncRspDeal();
		return;
	}
	if(sTlkDbgHpuDwnCtrl.writeOffs > sTlkDbgHpuDwnCtrl.writeLens){
		tlkdbg_hpudwn_sendFSyncRspDeal();
		return;
	}
	
	sTlkDbgHpuDwnCtrl.startTimer = clock_time();
	
	if((sTlkDbgHpuDwnCtrl.writeOffs & 0xFFF) == 0){
		flash_erase_sector(sTlkDbgHpuDwnCtrl.writeOffs);
	}
	flash_write_page(sTlkDbgHpuDwnCtrl.startAddr+sTlkDbgHpuDwnCtrl.writeOffs, dataLen, pData);
	sTlkDbgHpuDwnCtrl.recvNumb = numb;
	sTlkDbgHpuDwnCtrl.writeOffs += dataLen;
	tlkdbg_hpudwn_sendFSyncRspDeal();
}

extern void tlkdev_serial_clear1(void);
extern void tlkdev_serial_regCB1(tlkdev_serial_recvCB cb);

_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_recvStartCmdDeal(uint08 *pData, uint16 dataLen)
{
	if(sTlkDbgHpuDwnCtrl.isStart == 0){
		plic_interrupt_disable(IRQ1_SYSTIMER);
		plic_interrupt_disable(IRQ3_TIMER1);
		plic_interrupt_disable(IRQ4_TIMER0);
		plic_interrupt_disable(IRQ5_DMA);
		plic_interrupt_disable(IRQ12_ZB_DM);
		plic_interrupt_disable(IRQ13_ZB_BLE);
		plic_interrupt_disable(IRQ14_ZB_BT);
		plic_interrupt_disable(IRQ15_ZB_RT);
		plic_interrupt_disable(IRQ28_SOFT);
		tlkdev_serial_clear1();
		tlkdev_serial_regCB1(tlkdbg_hpudwn_input);
		tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_96M);
		#if (TLKDBG_HPUDWN_FAST_SPEED_ENABLE)
		tlkdev_serial_setRecvFifo(&sTlkDbgHpuDwnFifo);
		#endif
	}
	
	sTlkDbgHpuDwnCtrl.isStart = 1;
	sTlkDbgHpuDwnCtrl.isEarse = 0;
	sTlkDbgHpuDwnCtrl.sendNumb = 0;
	sTlkDbgHpuDwnCtrl.recvNumb = 0;
	sTlkDbgHpuDwnCtrl.startTimer = clock_time();
	tlkdbg_hpudwn_sendStartRspDeal();
}
/******************************************************************************
 * Function: tlkdbg_hpudwn_recvFlashCmdDeal
 * Descript: Launch download program.
 * Params:
 *     @fwSize--The size of the firmware, and the system will have more of that
 *         value to determine the size of the erase flash;
 *     @offset--Offset position of the firmware in the flash;
 * Return: None.
 * Others: None.
*******************************************************************************/
_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_recvFlashCmdDeal(uint08 *pData, uint16 dataLen)
{
//	uint32 index;
	uint32 fwSize;
	uint32 offset;

	if(sTlkDbgHpuDwnCtrl.isStart != 1) return;
	if(dataLen < 8) return;

	ARRAY_TO_UINT32L(pData, 0, fwSize);
	ARRAY_TO_UINT32L(pData, 4, offset);

	if((offset & 0x0FFF) != 0) return; //4096
	if(fwSize >= TLK_CFG_FLASH_CAP) return;

//	if(sTlkDbgHpuDwnCtrl.isEarse == 0){
//		sTlkDbgHpuDwnCtrl.isEarse = 1;
//		for(index=0; index<fwSize+4095; index+=4096){
//			flash_erase_sector(offset+index);
//		}
//	}
		
	sTlkDbgHpuDwnCtrl.isStart = 2;
	sTlkDbgHpuDwnCtrl.startAddr = 0;//offset;
	sTlkDbgHpuDwnCtrl.writeLens = fwSize;
	sTlkDbgHpuDwnCtrl.writeOffs = 0;
	tlkdbg_hpudwn_sendFlashRspDeal();
}

_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_recvCloseCmdDeal(uint08 *pData, uint16 dataLen)
{
	tlkdbg_hpudwn_sendCloseRspDeal();
}


_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_input(uint08 *pData, uint16 dataLen)
{
	uint08 mtype;
	uint08 ptype;
	uint16 index;
	uint16 rawCrc;
	uint16 calCrc;

//	tlkapi_array(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "=== recv", pData, dataLen);
	
	for(index=0; index<dataLen; index++){
		tlkdbg_hpudwn_makeRecvFrame(pData[index]);
		
		if(sTlkDbgHpuDwnCtrl.makeState != TLKDBG_HPUDWN_MSTATE_READY) continue;
		rawCrc = ((uint16)(sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen-3])<<8)
			| (sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen-4]);
		calCrc = tlkdbg_hpudwn_crc16_calc(sTlkDbgHpuDwnCtrl.recvFrame+2, sTlkDbgHpuDwnCtrl.recvFrameLen-6);
		if(rawCrc != calCrc){
			//tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Serial CRC Error: rawCrc[%x] - calCrc[%x]", rawCrc, calCrc);
			continue;
		}
		ptype = (sTlkDbgHpuDwnCtrl.recvFrame[3] & 0xF0) >> 4;
		
		if(ptype == TLKPRT_COMM_PTYPE_CMD){
			uint08 lens;
			uint08 msgID;
			mtype = sTlkDbgHpuDwnCtrl.recvFrame[4];
			msgID = sTlkDbgHpuDwnCtrl.recvFrame[5];
			//numb  = sTlkDbgHpuDwnCtrl.recvFrame[6];
			lens  = sTlkDbgHpuDwnCtrl.recvFrame[7];
			if(sTlkDbgHpuDwnCtrl.recvFrameLen < 4+TLKPRT_COMM_FRM_EXTLEN || sTlkDbgHpuDwnCtrl.recvFrameLen < 4+lens+TLKPRT_COMM_FRM_EXTLEN){
				//tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv CmdOrRspOrEvtPkt Length Error: frmLen-%d lens-%d", sTlkDbgHpuDwnCtrl.recvFrameLen, lens);
				return;
			}
			if(mtype >= TLKPRT_COMM_MTYPE_MAX){
				//tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv Error MsgType: mtype-%d", mtype);
				return;
			}
			if(mtype == TLKPRT_COMM_MTYPE_DBG && msgID == TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE && lens != 0){
				tlkdbg_hpudbg_recvCmdDeal(sTlkDbgHpuDwnCtrl.recvFrame+8, lens);
			}
		}
		else if(ptype == TLKPRT_COMM_PTYPE_DAT){
			uint16 numb;
			uint16 lens;
			uint08 datID;
			datID = sTlkDbgHpuDwnCtrl.recvFrame[4];
			numb  = (((uint16)sTlkDbgHpuDwnCtrl.recvFrame[6]) << 8) | sTlkDbgHpuDwnCtrl.recvFrame[5];
			lens  = (((uint16)sTlkDbgHpuDwnCtrl.recvFrame[8]) << 8) | sTlkDbgHpuDwnCtrl.recvFrame[7];
			if(sTlkDbgHpuDwnCtrl.recvFrameLen < 4+TLKPRT_COMM_FRM_EXTLEN || sTlkDbgHpuDwnCtrl.recvFrameLen < 4+lens+TLKPRT_COMM_FRM_EXTLEN){
				//tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv DatPkt Length Error: frmLen-%d lens-%d", sTlkDbgHpuDwnCtrl.recvFrameLen, lens);
				return;
			}
			if(datID == TLKPRT_COMM_SYS_DAT_PORT){
				tlkdbg_hpudbg_recvDatDeal(numb, sTlkDbgHpuDwnCtrl.recvFrame+9, lens);
			}
		}
		else{
			//tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv Error PktType: ptype-%d", ptype);
		}
		sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_HEAD;
		sTlkDbgHpuDwnCtrl.makeLength = 0;
	}	
}

#define TLKDBG_HPUDWN_RECV_ESCAPE_ENABLE      0
_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_makeRecvFrame(uint08 rbyte)
{
	#if (TLKDBG_HPUDWN_RECV_ESCAPE_ENABLE)
	if(rbyte == TLKPRT_COMM_HEAD_SIGN && sTlkDbgHpuDwnCtrl.makeState != TLKDBG_HPUDWN_MSTATE_HEAD){
		sTlkDbgHpuDwnCtrl.isEscape = false;
		sTlkDbgHpuDwnCtrl.makeLength = 0;
		sTlkDbgHpuDwnCtrl.makeState  = TLKDBG_HPUDWN_MSTATE_HEAD;
	}
	if(rbyte == TLKPRT_COMM_TAIL_SIGN && sTlkDbgHpuDwnCtrl.makeState != TLKDBG_HPUDWN_MSTATE_TAIL){
		sTlkDbgHpuDwnCtrl.isEscape = false;
		sTlkDbgHpuDwnCtrl.makeLength = 0;
		sTlkDbgHpuDwnCtrl.makeState  = TLKDBG_HPUDWN_MSTATE_HEAD;
		return;
	}
	if(rbyte == TLKPRT_COMM_ESCAPE_CHARS){
		if(sTlkDbgHpuDwnCtrl.makeState  == TLKDBG_HPUDWN_MSTATE_HEAD) return;
		if(sTlkDbgHpuDwnCtrl.isEscape){
			sTlkDbgHpuDwnCtrl.isEscape = false;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
			sTlkDbgHpuDwnCtrl.makeState  = TLKDBG_HPUDWN_MSTATE_HEAD;
		}else{
			sTlkDbgHpuDwnCtrl.isEscape = true;
		}
		return;
	}
	if(sTlkDbgHpuDwnCtrl.isEscape){
		sTlkDbgHpuDwnCtrl.isEscape = false;
		if(rbyte == TLKPRT_COMM_ESCAPE_CHARS_RAW){
			rbyte = TLKPRT_COMM_ESCAPE_CHARS;
		}else if(rbyte == TLKPRT_COMM_ESCAPE_CHARS_HEAD){
			rbyte = TLKPRT_COMM_HEAD_SIGN;
		}else if(rbyte == TLKPRT_COMM_ESCAPE_CHARS_TAIL){
			rbyte = TLKPRT_COMM_TAIL_SIGN;
		}else{
			sTlkDbgHpuDwnCtrl.makeLength = 0;
			sTlkDbgHpuDwnCtrl.makeState  = TLKDBG_HPUDWN_MSTATE_HEAD;
			return;
		}
	}
	#endif
	
	if(sTlkDbgHpuDwnCtrl.makeState == TLKDBG_HPUDWN_MSTATE_HEAD){
		#if (TLKDBG_HPUDWN_RECV_ESCAPE_ENABLE)
		if(rbyte == TLKPRT_COMM_HEAD_SIGN0)
		{
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_ATTR;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
			sTlkDbgHpuDwnCtrl.recvFrameLen = 0;
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_HEAD_SIGN0;
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_HEAD_SIGN1;
		}
		#else
		if(sTlkDbgHpuDwnCtrl.makeLength == 0 && rbyte == TLKPRT_COMM_HEAD_SIGN0){
			sTlkDbgHpuDwnCtrl.makeLength ++;
		}else if(sTlkDbgHpuDwnCtrl.makeLength == 1 && rbyte == TLKPRT_COMM_HEAD_SIGN1){
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_ATTR;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
			sTlkDbgHpuDwnCtrl.recvFrameLen = 0;
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_HEAD_SIGN0;
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_HEAD_SIGN1;
		}else{
			sTlkDbgHpuDwnCtrl.makeLength = 0;
		}
		#endif
	}
	else if(sTlkDbgHpuDwnCtrl.makeState == TLKDBG_HPUDWN_MSTATE_ATTR){
		sTlkDbgHpuDwnCtrl.makeLength ++;
		sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = rbyte;
		if(sTlkDbgHpuDwnCtrl.makeLength < 2) return;
		
		sTlkDbgHpuDwnCtrl.makeLength = (((uint16)(sTlkDbgHpuDwnCtrl.recvFrame[3] & 0x0F))<<8)|sTlkDbgHpuDwnCtrl.recvFrame[2];
//		tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudwn_makeRecvFrame 00: %d", sTlkDbgHpuDwnCtrl.makeLength);
		if(sTlkDbgHpuDwnCtrl.makeLength+4 > TLKDBG_HPUDWN_RECV_BUFF_SIZE || sTlkDbgHpuDwnCtrl.makeLength < 4){
//			tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudwn_makeRecvFrame 01");
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_HEAD;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
		}else if(sTlkDbgHpuDwnCtrl.makeLength == 4){
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_CHECK;
			sTlkDbgHpuDwnCtrl.makeLength = 2;
		}else{
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_BODY;
			sTlkDbgHpuDwnCtrl.makeLength -= 4;
		}
	}else if(sTlkDbgHpuDwnCtrl.makeState == TLKDBG_HPUDWN_MSTATE_BODY){
		sTlkDbgHpuDwnCtrl.makeLength --;
		sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = rbyte;
		if(sTlkDbgHpuDwnCtrl.makeLength != 0) return;
		sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_CHECK;
		sTlkDbgHpuDwnCtrl.makeLength = 0;
	}else if(sTlkDbgHpuDwnCtrl.makeState == TLKDBG_HPUDWN_MSTATE_CHECK){
		sTlkDbgHpuDwnCtrl.makeLength ++;
		sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = rbyte;
		if(sTlkDbgHpuDwnCtrl.makeLength < 2) return;
		sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_TAIL;
		sTlkDbgHpuDwnCtrl.makeLength = 0;
	}else if(sTlkDbgHpuDwnCtrl.makeState == TLKDBG_HPUDWN_MSTATE_TAIL){
		#if (TLKDBG_HPUDWN_RECV_ESCAPE_ENABLE)
		if(rbyte == TLKPRT_COMM_TAIL_SIGN0){
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_TAIL_SIGN0;
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_TAIL_SIGN1;
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_READY;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
		}else{
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_HEAD;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
		}
		#else
		if(sTlkDbgHpuDwnCtrl.makeLength == 0 && rbyte == TLKPRT_COMM_TAIL_SIGN0){
			sTlkDbgHpuDwnCtrl.makeLength ++;
		}else if(sTlkDbgHpuDwnCtrl.makeLength == 1 && rbyte == TLKPRT_COMM_TAIL_SIGN1){
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_TAIL_SIGN0;
			sTlkDbgHpuDwnCtrl.recvFrame[sTlkDbgHpuDwnCtrl.recvFrameLen++] = TLKPRT_COMM_TAIL_SIGN1;
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_READY;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
		}else{
//			tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkdbg_hpudwn_makeRecvFrame 02");
			sTlkDbgHpuDwnCtrl.makeState = TLKDBG_HPUDWN_MSTATE_HEAD;
			sTlkDbgHpuDwnCtrl.makeLength = 0;
		}
		#endif
	}
}
_attribute_ram_code_sec_noinline_
static int tlkdbg_hpudwn_makeSendFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	uint16 tempVar;
	uint08 *pFrame;
	uint16 frmLen;
	
//	tlkapi_trace(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "send 01:", &sTlkDevSerialSendIsBusy, 1);
	if(headLen == 0 && pHead != nullptr) pHead = nullptr;
	if(bodyLen == 0 && pBody != nullptr) pBody = nullptr;
	if((headLen+bodyLen) == 0) return -TLK_EPARAM;
	if(headLen+bodyLen+TLKPRT_COMM_FRM_EXTLEN > TLKDBG_HPUDWN_SEND_BUFF_SIZE){
		return -TLK_ELENGTH;
	}

	frmLen = 0;
	pFrame = sTlkDbgHpuDwnCtrl.sendFrame;
		
	pFrame[frmLen++] = TLKPRT_COMM_HEAD_SIGN0;
	pFrame[frmLen++] = TLKPRT_COMM_HEAD_SIGN1;
	pFrame[frmLen++] = 0x00; //Length[0~7]
	pFrame[frmLen++] = (pktType & 0x0F) << 4; //Length[8~11]+pktType[0~4]
	if(headLen != 0){
		tmemcpy(pFrame+frmLen, pHead, headLen);
		frmLen += headLen;
	}
	if(bodyLen != 0){
		tmemcpy(pFrame+frmLen, pBody, bodyLen);
		frmLen += bodyLen;
	}
	pFrame[2] |= (frmLen & 0x00FF);
	pFrame[3] |= (frmLen & 0x0F00) >> 8;
	tempVar = tlkdbg_hpudwn_crc16_calc(pFrame+2, frmLen-2);
	pFrame[frmLen++] = (tempVar & 0x00FF);
	pFrame[frmLen++] = (tempVar & 0xFF00) >> 8;
	pFrame[frmLen++] = TLKPRT_COMM_TAIL_SIGN0;
	pFrame[frmLen++] = TLKPRT_COMM_TAIL_SIGN1;
	sTlkDbgHpuDwnCtrl.sendFrameLen = frmLen;
		
	return TLK_ENONE;
}


//IBM - CRC16
_attribute_ram_code_sec_noinline_
static uint16 tlkdbg_hpudwn_crc16_calc(uint08 *pData, uint16 dataLen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;

	while(dataLen--){
		wChar = *(pData++);
		tlkdbg_hpudwn_crc16_InvertUint08(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++){
			if(wCRCin & 0x8000) wCRCin = (wCRCin << 1) ^ wCPoly;
			else wCRCin = wCRCin << 1;
		}
	}
	tlkdbg_hpudwn_crc16_InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}

_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_crc16_InvertUint08(uint08 *pDst, uint08 *pSrc)
{
	int i;
	unsigned char tmp;
	tmp = 0;
	for(i=0; i<8; i++){
		if(pSrc[0]& (1 << i)) tmp |= 1<<(7-i);
	}
	pDst[0] = tmp;
}
_attribute_ram_code_sec_noinline_
static void tlkdbg_hpudwn_crc16_InvertUint16(uint16 *pDst, uint16 *pSrc)
{
	int i;
	unsigned short tmp;
	tmp = 0;
	for(i=0; i<16; i++){
		if(pSrc[0]& (1 << i)) tmp |= 1<<(15 - i);
	}
	pDst[0] = tmp;
}





#endif //#if (TLKDBG_CFG_HPU_DWN_ENABLE)
#endif //#if (TLK_CFG_DBG_ENABLE)

