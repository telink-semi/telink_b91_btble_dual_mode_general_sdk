/********************************************************************************************************
 * @file     tlkapp_dfu.h
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

#ifndef TLKAPP_DFU_H
#define TLKAPP_DFU_H


#define TLKAPP_DFU_MAX_SIZE             (0xBE000)
#define TLKAPP_DFU_MIN_SIZE             (0x400)


#define TLKAPP_DFU_LOAD_INNER           1
#define TLKAPP_DFU_LOAD_OUTER           2

#define TLKAPP_DFU_START_FLAG           0x4B
#define TLKAPP_DFU_START_OFFSET         0x20
#define TLKAPP_DFU_START_ADDRESS        0x8000

#define TLKAPP_JUMP_TO_APP()    ((void(*)(void))(0x20000000 + TLKAPP_DFU_START_ADDRESS))()


void tlkapp_dfu_load(void);





#endif /* TLKAPP_DFU_H */

