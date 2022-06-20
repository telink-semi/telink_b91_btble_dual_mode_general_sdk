/********************************************************************************************************
 * @file     btble_scheduler.h
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

#ifndef _BT_LL_SCHEDULER_H_
#define _BT_LL_SCHEDULER_H_

#include "tlkstk/inner/tlkstk_inner.h"

uint32_t bt_ll_system_tick_bt_acl_isr(uint8_t sequence, uint8_t link);
uint32_t bt_ll_system_tick_access_isr(uint8_t sequence, uint8_t link);

#ifndef	MAX_BLE_LINK
#define	MAX_BLE_LINK				3
#endif

enum {

	ID_LINK_ACCESS					= 	MAX_BT_ACL_LINK,
	ID_LINK_BLE						=	(MAX_BT_ACL_LINK + 1),				ID_LINK_BLE_ADV	= ID_LINK_BLE,
	ID_LINK_BLE_CONN				=	(MAX_BT_ACL_LINK + 2),
	MAX_SCHEDULE_LINK				=	(ID_LINK_BLE + MAX_BLE_LINK),

	FLAG_SCHEDULE_LINK_ID_NULL		=	0x80,
	FLAG_SCHEDULE_LINK_ID_CHANGE	=	0x40,
	SCHEDULER_LATENCY_DEFAULT		=	2500,		// 1250 + 625 + 25

	FLAG_SCHEDULE_START				=	0,
	FLAG_SCHEDULE_DONE				=	0x80,
	FLAG_SCHEDULE_CHANGE			=	0x81,
	FLAG_SCHEDULE_EXPIRE			=	0xC0,  //add by SiHui, now only used for standard BLE
	FLAG_SCHEDULE_RF_END			=	0xE0,  //add by SiHui, now only used for standard BLE

	FLAG_SCHEDULE_TASK_DISABLE		=	1,

	SCHEDULER_TIME_FLEXIBLE			=	0x00,
	SCHEDULER_TIME_FIX				=	0x01,

	SCHEDULER_INTERVAL_7MS5_0		=	0,
	SCHEDULER_INTERVAL_7MS5_1		=	1,
	SCHEDULER_INTERVAL_15MS_0		=	2,
	SCHEDULER_INTERVAL_15MS_1		=	3,

	SCHEDULER_PRIORITY_SNIFF		=	0x01,
	SCHEDULER_PRIORITY_PAGETOACL	=	0x10,
	SCHEDULER_PRIORITY_ROLESWITCH	=	0x10,
	SCHEDULER_PRIORITY_SCO			=	0x20,
	
	SCHEDULER_PRIORITY_BLE			=	0x30,

	SCHEDULER_PRIORITY_BLE_SYNC		=	0x08,  //when BLE connection create & update, need high priority
	SCHEDULER_PRIORITY_BLE_EXTEND	=	0x48,  //when BLE connection extend timing, priority between BT ACL normal and BT ACL voice task
	SCHEDULER_PRIORITY_ADV			=	0x40,

	SCHEDULER_PRIORITY_INQUIRY		=	0x50,
	SCHEDULER_PRIORITY_PAGE			=	0x50,
	SCHEDULER_PRIORITY_INQUIRYSCAN	=	0x50,
	SCHEDULER_PRIORITY_PAGESCAN		=	0x50,
	SCHEDULER_PRIORITY_ACL			=	0x50,
	SCHEDULER_PRIORITY_ACL_URGENT   =	0x40,  //add by ZhangJian for SiHui's request: ACL with voice task set by BT application
	SCHEDULER_PRIORITY_SCAN_LOW		=	0x60,

	SCHEDULER_PRIORITY_LOWEST		=	0xff,

	SCHEDULER_MODE_STANDARD			=	0x00,
	SCHEDULER_MODE_TWS				=	0x01,

	SCHEDULER_LOWPOWER_IDLE				= 	0x0,
	SCHEDULER_LOWPOWER_ENABLE			= 	0x80,
	SCHEDULER_LOWPOWER_ENABLE_BT		= 	0x40,
	SCHEDULER_LOWPOWER_ENABLE_BLE		= 	0x20,
	SCHEDULER_LOWPOWER_BUSY				= 	0x01,
	SCHEDULER_LOWPOWER_EXIT				= 	0x04,
	SCHEDULER_LOWPOWER_MASK				= 	0x70,
};

/* For example, ID_LINK_BLE is 4, MAX_SCHEDULE_LINK is 7, ble_task_mask should be BIT(4~6)
 * (2<<7) - 1: BIT(0~6)
 * (2<<4) - 1: BIT(0~3)
 * BIT(4~6) = BIT(0~6) -  BIT(0~3) = (2<<7) - (2<<4)
 */
#define SCHEDULER_TASK_BLE_MASK		((2<<MAX_SCHEDULE_LINK) - (2<<ID_LINK_BLE))

typedef	uint32_t (*func_system_tick_isr_t) (uint8_t sequence, uint8_t link);
extern func_system_tick_isr_t	sys_tick_isr_ble_access_cb;

typedef struct link_scheduler_item
{
	u8			en;
	u8			skip;
	u8			priority;
	u8			type;

	u8			slot12_end;
	u8			duration;
	u8			interval;
	u8			win1m25;

	u32			tick;
	u32			latency;
	func_system_tick_isr_t func;
} link_scheduler_t;

typedef struct env_scheduler
{
//0x00
	u8			cur_id;
	u8			cur_priority;
	u8			next_id;
	u8			last_id;			//id of last flexible link

	u8			sequence;
	u8			keep;
	u16			cur_latency;

	u32			cur_tick;
	u32			next_tick;

//0x10
	u8			next_duration;
	u8			cur_duration;
	u8			inq_page_priority;
	u8			access;

	u8			scan_priority;
	u8			last_link;
	u8			slot12_offset;
	u8			lp_bt_last;

	u8			max_bt;
	u8			nbt;				//nbt < max_bt : bt access enable
	u8			max_ble;
	u8			nble;				//nble < max_ble: ble access enable

	u8			nflex;				// number of flexible link
	u8			mode;
	u8			bt_access;			// number of access: switch between BT/BLE
	u8			hslot24;

//0x20
	u8			lp_bt;
	u8			lp_ble;
	u8			lp_en;
	u8			lp_link;


	u32			lp_tick;
	u32			lp_sniff_interval;
	u32			hslot;

//0x30
	u32			fno24;
	u32			no;
	u32			irq0;
	u32			irq1;

//0x40
	u32    	 	deepRet_earlyWakeupTick;

	link_scheduler_t	link_scheduler[MAX_SCHEDULE_LINK];

	u8          max_bt_bkup;
	u8          sch_sel_go_on;
	u8          sco_link_pending;
	u8			standard_ble;

	u32         next_sco_tick;
	u32			system_irq_tick;
} env_scheduler_t ;


extern env_scheduler_t	env_sch;

static inline void bt_ll_schedule_disable (uint8_t id) {if (id < MAX_SCHEDULE_LINK) {env_sch.link_scheduler[id].en = 0;}}

void	bt_ll_schedule_keep (int en);


static inline int	bt_ll_schedule_end_pending ()
{return (env_sch.mode) && (!(FLAG_SCHEDULE_LINK_ID_NULL & env_sch.next_id) || (FLAG_SCHEDULE_LINK_ID_NULL & env_sch.cur_id));}

static inline void bt_ll_shedule_set_slot12_pre_end(int id, u8 slot) {if (id<MAX_SCHEDULE_LINK) env_sch.link_scheduler[id].slot12_end = slot;}

static inline u8 bt_ll_shedule_slot12_pre_end (int id, u32 slot, int sco)
{
	slot += env_sch.slot12_offset;
	slot &= (BIT(27) - 1);
	slot %= 12;
	u8 ref = env_sch.link_scheduler[id].slot12_end + (sco ? 2 : 0);
	return slot > ref && slot < 10 ? slot : 0;
}

static inline int bt_ll_schedule_get_ble_acl (uint8_t sch_link) {return sch_link >= ID_LINK_BLE && sch_link < MAX_SCHEDULE_LINK ? sch_link - ID_LINK_BLE : FLAG_SCHEDULE_LINK_ID_NULL;}

static inline int bt_ll_schedule_get_bt_acl (uint8_t sch_link) {return sch_link < ID_LINK_ACCESS ? sch_link : FLAG_SCHEDULE_LINK_ID_NULL;}

static inline int bt_ll_schedule_get_ble_link (int acl) {return acl + ID_LINK_BLE;}

void	bt_ll_schedule_set_mode (int m, int max_bt, int max_ble);

void	bt_ll_schedule_set_ble_callback (void *p);

void	bt_ll_schedule_set_bt_callback (void *p);

void	bt_ll_schedule_init ();

void	bt_ll_schedule_set_enable (uint8_t id, uint8_t priority, uint8_t type, uint32_t tick, uint32_t win1m25, uint32_t latency);

void 	bt_ll_schedule_set_duration_interval (uint8_t id, uint8_t duration, uint8_t interval);

void 	bt_ll_schedule_set_inquiry_page_mode (uint8_t priority, uint8_t priority_scan, uint8_t duration, uint8_t interval);

int	    bt_ll_schedule_set_current_priority (uint8_t id, uint8_t priority);

int		bt_ll_schedule_next (u32 slot);

int		bt_ll_schedule_end (int link, int disable);

int		bt_ll_schedule_select (u8 id, u32 interval);

int		bt_ll_schedule_link_idle (int link, int local_idle, int peer_idle);

u32		bt_ll_schedule_get_next_available_tick (int next_tick);

u32		bt_ll_schedule_get_next_7ms5_tick (u32 * out_nframe);

u8		bt_ll_schedule_get_slot12_offset (void);

int		bt_ll_schedule_update_hslot24 ();

void	bt_ll_schedule_access_scan_pause (void);
void	bt_ll_schedule_access_scan_restore (void);


void   bt_ll_schedule_set_priority (uint8_t id, uint8_t priority);
u32    bt_ll_set_next_irq(u32 tick);

extern void btble_schedule_init(void);

#endif // _BT_LL_SCHEDULER_H_
