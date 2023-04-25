/********************************************************************************************************
 * @file     compiler.h
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

#ifndef COMPILER_H_
#define COMPILER_H_


#define _attribute_retention_code_   			__attribute__((section(".retention_code"))) __attribute__((noinline))
#define _attribute_iram_data_   				__attribute__((section(".iram_data")))
#define _attribute_ram_code_sec_      			__attribute__((section(".ram_code")))
#define _attribute_ram_code_sec_noinline_      	__attribute__((section(".ram_code"))) __attribute__((noinline))
#define _attribute_text_sec_   					__attribute__((section(".text")))
#define _attribute_aes_data_sec_      			__attribute__((section(".aes_data")))
#define _attribute_data_retention_sec_   		__attribute__((section(".retention_data")))

#define _attribute_aligned_(s)					__attribute__((aligned(s)))

/// Pack a structure field
#define __PACKED __attribute__ ((__packed__))

/*******************************      BT/BLE Use     ******************************/
#define _attribute_ble_retention_code_   		__attribute__((section(".retention_code")))__attribute__((noinline))
#define _attribute_ble_ram_code_      			__attribute__((section(".ram_code"))) __attribute__((noinline))
#define _attribute_ble_data_retention_   		__attribute__((section(".retention_data")))

#define _attribute_bt_retention_code_   		__attribute__((section(".retention_code")))__attribute__((noinline))
#define _attribute_bt_ram_code_      			__attribute__((section(".ram_code"))) __attribute__((noinline))
#define _attribute_bt_data_retention_   		__attribute__((section(".retention_data")))

#define _attribute_packed_				__attribute__((packed))
#define _attribute_session_(s)			__attribute__((section(s)))
#define _attribute_custom_code_  		_attribute_session_(".custom") volatile
#define _attribute_noinline_ 			__attribute__((noinline))
#define _attribute_no_inline_   		__attribute__((noinline))
#define _inline_ 						inline
#define _attribute_data_dlm_   			_attribute_session_(".dlm_data")//dlm:Data Local Memory
#define _attribute_data_no_init_		__attribute__((section(".data_no_init")))


#define _attribute_iram_data_   	    __attribute__((section(".iram_data")))

#define _attribute_data_retention_   	__attribute__((section(".retention_data")))
#define _attribute_ram_code_            __attribute__((section(".ram_code"))) __attribute__((noinline))
#define _attribute_ram_code_without_oninline      __attribute__((section(".ram_code")))

#define _attribute_text_code_           __attribute__((section(".text")))



#define ASSERT_ERR(x)

#endif
