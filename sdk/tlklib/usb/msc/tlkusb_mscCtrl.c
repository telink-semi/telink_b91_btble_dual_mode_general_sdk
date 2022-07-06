/********************************************************************************************************
 * @file     tlkusb_mscCtrl.c
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
#if (TLKUSB_MSC_ENABLE)
#include "drivers.h"
#include "tlkdev/ext/xtx/tlkdev_xt2602e.h"
#include "tlklib/usb/msc/tlkusb_mscDefine.h"
#include "tlklib/usb/msc/tlkusb_msc.h"
#include "tlklib/usb/msc/tlkusb_mscDesc.h"
#include "tlklib/usb/msc/tlkusb_mscCtrl.h"

extern void nand_flash_fat_init(int reset);
extern void usb_mass_storage_init(void);
extern void tlkusb_core_handler(void);
extern void mass_storage_task(void);

extern bool tusb_init(void);
extern void tud_task (void);

static int  tlkusb_mscctrl_init(void);
static void tlkusb_mscctrl_reset(void);
static void tlkusb_mscctrl_deinit(void);
static void tlkusb_mscctrl_handler(void);
static int  tlkusb_mscctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_mscctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_mscctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_mscctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_mscctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_mscctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);

const tlkusb_modCtrl_t sTlkUsbMscModCtrl = {
	tlkusb_mscctrl_init, //Init
	tlkusb_mscctrl_reset, //Reset
	tlkusb_mscctrl_deinit, //Deinit
	tlkusb_mscctrl_handler, //Handler
	tlkusb_mscctrl_getClassInf, //GetClassInf
	tlkusb_mscctrl_setClassInf, //SetClassInf
	tlkusb_mscctrl_getClassEdp, //GetClassEdp
	tlkusb_mscctrl_setClassEdp, //SetClassEdp
	tlkusb_mscctrl_getInterface, //GetInterface
	tlkusb_mscctrl_setInterface, //SetInterface
};



static int tlkusb_mscctrl_init(void)
{
	tlkapi_chip_switchClock(TLKAPI_CHIP_CLOCK_96M);

	core_disable_interrupt();
	
	reg_usb_ep_buf_addr(TLKUSB_MSC_EDP_IN) = 0x80;
	reg_usb_ep_buf_addr(TLKUSB_MSC_EDP_OUT) = 0xC0;
	
	#if (TLK_DEV_XT2602E_ENABLE)
	nand_flash_fat_init(0);
	tlkdev_xt2602e_init();
	usb_mass_storage_init();
	#elif (TLK_DEV_XTSD04G_ENABLE)
	tusb_init();
	#endif

	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC | FLD_CTRL_EP_AUTO_INTF);
	reg_usb_ep_max_size = 8;

	plic_interrupt_disable(IRQ15_ZB_RT);
	plic_interrupt_disable(IRQ12_ZB_DM);
	plic_interrupt_disable(IRQ1_SYSTIMER);
	plic_interrupt_disable(IRQ14_ZB_BT);
	plic_interrupt_disable(IRQ13_ZB_BLE);
	plic_interrupt_disable(IRQ3_TIMER1);
	plic_interrupt_disable(IRQ4_TIMER0);
	plic_interrupt_disable(IRQ19_UART0);
	plic_interrupt_disable(IRQ18_UART1);
	plic_interrupt_disable(IRQ2_ALG);
	plic_interrupt_disable(IRQ25_GPIO);
	
	plic_interrupt_enable(IRQ11_USB_ENDPOINT);
	plic_set_priority(IRQ11_USB_ENDPOINT, 3);
	core_enable_interrupt();

	reg_usb_ep_ctrl(TLKUSB_MSC_EDP_OUT) = FLD_EP_DAT_ACK;
	
	return TLK_ENONE;
}
static void tlkusb_mscctrl_reset(void)
{
	reg_usb_ep_ctrl(TLKUSB_MSC_EDP_OUT) = FLD_EP_DAT_ACK;
}
static void tlkusb_mscctrl_deinit(void)
{
	plic_interrupt_disable(IRQ11_USB_ENDPOINT);
	
}
static void tlkusb_mscctrl_handler(void)
{
	#if (TLK_DEV_XT2602E_ENABLE)
	mass_storage_task();
	#elif (TLK_DEV_XTSD04G_ENABLE)
	tud_task();
	#endif

//	tlkusb_core_handler();
}

static int tlkusb_mscctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	
	return -TLK_ENOSUPPORT;
}
static int tlkusb_mscctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	
	return -TLK_ENOSUPPORT;
}
static int tlkusb_mscctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_mscctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_mscctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
//	uint08 infNum = (pSetup->wIndex) & 0x07;
//	usbhw_write_ctrl_ep_data(sTlkUsbAudAltInf[infNum]);
	return TLK_ENONE;
}
static int tlkusb_mscctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
//	uint08 enable = (pSetup->wValue) & 0xff;
//	uint08 infNum = (pSetup->wIndex) & 0x07;
//	if(infNum == TLKUSB_AUD_INF_MIC){
//		sTlkUsbAudAltInf[infNum] = enable;
//		if(enable){
//			reg_usb_ep_ptr(TLKUSB_MSC_EDP_IN) = 0;
//			reg_usb_ep_ctrl(TLKUSB_MSC_EDP_IN) = BIT(0); //ACK first packet
//		}
//	}
	return TLK_ENONE;
}







#endif //#if (TLKUSB_MSC_ENABLE)


