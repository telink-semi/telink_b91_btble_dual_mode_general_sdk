/********************************************************************************************************
 * @file     tlkmmi_testFunc.h
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
#ifndef TLKMMI_TESTFUNC_H
#define TLKMMI_TESTFUNC_H

#if (TLKMMI_TEST_ENABLE)


typedef enum{
	TLK_FUNC_MAJOR_TYPE_NONE = 0,
	TLK_FUNC_MAJOR_TYPE_BTH,
	TLK_FUNC_MAJOR_TYPE_BTP,
	TLK_FUNC_MAJOR_TYPE_BTC,

	TLK_FUNC_MAJOR_TYPE_MAX,
}TLK_FUNC_MAJOR_TYPE_ENUM;


int tlk_FuncCall(uint08 majorType, uint08 minorType,uint08 funcID, uint08 *pData, uint16 dataLen);


#endif //#if (TLKMMI_TEST_ENABLE)

#endif //TLKMMI_TESTFUNC_H

