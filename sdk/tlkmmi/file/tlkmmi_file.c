/********************************************************************************************************
 * @file     tlkmmi_file.c
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
#if (TLKMMI_FILE_ENABLE)
#include "tlkmdi/misc/tlkmdi_file.h"
#include "tlkmmi_file.h"
#include "tlkmmi_fileAdapt.h"
#include "tlkmmi_fileCtrl.h"
#include "tlkmmi_fileTask.h"




extern bool tlkmmi_file_dfuIsStart(void);



int tlkmmi_file_init(void)
{
	tlkmmi_file_taskInit();
	tlkmmi_file_adaptInit();
	tlkmmi_file_ctrlInit();
	return TLK_ENONE;
}


bool tlkmmi_file_isBusy(void)
{
	return tlkmmi_file_dfuIsStart();
}



#endif //#if (TLKMMI_FILE_ENABLE)

