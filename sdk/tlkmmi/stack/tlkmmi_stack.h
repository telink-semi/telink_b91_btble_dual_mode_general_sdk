/********************************************************************************************************
 * @file     tlkmmi_stack.h
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
#ifndef TLKMMI_STACK_H
#define TLKMMI_STACK_H

#if (TLKMMI_STACK_ENABLE)


#define TLKMMI_STACK_PROCID         TLKTSK_PROCID_STACK


int tlkmmi_stack_init(void);



#endif //#if (TLKMMI_STACK_ENABLE)

#endif //TLKMMI_STACK_H
