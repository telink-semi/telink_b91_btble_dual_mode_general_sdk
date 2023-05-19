/********************************************************************************************************
 * @file     btp.c
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
#include "drivers.h"
#include "btp_stdio.h"
#include "btp_adapt.h"
#include "../bth/bth.h"
#include "../bth/bth_device.h"
#include "../bth/bth_handle.h"
#include "../bth/bth_l2cap.h"
#include "btp.h"
#include "btp_config.h"
#include "btp_define.h"
#include "sdp/btp_sdp.h"
#include "a2dp/btp_a2dp.h"
#include "avrcp/btp_avrcp.h"
#include "rfcomm/btp_rfcomm.h"
#include "hfp/btp_hfp.h"
#include "pbap/btp_pbap.h"
#include "spp/btp_spp.h"
#include "att/btp_att.h"
#include "hid/btp_hid.h"
#include "iap/btp_iap.h"
#include "browsing/btp_browsing.h"
#if (TLKBTP_CFG_PTSL2C_ENABLE)
#include "pts/btp_ptsL2c.h"
#endif
#if (TLKBTP_CFG_PTSHID_ENABLE)
#include "pts/btp_ptsHid.h"
#endif


int btp_init(void)
{
	btp_event_init();
	btp_adapt_init();
	#if (TLKBTP_CFG_SDP_ENABLE)
	btp_sdp_init();
	#endif
	#if (TLKBTP_CFG_PTSL2C_ENABLE)
	btp_ptsl2c_init();
	#endif
	#if (TLKBTP_CFG_AVRCP_ENABLE)
	btp_avrcp_init();
	#endif
	#if (TLKBTP_CFG_A2DP_ENABLE)
	btp_a2dp_init();
	#endif
	#if (TLKBTP_CFG_RFC_ENABLE)
	btp_rfcomm_init();
	#endif
	#if (TLKBTP_CFG_HFP_ENABLE)
	btp_hfp_init();
	#endif
	#if (TLKBTP_CFG_PBAP_ENABLE)
	btp_pbap_init();
	#endif
	#if (TLKBTP_CFG_SPP_ENABLE)
	btp_spp_init();
	#endif
	#if (TLKBTP_CFG_IAP_ENABLE)
	btp_iap_init();
	#endif
	#if (TLKBTP_CFG_ATT_ENABLE)
	btp_att_init();
	#endif
	#if (TLKBTP_CFG_HID_ENABLE)
	btp_hid_init();
	#endif
	#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
	btp_browsing_init();
	bth_l2cap_setExtFeatureBits(BTH_L2CAP_EXT_FEATURE_ENHANCED_RTN_MODE | BTH_L2CAP_EXT_FEATURE_FCS_OPTION);
	#endif
	#if (TLKBTP_CFG_PTSHID_ENABLE)
	btp_ptshid_init();
	#endif
	
	return TLK_ENONE;
}

void btp_handler(void)
{
	btp_adapt_handler();
}


void btp_destroy(uint16 aclHandle)
{
	#if (TLKBTP_CFG_SDP_ENABLE)
	btp_sdp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_A2DP_ENABLE)
	btp_a2dp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_AVRCP_ENABLE)
	btp_avrcp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_PTSL2C_ENABLE)
	btp_ptsl2c_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_PTSHID_ENABLE)
	btp_ptshid_destroy(aclHandle);
	#endif

	#if (TLKBTP_CFG_PBAP_ENABLE)
	btp_pbap_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_HFP_ENABLE)
	btp_hfp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_SPP_ENABLE)
	btp_spp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_IAP_ENABLE)
	btp_iap_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_RFC_ENABLE)
	btp_rfcomm_destroy(aclHandle);
	#endif

	#if (TLKBTP_CFG_ATT_ENABLE)
	btp_att_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_HID_ENABLE)
	btp_hid_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
	btp_browsing_destroy(aclHandle);
	#endif
}


uint16 gBtpConnTimeout = 5000/BTP_TIMER_TIMEOUT_MS;
uint16 gBtpDiscTimeout = 5000/BTP_TIMER_TIMEOUT_MS;

void btp_setConnTimeout(uint16 timeout)
{
	if(timeout < 3000) timeout = 3000;
	else if(timeout > 15000) timeout = 15000;
	gBtpConnTimeout = timeout / BTP_TIMER_TIMEOUT_MS;
}
void btp_setDiscTimeout(uint16 timeout)
{
	if(timeout < 3000) timeout = 3000;
	else if(timeout > 15000) timeout = 15000;
	gBtpDiscTimeout = timeout / BTP_TIMER_TIMEOUT_MS;
}


#endif //#if (TLK_STK_BTP_ENABLE)

