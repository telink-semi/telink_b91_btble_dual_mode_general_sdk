/********************************************************************************************************
 * @file	tlkdrv_gsensor.c
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
#include "tlkdrv_gsensor.h"


static const tlkdrv_gsensor_modinf_t *tlkdrv_gsensor_getDev(TLKDRV_GSENSOR_DEV_ENUM dev);

#if (TLKDRV_GSENSOR_SC7A20_ENABLE)
extern const tlkdrv_gsensor_modinf_t gcTlkDrvSc7a20Infs;
#endif
#if (TLKDRV_GSENSOR_MMC5603_ENABLE)
extern const tlkdrv_gsensor_modinf_t gcTlkDrvMMC5603Infs;
#endif
#if (TLKDRV_GSENSOR_DSDA217_ENABLE)
extern const tlkdrv_gsensor_modinf_t gcTlkDrvDsda217Infs;
#endif
static const tlkdrv_gsensor_modinf_t *spTlkDrvSensorModInfs[TLKDRV_GSENSOR_DEV_MAX] = {
	#if (TLKDRV_GSENSOR_SC7A20_ENABLE)
	&gcTlkDrvSc7a20Infs,
	#else
	nullptr,
	#endif
	#if (TLKDRV_GSENSOR_MMC5603_ENABLE)
	&gcTlkDrvMMC5603Infs,
	#else
	nullptr,
	#endif
	#if (TLKDRV_GSENSOR_DSDA217_ENABLE)
	&gcTlkDrvDsda217Infs,
	#else
	nullptr,
	#endif
};



bool tlkdrv_gsensor_isOpen(TLKDRV_GSENSOR_DEV_ENUM dev)
{
	const tlkdrv_gsensor_modinf_t *pModInf;
	pModInf = tlkdrv_gsensor_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}

int tlkdrv_gsensor_init(TLKDRV_GSENSOR_DEV_ENUM dev)
{
	const tlkdrv_gsensor_modinf_t *pModInf;
	pModInf = tlkdrv_gsensor_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Init();
}
int tlkdrv_gsensor_open(TLKDRV_GSENSOR_DEV_ENUM dev)
{
	const tlkdrv_gsensor_modinf_t *pModInf;
	pModInf = tlkdrv_gsensor_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_gsensor_close(TLKDRV_GSENSOR_DEV_ENUM dev)
{
	const tlkdrv_gsensor_modinf_t *pModInf;
	pModInf = tlkdrv_gsensor_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_gsensor_reset(TLKDRV_GSENSOR_DEV_ENUM dev)
{
	const tlkdrv_gsensor_modinf_t *pModInf;
	pModInf = tlkdrv_gsensor_getDev(dev);
	if(pModInf == nullptr || pModInf->Reset == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Reset();
}
int tlkdrv_gsensor_handler(TLKDRV_GSENSOR_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_gsensor_modinf_t *pModInf;
	pModInf = tlkdrv_gsensor_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}





static const tlkdrv_gsensor_modinf_t *tlkdrv_gsensor_getDev(TLKDRV_GSENSOR_DEV_ENUM dev)
{
	if(dev >= TLKDRV_GSENSOR_DEV_MAX) return nullptr;
	return spTlkDrvSensorModInfs[dev];
}


