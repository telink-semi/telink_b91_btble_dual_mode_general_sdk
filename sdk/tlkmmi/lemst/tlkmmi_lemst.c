/********************************************************************************************************
 * @file     tlkmmi_lemst.c
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
#if (TLKMMI_LEMST_ENABLE)
#include "tlkmmi/lemst/tlkmmi_lemst.h"
#include "tlkmmi/lemst/tlkmmi_lemstAdapt.h"
#include "tlkmmi/lemst/tlkmmi_lemstCtrl.h"
#include "tlkmmi/lemst/tlkmmi_lemstAcl.h"
#include "tlkmmi/lemst/tlkmmi_lemstTask.h"




int tlkmmi_lemst_init(void)
{
	tlkmmi_lemst_taskInit();
	tlkmmi_lemst_adaptInit();
	tlkmmi_lemst_ctrlInit();
	tlkmmi_lemst_aclInit();
	
	return TLK_ENONE;
}








#endif //TLKMMI_LEMST_ENABLE

