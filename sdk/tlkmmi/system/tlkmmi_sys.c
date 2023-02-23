/********************************************************************************************************
 * @file     tlkmmi_sys.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysAdapt.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysDev.h"
#include "tlkmmi_sysDbg.h"
#include "tlkmmi_sysTask.h"

tlkapi_timer_t gTlkMmiSysDbgTimer;
bool tlkmmi_sys_timer(tlkapi_timer_t *pTimer,uint32 userArg);
extern tlk_debug_info_t **tlk_debug_get_dbgInfo();
extern int tlkmmi_sys_commDbgItemEvtSend(TLK_DEBUG_MAJOR_ID_ENUM majorId);


int tlkmmi_sys_init(void)
{
	tlkmmi_sys_taskInit();
	tlkmmi_sys_adaptInit();
	tlkmmi_sys_ctrlInit();
	tlkmmi_sys_dbgInit();
	tlkmmi_sys_devInit();
	tlkmmi_sys_adaptInitTimer(&gTlkMmiSysDbgTimer, tlkmmi_sys_timer, NULL, TLKMMI_SYS_TIMEOUT);
	
	return TLK_ENONE;
}

bool tlkmmi_sys_timer(tlkapi_timer_t *pTimer,uint32 userArg)
{

	static int majorID = 0;

	int unitCnt = 0;
	if(majorID == TLK_MAJOR_DBGID_MAX)
	{
		majorID = 0;
		return false;
	}
#if (TLK_CFG_DBG_ENABLE)
	if(tlk_debug_get_dbgInfo()[majorID] == nullptr)
	{
		majorID++;
		return true;
	}
	unitCnt = tlk_debug_get_dbgInfo()[majorID]->unitCnt;
	if( tlkmmi_sys_commDbgItemEvtSend(majorID) >= unitCnt)
	{
		majorID++;
	}
	
	return true;
#else
	return TLK_ENONE;
#endif
}

void tlkmmi_sys_start()
{
	tlkmmi_sys_adaptInsertTimer(&gTlkMmiSysDbgTimer);
}
void tlkmmi_sys_close()
{
	tlkmmi_sys_adaptRemoveTimer(&gTlkMmiSysDbgTimer);
}

#endif //#if (TLKMMI_SYSTEM_ENABLE)

