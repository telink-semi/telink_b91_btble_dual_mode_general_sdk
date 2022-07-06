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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/btp_adapt.h"
#include "tlkstk/bt/btp/btp.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"


int btp_init(void)
{
	btp_event_init();
	btp_adapt_init();
	#if (TLKBTP_CFG_SDP_ENABLE)
	btp_sdp_init();
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
	
	return TLK_ENONE;
}

void btp_process(void)
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

	#if (TLKBTP_CFG_PBAP_ENABLE)
	btp_pbap_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_HFP_ENABLE)
	btp_hfp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_SPP_ENABLE)
	btp_spp_destroy(aclHandle);
	#endif
	#if (TLKBTP_CFG_RFC_ENABLE)
	btp_rfcomm_destroy(aclHandle);
	#endif
}
