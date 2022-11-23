/********************************************************************************************************
 * @file     tlkmmi_audioStatus.c
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
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_AUDIO_ENABLE)
#include "tlkmmi/audio/tlkmmi_audioCtrl.h"
#include "tlkmmi/audio/tlkmmi_audioComm.h"
#include "tlkmmi/audio/tlkmmi_audioStatus.h"
#include "tlkmmi/audio/tlkmmi_audioModinf.h"



#if (TLKMDI_CFG_AUDTONE_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioToneFunc = {
	TLKMMI_AUDIO_OPTYPE_TONE, TLKMMI_AUDIO_TONE_PRIORITY, true,  true
};
#endif
#if (TLKMDI_CFG_AUDPLAY_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioPlayFunc = {
	TLKMMI_AUDIO_OPTYPE_PLAY, TLKMMI_AUDIO_PLAY_PRIORITY, false, false
};
#endif
#if (TLKMDI_CFG_AUDHFP_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioHfFunc = {
	TLKMMI_AUDIO_OPTYPE_HF,  TLKMMI_AUDIO_HFP_PRIORITY,   false, false
};
#endif
#if (TLKMDI_CFG_AUDHFP_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioAgFunc = {
	TLKMMI_AUDIO_OPTYPE_AG,  TLKMMI_AUDIO_HFP_PRIORITY,   false, false
};
#endif
#if (TLKMDI_CFG_AUDSCO_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioScoFunc = {
	TLKMMI_AUDIO_OPTYPE_SCO,  TLKMMI_AUDIO_SCO_PRIORITY,  true,  true
};
#endif
#if (TLKMDI_CFG_AUDSRC_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioSrcFunc = {
	TLKMMI_AUDIO_OPTYPE_SRC,  TLKMMI_AUDIO_SRC_PRIORITY,  false, false
};
#endif
#if (TLKMDI_CFG_AUDSNK_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioSnkFunc = {
	TLKMMI_AUDIO_OPTYPE_SNK,  TLKMMI_AUDIO_SNK_PRIORITY,  false, false
};
#endif
#if (TLKMDI_CFG_AUDUAC_ENABLE)
static const tlkmmi_audio_statusFunc_t sTlkMMidAudioUacFunc = {
	TLKMMI_AUDIO_OPTYPE_UAC,  TLKMMI_AUDIO_UAC_PRIORITY,  false, false
};
#endif
static const tlkmmi_audio_statusFunc_t *spTlkMMidAudioFuncs[TLKMMI_AUDIO_OPTYPE_MAX] = {
	nullptr,
#if (TLKMDI_CFG_AUDTONE_ENABLE)
	&sTlkMMidAudioToneFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDPLAY_ENABLE)
	&sTlkMMidAudioPlayFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDHFP_ENABLE)
	&sTlkMMidAudioHfFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDHFP_ENABLE)
	&sTlkMMidAudioAgFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDSCO_ENABLE)
	&sTlkMMidAudioScoFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDSRC_ENABLE)
	&sTlkMMidAudioSrcFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDSNK_ENABLE)
	&sTlkMMidAudioSnkFunc,
#else
	nullptr,
#endif
#if (TLKMDI_CFG_AUDUAC_ENABLE)
	&sTlkMMidAudioUacFunc,
#else
	nullptr,
#endif
};
static tlkmmi_audio_statusCtrl_t sTlkMMidAudioStatusCtrl = {0};


/******************************************************************************
 * Function: tlkmmi_audio_statusInit
 * Descript: Initial all of audio status control block. 
 * Params: None.
 * Return: Return TLK_ENONE is success and other value is filure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_statusInit(void)
{
	tmemset(&sTlkMMidAudioStatusCtrl, 0, sizeof(tlkmmi_audio_statusCtrl_t));
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_getCurOptype
 * Descript: Get the operate type of the Current of audio status control block 
 *           in it's array. 
 * Params: None.
 * Return: Retur the operate type.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_getCurOptype(void)
{
	if(sTlkMMidAudioStatusCtrl.nowNumb == 0) return TLKMMI_AUDIO_OPTYPE_NONE;
	else return sTlkMMidAudioStatusCtrl.item[0].optype;
}

/******************************************************************************
 * Function: tlkmmi_audio_isHaveOptype
 * Descript: Check the audio status control block array have match 
 *           the special operate type.
 * Params: None.
 * Return: Retur true(there exist a audio control block's 
 *         opertate type equal the optye).
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isHaveOptype(uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioStatusCtrl.nowNumb; index++){
		if(sTlkMMidAudioStatusCtrl.item[index].optype == optype) break;
	}
	if(index == sTlkMMidAudioStatusCtrl.nowNumb) return false;
	else return true;
}

/******************************************************************************
 * Function: tlkmmi_audio_getCurStatus
 * Descript: Get the current audio status control block.
 * Params: None.
 * Return: Retur current audio control block.
 * Others: None.
*******************************************************************************/
tlkmmi_audio_statusItem_t *tlkmmi_audio_getCurStatus(void)
{
	if(sTlkMMidAudioStatusCtrl.nowNumb == 0) return nullptr;
	else return &sTlkMMidAudioStatusCtrl.item[0];
}

/******************************************************************************
 * Function: tlkmmi_audio_getSndStatus
 * Descript: Get the 2nd audio status control block.
 * Params: None.
 * Return: Retur 2nd audio control block.
 * Others: None.
*******************************************************************************/
tlkmmi_audio_statusItem_t *tlkmmi_audio_getSndStatus(void)
{
	if(sTlkMMidAudioStatusCtrl.nowNumb < 2) return nullptr;
	else return &sTlkMMidAudioStatusCtrl.item[1];
}

/******************************************************************************
 * Function: tlkmmi_audio_statusIndex
 * Descript: Get the current audio status control block index. 
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: The audio status control block index.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_statusIndex(uint16 handle, uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioStatusCtrl.nowNumb; index++){
//		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_statusIndex: %d, %d, %d, %d", 
//			optype, handle, sTlkMMidAudioStatusCtrl.item[index].optype, sTlkMMidAudioStatusCtrl.item[index].handle);
		if(sTlkMMidAudioStatusCtrl.item[index].optype == optype && sTlkMMidAudioStatusCtrl.item[index].handle == handle){
			break;
		}
	}
	if(index == sTlkMMidAudioStatusCtrl.nowNumb) return -TLK_ENOOBJECT;
	return index;
}
int tlkmmi_audio_statusIndexByOptype(uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioStatusCtrl.nowNumb; index++){
		if(sTlkMMidAudioStatusCtrl.item[index].optype == optype){
			break;
		}
	}
	if(index == sTlkMMidAudioStatusCtrl.nowNumb) return -TLK_ENOOBJECT;
	return index;
}

/******************************************************************************
 * Function: tlkmmi_audio_insertStatus
 * Descript: Suspend the current audio task when a new audio task insert 
 *           and start running.
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_insertStatus(uint16 handle, uint08 optype)
{
	bool isSucc;
	uint08 iIndex;
	uint08 jIndex;
	uint16 tmpHdl;
	const tlkmmi_audio_statusFunc_t *pFunc0, *pFunc1;

	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: prev status - ");
	tlkmmi_audio_printfStatus();
	
	if(optype >= TLKMMI_AUDIO_OPTYPE_MAX || optype == 0){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure - error optype %d", optype);
		return -TLK_EPARAM;
	}
	if(tlkmmi_audio_statusIndex(handle, optype) >= 0){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure - repeat insert %d", optype);
		return -TLK_EREPEAT;
	}
	if(sTlkMMidAudioStatusCtrl.nowNumb >= TLKMMI_AUDIO_STATUS_ITEM_NUMB){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure - status full %d", optype);
		return -TLK_EQUOTA;
	}
	
	pFunc0 = spTlkMMidAudioFuncs[optype];
	if(pFunc0 == nullptr){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure - not supported %d", optype);
		return -TLK_ENOSUPPORT;
	}
	
	if(sTlkMMidAudioStatusCtrl.nowNumb == 0){
		//If there is no record in the status list, insert it to the first position
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus[0]: success - %d", optype);
		sTlkMMidAudioStatusCtrl.item[0].handle = handle;
		sTlkMMidAudioStatusCtrl.item[0].optype = optype;
		sTlkMMidAudioStatusCtrl.item[0].priority = pFunc0->priority;
		isSucc = tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_OPENED);
		if(!isSucc){
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure -- open 01 - %d", pFunc0->optype);
			return -TLK_EFAIL;
		}else{
			sTlkMMidAudioStatusCtrl.nowNumb ++;
			tlkmmi_audio_optypeChanged(pFunc0->optype, handle, 0, 0);
			return TLK_ENONE;
		}
	}
	
	pFunc1 = spTlkMMidAudioFuncs[sTlkMMidAudioStatusCtrl.item[0].optype];
	if(pFunc1 == nullptr) tmpHdl = 0;
	else tmpHdl = sTlkMMidAudioStatusCtrl.item[0].handle;
	if(pFunc0->isFirst && pFunc1 != nullptr && pFunc0->priority <= pFunc1->priority){
		//If the state to be inserted is restricted to the first position, but the first position does not allow it to be inserted, failure is returned
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure - low priority - %d", optype);
		return -TLK_EFAIL;
	}
	for(iIndex=0; iIndex<sTlkMMidAudioStatusCtrl.nowNumb; iIndex++){
		if(sTlkMMidAudioStatusCtrl.item[iIndex].priority < pFunc0->priority) break;
	}
	if(iIndex == 0 && pFunc1->isMutex){
		//If the insertion position is the first, and the state of the first position does not allow the back row, the state is removed directly
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus[1]: success - %d", optype);
		sTlkMMidAudioStatusCtrl.item[0].handle = handle;
		sTlkMMidAudioStatusCtrl.item[0].optype = optype;
		sTlkMMidAudioStatusCtrl.item[0].priority = pFunc0->priority;
		tlkmmi_audio_modinfSwitch(pFunc1->optype, tmpHdl, TLK_STATE_CLOSED);
		isSucc = tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_OPENED);
		if(!isSucc){
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure -- open 02 - %d", pFunc0->optype);
			tlkmmi_audio_removeStatus(handle, optype);
			return -TLK_EFAIL;
		}else{
			tlkmmi_audio_optypeChanged(pFunc0->optype, handle, pFunc1->optype, tmpHdl);
			return TLK_ENONE;
		}
	}
	if(iIndex != 0) pFunc1 = nullptr;
	if(iIndex != sTlkMMidAudioStatusCtrl.nowNumb){
		//When inserting in the middle position, we need to move the state after the insertion position.
		for(jIndex=sTlkMMidAudioStatusCtrl.nowNumb; jIndex>iIndex; jIndex--){
			tmemcpy(&sTlkMMidAudioStatusCtrl.item[jIndex], &sTlkMMidAudioStatusCtrl.item[jIndex-1], sizeof(tlkmmi_audio_statusItem_t));
		}
	}
	
	sTlkMMidAudioStatusCtrl.item[iIndex].handle = handle;
	sTlkMMidAudioStatusCtrl.item[iIndex].optype = optype;
	sTlkMMidAudioStatusCtrl.item[iIndex].priority = pFunc0->priority;
	sTlkMMidAudioStatusCtrl.nowNumb ++;
	if(pFunc1 != nullptr){
		tlkmmi_audio_modinfSwitch(pFunc1->optype, tmpHdl, TLK_STATE_PAUSED);
	}
	if(iIndex == 0){
		isSucc = tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_OPENED);
		if(isSucc){
			tlkmmi_audio_optypeChanged(pFunc0->optype, handle, pFunc1->optype, tmpHdl);
		}else{
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: failure -- open 03 - %d", pFunc0->optype);
			tlkmmi_audio_removeStatus(handle, pFunc0->optype);
			return -TLK_EFAIL;
		}
	}
	
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus[2]: success - %d", optype);
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertStatus: next status - ");
	tlkmmi_audio_printfStatus();
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_removeStatus
 * Descript: Suspend the current task and delete it.Then resume the task 
 *           which been suspend before.
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: Operating results. TLK_ENONE means success, others means failture.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_removeStatus(uint16 handle, uint08 optype)
{
	int index;
	bool isSucc;
	uint16 tmpHdl;
	const tlkmmi_audio_statusFunc_t *pFunc0; //To be removed
	const tlkmmi_audio_statusFunc_t *pFunc1; //reopen

//	tlkmmi_audio_printfStatus();

	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeStatus:{handle-%d,optype-%d}", handle, optype);

	index = tlkmmi_audio_statusIndex(handle, optype);
	if(index < 0){
		tlkapi_error(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeStatus: failure -- no node - %d", optype);
		return -TLK_ENOOBJECT;
	}

	tmpHdl = 0;
	pFunc0 = nullptr;
	pFunc1 = nullptr;
	if(index == 0) pFunc0 = spTlkMMidAudioFuncs[sTlkMMidAudioStatusCtrl.item[index].optype]; 
	if(index+1 != sTlkMMidAudioStatusCtrl.nowNumb){
		tmemcpy(&sTlkMMidAudioStatusCtrl.item[index], &sTlkMMidAudioStatusCtrl.item[index+1], (sTlkMMidAudioStatusCtrl.nowNumb-index-1)*sizeof(tlkmmi_audio_statusItem_t));
	}
	sTlkMMidAudioStatusCtrl.nowNumb --;
	if(index == 0 && sTlkMMidAudioStatusCtrl.nowNumb != 0){
		pFunc1 = spTlkMMidAudioFuncs[sTlkMMidAudioStatusCtrl.item[0].optype];
		tmpHdl = sTlkMMidAudioStatusCtrl.item[0].handle;
	}
	sTlkMMidAudioStatusCtrl.item[sTlkMMidAudioStatusCtrl.nowNumb].handle = 0;
	sTlkMMidAudioStatusCtrl.item[sTlkMMidAudioStatusCtrl.nowNumb].optype = 0;
	sTlkMMidAudioStatusCtrl.item[sTlkMMidAudioStatusCtrl.nowNumb].priority = 0;
	
	if(pFunc0 != nullptr) tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_CLOSED);
	if(pFunc1 != nullptr){
		isSucc = tlkmmi_audio_modinfSwitch(pFunc1->optype, tmpHdl, TLK_STATE_OPENED);
		if(!isSucc){
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeStatus: failure -- reopen %d", pFunc1->optype);
			tlkmmi_audio_removeStatus(pFunc1->optype, tmpHdl);
			return -TLK_EFAIL;
		}else{
			tlkmmi_audio_optypeChanged(pFunc1->optype, tmpHdl, optype, handle);
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeStatus:changed - %d  %d  %d  %d", 
				pFunc1->optype, tmpHdl, optype, handle);
		}
	}
	if(sTlkMMidAudioStatusCtrl.nowNumb == 0){
		tlkmmi_audio_optypeChanged(0, 0, optype, handle);
	}
	
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeStatus: optype-%d", optype);
	tlkmmi_audio_printfStatus();
	return index;
}

bool tlkmmi_audio_statusIsSwitch(uint08 dstOptype, uint08 srcOptype)
{
	const tlkmmi_audio_statusFunc_t *pDstFunc;
	const tlkmmi_audio_statusFunc_t *pSrcFunc;

	if(dstOptype >= TLKMMI_AUDIO_OPTYPE_MAX) pDstFunc = nullptr;
	else pDstFunc = spTlkMMidAudioFuncs[dstOptype];
	if(srcOptype >= TLKMMI_AUDIO_OPTYPE_MAX) pSrcFunc = nullptr;
	else pSrcFunc = spTlkMMidAudioFuncs[srcOptype];
	if(pDstFunc == nullptr) return false;
	if(pSrcFunc == nullptr || pDstFunc->priority > pSrcFunc->priority) return true;
	return false;
}

/******************************************************************************
 * Function: tlkmmi_audio_removeStatusByOptype
 * Descript: Remove the special audio status control block by it's opreate type.
 * Params: @optype[IN]--The operate type.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_removeStatusByOptype(uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioStatusCtrl.nowNumb; index++){
		if(sTlkMMidAudioStatusCtrl.item[index].optype == optype){
			tlkmmi_audio_removeStatus(sTlkMMidAudioStatusCtrl.item[index].handle, optype);
			index = 0;
		}
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_removeStatusByHandle
 * Descript: Remove the special audio status control block by it's handle.
 * Params: @handle[IN]--The handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_removeStatusByHandle(uint16 handle)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioStatusCtrl.nowNumb; index++){
		if(sTlkMMidAudioStatusCtrl.item[index].handle == handle){
			tlkmmi_audio_removeStatus(handle, sTlkMMidAudioStatusCtrl.item[index].optype);
			index = 0;
		}
	}
}

void tlkmmi_audio_printfStatus(void)
{
	uint08 index;
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_printfStatus: nowNumb-%d", sTlkMMidAudioStatusCtrl.nowNumb);
	for(index=0; index<sTlkMMidAudioStatusCtrl.nowNumb; index++){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "      item{index-%d,handle-%d,optype-%d,prio-%d}", index, sTlkMMidAudioStatusCtrl.item[index].handle,
			sTlkMMidAudioStatusCtrl.item[index].optype, sTlkMMidAudioStatusCtrl.item[index].priority);
	}
}


#endif //#if (TLKMMI_AUDIO_ENABLE)

