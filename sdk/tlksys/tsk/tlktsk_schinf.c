/********************************************************************************************************
 * @file     tlktsk.c
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
#include "tlktsk_schinf.h"



static void tlktsk_schinf_process(void *pUsrArg);
static const tlktsk_schinf_t scTlkTskSchInfAudio = {
	1,//priority
	0,//reserved
	1024,//stackSize
	tlktsk_schinf_process,
};
static const tlktsk_schinf_t scTlkTskSchInfFileM = {
	2,//priority
	0,//reserved
	1024,//stackSize
	tlktsk_schinf_process,
};
static const tlktsk_schinf_t scTlkTskSchInfStack = {
	2,//priority
	0,//reserved
	1024,//stackSize
	tlktsk_schinf_process,
};
static const tlktsk_schinf_t scTlkTskSchInfSystem = {
	2,//priority
	0,//reserved
	1024,//stackSize
	tlktsk_schinf_process,
};
static const tlktsk_schinf_t scTlkTskSchInfTest = {
	2,//priority
	0,//reserved
	1024,//stackSize
	tlktsk_schinf_process,
};
static const tlktsk_schinf_t scTlkTskSchInfView = {
	2,//priority
	0,//reserved
	1024,//stackSize
	tlktsk_schinf_process,
};
static const tlktsk_schinf_t *spTlkTskSchInfs[TLKTSK_PROCID_MAX] = {
	&scTlkTskSchInfAudio,
	&scTlkTskSchInfFileM,
	&scTlkTskSchInfStack,
	&scTlkTskSchInfSystem,
	&scTlkTskSchInfTest,
	&scTlkTskSchInfView,
};
static uint08 sTlkTskSchTable[TLKTSK_PROCID_MAX][TLKTSK_CFG_TSK_NUM_IN_PROC];


int tlktsk_schinf_init(void)
{
	tmemset(sTlkTskSchTable, 0, TLKTSK_CFG_TSK_NUM_IN_PROC*TLKTSK_PROCID_MAX);
	return TLK_ENONE;
}
int tlktsk_schinf_mount(uint08 schID, uint08 tskID)
{
	uint08 index;
	
	if(schID >= TLKTSK_PROCID_MAX || tskID == 0) return -TLK_EPARAM;

	//Check for repeated mount
	for(index=0; index<TLKTSK_CFG_TSK_NUM_IN_PROC; index++){
		if(sTlkTskSchTable[schID][index] == tskID) break;
	}
	if(index != TLKTSK_CFG_TSK_NUM_IN_PROC) return -TLK_EREPEAT;

	//Finding an idle Node
	for(index=0; index<TLKTSK_CFG_TSK_NUM_IN_PROC; index++){
		if(sTlkTskSchTable[schID][index] == 0) break;
	}
	if(index == TLKTSK_CFG_TSK_NUM_IN_PROC) return -TLK_EQUOTA;
	
	sTlkTskSchTable[schID][index] = tskID;
	return TLK_ENONE;
}

int tlktsk_schinf_start(uint08 schID)
{
	return TLK_ENONE;
}
int tlktsk_schinf_pause(uint08 schID)
{
	return TLK_ENONE;
}
int tlktsk_schinf_close(uint08 schID)
{
	return TLK_ENONE;
}

int tlktsk_schinf_handler(uint08 schID)
{
	if(schID >= TLKTSK_PROCID_MAX || spTlkTskSchInfs[schID] == nullptr) return -TLK_EPARAM;
	spTlkTskSchInfs[schID]->Process(&schID);
	return TLK_ENONE;
}


static void tlktsk_schinf_process(void *pUsrArg)
{
	uint08 index;
	uint08 schID = ((uint08*)pUsrArg)[0];
	if(schID >= TLKTSK_PROCID_MAX) return;
	for(index=0; index<TLKTSK_CFG_TSK_NUM_IN_PROC; index++){
		if(sTlkTskSchTable[schID][index] == 0) break;
		tlktsk_modinf_handler(sTlkTskSchTable[schID][index]);
	}
}


