/********************************************************************************************************
 * @file     tlkstk_myudb.h
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

#ifndef TLKSTK_MYUDB_H
#define TLKSTK_MYUDB_H


#include "app/app_config.h"
#include "tlkstk_logdef.h"
#include "tlkdrv/B91/reg_include/usb_reg.h"

#define TLKSTK_MYUDB_DEBUG_ENABLE             0
#define TLKSTK_MYUDB_CTRL_DEBUG_ENABLE        (1 && TLKSTK_MYUDB_DEBUG_ENABLE)


#if (TLKSTK_MYUDB_DEBUG_ENABLE)
#define	usb_send_str(s)	tlkapi_debug_sendData(s, 0, 0)
#define	usb_send_data(p,n) tlkapi_debug_sendData(0,p,n)
#define my_dump_str_data(en,s,p,n)		if(en){tlkapi_debug_sendData(s,(u8*)(p),n);}
#define my_dump_str_u32s(en,s,d0,d1,d2,d3)		if(en){tlkapi_debug_sendU32s(s,(u32)(d0),(u32)(d1),(u32)(d2),(u32)(d3));}
#define my_uart_send_str_data			tlkapi_debug_sendData
#else
#define	usb_send_str(s)
#define	usb_send_data(p,n)
#define my_dump_str_data(en,s,p,n)
#define my_dump_str_u32s(en,s,d0,d1,d2,d3)
#define my_uart_send_str_data
#endif



#ifndef			VCD_EN
#define			VCD_EN						(1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef			VCD_BLE_EN
#define			VCD_BLE_EN					(0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif



#define			SL_STACK_VCD_EN				1
/* Timing VCD id enable */
#define			SL_STACK_TIMING_EN			0
#define			SL_STACK_RESET_EN			0
/* schedule VCD id enable */
#define			SL_STACK_SCH01_EN			1
#define			SL_STACK_SCH02_EN			0
/* frame VCD id enable */
#define			SL_STACK_FRAME_EN			1
#define			SL_STACK_FRAME_ST_EN		1
#define			SL_STACK_RADIO_EN			0
#define			SL_STACK_FLOW_EN			1
/* access link VCD id enable */
#define			SL_STACK_INQ_EN				1
#define			SL_STACK_INQSCAN_EN			1
#define			SL_STACK_PAGE_EN			1
#define			SL_STACK_PAGESCAN_EN		1

/* acl-c link VCD id enable */
#define			SL_STACK_ACL_EN				1
#define			SL_STACK_LMP_EN				1
#define			SL_STACK_CON_EN				0
#define			SL_STACK_RSW_EN				0
#define			SL_STACK_SNIFF_EN			0
#define			SL_STACK_AFH_EN             0
#define			SL_STACK_AFH_CLS_EN         0
#define			SL_STACK_DETACH_EN			0

/* sco/e-sco link VCD id enable */
#define			SL_STACK_SCO_EN				0
/* authen/pair/encrypt VCD id enable */
#define			SL_STACK_AUTH_EN			0
#define			SL_STACK_PAIR_EN			0
#define			SL_STACK_ENCRYPT_EN			0
/* PA VCD id enable */
#define			SL_STACK_PA_EN				0
#define			SL_STACK_TEST_EN			0
#define			SL_STACK_HCI_EN			    0
/* application VCD id enable */
#define			SL_APP_MUSIC_EN				0
#define			SL_APP_MUSIC_02_EN			0
#define			SL_APP_AUDIO_EN				0
#define			SL_APP_EC_EN				0
#define			SL_APP_SP_EN				0






extern void tlkapi_debug_sendData(char *pStr, uint08 *pData, uint16 dataLen);
extern void tlkapi_debug_sendU32s(void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);





///////////////////////////////////////////////////////////////////////////////////////
#define			GLOBAL_INT_DISABLE()		u32 rie = core_disable_interrupt ()
#define			GLOBAL_INT_RESTORE()		core_restore_interrupt(rie)
#define			LOG_EVENT_TIMESTAMP		0
#define			LOG_DATA_B1_0			0
#define			LOG_DATA_B1_1			1

#define			get_systemtick()  	    stimer_get_tick()


#if(VCD_MODE)
#define			log_uart(d)				uart_send_byte_dma(1,d)
#define         DEBUG_PORT				GPIO_PE2
#define			log_ref_gpio_h()		gpio_set_high_level(DEBUG_PORT)
#define			log_ref_gpio_l()		gpio_set_low_level(DEBUG_PORT)
#else
#define			log_uart(d)				reg_usb_ep8_dat=d
#define			log_ref_gpio_h()
#define			log_ref_gpio_l()
#endif




#define	log_hw_ref()	if(VCD_EN){GLOBAL_INT_DISABLE();log_ref_gpio_h();log_uart(0x20);int t=stimer_get_tick();log_uart(t);log_uart(t>>8);log_uart(t>>16);log_ref_gpio_l();GLOBAL_INT_RESTORE();}

// 4-byte sync word: 00 00 00 00
#define	log_sync(en)	if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0);log_uart(0);log_uart(0);log_uart(0);GLOBAL_INT_RESTORE();}
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
#define	log_tick(en,id)	if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0x20|(id&31));int t=stimer_get_tick();log_uart(t);log_uart(t>>8);log_uart(t>>16);GLOBAL_INT_RESTORE();}

//1-byte (000_id-5bits)
#define	log_event(en,id) if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0x00|(id&31));GLOBAL_INT_RESTORE();}

//1-byte (01x_id-5bits) 1-bit data: id0 & id1 reserved for hardware
#define	log_task(en,id,b)	if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(((b)?0x60:0x40)|(id&31));int t=stimer_get_tick();log_uart(t);log_uart(t>>8);log_uart(t>>16);GLOBAL_INT_RESTORE();}

//2-byte (10-id-6bits) 8-bit data
#define	log_b8(en,id,d)	if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0x80|(id&63));log_uart(d);GLOBAL_INT_RESTORE();}

//3-byte (11-id-6bits) 16-bit data
#define	log_b16(en,id,d) if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0xc0|(id&63));log_uart(d);log_uart((d)>>8);GLOBAL_INT_RESTORE();}


//BLE used only //
#define	log_sync_mainloop(en)	if(VCD_BLE_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0);log_uart(0);log_uart(0);log_uart(0);GLOBAL_INT_RESTORE();}

#define	log_tick_mainloop(en,id) if(VCD_EN&&en) {GLOBAL_INT_DISABLE();log_uart(0x20|(id&31));int t=stimer_get_tick();log_uart(t);log_uart(t>>8);log_uart(t>>16);GLOBAL_INT_RESTORE();}

#define	log_tick_irq(en,id)		if(VCD_BLE_EN&&en) {log_uart(0x20|(id&31));int t=stimer_get_tick();log_uart(t);log_uart(t>>8);log_uart(t>>16);}

#define	log_event_irq(en,id) 	if(VCD_BLE_EN&&en) {log_uart(0x00|(id&31));}

#define	log_task_irq(en,id,b)	if(VCD_BLE_EN&&en) {log_uart(((b)?0x60:0x40)|(id&31));int t=stimer_get_tick();log_uart(t);log_uart(t>>8);log_uart(t>>16);}

#define	log_b8_irq(en,id,d)		if(VCD_BLE_EN&&en) {log_uart(0x80|(id&63));log_uart(d);}

#define	log_b16_irq(en,id,d)	if(VCD_BLE_EN&&en) {log_uart(0xc0|(id&63));log_uart(d);log_uart((d)>>8);}


#ifndef	DUMP_BLE_MSG
#define DUMP_BLE_MSG     (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_ACL_MSG
#define DUMP_ACL_MSG     (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_HCI_MSG
#define DUMP_HCI_MSG     (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_LMP_MSG
#define DUMP_LMP_MSG     (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_EVT_MSG
#define DUMP_EVT_MSG     (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_APP_MSG
#define DUMP_APP_MSG     (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_ENC_MSG
#define DUMP_ENC_MSG     (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_AUTH_MSG
#define DUMP_AUTH_MSG    (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_PAIR_MSG
#define DUMP_PAIR_MSG    (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_RSW_MSG
#define DUMP_RSW_MSG     (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif


#ifndef	DUMP_QOS_MSG
#define DUMP_QOS_MSG     (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_AFH_MSG
#define DUMP_AFH_MSG     (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_NAME_MSG
#define DUMP_NAME_MSG    (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif


#ifndef	DUMP_PAGE_MSG
#define DUMP_PAGE_MSG    (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif


#ifndef	DUMP_DETACH_MSG
#define DUMP_DETACH_MSG    (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif


#ifndef	DUMP_TIMER_MSG
#define DUMP_TIMER_MSG    (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_RESET_CORE_MSG
#define DUMP_RESET_CORE_MSG    (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_SNIFF_MSG
#define DUMP_SNIFF_MSG    (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_SCO_MSG
#define DUMP_SCO_MSG     (0 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif

#ifndef	DUMP_TASK_MSG
#define DUMP_TASK_MSG    (1 && TLKSTK_MYUDB_CTRL_DEBUG_ENABLE)
#endif


#endif //TLKSTK_MYUDB_H
