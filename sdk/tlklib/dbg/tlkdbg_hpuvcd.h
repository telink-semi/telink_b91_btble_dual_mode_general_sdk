/********************************************************************************************************
 * @file     tlkdbg_hpuvcd.h
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
#ifndef TLKDBG_HPU_VCD_H
#define TLKDBG_HPU_VCD_H

#if (TLKDBG_CFG_HPU_VCD_ENABLE)


//HPU - Hardware Protocol UART


#define TLKDBG_HPU_VCD_BUFFER_SIZE       1024
#define TLKDBG_HPU_VCD_CACHE_SIZE        64


void tlkdbg_hpuvcd_init(void);
void tlkdbg_hpuvcd_reset(void);
bool tlkdbg_hpuvcd_isBusy(void);
void tlkdbg_hpuvcd_handler(void);

void tlkdbg_hpuvcd_ref(void);
void tlkdbg_hpuvcd_sync(bool enable);
void tlkdbg_hpuvcd_tick(uint08 id);
void tlkdbg_hpuvcd_level(uint08 id, uint08 level);
void tlkdbg_hpuvcd_event(uint08 id);
void tlkdbg_hpuvcd_byte(uint08 id, uint08 value);
void tlkdbg_hpuvcd_word(uint08 id, uint16 value);

void tlkdbg_hpuvcd_setOpen(bool isOpen);

#endif //#if (TLKDBG_CFG_HPU_VCD_ENABLE)

#endif //TLKDBG_HPU_VCD_H

