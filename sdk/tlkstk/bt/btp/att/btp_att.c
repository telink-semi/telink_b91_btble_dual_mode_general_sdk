/********************************************************************************************************
 * @file     btp_att.c
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
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_l2cap.h"
#include "tlkstk/bt/bth/bth_signal.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#if (TLKBTP_CFG_ATT_ENABLE)
#include "tlkstk/bt/btp/btp_adapt.h"
#include "tlkstk/bt/btp/att/btp_att.h"
#include "tlkstk/bt/btp/att/btp_attInner.h"
#include "tlkstk/bt/btp/att/btp_attSrv.h"
#include "tlkstk/bt/btp/att/btp_attClt.h"



int btp_att_init(void)
{
	btp_att_innerInit();
	#if (TLKBTP_CFG_ATTSRV_ENABLE)
	btp_attsrv_init();
	#endif
	
	return TLK_ENONE;
}




#endif //#if (TLKBTP_CFG_ATT_ENABLE)

