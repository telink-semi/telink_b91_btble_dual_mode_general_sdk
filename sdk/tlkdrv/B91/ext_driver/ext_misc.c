/********************************************************************************************************
 * @file     ext_misc.c
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

#include "../timer.h"
#include "../trng.h"
#include "../sys.h"
#include "../stimer.h"
#include "../clock.h"
#include "../uart.h"
#include "ext_misc.h"

extern int rand(void);

/******************************* stimer_start ******************************************************************/
_attribute_retention_code_ unsigned int get_u32_safe (unsigned int v)
{
	return v;
}
/******************************* stimer_end ********************************************************************/


/******************************* timer_start ******************************************************************/
_attribute_data_retention_sec_	hw_timer_t		hw_timer_ctl;

/******************************* timer_end ********************************************************************/



/******************************* trng_start ******************************************************************/
_attribute_no_inline_
void generateRandomNum(int len, unsigned char *data)
{
	int i;
	unsigned int randNums = 0;
    /* if len is odd */
	for (i=0; i<len; i++ ) {
		if( (i & 3) == 0 ){
			randNums = trng_rand();
		}

		data[i] = randNums & 0xff;
		randNums >>=8;
	}
}
/******************************* trng_end ********************************************************************/



/******************************* flash_start *****************************************************************/
_attribute_data_retention_sec_   _attribute_aligned_(4)	Flash_CapacityDef	flash_capacity;

void flash_set_capacity(Flash_CapacityDef flash_cap)
{
	flash_capacity = flash_cap;
}

Flash_CapacityDef flash_get_capacity(void)
{
	return flash_capacity;
}
/******************************* flash_end *****************************************************************/





/******************************* uart_start *****************************************************************/
extern unsigned char uart_dma_tx_chn[2];
extern unsigned char uart_dma_rx_chn[2];
extern dma_config_t uart_tx_dma_config[2];
/**
* @brief       This function serves to receive data function by DMA, this  function tell the DMA to get data from the uart data fifo.
* @param[in]   uart_num - UART0 or UART1.
* @param[in]   addr     - pointer to the buffer  receive data.
* @param[in]   rev_size - the receive length of DMA,The maximum transmission length of DMA is 0xFFFFFC bytes, so dont'n over this length.
* @note        The DMA version of A0 has some limitians.
*              0:We should know the real receive length-len.
*              1:If the data length we receive isn't the multiple of 4(the DMA carry 4-byte one time),like 5,it will carry 8 byte,
*                while the last 3-byte data is random.
*              2:The receive buff length sholud be equal to rec_size.The relation of the receive buff length and rec_size and
*                the real receive data length-len : REC_BUFF_LEN=rec_size= ((len%4)==0 ? len : ((len/4)+1)*4).
*              The DMA version of A1 can receive any length of data,the rev_size is useless.
* @return      none
*/
void uart_receive_dma_buffer_set(uart_num_e uart_num, unsigned char * addr, unsigned int len)
{
	dma_chn_dis(uart_dma_rx_chn[uart_num]);
	/*In order to be able to receive data of unknown length(A0 doesn't suppport),the DMA SIZE is set to the longest value 0xffffffff.After entering suspend and wake up, and then continue to receive,
	DMA will no longer move data from uart fifo, because DMA thinks that the last transmission was not completed and must disable dma_chn first.modified by minghai,confirmed qiangkai 2020.11.26.*/
	dma_set_address(uart_dma_rx_chn[uart_num],reg_uart_data_buf_adr(uart_num),(unsigned int)convert_ram_addr_cpu2bus(addr));

	reg_dma_size(uart_dma_rx_chn[uart_num])=len;


	dma_chn_en(uart_dma_rx_chn[uart_num]);
}

void uart0_init(unsigned int baudrate)
{
	uart_reset(UART0);  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset
	uart_set_pin(UART0_TX_PB2, UART0_RX_PB3);
	unsigned short div;
	unsigned char bwpc;
	uart_cal_div_and_bwpc(baudrate, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(UART0, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);
}
/******************************* uart_end *******************************************************************/



