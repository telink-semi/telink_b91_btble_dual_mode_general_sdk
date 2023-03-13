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
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmdi/misc/tlkmdi_usb.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/bt/tlkmdi_bt.h"
#if (TLK_MDI_FILE_ENABLE)
#include "tlkmdi/misc/tlkmdi_file.h"
#endif
#if (TLK_MDI_FS_ENABLE)
#include "tlkmdi/misc/tlkmdi_fs.h"
#endif
#if (TLK_MDI_KEY_ENABLE)
#include "tlkmdi/misc/tlkmdi_key.h"
#endif
#if (TLK_MDI_LED_ENABLE)
#include "tlkmdi/misc/tlkmdi_led.h"
#endif
#if (TLK_CFG_DBG_ENABLE)
#include "tlkmdi/misc/tlkmdi_debug.h"
#endif
#if (TLK_MDI_BATTERY_ENABLE)
#include "tlkmdi/misc/tlkmdi_battery.h"
#endif


#include "tlkmdi/tlkmdi.h"


extern uint tlkcfg_getWorkMode(void);


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
	tlkapi_timer_init();

	#if (TLK_CFG_COMM_ENABLE)
	tlkmdi_comm_init();
	#endif
	
	#if (TLK_MDI_DEBUG_ENABLE)
	tlkmdi_debug_init();
	#endif
	#if (TLK_MDI_KEY_ENABLE)
	tlkmdi_key_init();
	#endif
	#if (TLK_MDI_LED_ENABLE)
	tlkmdi_led_init();
	#endif
	#if (TLK_MDI_USB_ENABLE)
	tlkmdi_usb_init();
	#endif
	
	if(tlkcfg_getWorkMode() != TLK_WORK_MODE_NORMAL){
		return TLK_ENONE;
	}

	#if (TLK_MDI_FS_ENABLE)
	tlkmdi_fs_init();
	#endif
	#if (TLK_MDI_FILE_ENABLE)
	tlkmdi_file_init();
	#endif
	#if (TLK_MDI_BATTERY_ENABLE)
	tlkmdi_battery_init();
	#endif

	#if (TLK_STK_BT_ENABLE)
	tlkmdi_bt_init();
	#endif
		
	#if (TLK_MDI_AUDIO_ENABLE)
	tlkmdi_audio_init();
	#endif	
	
	
	return TLK_ENONE;
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
	if(0
		#if (TLK_MDI_DEBUG_ENABLE)
		|| tlkmdi_debug_pmIsBusy()
		#endif
		#if (TLK_CFG_COMM_ENABLE)
		|| tlkmdi_comm_pmIsBusy()
		#endif
	){
		return true;
	}
	return false;
}

