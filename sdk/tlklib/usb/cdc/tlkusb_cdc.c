/********************************************************************************************************
 * @file     tlkusb_msc.c
 *
 * @brief    This is the header file for BTBLE SDK
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
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLKUSB_CDC_ENABLE)
#include "tlklib/usb/cdc/tlkusb_cdcConfig.h"
#include "tlklib/usb/cdc/tlkusb_cdcDefine.h"
#include "tlklib/usb/cdc/tlkusb_cdc.h"
#include "drivers.h"


extern const tlkusb_modCtrl_t sTlkUsbCdcModCtrl;
extern const tlkusb_modDesc_t sTlkUsbCdcModDesc;
const tlkusb_module_t gTlkUsbCdcModule = {
	TLKUSB_MODTYPE_CDC,
	&sTlkUsbCdcModDesc,
	&sTlkUsbCdcModCtrl,
};


static TlkUsbCdcRecvCB sTlkUsbCdcRecvCB;

int tlkusb_cdc_init(void)
{
	return TLK_ENONE;
}

void tlkusb_cdc_regRecvCB(TlkUsbCdcRecvCB cb)
{
	sTlkUsbCdcRecvCB = cb;
}

int tlkusb_cdc_sendData(uint08 intfNum, uint08 *pData, uint08 dataLen)
{
	uint08 index;
	
	if(dataLen > TLKUSB_CDC_TXRX_EDPSIZE) return -TLK_ELENGTH;
	
	if(intfNum == TLKUSB_CDC_INF_CCI || intfNum == TLKUSB_CDC_INF_DCI){
		if(usbhw_is_ep_busy(TLKUSB_CDC_EDP_TX)) return -TLK_EBUSY;
		usbhw_reset_ep_ptr(TLKUSB_CDC_EDP_TX);
		for(index=0; index<dataLen; index++){
			usbhw_write_ep_data(TLKUSB_CDC_EDP_TX, pData[index]);
		}
		usbhw_data_ep_ack(TLKUSB_CDC_EDP_TX);
	}
	#if (TLKUSB_CDC_SECOND_ENABLE)
	else if(intfNum == TLKUSB_CDC2_INF_CCI || intfNum == TLKUSB_CDC2_INF_DCI){
		if(usbhw_is_ep_busy(TLKUSB_CDC2_EDP_TX)) return -TLK_EBUSY;
		usbhw_reset_ep_ptr(TLKUSB_CDC2_EDP_TX);
		for(index=0; index<dataLen; index++){
			usbhw_write_ep_data(TLKUSB_CDC2_EDP_TX, pData[index]);
		}
		usbhw_data_ep_ack(TLKUSB_CDC2_EDP_TX);
	}
	#endif
	else{
		return -TLK_EPARAM;
	}
	
	return TLK_ENONE;
}


#endif //#if (TLKUSB_MSC_ENABLE)


