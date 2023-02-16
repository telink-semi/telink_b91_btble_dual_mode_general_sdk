/********************************************************************************************************
 * @file     tlktsk_modinf.c
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
#include "tlktsk_stdio.h"
#include "tlktsk_modinf.h"
#include "tlktsk.h"


static tlktsk_modinf_t *sTlkTskModInfs[TLKTSK_TASKID_MAX];


int tlktsk_modinf_init(void)
{
	tmemset(sTlkTskModInfs, 0, TLKTSK_TASKID_MAX*sizeof(tlktsk_modinf_t));
	return TLK_ENONE;
}

int tlktsk_modinf_mount(const tlktsk_modinf_t *pModinf)
{
	if(pModinf == nullptr || pModinf->taskID == 0 || pModinf->taskID >= TLKTSK_TASKID_MAX
		|| pModinf->Start == nullptr || pModinf->Pause == nullptr || pModinf->Close == nullptr 
		|| pModinf->IntvUs == nullptr || pModinf->Handler == nullptr){
		return -TLK_EPARAM;
	}
	if(tlktsk_modinf_isHave(pModinf)) return -TLK_EREPEAT;
	sTlkTskModInfs[pModinf->taskID] = (tlktsk_modinf_t*)pModinf;
	return TLK_ENONE;
}


int tlktsk_modinf_start(uint08 taskID)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return -TLK_EPARAM;
	if(sTlkTskModInfs[taskID]->Start == nullptr) return -TLK_ENOSUPPORT;
	return sTlkTskModInfs[taskID]->Start();
}
int tlktsk_modinf_pause(uint08 taskID)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return -TLK_EPARAM;
	if(sTlkTskModInfs[taskID]->Pause == nullptr) return -TLK_ENOSUPPORT;
	return sTlkTskModInfs[taskID]->Pause();
}
int tlktsk_modinf_close(uint08 taskID)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return -TLK_EPARAM;
	return sTlkTskModInfs[taskID]->Close();
}
int tlktsk_modinf_input(uint08 taskID, uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen) //inType:MsgID for inner message, MType for outer message
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return -TLK_EPARAM;
	if(sTlkTskModInfs[taskID]->Input == nullptr) return -TLK_ENOSUPPORT;
	return sTlkTskModInfs[taskID]->Input(mtype, msgID, pData, dataLen);
}
int  tlktsk_modinf_extmsg(uint08 taskID, uint08 mtype, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return -TLK_EPARAM;
	if(sTlkTskModInfs[taskID]->ExtMsg == nullptr) return -TLK_ENOSUPPORT;
	return sTlkTskModInfs[taskID]->ExtMsg(mtype, msgID, pHead, headLen, pData, dataLen);
}
uint tlktsk_modinf_intvUs(uint08 taskID)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return 0xFFFF;
	return sTlkTskModInfs[taskID]->IntvUs();
}
void tlktsk_modinf_wakeup(uint08 taskID)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return;
	if(sTlkTskModInfs[taskID]->Wakeup == nullptr) return;
	sTlkTskModInfs[taskID]->Wakeup();
}
void tlktsk_modinf_handler(uint08 taskID)
{
	if(taskID == 0 || taskID >= TLKTSK_TASKID_MAX || sTlkTskModInfs[taskID] == nullptr) return;
	sTlkTskModInfs[taskID]->Handler();
}

bool tlktsk_modinf_isHave(const tlktsk_modinf_t *pModinf)
{
	uint08 index;
	if(pModinf == nullptr) return false;
	for(index=0; index<TLKTSK_TASKID_MAX; index++){
		if(sTlkTskModInfs[index] == pModinf) break;
	}
	if(index == TLKTSK_TASKID_MAX) return false;
	return true;
}

