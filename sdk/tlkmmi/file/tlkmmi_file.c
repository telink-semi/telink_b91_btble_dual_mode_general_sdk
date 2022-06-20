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
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_FILE_ENABLE)
#include "tlkmmi/file/tlkmmi_file.h"
#include "tlkmmi/file/tlkmmi_fileComm.h"
#include "tlkmmi/file/tlkmmi_fileCtrl.h"




int tlkmmi_file_init(void)
{
	tlkmmi_file_commInit();
	tlkmmi_file_ctrlInit();
	return TLK_ENONE;
}






#endif //#if (TLKMMI_FILE_ENABLE)

