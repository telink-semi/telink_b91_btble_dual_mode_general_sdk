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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"

#include "drivers.h"


static void tlkdev_serial_sendHandler(void);
static void tlkdev_serial_recvHandler(void);

#if !(TLKDEV_SERIAL_DMA_ENABLE)
	#define TLKDEV_SERIAL_RECV_BUFF_NUMB      1 //4
	#define TLKDEV_SERIAL_RECV_BUFF_SIZE      127//(TLKPRT_COMM_FRM_MAXLEN)
	#define TLKDEV_SERIAL_SEND_BUFF_NUMB      1
	#define TLKDEV_SERIAL_SEND_BUFF_SIZE      (TLKPRT_COMM_FRM_MAXLEN)
	tlkapi_fifo_t sTlkDevSerialRecvFifo;
#else
	#define TLKDEV_SERIAL_RECV_BUFF_NUMB      8//16
	#define TLKDEV_SERIAL_RECV_BUFF_SIZE      152//64//(TLKPRT_COMM_FRM_MAXLEN)
	#define TLKDEV_SERIAL_SEND_BUFF_NUMB      8
	#define TLKDEV_SERIAL_SEND_BUFF_SIZE      64
	tlkapi_qfifo_t sTlkDevSerialRecvFifo;
	tlkapi_qfifo_t sTlkDevSerialSendFifo;
#endif

static uint08 sTlkDevSerialSendIsBusy;
static uint08 sTlkDevSerialRecvIsBusy;
__attribute__((aligned(4)))
static uint08 sTlkDevSerialRecvBuffer[TLKDEV_SERIAL_RECV_BUFF_NUMB*(TLKDEV_SERIAL_RECV_BUFF_SIZE+4)];
#if (TLKDEV_SERIAL_DMA_ENABLE)
__attribute__((aligned(4)))
static uint08 sTlkDevSerialSendBuffer[TLKDEV_SERIAL_SEND_BUFF_NUMB*(TLKDEV_SERIAL_SEND_BUFF_SIZE+4)];
#endif
static tlkdev_serial_recvCB sTlkDevSerialRecvCB;



int tlkdev_serial_init(void)
{
	unsigned short div;
	unsigned char bwpc;

	sTlkDevSerialRecvCB = nullptr;
	sTlkDevSerialSendIsBusy = false;
	sTlkDevSerialRecvIsBusy = false;
	
#if !(TLKDEV_SERIAL_DMA_ENABLE)	
	tlkapi_fifo_init(&sTlkDevSerialRecvFifo, false, false, sTlkDevSerialRecvBuffer, TLKDEV_SERIAL_RECV_BUFF_NUMB*TLKDEV_SERIAL_RECV_BUFF_SIZE);

	uart_reset(TLKDEV_SERIAL_PORT);
	uart_set_pin(TLKDEV_SERIAL_TX_PIN, TLKDEV_SERIAL_RX_PIN);
	uart_cal_div_and_bwpc(TLKDEV_SERIAL_BAUDRATE, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(TLKDEV_SERIAL_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);
	
	/* UART RX enable */
	uart_rx_irq_trig_level(TLKDEV_SERIAL_PORT, 1);
	plic_interrupt_enable(IRQ18_UART1);
	uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_RX_IRQ_MASK | UART_ERR_IRQ_MASK);
	plic_set_priority(IRQ18_UART1, IRQ_PRI_LEV3);
#else
	tlkapi_qfifo_init(&sTlkDevSerialRecvFifo, TLKDEV_SERIAL_RECV_BUFF_NUMB, (TLKDEV_SERIAL_RECV_BUFF_SIZE+4), 
		sTlkDevSerialRecvBuffer, TLKDEV_SERIAL_RECV_BUFF_NUMB*(TLKDEV_SERIAL_RECV_BUFF_SIZE+4));
	tlkapi_qfifo_init(&sTlkDevSerialSendFifo, TLKDEV_SERIAL_SEND_BUFF_NUMB, (TLKDEV_SERIAL_SEND_BUFF_SIZE+4), 
		sTlkDevSerialSendBuffer, TLKDEV_SERIAL_SEND_BUFF_NUMB*(TLKDEV_SERIAL_SEND_BUFF_SIZE+4));
	
	uart_reset(TLKDEV_SERIAL_PORT);
	uart_set_pin(TLKDEV_SERIAL_TX_PIN, TLKDEV_SERIAL_RX_PIN);
		
	uart_cal_div_and_bwpc(TLKDEV_SERIAL_BAUDRATE, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(TLKDEV_SERIAL_PORT, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);	
	uart_set_dma_rx_timeout(TLKDEV_SERIAL_PORT, bwpc, 12, UART_BW_MUL2);		

	uart_set_tx_dma_config(TLKDEV_SERIAL_PORT, TLKDEV_SERIAL_DMA_TX);
	uart_set_rx_dma_config(TLKDEV_SERIAL_PORT, TLKDEV_SERIAL_DMA_RX);
	
	uart_clr_tx_done(TLKDEV_SERIAL_PORT);

	dma_clr_irq_mask(TLKDEV_SERIAL_DMA_TX, TC_MASK|ABT_MASK|ERR_MASK);
	dma_clr_irq_mask(TLKDEV_SERIAL_DMA_RX, TC_MASK|ABT_MASK|ERR_MASK);
	
	uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_RXDONE_MASK);
	uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_TXDONE_MASK); 
	uart_set_irq_mask(TLKDEV_SERIAL_PORT, UART_ERR_IRQ_MASK);

	plic_interrupt_enable(IRQ18_UART1);
	plic_set_priority(IRQ18_UART1, IRQ_PRI_LEV2);
	
	uint08 *pBuffer = tlkapi_qfifo_getBuff(&sTlkDevSerialRecvFifo);
	uart_receive_dma(TLKDEV_SERIAL_PORT, pBuffer+4, TLKDEV_SERIAL_RECV_BUFF_SIZE);
#endif
	
	return TLK_ENONE;
}

bool tlkdev_serial_isBusy(void)
{
	return (sTlkDevSerialSendIsBusy || sTlkDevSerialRecvIsBusy/*|| uart_tx_is_busy(TLKDEV_SERIAL_PORT)*/)
		#if !(TLKDEV_SERIAL_DMA_ENABLE)
			|| !tlkapi_fifo_isEmpty(&sTlkDevSerialRecvFifo)
		#else
			|| !tlkapi_qfifo_isEmpty(&sTlkDevSerialSendFifo) || !tlkapi_qfifo_isEmpty(&sTlkDevSerialRecvFifo)
		#endif
		;
}
void tlkdev_serial_wakeup(void)
{
	uart_clr_tx_index(TLKDEV_SERIAL_PORT);
	uart_clr_rx_index(TLKDEV_SERIAL_PORT);
	#if (TLKDEV_SERIAL_DMA_ENABLE)
	uint08 *pBuffer = tlkapi_qfifo_getBuff(&sTlkDevSerialRecvFifo);
	uart_receive_dma(TLKDEV_SERIAL_PORT, pBuffer+4, TLKDEV_SERIAL_RECV_BUFF_SIZE);
	#endif
}


int tlkdev_serial_send(uint08 *pData, uint16 dataLen)
{
	#if (TLKDEV_SERIAL_DMA_ENABLE)
	uint16 count;
	uint16 tempVar;
	uint08 *pBuffer;
	#endif

#if (TLKDEV_SERIAL_DMA_ENABLE)
	count = (dataLen+TLKPRT_COMM_FRM_EXTLEN+TLKDEV_SERIAL_SEND_BUFF_SIZE-1)/TLKDEV_SERIAL_SEND_BUFF_SIZE;
//	tlkapi_trace(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "send 02: %d %d %d %d", count, sTlkDevSerialSendFifo.wptr,
//		sTlkDevSerialSendFifo.rptr, tlkapi_qfifo_usedNum(&sTlkDevSerialSendFifo));
	if(count > tlkapi_qfifo_idleNum(&sTlkDevSerialSendFifo)) return -TLK_EQUOTA;
#endif
#if !(TLKDEV_SERIAL_DMA_ENABLE)
	uint16 index;
	for(index=0; index<dataLen; index++){    
        uart_send_byte(TLKDEV_SERIAL_PORT, pData[index]);
    }
#else
//	tlkapi_trace(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "send 03: %d", count);
	while((count--) != 0 && dataLen != 0){
//		tlkapi_trace(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "send 04: %d", dataLen);
		pBuffer = tlkapi_qfifo_getBuff(&sTlkDevSerialSendFifo);
		if(pBuffer == nullptr) break;
		if(dataLen <= TLKDEV_SERIAL_SEND_BUFF_SIZE) tempVar = dataLen;
		else tempVar = TLKDEV_SERIAL_SEND_BUFF_SIZE;
		pBuffer[0] = (tempVar & 0x00FF);
		pBuffer[1] = (tempVar & 0xFF00) >> 8;
		pBuffer[2] = 0;
		pBuffer[3] = 0;
		tmemcpy(pBuffer+4, pData, tempVar);
		pData   += tempVar;
		dataLen -= tempVar;
		tlkapi_qfifo_dropBuff(&sTlkDevSerialSendFifo);
//		tlkapi_array(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "snd OK:", pBuffer, tempVar+4);
	}
#endif
	
	return TLK_ENONE;
}

void tlkdev_serial_clear(void)
{
	#if (TLKDEV_SERIAL_DMA_ENABLE)
	tlkapi_qfifo_clear(&sTlkDevSerialSendFifo);
	tlkapi_qfifo_clear(&sTlkDevSerialRecvFifo);
	#endif
}

void tlkdev_serial_regCB(tlkdev_serial_recvCB cb)
{
	sTlkDevSerialRecvCB = cb;
}


void tlkdev_serial_handler(void)
{
	tlkdev_serial_sendHandler();
	tlkdev_serial_recvHandler();
}

static void tlkdev_serial_sendHandler(void)
{
#if (TLKDEV_SERIAL_DMA_ENABLE)
	uint08 *pBuffer;
	if(!sTlkDevSerialSendIsBusy && !tlkapi_qfifo_isEmpty(&sTlkDevSerialSendFifo)){
		pBuffer = tlkapi_qfifo_getData(&sTlkDevSerialSendFifo);
		if(pBuffer == nullptr || (pBuffer[0] == 0 && pBuffer[1] == 0) || pBuffer[2] != 0 || pBuffer[3] != 0){
			tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "Serial Send Fail: DmaLen error! %x", pBuffer[0]);
			tlkapi_qfifo_dropData(&sTlkDevSerialSendFifo);
		}else{
			uint16 dataLen = ((uint16)pBuffer[1] << 8) | pBuffer[0];
			if(dataLen == 0 || dataLen > TLKDEV_SERIAL_SEND_BUFF_SIZE){
				tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "Serial Send Fail: DataLen[%d] error!", dataLen);
				tlkapi_qfifo_dropData(&sTlkDevSerialSendFifo);
			}else{
				sTlkDevSerialSendIsBusy = true;
				uart_send_dma(TLKDEV_SERIAL_PORT, pBuffer+4, dataLen);
				tlkapi_array(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "Serial Send OK:", pBuffer+4, dataLen);
			}
		}
	}
	if(sTlkDevSerialRecvIsBusy){
		pBuffer = tlkapi_qfifo_getBuff(&sTlkDevSerialRecvFifo);
		if(pBuffer != nullptr){
			sTlkDevSerialRecvIsBusy = false;
			uart_receive_dma(TLKDEV_SERIAL_PORT, pBuffer+4, TLKDEV_SERIAL_RECV_BUFF_SIZE);
		}
	}
#endif
}
static void tlkdev_serial_recvHandler(void)
{
#if !(TLKDEV_SERIAL_DMA_ENABLE)
	uint08 rbyte;
	while(tlkapi_fifo_readByte(&sTlkDevSerialRecvFifo, &rbyte) == TLK_ENONE){
		if(sTlkDevSerialRecvCB != nullptr) sTlkDevSerialRecvCB(&rbyte, 1);
	}
#else
	uint08 *pData;
	uint16 dataLen;
	if((reg_dma_tc_isr & FLD_DMA_CHANNEL5_IRQ) != 0){
		reg_dma_tc_isr |= FLD_DMA_CHANNEL5_IRQ;
		uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_RX_ERR);
		sTlkDevSerialRecvIsBusy = true;
	}
	while(!tlkapi_qfifo_isEmpty(&sTlkDevSerialRecvFifo)){
		pData = tlkapi_qfifo_getData(&sTlkDevSerialRecvFifo);
		dataLen = (((uint16)pData[1]) << 8) | pData[0];
		pData  += 4;
//		tlkapi_array(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "recv1:", pData, dataLen);
		if(sTlkDevSerialRecvCB != nullptr) sTlkDevSerialRecvCB(pData, dataLen);
		tlkapi_qfifo_dropData(&sTlkDevSerialRecvFifo);
	}
#endif
}


_attribute_ram_code_sec_ 
void uart1_irq_handler(void)
{
//	tlkapi_sendData(TLKDEV_SYS_DBG_FLAG, "irq:", 0, 0);
	
#if !(TLKDEV_SERIAL_DMA_ENABLE)
	if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RXBUF_IRQ_STATUS)){
		while(uart_get_rxfifo_num(TLKDEV_SERIAL_PORT) > 0){
			uint08 rxData = reg_uart_data_buf(TLKDEV_SERIAL_PORT, uart_rx_byte_index[TLKDEV_SERIAL_PORT]) ;
			uart_rx_byte_index[TLKDEV_SERIAL_PORT] ++;
			uart_rx_byte_index[TLKDEV_SERIAL_PORT] &= 0x03;
			tlkapi_fifo_writeByte(&sTlkDevSerialRecvFifo, rxData);
//			tlkapi_sendData(TLKDEV_SYS_DBG_FLAG, "rv:", &rxData, 1);
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
			tlkapi_qfifo_dropData(&sTlkDevSerialSendFifo);
			sTlkDevSerialSendIsBusy = false;
		}
	}
    if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RXDONE)){ //A0-SOC can't use RX-DONE status,so this interrupt can noly used in A1-SOC.
		uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
//		tlkapi_sendStr(TLKDEV_SYS_DBG_FLAG, "irq: recv");
		uint32 *pBuffer = (uint32*)tlkapi_qfifo_getBuff(&sTlkDevSerialRecvFifo);
		if(pBuffer != nullptr){
			pBuffer[0] = uart_get_dma_rev_data_len(TLKDEV_SERIAL_PORT, TLKDEV_SERIAL_DMA_RX);
			tlkapi_qfifo_dropBuff(&sTlkDevSerialRecvFifo);
		}
		pBuffer = (uint32*)tlkapi_qfifo_getBuff(&sTlkDevSerialRecvFifo);
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
//		uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
//		uart_reset(TLKDEV_SERIAL_PORT); 
//		sTlkDevSerialRecvIsBusy = true;
	}
	if(uart_get_irq_status(TLKDEV_SERIAL_PORT, UART_RX_ERR)){
		uart_clr_irq_status(TLKDEV_SERIAL_PORT, UART_CLR_RX);
		// it will clear rx_fifo and rx_err_irq ,rx_buff_irq,so it won't enter rx_buff interrupt.
		uart_reset(TLKDEV_SERIAL_PORT); //clear hardware pointer
		sTlkDevSerialRecvIsBusy = true;
	}
#endif
}

#endif

