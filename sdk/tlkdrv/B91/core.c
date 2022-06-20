/********************************************************************************************************
 * @file     core.c
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

#include "core.h"

#include "plic.h"
#include "compiler.h"


/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
 _attribute_retention_code_ void core_enable_interrupt(void)
{
#if SUPPORT_PFT_ARCH
	 plic_set_feature(FLD_FEATURE_VECTOR_MODE_EN | FLD_FEATURE_PREEMPT_PRIORITY_INT_EN);//enable vectored in PLI
#else
	 plic_set_feature(FLD_FEATURE_VECTOR_MODE_EN);//enable vectored in PLI
#endif
	set_csr(NDS_MSTATUS,1<<3);
	set_csr(NDS_MIE,1<<11 | 1 << 7 | 1 << 3);
}



_attribute_retention_code_ unsigned int core_disable_interrupt(void)
{
   unsigned int r = read_csr(NDS_MSTATUS) & BIT(3);
   if (r) {
	   clear_csr(NDS_MSTATUS, BIT(3));//global interrupts disable
	   NDS_FENCE_IORW; //ensure that MIE bit of mstatus reg is cleared at hardware level
   }
   return r;
}

_attribute_retention_code_ void core_restore_interrupt(u32 en)
{
   if (en) {
	   set_csr(NDS_MSTATUS, en); //global interrupts enable
	   NDS_FENCE_IORW;
   }
}


