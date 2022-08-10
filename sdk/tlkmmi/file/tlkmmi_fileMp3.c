/********************************************************************************************************
 * @file     tlkmmi_fileCtrl.c
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
#include "tlk_config.h" 
#include "tlkapi/tlkapi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_FILE_ENABLE)
#include "tlkprt/tlkprt_stdio.h"
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_adapt.h"
#include "tlkmdi/tlkmdi_file.h"
#include "tlkmmi/file/tlkmmi_file.h"
#include "tlkmmi/file/tlkmmi_fileCtrl.h"
#include "tlkmmi/file/tlkmmi_fileMp3.h"


static int tlkmmi_file_mp3Init(void);
static int tlkmmi_file_mp3Start(tlkmdi_file_unit_t *pUnit, bool isFast);
static int tlkmmi_file_mp3Close(tlkmdi_file_unit_t *pUnit, uint08 status);
static int tlkmmi_file_mp3Param(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen);
static int tlkmmi_file_mp3Save(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen);


const tlkmmi_file_recvIntf_t gcTlkMmiFileMp3Intf = {
	TLKPRT_FILE_TYPE_MP3,
	tlkmmi_file_mp3Init,
	tlkmmi_file_mp3Start,
	tlkmmi_file_mp3Close,
	tlkmmi_file_mp3Param,
	tlkmmi_file_mp3Save,
};


static int tlkmmi_file_mp3Init(void)
{
	return TLK_ENONE;
}
static int tlkmmi_file_mp3Start(tlkmdi_file_unit_t *pUnit, bool isFast)
{
	return -TLK_ENOSUPPORT;
}
static int tlkmmi_file_mp3Close(tlkmdi_file_unit_t *pUnit, uint08 status)
{
	return -TLK_ENOSUPPORT;
}
static int tlkmmi_file_mp3Param(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkmmi_file_mp3Save(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}







#endif //#if (TLKMMI_FILE_ENABLE)

