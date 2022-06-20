/********************************************************************************************************
 * @file     bit_op.h
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

#ifndef BIT_OP_H_
#define BIT_OP_H_

/**
 * @brief      This function writes a byte data to a specified digital register
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
void sub_wr(unsigned int addr, unsigned char value, unsigned char e, unsigned char s);


/**
 * @brief      This function writes a word data to a specified digital register
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
void sub_wr_word(unsigned int addr, unsigned int value, unsigned char e, unsigned char s);


#endif /* BIT_H_ */
