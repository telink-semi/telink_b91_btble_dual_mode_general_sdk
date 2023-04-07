/********************************************************************************************************
 * @file     host_stack.h
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

#ifndef STACK_BLE_HOST_HOST_STACK_H_
#define STACK_BLE_HOST_HOST_STACK_H_

#include "tlkstk/inner/tlkstk_inner.h"
#include "drivers.h"


#if 0

_attribute_aligned_(4)
typedef struct {
	u16 	l2cap_connParaUpdateReq_minIntevel;
	u16 	l2cap_connParaUpdateReq_maxInterval;
	u16 	l2cap_connParaUpdateReq_latency;
	u16 	l2cap_connParaUpdateReq_timeout;

	u32		l2cap_connParaUpReq_pending;  //must "u32"
//	u8		u8_rsvd[3];




} host_acl_ms_t;
extern host_acl_ms_t blhAclms[];

_attribute_aligned_(4)
typedef struct {
	u32	rsvd;

} host_acl_m_t;
extern host_acl_m_t blhAclm[];


_attribute_aligned_(4)
typedef struct {
	u32	rsvd;

} host_acl_s_t;
extern host_acl_s_t blhAcls[];





#endif








#endif /* STACK_BLE_HOST_HOST_STACK_H_ */
