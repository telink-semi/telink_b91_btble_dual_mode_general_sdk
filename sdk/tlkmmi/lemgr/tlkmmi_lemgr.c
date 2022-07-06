/********************************************************************************************************
 * @file     tlkmmi_lemgr.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrComm.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrCtrl.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAcl.h"




int tlkmmi_lemgr_init(void)
{
	tlkmmi_lemgr_aclInit();
	tlkmmi_lemgr_commInit();
	tlkmmi_lemgr_ctrlInit();
	
	return TLK_ENONE;
}









#endif //TLKMMI_LEMGR_ENABLE
