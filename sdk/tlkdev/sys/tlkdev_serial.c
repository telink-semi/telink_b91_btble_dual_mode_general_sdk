/********************************************************************************************************
 * @file     tlkdev_serial.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"

#include "tlkstk/inner/tlkstk_myudb.h"


typedef enum{
	TLKDEV_SERIAL_MSTATE_HEAD = 0,
	TLKDEV_SERIAL_MSTATE_ATTR,
	TLKDEV_SERIAL_MSTATE_BODY,
	TLKDEV_SERIAL_MSTATE_CHECK,
	TLKDEV_SERIAL_MSTATE_TAIL,
	TLKDEV_SERIAL_MSTATE_READY,
}TLKDEV_SERIAL_MSTATE_ENUM;


#define TLKDEV_SERIAL_PORT               UART1
#define TLKDEV_SERIAL_TX_PIN             UART1_TX_PD6
#define TLKDEV_SERIAL_RX_PIN             UART1_RX_PD7
#define TLKDEV_SERIAL_DMA_ENABLE         1

#define TLKDEV_SERIAL_DMA_TX             DMA4
#define TLKDEV_SERIAL_DMA_RX             DMA5

#if (TLK_DEV_SERIAL_ENABLE)

static void tlkdev_serial_recvHandler(void);
static void tlkapi_serial_makeFrame(uint08 rbyte);

#if !(TLKDEV_SERIAL_DMA_ENABLE)
	#define TLKDEV_SERIAL_RECV_BUFF_NUMB      1 //4
	#define TLKDEV_SERIAL_RECV_BUFF_SIZE      127//(TLKPRT_COMM_FRM_MAXLEN)
	#define TLKDEV_SERIAL_SEND_BUFF_NUMB      1
	#define TLKDEV_SERIAL_SEND_BUFF_SIZE      (TLKPRT_COMM_FRM_MAXLEN)
	tlkapi_fifo_t sTlkApiRecvFifo;
#else
	#define TLKDEV_SERIAL_RECV_BUFF_NUMB      16//4
	#define TLKDEV_SERIAL_RECV_BUFF_SIZE      88//64//(TLKPRT_COMM_FRM_MAXLEN)
	#define TLKDEV_SERIAL_SEND_BUFF_NUMB      8
	#define TLKDEV_SERIAL_SEND_BUFF_SIZE      64
	tlkapi_qfifo_t sTlkApiRecvFifo;
	tlkapi_qfifo_t sTlkApiSendFifo;
#endif


static uint08 sTlkDevSerialMakeState;
static uint08 sTlkDevSerialMakeLength;
static uint16 sTlkDevSerialMakeDataLen;

static uint08 sTlkDevSerialSendIsBusy;
static uint08 sTlkDevSerialRecvIsBusy;
static uint08 sTlkDevSerialFrame[TLKPRT_COMM_FRM_MAXLEN]; //Recv Make Frame
static uint08 sTlkDevSerialCache[TLKPRT_COMM_FRM_MAXLEN]; //Send Make Frame
__attribute__((aligned(4)))
static uint08 sTlkDevSerialRecvBuffer[TLKDEV_SERIAL_RECV_BUFF_NUMB*(TLKDEV_SERIAL_RECV_BUFF_SIZE+4)];
#if (TLKDEV_SERIAL_DMA_ENABLE)
__attribute__((aligned(4)))
static uint08 sTlkDevSerialSendBuffer[TLKDEV_SERIAL_SEND_BUFF_NUMB*(TLKDEV_SERIAL_SEND_BUFF_SIZE+4)];
#endif
static tlkdev_serial_recvCB sTlkDevSerialRecvCB;




_attribute_ram_code_sec_ 
void uart1_irq_handler(void)
{
//	my_dump_str_data(1, "irq:", 0, 0);
	
	#if !(TLKDEV_SERIAL_DMA_ENABLE)
		if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RXBUF_IRQ_STATUS)){
			while(uart_get_rxfifo_num(TLKDEV_SERIAL_PORT) > 0){
				uint08 rxData = reg_uart_data_buf(TLKDEV_SERIAL_PORT, uart_rx_byte_index[TLKDEV_SERIAL_PORT]) ;
				uart_rx_byte_index[TLKDEV_SERIAL_PORT] ++;
				uart_rx_byte_index[TLKDEV_SERIAL_PORT] &= 0x03;
				tlkapi_fifo_writeByte(&sTlkApiRecvFifo, rxData);
//				my_dump_str_data(TLKDEV_CFG_DBG_ENABLE, "rv:", &rxData, 1);
			}
		}
		if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RX_ERR)){
			uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
			// it will clear rx_fifo and rx_err_irq ,rx_buff_irq,so it won't enter rx_buff interrupt.
			uart_reset(TLKDEV_SERIAL_PORT); //clear hardware pointer
			uart_clr_rx_index(TLKDEV_SERIAL_PORT); //clear software pointer
		}
	#else
	    if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_TXDONE)){
		    uart_clr_tx_done(TLKDEV_SERIAL_PORT);
			if(sTlkDevSerialSendIsBusy){
				tlkapi_qfifo_dropData(&sTlkApiSendFifo);
				sTlkDevSerialSendIsBusy = false;
			}
		}
	    if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RXDONE)){ //A0-SOC can't use RX-DONE status,so this interrupt can noly used in A1-SOC.
			uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
			uint32 *pBuffer = (uint32*)tlkapi_qfifo_getBuff(&sTlkApiRecvFifo);
			if(pBuffer != nullptr){
				pBuffer[0] = uart_get_dma_rev_data_len(TLKDEV_SERIAL_PORT, TLKDEV_SERIAL_DMA_RX);
				tlkapi_qfifo_dropBuff(&sTlkApiRecvFifo);
			}
			pBuffer = (uint32*)tlkapi_qfifo_getBuff(&sTlkApiRecvFifo);
			if(pBuffer == nullptr){
				sTlkDevSerialRecvIsBusy = true;
			}else{
				pBuffer[0] = 0;
				uart_receive_dma(TLKDEV_SERIAL_PORT, (uint08*)(pBuffer+1), TLKDEV_SERIAL_RECV_BUFF_SIZE);
			}
	    }
		if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_TXBUF_IRQ_STATUS)){
			
		}
		if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RXBUF_IRQ_STATUS)){
//			uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
//			uart_reset(TLKDEV_SERIAL_PORT); 
//			sTlkDevSerialRecvIsBusy = true;
		}
		if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RX_ERR)){
			uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
			// it will clear rx_fifo and rx_err_irq ,rx_buff_irq,so it won't enter rx_buff interrupt.
			uart_reset(TLKDEV_SERIAL_PORT); //clear hardware pointer
			sTlkDevSerialRecvIsBusy = true;
		}
	#endif
	

}

int tlkdev_serial_init(void)
{
	unsigned short div;
	unsigned char bwpc;

	sTlkDevSerialRecvCB = nullptr;
	sTlkDevSerialSendIsBusy = false;
	sTlkDevSerialRecvIsBusy = false;
	sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_HEAD;
	sTlkDevSerialMakeLength = 0;
	
	#if !(TLKDEV_SERIAL_DMA_ENABLE)	
		tlkapi_fifo_init(&sTlkApiRecvFifo, false, false, sTlkDevSerialRecvBuffer, TLKDEV_SERIAL_RECV_BUFF_NUMB*TLKDEV_SERIAL_RECV_BUFF_SIZE);
	
		uart_reset(TLKDEV_SERIAL_PORT);
		uart_set_pin(TLKDEV_SERIAL_TX_PIN, TLKDEV_SERIAL_RX_PIN);
		uart_cal_div_and_bwpc(115200, sys_clk.pclk*1000*1000, &div, &bwpc);
		uart_init(TLKDEV_SERIAL_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);
		
		/* UART RX enable */
		uart_rx_irq_trig_level(TLKDEV_SERIAL_PORT, 1);
		plic_interrupt_enable(IRQ18_UART1);
		uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_RX_IRQ_MASK | UART_ERR_IRQ_MASK);
		plic_set_priority(IRQ18_UART1, IRQ_PRI_LEV3);
	#else
		tlkapi_qfifo_init(&sTlkApiRecvFifo, TLKDEV_SERIAL_RECV_BUFF_NUMB, (TLKDEV_SERIAL_RECV_BUFF_SIZE+4), 
			sTlkDevSerialRecvBuffer, TLKDEV_SERIAL_RECV_BUFF_NUMB*(TLKDEV_SERIAL_RECV_BUFF_SIZE+4));
		tlkapi_qfifo_init(&sTlkApiSendFifo, TLKDEV_SERIAL_SEND_BUFF_NUMB, (TLKDEV_SERIAL_SEND_BUFF_SIZE+4), 
			sTlkDevSerialSendBuffer, TLKDEV_SERIAL_SEND_BUFF_NUMB*(TLKDEV_SERIAL_SEND_BUFF_SIZE+4));
		
		uart_reset(TLKDEV_SERIAL_PORT);
		uart_set_pin(TLKDEV_SERIAL_TX_PIN, TLKDEV_SERIAL_RX_PIN);
		
		uart_cal_div_and_bwpc(115200, sys_clk.pclk*1000*1000, &div, &bwpc);
		uart_init(TLKDEV_SERIAL_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);	
		uart_set_dma_rx_timeout(TLKDEV_SERIAL_PORT, bwpc, 12, UART_BW_MUL1);		

		uart_set_tx_dma_config(TLKDEV_SERIAL_PORT, TLKDEV_SERIAL_DMA_TX);
		uart_set_rx_dma_config(TLKDEV_SERIAL_PORT, TLKDEV_SERIAL_DMA_RX);
		
		uart_clr_tx_done(TLKDEV_SERIAL_PORT);

		dma_clr_irq_mask(TLKDEV_SERIAL_DMA_TX, TC_MASK|ABT_MASK|ERR_MASK);
		dma_clr_irq_mask(TLKDEV_SERIAL_DMA_RX, TC_MASK|ABT_MASK|ERR_MASK);
		
		uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_RXDONE_MASK);
		uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_TXDONE_MASK); 
		uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_ERR_IRQ_MASK);

		plic_interrupt_enable(IRQ18_UART1);
		plic_set_priority(IRQ18_UART1, 2);
		
		uint08 *pBuffer = tlkapi_qfifo_getBuff(&sTlkApiRecvFifo);
		uart_receive_dma(TLKDEV_SERIAL_PORT, pBuffer+4, TLKDEV_SERIAL_RECV_BUFF_SIZE);
	#endif
	return TLK_ENONE;
}

bool tlkdev_serial_isBusy(void)
{
	return (sTlkDevSerialSendIsBusy || sTlkDevSerialRecvIsBusy/*|| uart_tx_is_busy(TLKDEV_SERIAL_PORT)*/)
		#if !(TLKDEV_SERIAL_DMA_ENABLE)
			|| !tlkapi_fifo_isEmpty(&sTlkApiRecvFifo)
		#else
			|| !tlkapi_qfifo_isEmpty(&sTlkApiSendFifo) || !tlkapi_qfifo_isEmpty(&sTlkApiRecvFifo)
		#endif
		;
}
void tlkdev_serial_wakeup(void)
{
	uart_clr_tx_index(TLKDEV_SERIAL_PORT);
	uart_clr_rx_index(TLKDEV_SERIAL_PORT);
	#if (TLKDEV_SERIAL_DMA_ENABLE)
	uint08 *pBuffer = tlkapi_qfifo_getBuff(&sTlkApiRecvFifo);
	uart_receive_dma(TLKDEV_SERIAL_PORT, pBuffer+4, TLKDEV_SERIAL_RECV_BUFF_SIZE);
	#endif
}


int tlkdev_serial_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	uint16 dataLen;
	uint16 tempVar;
	uint08 *pFrame;
	#if (TLKDEV_SERIAL_DMA_ENABLE)
	uint16 count;
	uint08 *pBuffer;
	#endif

//	my_dump_str_data(1, "send 01:", &sTlkDevSerialSendIsBusy, 1);
	if(headLen == 0 && pHead != nullptr) pHead = nullptr;
	if(bodyLen == 0 && pBody != nullptr) pBody = nullptr;
	if((headLen+bodyLen) == 0) return -TLK_EPARAM;
	if(headLen+bodyLen+TLKPRT_COMM_FRM_EXTLEN > TLKPRT_COMM_FRM_MAXLEN){
		return -TLK_ELENGTH;
	}

	#if (TLKDEV_SERIAL_DMA_ENABLE)
		count = (headLen+bodyLen+TLKPRT_COMM_FRM_EXTLEN+TLKDEV_SERIAL_SEND_BUFF_SIZE-1)/TLKDEV_SERIAL_SEND_BUFF_SIZE;
//		my_dump_str_u32s(TLKDEV_CFG_DBG_ENABLE, "send 02:", count, sTlkApiSendFifo.wptr,
//			sTlkApiSendFifo.rptr, tlkapi_qfifo_usedNum(&sTlkApiSendFifo));
		if(count > tlkapi_qfifo_idleNum(&sTlkApiSendFifo)) return -TLK_EQUOTA;
	#endif
	
	dataLen = 0;
	pFrame = sTlkDevSerialCache;
	pFrame[dataLen++] = TLKPRT_COMM_HEAD_SIGN0;
	pFrame[dataLen++] = TLKPRT_COMM_HEAD_SIGN1;
	pFrame[dataLen++] = 0x00; //Length[0~7]
	pFrame[dataLen++] = (pktType & 0x0F) << 4; //Length[8~11]+pktType[0~4]
	if(headLen != 0){
		tmemcpy(pFrame+dataLen, pHead, headLen);
		dataLen += headLen;
	}
	if(bodyLen != 0){
		tmemcpy(pFrame+dataLen, pBody, bodyLen);
		dataLen += bodyLen;
	}
	pFrame[2] |= (dataLen & 0x00FF);
	pFrame[3] |= (dataLen & 0x0F00) >> 8;
	tempVar = tlkalg_crc16_calc(pFrame+2, dataLen-2);
	pFrame[dataLen++] = (tempVar & 0x00FF);
	pFrame[dataLen++] = (tempVar & 0xFF00) >> 8;
	pFrame[dataLen++] = TLKPRT_COMM_TAIL_SIGN0;
	pFrame[dataLen++] = TLKPRT_COMM_TAIL_SIGN1;
	
	#if !(TLKDEV_SERIAL_DMA_ENABLE)
		uint16 index;
		for(index=0; index<dataLen; index++){    
	        uart_send_byte(TLKDEV_SERIAL_PORT, pFrame[index]);
	    }
	#else
//		my_dump_str_data(TLKDEV_CFG_DBG_ENABLE, "send 03:", &count, 1);
		while((count--) != 0 && dataLen != 0){
//			my_dump_str_data(TLKDEV_CFG_DBG_ENABLE, "send 04:", &dataLen, 2);
			pBuffer = tlkapi_qfifo_getBuff(&sTlkApiSendFifo);
			if(pBuffer == nullptr) break;
			if(dataLen <= TLKDEV_SERIAL_SEND_BUFF_SIZE) tempVar = dataLen;
			else tempVar = TLKDEV_SERIAL_SEND_BUFF_SIZE;
			pBuffer[0] = (tempVar & 0x00FF);
			pBuffer[1] = (tempVar & 0xFF00) >> 8;
			pBuffer[2] = 0;
			pBuffer[3] = 0;
			tmemcpy(pBuffer+4, pFrame, tempVar);
			pFrame  += tempVar;
			dataLen -= tempVar;
			tlkapi_qfifo_dropBuff(&sTlkApiSendFifo);

//			my_dump_str_data(TLKDEV_CFG_DBG_ENABLE, "snd OK:", pBuffer, tempVar+4);
		}
	#endif

	return TLK_ENONE;
}

void tlkdev_serial_regCB(tlkdev_serial_recvCB cb)
{
	sTlkDevSerialRecvCB = cb;
}

void tlkdev_serial_setBaudrate(uint32 baudrate)
{
    unsigned short div;
	unsigned char bwpc;

    uart_reset(TLKDEV_SERIAL_PORT);
	//uart_set_pin(UART0_TX_PB2, UART0_RX_PB3);// uart tx/rx pin set
	uart_set_pin(UART1_TX_PE0, UART1_RX_PE2);
	uart_cal_div_and_bwpc(baudrate, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(TLKDEV_SERIAL_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);
    
    /* UART RX enable */
    uart_rx_irq_trig_level(TLKDEV_SERIAL_PORT, 1);
    plic_interrupt_enable(IRQ18_UART1);
    uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_RX_IRQ_MASK);
}

void tlkdev_serial_handler(void)
{
	#if (TLKDEV_SERIAL_DMA_ENABLE)
		uint08 *pBuffer;
		if((reg_dma_tc_isr & FLD_DMA_CHANNEL5_IRQ) != 0){
			reg_dma_tc_isr |= FLD_DMA_CHANNEL5_IRQ;
			uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
			sTlkDevSerialRecvIsBusy = true;
		}
		if(!sTlkDevSerialSendIsBusy && !tlkapi_qfifo_isEmpty(&sTlkApiSendFifo) != 0){
			pBuffer = tlkapi_qfifo_getData(&sTlkApiSendFifo);
			if(pBuffer == nullptr || (pBuffer[0] == 0 && pBuffer[1] == 0) || pBuffer[2] != 0 || pBuffer[3] != 0){
				tlkapi_error(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "Serial Send Fail: DmaLen error! %x", pBuffer[0]);
				tlkapi_qfifo_dropData(&sTlkApiSendFifo);
			}else{
				uint16 dataLen = ((uint16)pBuffer[1] << 8) | pBuffer[0];
				if(dataLen == 0 || dataLen > TLKDEV_SERIAL_SEND_BUFF_SIZE){
					tlkapi_error(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "Serial Send Fail: DataLen[%d] error!", dataLen);
					tlkapi_qfifo_dropData(&sTlkApiSendFifo);
				}else{
					sTlkDevSerialSendIsBusy = true;
					uart_send_dma(TLKDEV_SERIAL_PORT, pBuffer+4, dataLen);
					tlkapi_array(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "Serial Send OK:", pBuffer+4, dataLen);
				}
			}
		}
		if(sTlkDevSerialRecvIsBusy){
			pBuffer = tlkapi_qfifo_getBuff(&sTlkApiRecvFifo);
			if(pBuffer != nullptr){
				sTlkDevSerialRecvIsBusy = false;
				uart_receive_dma(TLKDEV_SERIAL_PORT, pBuffer+4, TLKDEV_SERIAL_RECV_BUFF_SIZE);
			}
		}
	#endif
	tlkdev_serial_recvHandler();
}


static void tlkdev_serial_recvHandler(void)
{
	uint08 rbyte;
	uint16 rawCrc;
	uint16 calCrc;
	#if !(TLKDEV_SERIAL_DMA_ENABLE)
		while(tlkapi_fifo_readByte(&sTlkApiRecvFifo, &rbyte) == TLK_ENONE){
			tlkapi_serial_makeFrame(rbyte);
			if(sTlkDevSerialMakeState != TLKDEV_SERIAL_MSTATE_READY) continue;
			rawCrc = ((uint16)(sTlkDevSerialFrame[sTlkDevSerialMakeDataLen-3])<<8)
				| (sTlkDevSerialFrame[sTlkDevSerialMakeDataLen-4]);
			calCrc = tlkalg_crc16_calc(sTlkDevSerialFrame+2, sTlkDevSerialMakeDataLen-6);
			if(rawCrc != calCrc){
				tlkapi_error(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "Serial CRC Error: rawCrc[%x] - calCrc[%x]", rawCrc, calCrc);
			}else if(sTlkDevSerialRecvCB != nullptr){
				sTlkDevSerialRecvCB(sTlkDevSerialFrame, sTlkDevSerialMakeDataLen);
			}
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_HEAD;
			sTlkDevSerialMakeLength = 0;
		}
	#else
		uint16 index;
		uint08 *pData;
		uint16 dataLen;
		while(!tlkapi_qfifo_isEmpty(&sTlkApiRecvFifo)){
			pData = tlkapi_qfifo_getData(&sTlkApiRecvFifo);
			dataLen = (((uint16)pData[1]) << 8) | pData[0];
			pData  += 4;
//			tlkapi_array(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "recv1:", pData, dataLen);
			for(index=0; index<dataLen; index++){
				rbyte = pData[index];
				tlkapi_serial_makeFrame(rbyte);
				if(sTlkDevSerialMakeState != TLKDEV_SERIAL_MSTATE_READY) continue;
				rawCrc = ((uint16)(sTlkDevSerialFrame[sTlkDevSerialMakeDataLen-3])<<8)
					| (sTlkDevSerialFrame[sTlkDevSerialMakeDataLen-4]);
				calCrc = tlkalg_crc16_calc(sTlkDevSerialFrame+2, sTlkDevSerialMakeDataLen-6);
				if(rawCrc != calCrc){
					tlkapi_error(TLKDEV_DBG_FLAG, TLKDEV_DBG_SIGN, "Serial CRC Error: rawCrc[%x] - calCrc[%x]", rawCrc, calCrc);
				}else if(sTlkDevSerialRecvCB != nullptr){
					sTlkDevSerialRecvCB(sTlkDevSerialFrame, sTlkDevSerialMakeDataLen);
				}
				sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_HEAD;
				sTlkDevSerialMakeLength = 0;
			}
			tlkapi_qfifo_dropData(&sTlkApiRecvFifo);
		}
	#endif
}
static void tlkapi_serial_makeFrame(uint08 rbyte)
{
	if(sTlkDevSerialMakeState == TLKDEV_SERIAL_MSTATE_HEAD){
		if(sTlkDevSerialMakeLength == 0 && rbyte == TLKPRT_COMM_HEAD_SIGN0){
			sTlkDevSerialMakeLength ++;
		}else if(sTlkDevSerialMakeLength == 1 && rbyte == TLKPRT_COMM_HEAD_SIGN1){
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_ATTR;
			sTlkDevSerialMakeLength = 0;
			sTlkDevSerialMakeDataLen = 0;
			sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = TLKPRT_COMM_HEAD_SIGN0;
			sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = TLKPRT_COMM_HEAD_SIGN1;
		}else{
			sTlkDevSerialMakeLength = 0;
		}
	}else if(sTlkDevSerialMakeState == TLKDEV_SERIAL_MSTATE_ATTR){
		sTlkDevSerialMakeLength ++;
		sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = rbyte;
		if(sTlkDevSerialMakeLength < 2) return;
		sTlkDevSerialMakeLength = (((uint16)(sTlkDevSerialFrame[3] & 0x0F))<<8)|sTlkDevSerialFrame[2];
		if(sTlkDevSerialMakeLength+4 > TLKPRT_COMM_FRM_MAXLEN || sTlkDevSerialMakeLength < 4){
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_HEAD;
			sTlkDevSerialMakeLength = 0;
		}else if(sTlkDevSerialMakeLength == 4){
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_CHECK;
			sTlkDevSerialMakeLength = 2;
		}else{
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_BODY;
			sTlkDevSerialMakeLength -= 4;
		}
	}else if(sTlkDevSerialMakeState == TLKDEV_SERIAL_MSTATE_BODY){
		sTlkDevSerialMakeLength --;
		sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = rbyte;
		if(sTlkDevSerialMakeLength != 0) return;
		sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_CHECK;
		sTlkDevSerialMakeLength = 0;
	}else if(sTlkDevSerialMakeState == TLKDEV_SERIAL_MSTATE_CHECK){
		sTlkDevSerialMakeLength ++;
		sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = rbyte;
		if(sTlkDevSerialMakeLength < 2) return;
		sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_TAIL;
		sTlkDevSerialMakeLength = 0;
	}else if(sTlkDevSerialMakeState == TLKDEV_SERIAL_MSTATE_TAIL){
		if(sTlkDevSerialMakeLength == 0 && rbyte == TLKPRT_COMM_TAIL_SIGN0){
			sTlkDevSerialMakeLength ++;
		}else if(sTlkDevSerialMakeLength == 1 && rbyte == TLKPRT_COMM_TAIL_SIGN1){
			sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = TLKPRT_COMM_TAIL_SIGN0;
			sTlkDevSerialFrame[sTlkDevSerialMakeDataLen++] = TLKPRT_COMM_TAIL_SIGN1;
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_READY;
			sTlkDevSerialMakeLength = 0;
		}else{
			sTlkDevSerialMakeState = TLKDEV_SERIAL_MSTATE_HEAD;
			sTlkDevSerialMakeLength = 0;
		}
	}
}

#else

#if (TLKAPP_CONTROLLER_ENABLE)
#include "tlkstk/hci/bt_hci.h"


unsigned char  pending_dma_buf[PENDING_BUF_LEN];

#if TLKAPP_HCI_UART_MODE
_attribute_ram_code_sec_ static void tlkdev_serial_recvHandler(void);
static uint08 tlkdev_serial_send(uint08 * addr);
int tlkdev_serial_setBaudrate(uint32 baudrate);
void tlkdev_serial_regCB(tlk_hci_serial_sendCB cb)
{
	sTlk_hci_serial_sendCB = cb;
}
void tlkdev_serial_tx_rest_regCB(tlk_hci_serial_tx_reset_sendCB cb)
{
	sTlk_hci_serial_tx_reset_sendCB = cb;
}

void tlkdev_serial_set_baudrate_regCB(tlk_hci_cmd_vendor_CB cb)
{
	sTlk_hci_cmd_vendor_BaudrateCB = cb;
}
void tlkdev_standby_regCB(tlk_hci_cmd_vendor_CB cb)
{
	sTlk_hci_cmd_vendor_standbyCB = cb;
}
static void uart_dmarx_timeout_disable(uart_num_e uart_num)
{
	uint08 temp = (REG_ADDR8(0x140088+uart_num*0x40)&0xf0)|0x80;

	write_reg8(0x140088+uart_num*0x40,temp);
}
int tlkdev_standby_set(uint32_t data)
{
	start_reboot();
	return -1;
}
void uart_tx_reset_init(void)
{
	uart_set_tx_dma_config(UART_ID, UART_DMA_TX);

	uart_clr_tx_done(UART_ID);

	dma_clr_irq_mask(UART_DMA_TX,TC_MASK|ABT_MASK|ERR_MASK);

	uart_set_irq_mask(UART_ID, UART_TXDONE_MASK);

	plic_interrupt_enable(UART_IRQ);
}
static void rts_io_init(void)
{
    gpio_set_gpio_en(UART_RTS_PIN);
    gpio_output_en(UART_RTS_PIN);         //enable output
    gpio_input_dis(UART_RTS_PIN);         //disable input
}

int tlkdev_serial_init(void)
{
	unsigned short div;
	unsigned char bwpc;

	uart_reset(UART_ID);

	uart_set_pin(UART_TX_PIN,UART_RX_PIN);

	uart_cal_div_and_bwpc(UART_BAUDRATE, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_set_dma_rx_timeout(UART_ID, bwpc, 12, UART_BW_MUL1);
	uart_init(UART_ID, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);
#if UART_FLOW_CTR
	uart_cts_config(UART_ID, UART_CTS_PIN, CTS_STOP_VOLT);
	uart_set_cts_en(UART_ID);

#endif
	uart_set_tx_dma_config(UART_ID, UART_DMA_TX);
	uart_set_rx_dma_config(UART_ID, UART_DMA_RX);
	uart_clr_tx_done(UART_ID);

	dma_clr_irq_mask(UART_DMA_TX,TC_MASK|ABT_MASK|ERR_MASK);
	dma_clr_irq_mask(UART_DMA_RX,TC_MASK|ABT_MASK|ERR_MASK);

	uart_set_irq_mask(UART_ID, UART_RXDONE_MASK);
	uart_set_irq_mask(UART_ID, UART_TXDONE_MASK);

	plic_interrupt_enable(UART_IRQ);

	tmemset((uint08*)(pending_dma_buf),0,PENDING_BUF_LEN);
	uart_receive_dma(UART_ID,(uint08*)(pending_dma_buf), PENDING_BUF_LEN);
	uart_dmarx_timeout_disable(UART_ID);

    rts_io_init();

    tlkdev_serial_regCB(tlkdev_serial_send);
    tlkdev_serial_tx_rest_regCB(uart_tx_reset_init);
    tlkdev_serial_set_baudrate_regCB(tlkdev_serial_setBaudrate);
    tlkdev_standby_regCB(tlkdev_standby_set);

	return TLK_ENONE;
}


int tlkdev_serial_setBaudrate(uint32 baudrate)
{
	unsigned short div;
	unsigned char bwpc;

	uart_set_dma_enable(UART_ID,0);

	uart_clr_irq_mask(UART_ID, UART_TXDONE_MASK);
	uart_clr_irq_mask(UART_ID, UART_RXDONE_MASK);

	uart_reset(UART_ID);

	uart_cal_div_and_bwpc(baudrate, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_set_dma_rx_timeout(UART_ID, bwpc, 12, UART_BW_MUL1);
	uart_init(UART_ID, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);

	uart_set_tx_dma_config(UART_ID, UART_DMA_TX);
	uart_set_rx_dma_config(UART_ID, UART_DMA_RX);
	uart_clr_tx_done(UART_ID);
	uart_clr_irq_status(UART_ID,UART_CLR_RX);
	dma_clr_irq_mask(UART_DMA_TX,TC_MASK|ABT_MASK|ERR_MASK);
	dma_clr_irq_mask(UART_DMA_RX,TC_MASK|ABT_MASK|ERR_MASK);

	uart_set_irq_mask(UART_ID, UART_RXDONE_MASK);
	uart_set_irq_mask(UART_ID, UART_TXDONE_MASK);

	plic_interrupt_enable(UART_IRQ);

	tmemset((uint08*)(pending_dma_buf),0,PENDING_BUF_LEN);
	uart_receive_dma(UART_ID,(uint08*)(pending_dma_buf), PENDING_BUF_LEN);
	uart_dmarx_timeout_disable(UART_ID);

	return -1;
}

void tlkdev_serial_handler(void)
{
	tlkdev_serial_recvHandler();
}

/*
 * 3M baudrate : read wait 2us ,1bit 330ns,5*330ns
 * 2M baudrate : read wait 3us ,1bit 500ns,5*500ns
 */
#define READ_STATE_WAIT_TICK  sleep_us(2) // 3M
#define RTS_TO_READ_UART  5                //us
/**
 * @brief     This function serves to get uart state is busy or idle.
 * @param[in] uart_num - UART0 or UART1.
 * @return    0 or 1
 */
volatile unsigned int status_read_rts_tick ;

 _attribute_ram_code_sec_noinline_ uint8_t is_uart_rx_done(uart_num_e uart_num)
{
	volatile unsigned char status = 0;
	volatile unsigned int status_read_start_tick = reg_system_tick;
	status = (reg_uart_state(uart_num)&0xf0);
	if (status == 0) {
		READ_STATE_WAIT_TICK;
		status = (reg_uart_state(uart_num)&0xf0);
		if (status == 0) {

			READ_STATE_WAIT_TICK;

			status = (reg_uart_state(uart_num)&0xf0);

			if (status == 0) {

				UART_MANUAL_FLOW_CTR_RTS_STOP;
				status_read_rts_tick = reg_system_tick;
				if(clock_time_exceed(status_read_start_tick,10)){

					return 0;
				}
				status = (reg_uart_state(uart_num)&0xf0);
				if (status == 0){

					return 1;
				}
				else {
					UART_MANUAL_FLOW_CTR_RTS_START;
					return 0;
				}
				return 1;
			}
			return 0;
		 }
	}
	return 0;
}
 /**
  * @brief     This function serves to get uart state is busy or idle soon.
  * @param[in] uart_num - UART0 or UART1.
  * @return    0 or 1
  */
 _attribute_ram_code_sec_noinline_ uint8_t is_uart_rx_done_soon(uart_num_e uart_num)
{
	if(clock_time_exceed(status_read_rts_tick,RTS_TO_READ_UART)){
		return 0;
	}
	volatile uint8_t status = 0;
	status = (reg_uart_state(uart_num)&0xf0);

	if (status == 0) {

		return 1;
	}

	return 0;

}

uint08  tail_uart_data[3];
_attribute_ram_code_sec_ uint08 uart_dmabuf_uartbuf_process(uint32 *dma_size,uint08 *buf_cnt)
{
	if(!is_uart_rx_done_soon(UART_ID)) return 0;
	GLOBAL_INT_DISABLE();
	if(!is_uart_rx_done_soon(UART_ID)){
		GLOBAL_INT_RESTORE();
		return 0;
	}
	uart_set_rx_dma_enable(UART_ID,0);
	*dma_size = reg_dma_size(UART_DMA_RX);
    *buf_cnt = reg_uart_buf_cnt(UART_ID)&3;
	uart_set_rx_dma_enable(UART_ID,1);
	GLOBAL_INT_RESTORE();

	uint32 read_dma_size= *dma_size;
	uint08 read_buf_cnt = *buf_cnt;

	if((UART_DMA_SIZE!=read_dma_size)||read_buf_cnt){
		if(read_buf_cnt){

			uart_set_rx_dma_enable(UART_ID,0);
			for(uint08 i=0;i<read_buf_cnt;i++){
				tail_uart_data[i] =	reg_uart_data_buf(UART_ID,i);
			}
			uart_clr_irq_status(UART_ID,UART_CLR_RX);
			uart_set_rx_dma_enable(UART_ID,1);
		}

		return 1;
	}
	else {
			return 0;
	}
}
_attribute_ram_code_sec_ static void tlkdev_serial_recvHandler(void)
{
	if(is_uart_rx_done(UART_ID)){

		uint32 read_dma_size;
		uint08  read_buf_cnt;

		if(!uart_dmabuf_uartbuf_process(&read_dma_size,&read_buf_cnt)){
			UART_MANUAL_FLOW_CTR_RTS_START;
		    return;
		}

		uint08 *p = pending_dma_buf;
		u16 dma_offset = PENDING_BUF_LEN - read_dma_size*4;
		u16 rx_len = 0 ;

		if(dma_offset+read_buf_cnt){

			GLOBAL_INT_DISABLE();
			// dma buf + copy tail_uart_data
			if(read_buf_cnt){
			tmemcpy(p+dma_offset,tail_uart_data,read_buf_cnt);
			}
			// length in pending buf
			rx_len = dma_offset+read_buf_cnt;

			GLOBAL_INT_RESTORE();

	   }

		data_push_controller_hci_fifo_process(pending_dma_buf,rx_len);
		//reset dma&uart
		GLOBAL_INT_DISABLE();
		tmemset(pending_dma_buf,0,8);
		uart_receive_dma(UART_ID,(uint08 *)(pending_dma_buf), PENDING_BUF_LEN);
		GLOBAL_INT_RESTORE();
		//release RTS
		UART_MANUAL_FLOW_CTR_RTS_START;
	}

}

#define SERIAL_BUSY_TIMEOUT 100*1000
uint08 tlkdev_serial_send_flag = 0;
uint08 tlkdev_serial_tick = 0;
uint08 tlkdev_serial_send(uint08 * addr)
{
	if(tlkdev_serial_send_flag == 1){

		if(clock_time_exceed(tlkdev_serial_tick,SERIAL_BUSY_TIMEOUT)){

			uart_tx_reset_init();
			tlkdev_serial_send_flag = 0;
			my_dump_str_u32s(TLKAPP_HCI_UART_MODE,"tlkdev_serial_send",tlkdev_serial_send_flag,0,0,0);

		}
		else return 1;
	}
	int len = addr[0]+addr[1]*256;
	uart_send_dma(UART_ID,addr+4,len);
	tlkdev_serial_tick = clock_time();
	tlkdev_serial_send_flag = 1;
	return 0;
}
_attribute_ram_code_sec_ void UART_IRQHandler(void)
{
    if(uart_get_irq_status(UART_ID,UART_TXDONE))
	{
    	tlkdev_serial_send_flag = 0;
	    uart_clr_tx_done(UART_ID);

	}
}
#endif
#endif

#endif

