/********************************************************************************************************
 * @file     bit_op.c
 *
 * @brief    This is the source file for BTBLE SDK
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

#include "sys.h"
/**
 * @brief      This function writes a byte data to a specified digital register
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
void sub_wr(unsigned int addr, unsigned char value, unsigned char e, unsigned char s)
{
	unsigned char v, mask, tmp1, target, tmp2;

	v = read_reg8(addr);
	mask = BIT_MASK_LEN(e - s + 1);
	tmp1 = value & mask;

	tmp2 = v & (~BIT_RNG(s,e));

	target = (tmp1 << s) | tmp2;
	write_reg8(addr, target);
}

/**
 * @brief      This function writes a word data to a specified digital register
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
void sub_wr_word(unsigned int addr, unsigned int value, unsigned char e, unsigned char s)
{
	unsigned int v, mask, tmp1, target, tmp2;

	v = read_reg32(addr);

	if((e==31)&&(s==0))
	{
		target = value;
	}
	else
	{
		mask = BIT_MASK_LEN(e - s + 1);
		tmp1 = value & mask;

		tmp2 = v & (~BIT_RNG(s,e));

		target = (tmp1 << s) | tmp2;
	}

	write_reg32(addr, target);
}


