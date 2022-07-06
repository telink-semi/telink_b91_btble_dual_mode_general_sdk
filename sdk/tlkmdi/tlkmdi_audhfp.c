/********************************************************************************************************
 * @file     tlkmdi_audhfp.c
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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#if (TLKMDI_CFG_AUDHFP_ENABLE)
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_audhfp.h"



#define TLKMDI_AUDHF_DBG_FLAG         (TLKMDI_HF_DBG_ENABLE | TLKMDI_DBG_FLAG) 
#define TLKMDI_AUDHF_DBG_SIGN         TLKMDI_DBG_SIGN


static bool tlkmdi_audhfp_timer(tlkapi_timer_t *pTimer, void *pUsrArg);


static tlkmdi_audhfp_ctrl_t sTlkMdiAudHfpCtrl;



/******************************************************************************
 * Function: tlkmdi_audhfp_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_audhfp_init(void)
{
	tlkmdi_adapt_initTimer(&sTlkMdiAudHfpCtrl.timer, tlkmdi_audhfp_timer, &sTlkMdiAudHfpCtrl, TLKMDI_AUDHF_TIMEOUT);
	
	
	return TLK_ENONE;
}

int tlkmdi_audhfp_start(uint16 handle)
{
	return -TLK_ENOSUPPORT;
}
int tlkmdi_audhfp_close(uint16 handle)
{
	return -TLK_ENOSUPPORT;
}

/******************************************************************************
 * Function: tlkmdi_audhfp_isBusy.
 * Descript: Get the HF control block is in-using.
 * Params: None.
 * Return: Return true or false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_isBusy(void)
{
	return (sTlkMdiAudHfpCtrl.status == TLK_STATE_OPENED);
}

/******************************************************************************
 * Function: tlkmdi_audhfp_intval.
 * Descript: Get the HF control block interval.
 * Params: None.
 * Return: Return the interval.
 * Others: None.
*******************************************************************************/
uint tlkmdi_audhfp_intval(void)
{
	return 1000;
}

/******************************************************************************
 * Function: tlkmdi_audhfp_switch.
 * Descript: Trigger to Change the HF control block status.
 * Params: None.
 * Return: Return true or false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_switch(uint16 handle, uint08 status)
{
	sTlkMdiAudHfpCtrl.status = status;
	sTlkMdiAudHfpCtrl.handle = handle;
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audhfp_irqProc.
 * Descript: Get the HF status.
 * Params: None.
 * Return: Return HF status.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_irqProc(void)
{
	if(sTlkMdiAudHfpCtrl.status == TLK_STATE_OPENED) return true;
	else return false;
}


static bool tlkmdi_audhfp_timer(tlkapi_timer_t *pTimer, void *pUsrArg)
{
	return false;
}



#endif //#if (TLKMDI_CFG_AUDHFP_ENABLE)

