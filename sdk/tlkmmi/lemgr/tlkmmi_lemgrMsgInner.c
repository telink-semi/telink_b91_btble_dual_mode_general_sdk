/********************************************************************************************************
 * @file     tlkmmi_lemgrInner.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_lemgr.h"
#include "tlkmmi_lemgrMsgInner.h"



/******************************************************************************
 * Function: tlkmmi_lemgr_sendCommXxx.
 * Descript: send the lemgr command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemgr_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 head[4];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_LE;
	head[headLen++] = cmdID;
	return tlktsk_sendInnerExtMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}
int tlkmmi_lemgr_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 head[8];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_LE;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlktsk_sendInnerExtMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}
int tlkmmi_lemgr_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 head[4];
	uint08 headLen = 0;
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_LE;
	head[headLen++] = evtID;
	return tlktsk_sendInnerExtMsg(TLKTSK_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, headLen, pData, dataLen);
}


int tlkmmi_lemgr_innerMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	return -TLK_ENOSUPPORT;
}





#endif //TLKMMI_LEMGR_ENABLE

