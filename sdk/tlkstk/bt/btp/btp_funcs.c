/********************************************************************************************************
 * @file     btp_funcs.c
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
#if (TLK_STK_BTP_ENABLE)
#include "btp_stdio.h"
#include "btp.h"
#include "btp_funcs.h"
#include "btp_module.h"
#include "sdp/btp_sdp.h"
#include "a2dp/btp_a2dp.h"
#include "avrcp/btp_avrcp.h"
#include "rfcomm/btp_rfcomm.h"
#include "hfp/btp_hfp.h"
#include "spp/btp_spp.h"
#include "a2dp/btp_a2dp.h"
#include "pbap/btp_pbap.h"
#include "hid/btp_hid.h"
#include "att/btp_att.h"

static const btp_func_item_t scBtpFunSet[] = {

};


int btp_getSetNum()
{
	return sizeof(scBtpFunSet)/sizeof(scBtpFunSet[0]);
}
int btp_FuncCall(uint08 funcType, uint08 funcID, uint08 *pData, uint16 dataLen)
{
	int ret = 0;
	if(funcID > TLK_FUNC_TYPE_BTP_MAX)
		return -TLK_EPARAM;
	for(int i = 0;i<btp_getSetNum();i++)
	{
		if(scBtpFunSet[i].type == funcType && scBtpFunSet[i].funID == funcID)ret = scBtpFunSet[i].Func(pData,dataLen);
	}
	return ret;
}

#endif //#if (TLK_STK_BTP_ENABLE)

