/********************************************************************************************************
 * @file     tlkapp_irq.c
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
#include "types.h"
#include "drivers.h"
#include "tlkapp_config.h"



extern void trap_entry(void);


_attribute_retention_code_ void  entry_null(void)
{
	trap_entry ();
}


typedef   void   (* interrupt_ptr_t)(void);

__attribute__((section(".retention_data"))) interrupt_ptr_t   g_interrupt_tbl[64] __attribute__ ((aligned (256))) = {
		trap_entry,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,

		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
};


/******************************************************************************
 * Function: tlkapp_irq_init
 * Descript: Set the start address of interrupt ISR function table mapping to
 *           MCU hardware interrupt entry
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_irq_init(void)
{
	/*set the start address of interrupt ISR function table 
	mapping to  MCU hardware interrupt entry*/
	write_csr(NDS_MTVEC, (unsigned long)g_interrupt_tbl);
}

