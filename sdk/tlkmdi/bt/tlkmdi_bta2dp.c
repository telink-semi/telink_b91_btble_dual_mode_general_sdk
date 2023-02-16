/********************************************************************************************************
 * @file     tlkmdi_bthfp.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLK_MDI_BTHFP_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmdi/bt/tlkmdi_bta2dp.h"

#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlksys/prt/tlkpto_comm.h"


#define TLKMDI_BTA2DP_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_A2DP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTA2DP_DBG_SIGN       "[MHFP]"

#if (TLK_STK_BTP_ENABLE)
static void tlkmdi_bta2dp_avrcpVolumeChangeEvt(uint16 aclHandle, uint08 volume);
#endif


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bta2dp_init(void)
{	
	#if (TLK_STK_BTP_ENABLE)
    btp_avrcp_regVolumeChangeCB(tlkmdi_bta2dp_avrcpVolumeChangeEvt);
	#endif
		
	return TLK_ENONE;
}

#if (TLK_STK_BTP_ENABLE)
static void tlkmdi_bta2dp_avrcpVolumeChangeEvt(uint16 aclHandle, uint08 volume)
{
	uint08 buffLen = 0;
	uint08 buffer[6];
	buffer[buffLen++] = (aclHandle & 0xFF);
	buffer[buffLen++] = (aclHandle & 0xFF00) >> 8;
	buffer[buffLen++] = btp_hfphf_isIosDev(aclHandle);
	if(btp_a2dp_isSrc(aclHandle)) buffer[buffLen++] = 0x01;
	else buffer[buffLen++] = 0x00;
	buffer[buffLen++] = volume;
	tlktsk_sendInnerMsg(TLKTSK_TASKID_AUDIO, TLKPTI_AUD_MSGID_AVRCP_CHG_VOLUME_EVT, buffer, buffLen);
}
#endif



#endif //#if (TLK_MDI_BTHFP_ENABLE)

