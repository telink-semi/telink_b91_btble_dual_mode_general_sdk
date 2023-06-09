/********************************************************************************************************
 * @file     tlkos_kernel.h
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
#ifndef TLKOS_KERNEL_H
#define TLKOS_KERNEL_H

#if (TLK_CFG_OS_ENABLE)



uint tlkos_enter_critical(void);
void tlkos_leave_critical(uint irqMsk);

uint tlkos_disable_interrupt(void);
void tlkos_restore_interrupt(uint irqMsk);


void tlkos_delay(uint value);



#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_KERNEL_H

