/********************************************************************************************************
 * @file     udisk.h
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

#pragma once


/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief      This is mass storage interrupt handler function.
 * @return     none.
 */
_attribute_ram_code_sec_ void mass_storage_irq_handler(void);
/**
 * @brief      This function serves to initialization nand spi flash.
 * @param[in]  reset - para = 1, nandflash will be initial.
 * @return     none.
 */
void nand_flash_fat_init(int reset);
void usb_mass_storage_init(void);
/**
 * @brief      This is mass storage task.
 * @return     none.
 */
void mass_storage_task(void);

/**
 * @brief      This function serves to remap address get from usb to fat16 in nand flash.
 * @param[in]  adr - the address get from usb
 * @return     the remap address in nand flash.
 */
unsigned int  fat_remap(unsigned int adr);
/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

