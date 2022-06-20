/********************************************************************************************************
 * @file     dcd_tlsr9x1x_usb_msc.c
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

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 William D. Jones
 * Copyright (c) 2019-2020 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "../../../tusb_option.h"


#include "drivers.h"
#include "../../../device/dcd.h"
#include "../../../common/tusb_types.h"
/*------------------------------------------------------------------*/
/* MACRO TYPEDEF CONSTANT ENUM
 *------------------------------------------------------------------*/


/* Telink USB has nine endpoints, endpoint 0 and endpoint 1 to 8. Except endpoint 0 (support IN and OUT),
each endpoint can only be either an IN endpoint or an OUT endpoint (Endpoint 5 and endpoint 6 can only
be IN endpoints, other endpoints can only be OUT endpoints.). Endpoint 0 can only use the control transfer,
endpoint 1 to 8 supports other three transfer types except the control transfer, endpoint 6 and endpoint 7
uses the isochronous transfer by default */
//0x01
//0x02
//0x03
//0x04
//0x85
//0x86
//0x07
//0x08

//fifo 1 2 3 6 7 4 8 5
// Xfer control
typedef struct
{
  uint8_t * buffer;
  // tu_fifo_t * ff; // TODO support dcd_edpt_xfer_fifo API
  uint16_t total_len;
  uint16_t queued_len;
  uint16_t max_size;
  bool isochronous;
  bool short_packet;
} xfer_ctl_t;
/* reset by dcd_init(), this is used by dcd_edpt_open() to assign USBD peripheral buffer addresses */
static uint8_t bufseg_addr;
xfer_ctl_t xfer_status[8][2];
#define XFER_CTL_BASE(_ep, _dir) &xfer_status[_ep][_dir]

// Accessing endpoint regs
typedef volatile uint8_t * ep_regs_t;


/*------------------------------------------------------------------*/
/* Controller API
 *------------------------------------------------------------------*/
void dcd_init ()
{
	
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC|FLD_CTRL_EP_AUTO_CFG);//"FLD_CTRL_EP_AUTO_CFG" manual enable,mean that software to handle set_configuration control transmission
	usbhw_set_eps_en(FLD_USB_EDP8_EN|FLD_USB_EDP1_EN|FLD_USB_EDP2_EN|FLD_USB_EDP3_EN|FLD_USB_EDP4_EN|FLD_USB_EDP5_EN|FLD_USB_EDP6_EN|FLD_USB_EDP7_EN);
	//usbhw_set_eps_irq_mask(1<<(rhport&0x07));
	usb_set_pin_en();
	usb_dp_pullup_en (0);
}

// There is no "USB peripheral interrupt disable" bit on MSP430, so we have
// to save the relevant registers individually.
// WARNING: Unlike the ARM/NVIC routines, these functions are _not_ idempotent
// if you modified the registers saved in between calls so they don't match
// the mirrors; mirrors will be updated to reflect most recent register
// contents.
void dcd_int_enable ()
{
	plic_interrupt_enable(IRQ11_USB_ENDPOINT);	
}

void dcd_int_disable ()
{
  plic_interrupt_disable(IRQ11_USB_ENDPOINT);	
}

/*------------------------------------------------------------------*/
/* DCD Endpoint port
 *------------------------------------------------------------------*/

bool dcd_edpt_open (uint8_t rhport, tusb_desc_endpoint_t const * desc_edpt)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(desc_edpt->bEndpointAddress);
  uint8_t const dir   = tu_edpt_dir(desc_edpt->bEndpointAddress);

  xfer_ctl_t * xfer = XFER_CTL_BASE(epnum, dir);
  xfer->max_size = desc_edpt->wMaxPacketSize.size;
  
  reg_usb_ep_buf_addr(epnum) = bufseg_addr;
  bufseg_addr += xfer->max_size;
  BM_SET(reg_usb_edp_en,BIT(epnum&0x07));
  if(desc_edpt->bmAttributes.xfer == 1)
  {
	  BM_SET(reg_usb_iso_mode,BIT(epnum&0x07));
    xfer->isochronous = 1;
  }
  else
  {
	  BM_CLR(reg_usb_iso_mode,BIT(epnum&0x07));
    xfer->isochronous = 0;
  }
	  // Buffer allocation scheme:

  // For simplicity, only single buffer for now, since tinyusb currently waits
  // for an xfer to complete before scheduling another one. This means only
  // the X buffer is used.
  //
  // 1904 bytes are available, the max endpoint size supported on msp430 is
  // 64 bytes. This is enough RAM for all 14 endpoints enabled _with_ double
  // bufferring (64*14*2 = 1792 bytes). Extra RAM exists for triple and higher
  // order bufferring, which must be maintained in software.
  //
  // For simplicity, each endpoint gets a hardcoded 64 byte chunk (regardless
  // of actual wMaxPacketSize) whose start address is the following:
  // addr = 128 * (epnum - 1) + 64 * dir.
  //
  // Double buffering equation:
  // x_addr = 256 * (epnum - 1) + 128 * dir
  // y_addr = x_addr + 64
  // Address is right-shifted by 3 to fit into 8 bits.

  // IN and OUT EP registers have the same structure.

  // FIXME: I was able to get into a situation where OUT EP 3 would stall
  // while debugging, despite stall code never being called. It appears
  // these registers don't get cleared on reset, being part of RAM.
  // Investigate and see if I can duplicate.
  // Also, DBUF got set on OUT EP 2 while debugging. Only OUT EPs seem to be
  // affected at this time. USB RAM directly precedes main RAM; perhaps I'm
  // overwriting registers via buffer overflow w/ my debugging code?

  return true;
}
static void transmit_packet(uint8_t ep_num);
bool dcd_edpt_xfer (uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
  (void) rhport;

  volatile uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);

  xfer_ctl_t * xfer = XFER_CTL_BASE(epnum, dir);
  xfer->buffer = buffer;
  xfer->total_len = total_bytes;
  xfer->queued_len = 0;
  xfer->short_packet = false;

   if(dir == TUSB_DIR_IN)
	   transmit_packet(1);
   if(dir == TUSB_DIR_OUT)
   {
	 //  printf("usbhw_data_ep_ack = %d,",epnum);
	   usbhw_data_ep_ack(epnum);
   }
   dcd_int_enable();
	   //transmit_packet(6);
//    {
//      usbhw_data_ep_ack(epnum);
//      printf("usbhw_data_ep_ack = %d\n\r",epnum);
//      usbhw_clr_eps_irq(FLD_USB_EDP6_IRQ);
//   }
//     else
//     {
    	// 	 usbhw_data_ep_ack(epnum);
//    	       printf("usbhw_data_ep_ack = %d\n\r",epnum);
//    	       usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
//     }
   // else
    //{
   //   usbhw_set_eps_irq_mask(1<<(epnum&0x07));
   // }

  return true;
}

void dcd_edpt_stall (uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(ep_addr);
    usbhw_data_ep_stall(epnum);
}

void dcd_edpt_clear_stall (uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(ep_addr);

    reg_usb_ep_ctrl(epnum) = 0;
}

void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request)
{
  (void) rhport;
  (void) request;

  // FIXME: Per manual, we should be clearing the NAK bits of EP0 after the
  // Status Phase of a control xfer is done, in preparation of another possible
  // SETUP packet. However, from my own testing, SETUP packets _are_ correctly
  // handled by the USB core without clearing the NAKs.
  //
  // Right now, clearing NAKs in this callbacks causes a direction mismatch
  // between host and device on EP0. Figure out why and come back to this.
  // USBOEPCNT_0 &= ~NAK;
  // USBIEPCNT_0 &= ~NAK;
}

/*------------------------------------------------------------------*/

static void receive_packet(uint8_t ep_num)
{
  xfer_ctl_t * xfer = XFER_CTL_BASE(ep_num, TUSB_DIR_OUT);
  uint8_t xfer_size;

    xfer_size =reg_usb_ep_ptr(ep_num);

  uint16_t remaining = xfer->total_len - xfer->queued_len;
  uint16_t to_recv_size;

  if(remaining <= xfer->max_size) {
    // Avoid buffer overflow.
    to_recv_size = (xfer_size > remaining) ? remaining : xfer_size;
  } else {
    // Room for full packet, choose recv_size based on what the microcontroller
    // claims.
    to_recv_size = (xfer_size > xfer->max_size) ? xfer->max_size : xfer_size;
  }

    uint8_t * base = (xfer->buffer + xfer->queued_len);

    usbhw_reset_ep_ptr(ep_num);
      for(uint16_t i = 0; i < to_recv_size ; i++)
      {
        base[i] = reg_usb_ep_dat(ep_num);
      }

  xfer->queued_len += xfer_size;

  xfer->short_packet = (xfer_size < xfer->max_size);
  if((xfer->total_len == xfer->queued_len) || xfer->short_packet)
  {
    dcd_event_xfer_complete(0, ep_num, xfer->queued_len, XFER_RESULT_SUCCESS, true);
    dcd_int_disable();
  }
  else
  {
		usbhw_data_ep_ack(ep_num);
  }
}

static void transmit_packet(uint8_t ep_num)
{
  xfer_ctl_t * xfer = XFER_CTL_BASE(ep_num, TUSB_DIR_IN);

  // First, determine whether we should even send a packet or finish
  // up the xfer.
  bool zlp = (xfer->total_len == 0); // By necessity, xfer->total_len will
                                     // equal xfer->queued_len for ZLPs.
                                     // Of course a ZLP is a short packet.
  if((!zlp && (xfer->total_len == xfer->queued_len)) || xfer->short_packet)
  {
    dcd_event_xfer_complete(0, ep_num | TUSB_DIR_IN_MASK, xfer->queued_len, XFER_RESULT_SUCCESS, true);
    dcd_int_disable();
    return;
  }

  // Then actually commit to transmit a packet.
  uint16_t remaining = xfer->total_len - xfer->queued_len;
  uint8_t xfer_size = (xfer->max_size < xfer->total_len) ? xfer->max_size : remaining;


  if(xfer_size < xfer->max_size)
  {
    // Next "xfer complete interrupt", the transfer will end.
    xfer->short_packet = true;
  }


  uint8_t * base = (xfer->buffer + xfer->queued_len);
  xfer->queued_len += xfer_size;
  usbhw_reset_ep_ptr(ep_num);
  for(int i = 0; i < xfer_size; i++)
  {
    reg_usb_ep_dat(ep_num) = base[i];
  }
  usbhw_data_ep_ack(ep_num);

}

void dcd_int_handler(uint8_t rhport)
{
  (void) rhport;
  uint8_t irq = reg_usb_ep_irq_status;

	if(irq & FLD_USB_EDP5_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP5_IRQ);
		receive_packet(5);

	 }
	if(irq & FLD_USB_EDP6_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP6_IRQ);
		 receive_packet(6);

	 }
	if(irq & FLD_USB_EDP8_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP8_IRQ);
		transmit_packet(8);

	 }
	if(irq & FLD_USB_EDP7_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP7_IRQ);
		transmit_packet(7);

	 }
  	if(irq & FLD_USB_EDP4_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP4_IRQ);
  		transmit_packet(4);
	 }
	if(irq & FLD_USB_EDP3_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP3_IRQ);
		transmit_packet(3);
	}
	if(irq & FLD_USB_EDP2_IRQ) 
	 {
		 usbhw_clr_eps_irq(FLD_USB_EDP2_IRQ);
		transmit_packet(2);
	 }
  	if(irq & FLD_USB_EDP1_IRQ) 
	 {
  		usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
  		transmit_packet(1);
	 }
}
