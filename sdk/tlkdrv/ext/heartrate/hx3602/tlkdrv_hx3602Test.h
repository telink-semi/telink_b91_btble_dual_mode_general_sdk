/********************************************************************************************************
 * @file	tlkdrv_hx3602Test.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef _hx3602_FACTORY_TEST_H_
#define _hx3602_FACTORY_TEST_H_

#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LEAK_LIGHT_TEST = 1,
	GRAY_CARD_TEST = 2,
    
} TEST_MODE_t;


typedef struct{
	int32_t gr_data_final;
	int32_t ir_data_final;
    
} FT_RESULTS_t;

FT_RESULTS_t hx3602_factroy_test(TEST_MODE_t  test_mode);


#endif //#if (TLKDRV_HEARTRATE_HX3602_ENABLE)

#endif // _hx3600_FACTORY_TEST_H_
