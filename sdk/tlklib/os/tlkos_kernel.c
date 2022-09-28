/********************************************************************************************************
 * @file     tlkos_kernel.c
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_kernel.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#endif
#include "drivers.h"


//_attribute_ram_code_sec_noinline_ 
uint tlkos_enter_critical(void)
{
	return core_enter_critical(1, 1);
}
//_attribute_ram_code_sec_noinline_ 
void tlkos_leave_critical(uint irqMsk)
{
	core_leave_critical(1, irqMsk);
}

_attribute_ram_code_sec_noinline_
uint tlkos_disable_interrupt(void)
{
	return core_disable_interrupt();
}
_attribute_ram_code_sec_noinline_
void tlkos_restore_interrupt(uint irqMsk)
{
	core_restore_interrupt(irqMsk);
}

void tlkos_delay(uint value)
{
#if (TLK_OS_FREERTOS_ENABLE)
	vTaskDelay(value);
#endif
}

void tlkos_start_run(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	vTaskStartScheduler();
#endif
}




#endif //#if (TLK_CFG_OS_ENABLE)

