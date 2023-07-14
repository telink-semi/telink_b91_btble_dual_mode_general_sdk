/********************************************************************************************************
 * @file	tlkdrv_heartrate.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_heartRate.h"


static const tlkdrv_heartRate_modinf_t *tlkdrv_heartRate_getDev(TLKDRV_HEARTRATE_DEV_ENUM dev);

extern const tlkdrv_heartRate_modinf_t gcTlkDrvHx3602Infs;
static const tlkdrv_heartRate_modinf_t *spTlkDrvHeartRateModInfs[TLKDRV_HEARTRATE_DEV_MAX] = {
	#if (TLKDRV_HEARTRATE_HX3602_ENABLE)
	&gcTlkDrvHx3602Infs,
	#else
	nullptr,
	#endif
};



bool tlkdrv_heartRate_isOpen(TLKDRV_HEARTRATE_DEV_ENUM dev)
{
	const tlkdrv_heartRate_modinf_t *pModInf;
	pModInf = tlkdrv_heartRate_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}

int tlkdrv_heartRate_init(TLKDRV_HEARTRATE_DEV_ENUM dev)
{
	const tlkdrv_heartRate_modinf_t *pModInf;
	pModInf = tlkdrv_heartRate_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Init();
}
int tlkdrv_heartRate_open(TLKDRV_HEARTRATE_DEV_ENUM dev)
{
	const tlkdrv_heartRate_modinf_t *pModInf;
	pModInf = tlkdrv_heartRate_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_heartRate_close(TLKDRV_HEARTRATE_DEV_ENUM dev)
{
	const tlkdrv_heartRate_modinf_t *pModInf;
	pModInf = tlkdrv_heartRate_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_heartRate_reset(TLKDRV_HEARTRATE_DEV_ENUM dev)
{
	const tlkdrv_heartRate_modinf_t *pModInf;
	pModInf = tlkdrv_heartRate_getDev(dev);
	if(pModInf == nullptr || pModInf->Reset == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Reset();
}
int tlkdrv_heartRate_handler(TLKDRV_HEARTRATE_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_heartRate_modinf_t *pModInf;
	pModInf = tlkdrv_heartRate_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}





static const tlkdrv_heartRate_modinf_t *tlkdrv_heartRate_getDev(TLKDRV_HEARTRATE_DEV_ENUM dev)
{
	if(dev >= TLKDRV_HEARTRATE_DEV_MAX) return nullptr;
	return spTlkDrvHeartRateModInfs[dev];
}


