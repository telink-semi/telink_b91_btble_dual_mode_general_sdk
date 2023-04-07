/********************************************************************************************************
 * @file     gap_stack.h
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

#ifndef STACK_BLE_GAP_STACK_H_
#define STACK_BLE_GAP_STACK_H_





typedef int (*host_ota_mian_loop_callback_t)(void);
typedef int (*host_ota_terminate_callback_t)(u16);
extern host_ota_mian_loop_callback_t    		host_ota_main_loop_cb;
extern host_ota_terminate_callback_t    		host_ota_terminate_cb;







/******************************* gap event start ************************************************************************/

extern u32		gap_eventMask;


int blc_gap_send_event (u32 h, u8 *para, int n);

/******************************* gap event  end ************************************************************************/



/******************************* gap start ************************************************************************/

typedef struct
{
	u16 effective_MTU;
	u16 indicate_handle;

	u8 *pPendingPkt;

	u16 mtu_exg_pending;
	u16 rsvd;

}gap_ms_para_t;

typedef struct
{
	u16 	l2cap_connParaUpdateReq_minIntevel;
	u16 	l2cap_connParaUpdateReq_maxInterval;
	u16 	l2cap_connParaUpdateReq_latency;
	u16 	l2cap_connParaUpdateReq_timeout;

	u8		l2cap_connParaUpReq_pending;
	u8 		data_pending_time;    //10ms unit
	u8      rsvd[2];
	u32 	l2cap_connParaUpReqSendAfterConn_us;

	u32 	att_service_discover_tick;

}gap_s_para_t;



extern gap_ms_para_t gap_ms_para[];
extern gap_s_para_t  gap_s_para[];



int blt_gap_conn_complete_handler(u16 connHandle, u8 *p);
int blt_gap_conn_terminate_handler(u16 connHandle, u8 *p);
u8  blt_gap_getSlaveDeviceIndex_by_connIdx(u8 conn_idx);

gap_s_para_t 	*bls_gap_getSlavepara(u16 connHandle);
gap_ms_para_t 	*blc_gap_getMasterSlavePara(u16 connHandle);



/******************************* gap end ************************************************************************/


#endif /* GAP_STACK_H_ */
