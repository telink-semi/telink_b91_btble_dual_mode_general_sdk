/********************************************************************************************************
 * @file	tlkmmi_testStdio.h
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
#ifndef TLKMMI_TEST_STDIO_H
#define TLKMMI_TEST_STDIO_H

#if (TLKMMI_TEST_ENABLE)


#include "tlkmmi_testAdapt.h"

typedef struct
{
	int(*Init)(void);
	int(*Start)(void);
	int(*Pause)(void);
	int(*Close)(void);
	int(*Input)(uint08 msgID, uint08 *pData, uint16 dataLen);
	void(*Handler)(void);
}tlkmmi_testModinf_t;





#endif //#if (TLKMMI_TEST_MOD_H)

#endif //TLKMMI_TEST_STDIO_H

