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
#include "tlkmmi/tlkmmi.h"
#include "tlkmmi/stack/tlkmmi_stack.h"
#include "tlkmmi/audio/tlkmmi_audio.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
#include "tlkmmi/lemst/tlkmmi_lemst.h"
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/file/tlkmmi_file.h"
#include "tlkmmi/test/tlkmmi_test.h"
#include "tlkmmi/system/tlkmmi_sys.h"
#include "tlkmmi/view/tlkmmi_view.h"
#include "tlkmmi/test/tlkmmi_test.h"


extern unsigned char gTlkWorkMode;

/******************************************************************************
 * Function: tlkmmi_init
 * Descript: 
 * Params:
 * Return: TLK_NONE means success,others means fail.
 * Others: None.
*******************************************************************************/
int tlkmmi_init(void)
{
	#if (TLKMMI_SYSTEM_ENABLE)
	tlkmmi_sys_init();
	#endif
	#if (TLKMMI_STACK_ENABLE)
	tlkmmi_stack_init();
	#endif
	if(gTlkWorkMode != TLK_WORK_MODE_NORMAL){
		#if (TLKMMI_TEST_ENABLE)
		tlkmmi_test_init();
		#endif
		return TLK_ENONE;
	}
	
	#if (TLKMMI_AUDIO_ENABLE)
	tlkmmi_audio_init();
	#endif
	#if (TLKMMI_BTMGR_ENABLE)
	tlkmmi_btmgr_init();
	#endif
	#if (TLKMMI_LEMGR_ENABLE)
	tlkmmi_lemgr_init();
	#endif
	#if (TLKMMI_LEMST_ENABLE)
	tlkmmi_lemst_init();
	#endif
	#if (TLKMMI_PHONE_ENABLE)
	tlkmmi_phone_init();
	#endif
	#if (TLKMMI_FILE_ENABLE)
	tlkmmi_file_init();
	#endif
	#if (TLKMMI_VIEW_ENABLE)
	tlkmmi_view_init();
	#endif

	return TLK_ENONE;
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


