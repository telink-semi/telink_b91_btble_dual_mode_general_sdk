/********************************************************************************************************
 * @file     tlkmmi.c
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
#include "tlkmmi/tlkmmi_stdio.h"
#include "tlkmmi/tlkmmi_adapt.h"
#include "tlkmmi/tlkmmi.h"


/******************************************************************************
 * Function: tlkmmi_init
 * Descript: 
 * Params:
 * Return: TLK_NONE means success,others means fail.
 * Others: None.
*******************************************************************************/
int tlkmmi_init(void)
{
	tlkmmi_adapt_init();
	#if (TLKMMI_AUDIO_ENABLE)
	tlkmmi_audio_init();
	#endif //TLKMMI_AUDIO_ENABLE
	#if (TLKMMI_BTMGR_ENABLE)
	tlkmmi_btmgr_init();
	#endif //TLKMMI_BTMGR_ENABLE
	#if (TLKMMI_LEMGR_ENABLE)
	tlkmmi_lemgr_init();
	#endif
	#if (TLKMMI_PHONE_ENABLE)
	tlkmmi_phone_init();
	#endif
	#if (TLKMMI_FILE_ENABLE)
	tlkmmi_file_init();
	#endif

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_process
 * Descript: 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_process(void)
{
	tlkmmi_adapt_handler();
}

/******************************************************************************
 * Function: tlkmmi_isbusy
 * Descript: 
 * Params:
 * Return: true/false.
 * Others: None.
*******************************************************************************/
bool tlkmmi_pmIsbusy(void)
{
	bool isBusy = false;
	#if (TLKMMI_FILE_ENABLE)
	if(!isBusy) isBusy = tlkmmi_file_isBusy();
	#endif
	#if (TLKMMI_AUDIO_ENABLE)
	if(!isBusy) isBusy = tlkmmi_audio_isBusy();
	#endif
	return isBusy;
}


