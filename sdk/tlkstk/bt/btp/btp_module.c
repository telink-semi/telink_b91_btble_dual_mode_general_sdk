/********************************************************************************************************
 * @file     btp_module.c
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
#include "btp_adapt.h"
#include "btp.h"
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


int btp_module_connect(uint16 aclHandle, uint08 ptype, uint08 usrID, uint08 channel)
{
	int ret;

	ret = -TLK_ENOSUPPORT;
	switch(ptype){
		case BTP_PTYPE_SDP:
			ret = btp_sdp_connect(aclHandle, usrID);
			break;
		case BTP_PTYPE_RFC:
			ret = btp_rfcomm_connect(aclHandle);
			break;
		case BTP_PTYPE_HFP:
			ret = btp_hfp_connect(aclHandle, usrID, channel);
			break;
		case BTP_PTYPE_SPP:
			ret = btp_spp_connect(aclHandle, channel);
			break;
		case BTP_PTYPE_A2DP:
			#if (TLKBTP_CFG_A2DP_ENABLE)
			ret = btp_a2dp_connect(aclHandle, usrID);
			#endif
			break;
		case BTP_PTYPE_AVRCP:
			ret = btp_avrcp_connect(aclHandle, usrID);
			break;
		case BTP_PTYPE_PBAP:
			ret = btp_pbap_connect(aclHandle, usrID, channel, false);
			break;
		case BTP_PTYPE_HID:
            ret = btp_hid_connect(aclHandle, usrID);
		    break;
		case BTP_PTYPE_ATT:
            ret = btp_att_connect(aclHandle, usrID);
		    break;
	}
	return ret;
}
int btp_module_disconn(uint16 aclHandle, uint08 ptype, uint08 usrID)
{
	int ret;

	ret = -TLK_ENOSUPPORT;
	switch(ptype){
		case BTP_PTYPE_SDP:
			ret = btp_sdp_disconn(aclHandle, usrID);
			break;
		case BTP_PTYPE_RFC:
			ret = btp_rfcomm_disconn(aclHandle);
			break;
		case BTP_PTYPE_HFP:
			ret = btp_hfp_disconn(aclHandle, usrID);
			break;
		case BTP_PTYPE_SPP:
			ret = btp_spp_disconn(aclHandle);
			break;
		case BTP_PTYPE_A2DP:
			#if (TLKBTP_CFG_A2DP_ENABLE)
			ret = btp_a2dp_disconn(aclHandle);
			#endif
			break;
		case BTP_PTYPE_AVRCP:
			ret = btp_avrcp_disconn(aclHandle, usrID);
			break;
		case BTP_PTYPE_PBAP:
			ret = btp_pbap_disconn(aclHandle, usrID);
			break;
		case BTP_PTYPE_HID:
			ret = btp_hid_disconn(aclHandle, usrID);
			break;
		case BTP_PTYPE_ATT:
			ret = btp_att_disconn(aclHandle, usrID);
			break;
	}
	return ret;
}


#endif //#if (TLK_STK_BTP_ENABLE)

