/********************************************************************************************************
 * @file     ble_host.h
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

#ifndef STACK_BLE_HOST_BLE_HOST_H_
#define STACK_BLE_HOST_BLE_HOST_H_

#include "tlkstk/ble/ble_common.h"
#include "tlkstk/ble/ble_format.h"

#include "tlkstk/ble/host/l2cap/l2cap.h"
#include "tlkstk/ble/host/l2cap/l2cap_signal.h"


#include "tlkstk/ble/host/attr/att.h"
#include "tlkstk/ble/host/attr/gatt.h"


#include "tlkstk/ble/host/smp/smp.h"
#include "tlkstk/ble/host/smp/smp_storage.h"

#include "tlkstk/ble/host/gap/gap.h"
#include "tlkstk/ble/host/gap/gap_event.h"


/*********************************************************/
//Remove when file merge to SDK //
#include "tlkstk/ble/ble_stack.h"
#include "tlkstk/ble/ble_config.h"
#include "tlkstk/ble/debug.h"
#include "tlkstk/ble/trace.h"

#include "tlkstk/ble/host/host_stack.h"
#include "tlkstk/ble/host/l2cap/l2cap_stack.h"
#include "tlkstk/ble/host/attr/att_stack.h"
#include "tlkstk/ble/host/attr/gatt_stack.h"
#include "tlkstk/ble/host/attr/att_stack.h"
#include "tlkstk/ble/host/smp/smp_stack.h"
#include "tlkstk/ble/host/gap/gap_stack.h"
/*********************************************************/


#endif /* STACK_BLE_HOST_BLE_HOST_H_ */
