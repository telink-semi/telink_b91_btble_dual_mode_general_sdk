/********************************************************************************************************
 * @file	tlkdrv_touch.c
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
#include "tlkdrv_touch.h"

static const tlkdrv_touch_modinf_t *tlkdrv_touch_getDev(TLKDRV_TOUCH_DEV_ENUM dev);

extern const tlkdrv_touch_modinf_t sTlkDrvFt3168ModInf;
static const tlkdrv_touch_modinf_t *spTlkDrvTouchModInf[TLKDRV_TOUCH_DEV_MAX] = {
	#if (TLKDRV_TOUCH_FT3168_ENABLE)
	&sTlkDrvFt3168ModInf,
	#else
	nullptr,
	#endif
};



bool tlkdrv_touch_isOpen(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}
bool tlkdrv_touch_isReady(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->IsReady == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsReady();
}

int tlkdrv_touch_init(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Init();
}
int tlkdrv_touch_open(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_touch_close(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_touch_getPoint(TLKDRV_TOUCH_DEV_ENUM dev, tlkdrv_touch_point_t *pPoint)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->GetPoint == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->GetPoint(pPoint);
}
int tlkdrv_touch_getGesture(TLKDRV_TOUCH_DEV_ENUM dev, tlkdrv_touch_gesture_t *pGesture)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->GetGesture == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->GetGesture(pGesture);
}
int tlkdrv_touch_handler(TLKDRV_TOUCH_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}


static const tlkdrv_touch_modinf_t *tlkdrv_touch_getDev(TLKDRV_TOUCH_DEV_ENUM dev)
{
	if(dev >= TLKDRV_TOUCH_DEV_MAX) return nullptr;
	return spTlkDrvTouchModInf[dev];
}


