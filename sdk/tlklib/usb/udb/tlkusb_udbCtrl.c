/********************************************************************************************************
 * @file     tlkusb_desc.c
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLKUSB_UDB_ENABLE)
#include "drivers.h"
#include "tlklib/usb/udb/tlkusb_udbDefine.h"
#include "tlklib/usb/udb/tlkusb_udb.h"
#include "tlklib/usb/udb/tlkusb_udbDesc.h"
#include "tlklib/usb/udb/tlkusb_udbCtrl.h"

#include "tlkstk/inner/tlkstk_myudb.h"



extern void tlkusb_udb_recvHander(void);

static int tlkusb_udbctrl_init(void);
static void tlkusb_udbctrl_reset(void);
static void tlkusb_udbctrl_deinit(void);
static void tlkusb_udbctrl_handler(void);

const tlkusb_modCtrl_t sTlkUsbUdbModCtrl = {
	tlkusb_udbctrl_init, //Init
	tlkusb_udbctrl_reset, //Reset
	tlkusb_udbctrl_deinit, //Deinit
	tlkusb_udbctrl_handler, //Handler
	nullptr, //GetClassInf
	nullptr, //SetClassInf
	nullptr, //GetClassEdp
	nullptr, //SetClassEdp
	nullptr, //GetInterface
	nullptr, //SetInterface
};
#if (TLKUSB_UDB_VCD_ENABLE)
static uint32 sTlkUsbUdbVcdTicks;
#endif



static int tlkusb_udbctrl_init(void)
{
	reg_usb_ep_buf_addr(TLKUSB_UDB_EDP_DBG_IN) = 0x80;
	reg_usb_ep_buf_addr(TLKUSB_UDB_EDP_DBG_OUT) = 0xC0;
	#if (TLKUSB_UDB_VCD_ENABLE)
	reg_usb_ep_max_size = (128 >> 2);
	reg_usb_ep8_send_thre = 0x40;
	reg_usb_ep8_send_max = 128 >> 3;
	reg_usb_ep_buf_addr(TLKUSB_UDB_EDP_VCD_IN) = 0;
	reg_usb_ep8_fifo_mode = 1;
	#endif

	sTlkUsbUdbVcdTicks = 0;

	reg_usb_mdev &= ~BIT(3);			//vendor command: bRequest[7] = 0
	
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC);
	reg_usb_ep_ctrl(TLKUSB_UDB_EDP_DBG_OUT) = FLD_EP_DAT_ACK;
	
	return TLK_ENONE;
}
static void tlkusb_udbctrl_reset(void)
{
	reg_usb_ep_ctrl(TLKUSB_UDB_EDP_DBG_OUT) = FLD_EP_DAT_ACK;
	
}

static void tlkusb_udbctrl_deinit(void)
{
	
}

static void tlkusb_udbctrl_handler(void)
{
	#if (TLKUSB_UDB_VCD_ENABLE)
	if(sTlkUsbUdbVcdTicks == 0 || clock_time_exceed(sTlkUsbUdbVcdTicks, 10000)){
		sTlkUsbUdbVcdTicks = clock_time() | 1;
		log_sync(1); //SL_STACK_VCD_EN
	}
	#endif
//	tlkusb_udb_sendHander();
	tlkusb_udb_recvHander();
}


#endif

