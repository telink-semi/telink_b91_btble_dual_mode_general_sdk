/********************************************************************************************************
 * @file     tlkdev_hciuart.c
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
#if (TLK_DEV_HCIUART_ENABLE)
#include "tlkdev/tlkdev_stdio.h"
#include "tlkprt/tlkprt_comm.h"
#include "tlkdev/sys/tlkdev_hciuart.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"

#include "drivers.h"
#include "tlkstk/inner/tlkstk_myudb.h"

#include "tlkstk/hci/bt_hci.h"


void tlkdev_hciuart_regCB(tlk_hci_serial_sendCB cb);

extern unsigned char uart_dma_tx_chn[2];
extern unsigned char uart_dma_rx_chn[2];
extern unsigned int uart_dma_rev_size[2];
/**
 * @brief     This function serves to set uart rx_dma channel and tx dma disable.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] en      - enable or disable.
 * @return    none
 */
void uart_set_dma_enable(uart_num_e uart_num, u8 en)
{
	if(!en){
		dma_chn_dis(uart_dma_rx_chn[uart_num]);
		dma_chn_dis(uart_dma_tx_chn[uart_num]);
	}
	else{
		dma_chn_en(uart_dma_rx_chn[uart_num]);
		dma_chn_en(uart_dma_tx_chn[uart_num]);
	}


}
/**
 * @brief     This function serves to set only uart rx_dma channel enable or disable.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] en      - enable or disable.
 * @return    none
 */
_attribute_ram_code_sec_noinline_ void uart_set_rx_dma_enable(uart_num_e uart_num, u8 en)
{
	if(!en){
		dma_chn_dis(uart_dma_rx_chn[uart_num]);
	}
	else{
		dma_chn_en(uart_dma_rx_chn[uart_num]);
	}

}



unsigned char  pending_dma_buf[PENDING_BUF_LEN];

_attribute_ram_code_sec_ static void tlkdev_hciuart_recvHandler(void);
static uint08 tlkdev_hciuart_send(uint08 * addr);
int tlkdev_hciuart_setBaudrate(uint32 baudrate);
void tlkdev_hciuart_regCB(tlk_hci_serial_sendCB cb)
{
	sTlk_hci_serial_sendCB = cb;
}
void tlkdev_hciuart_tx_rest_regCB(tlk_hci_serial_tx_reset_sendCB cb)
{
	sTlk_hci_serial_tx_reset_sendCB = cb;
}

void tlkdev_hciuart_set_baudrate_regCB(tlk_hci_cmd_vendor_CB cb)
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

int tlkdev_hciuart_init(void)
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

    tlkdev_hciuart_regCB(tlkdev_hciuart_send);
    tlkdev_hciuart_tx_rest_regCB(uart_tx_reset_init);
    tlkdev_hciuart_set_baudrate_regCB((tlk_hci_cmd_vendor_CB)tlkdev_hciuart_setBaudrate);
    tlkdev_standby_regCB(tlkdev_standby_set);

	return TLK_ENONE;
}


int tlkdev_hciuart_setBaudrate(uint32 baudrate)
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

void tlkdev_hciuart_handler(void)
{
	tlkdev_hciuart_recvHandler();
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
	else{
		return 0;
	}
}
_attribute_ram_code_sec_ static void tlkdev_hciuart_recvHandler(void)
{
	if(is_uart_rx_done(UART_ID)){

		uint32 read_dma_size;
		uint08  read_buf_cnt;

		if(!uart_dmabuf_uartbuf_process(&read_dma_size,&read_buf_cnt)){
			UART_MANUAL_FLOW_CTR_RTS_START;
		    return;
		}
		uint08 *p = pending_dma_buf;
		uint16 dma_offset = PENDING_BUF_LEN - read_dma_size*4;
		uint16 rx_len = 0 ;
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
uint08 tlkdev_hciuart_send_flag = 0;
uint08 tlkdev_hciuart_tick = 0;
uint08 tlkdev_hciuart_send(uint08 * addr)
{
	if(tlkdev_hciuart_send_flag == 1){
		if(clock_time_exceed(tlkdev_hciuart_tick,SERIAL_BUSY_TIMEOUT)){
			uart_tx_reset_init();
			tlkdev_hciuart_send_flag = 0;
			my_dump_str_u32s(TLKAPP_HCI_UART_MODE,"tlkdev_hciuart_send",tlkdev_hciuart_send_flag,0,0,0);

		}
		else return 1;
	}
	int len = addr[0]+addr[1]*256;
	uart_send_dma(UART_ID,addr+4,len);
	tlkdev_hciuart_tick = clock_time();
	tlkdev_hciuart_send_flag = 1;
	return 0;
}
_attribute_ram_code_sec_ void UART_IRQHandler(void)
{
    if(uart_get_irq_status(UART_ID,UART_TXDONE))
	{
    	tlkdev_hciuart_send_flag = 0;
	    uart_clr_tx_done(UART_ID);

	}
}


#endif

