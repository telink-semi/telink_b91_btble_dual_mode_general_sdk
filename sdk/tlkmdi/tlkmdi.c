/********************************************************************************************************
 * @file     tlkmdi.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmdi/tlkmdi_adapt.h"
#include "tlkmdi/tlkmdi_comm.h"
#include "tlkmdi/tlkmdi_event.h"
#include "tlkmdi/tlkmdi_btacl.h"
#include "tlkmdi/tlkmdi_btinq.h"
#include "tlkmdi/tlkmdi_btrec.h"
#include "tlkmdi/tlkmdi_hfp.h"
#include "tlkmdi/tlkmdi.h"


/******************************************************************************
 * Function: tlkmdi_init
 * Descript: Trigger to initial the comm(serial) block and adapt and event. 
 *           It is  also initial the acl and inquiry mp3 and so on.This is 
 *           the main interface to initial all the Bt basic function.
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_init(void)
{
	tlkmdi_comm_init();
	tlkmdi_adapt_init();
	tlkmdi_event_init();

	#if (TLK_MDI_BTACL_ENABLE)
	tlkmdi_btacl_init();
	#endif
	#if (TLK_MDI_BTINQ_ENABLE)
	tlkmdi_btinq_init();
	#endif 
	#if (TLK_MDI_BTREC_ENABLE)
	tlkmdi_btrec_init();
	#endif

	tlkmdi_audio_init();

	#if (TLK_MDI_HFP_ENABLE)
	tlkmdi_hfp_init();
	#endif
	#if (TLK_MDI_MP3_ENABLE)
	tlkmdi_mp3_init();
	#endif
	#if (TLKMDI_CFG_AUDTONE_ENABLE)
	tlkmdi_audtone_init();
	#endif
	#if (TLKMDI_CFG_AUDPLAY_ENABLE)
	tlkmdi_audplay_init();
	#endif
	#if (TLKMDI_CFG_AUDHFP_ENABLE)
	tlkmdi_audhfp_init();
	#endif
	#if (TLKMDI_CFG_AUDSCO_ENABLE)
	tlkmdi_audsco_init();
	#endif
	#if (TLKMDI_CFG_AUDSRC_ENABLE)
	tlkmdi_audsrc_init();
	#endif
	#if (TLKMDI_CFG_AUDSNK_ENABLE)
	tlkmdi_audsnk_init();
	#endif
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_process
 * Descript: Trigger to start the process and event handler.
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_process(void)
{
	tlkmdi_adapt_handler();
	tlkmdi_event_handler();
}

/******************************************************************************
 * Function: tlkmdi_isbusy
 * Descript: Trigger to check is adapt is busy.
 * Params:
 * Return: Return true is success, other value is failure.
 * Others: None.
*******************************************************************************/
bool tlkmdi_pmIsbusy(void)
{
	if(tlkmdi_event_count() != 0 || tlkmdi_adapt_isbusy()) return true;
	return false;
}

