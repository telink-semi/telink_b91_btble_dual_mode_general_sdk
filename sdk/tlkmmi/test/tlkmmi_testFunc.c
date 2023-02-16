/********************************************************************************************************
 * @file     tlkmmi_testFunc.c
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
#if (TLKMMI_TEST_ENABLE)
#if (TLK_STK_BTH_ENABLE)
#include "tlkstk/bt/bth/bth_funcs.h"
#endif
#if (TLK_STK_BTP_ENABLE)
#include "tlkstk/bt/btp/btp_funcs.h"
#endif
#include "tlkstk/bt/btc/btc_funcs.h"
#include "tlkmmi_testFunc.h"


int tlk_FuncCall(uint08 majorType, uint08 minorType,uint08 funcID, uint08 *pData, uint16 dataLen)
{
	int ret = 0;
	if(majorType > TLK_FUNC_MAJOR_TYPE_MAX)
		return -TLK_EPARAM;
	switch(majorType)
	{
		case TLK_FUNC_MAJOR_TYPE_BTH:
			#if (TLK_STK_BTH_ENABLE)
			ret = bth_FuncCall(minorType, funcID, pData, dataLen);
			#endif
			break;
		case TLK_FUNC_MAJOR_TYPE_BTC:
			ret = btc_FuncCall(minorType, funcID, pData, dataLen);
			break;
		case TLK_FUNC_MAJOR_TYPE_BTP:
			#if (TLK_STK_BTP_ENABLE)
			ret = btp_FuncCall(minorType, funcID, pData, dataLen);
			#endif
			break;
		default:
			break;		
	}
	return ret;
}


#endif //#if (TLKMMI_TEST_ENABLE)

