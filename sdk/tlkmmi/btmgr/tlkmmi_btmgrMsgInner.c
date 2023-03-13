/********************************************************************************************************
 * @file     tlkmmi_btmgrMsgOuter.c
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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrMsgInner.h"
//#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
//#include "tlkstk/bt/bth/bth_handle.h"
//#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"


static int tlkmmi_btmgr_setHfpVolumeDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_btmgr_setAvrcpVolumeDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_btmgr_setAvrcpDefaultVolumeDeal(uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmmi_btmgr_sendCommXxx.
 * Descript: send the btmgr command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 head[4];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_BT;
	head[headLen++] = cmdID;
	return tlktsk_sendInnerExtMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}
int tlkmmi_btmgr_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 head[8];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_BT;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlktsk_sendInnerExtMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}
int tlkmmi_btmgr_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 head[4];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_BT;
	head[headLen++] = evtID;
	return tlktsk_sendInnerExtMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}



int tlkmmi_btmgr_innerMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	if(msgID == TLKPTI_BT_MSGID_SET_HFP_VOLUME){
		return tlkmmi_btmgr_setHfpVolumeDeal(pData, dataLen);
	}else if(msgID == TLKPTI_BT_MSGID_SET_AVRCP_VOLUME){
		return tlkmmi_btmgr_setAvrcpVolumeDeal(pData, dataLen);
	}else if(msgID == TLKPTI_BT_MSGID_SET_AVRCP_DEF_VOL){
		return tlkmmi_btmgr_setAvrcpDefaultVolumeDeal(pData, dataLen);
	}
	
	return -TLK_ENOSUPPORT;
}


static int tlkmmi_btmgr_setHfpVolumeDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_STK_BTP_ENABLE)
	return btp_hfphf_setSpkVolume(pData[0]);
#else
	return -TLK_ENOSUPPORT;
#endif
}
static int tlkmmi_btmgr_setAvrcpVolumeDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_STK_BTP_ENABLE)
	uint16 handle;
	uint08 isSrc;
	uint08 volume;
	uint08 iosVolume;
	uint08 androidVolume;
	handle = ((uint16)pData[1] << 8) | pData[0];
	isSrc  = pData[2];
	iosVolume = pData[3];
	androidVolume =  pData[4];
	if(btp_hfphf_isIosDev(handle)) volume = iosVolume;
	else volume = androidVolume;
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_setAvrcpVolumeDeal: %d-%d-%d",
		iosVolume, androidVolume, volume);
	return btp_avrcp_setVolume(handle, volume, isSrc);
#else
	return -TLK_ENOSUPPORT;
#endif
}
static int tlkmmi_btmgr_setAvrcpDefaultVolumeDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_STK_BTP_ENABLE)
	return btp_avrcp_setDefaultVolume(pData[0]);
#else
	return -TLK_ENOSUPPORT;
#endif
}



#endif //#if (TLKMMI_BTMGR_ENABLE)

