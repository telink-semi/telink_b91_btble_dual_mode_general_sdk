/********************************************************************************************************
 * @file     tlkusb_msc.h
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
#ifndef TLKUSB_MSC_DESC_H
#define TLKUSB_MSC_DESC_H

#if (TLK_USB_MSC_ENABLE)


//#define TLK_DEV_XTSD04G_ENABLE             (0 && TLK_CFG_FS_ENABLE)
//#define TLK_DEV_XT2602E_ENABLE             (1 && TLK_CFG_FS_ENABLE)


#define TLKUSB_MSC_STRING_PRODUCT       L"Telink B91 BTBLE Storage"
#define TLKUSB_MSC_STRING_SERIAL0       L"TLSR9218-XTSD04G0"
#define TLKUSB_MSC_STRING_SERIAL1       L"TLSR9218-XT2602E"


typedef struct {
	tlkusb_stdConfigureDesc_t config;
//	tlkusb_stdAssociateDesc_t audIAD;
	tlkusb_stdInterfaceDesc_t mscInf;
	tlkusb_stdEndpointDesc_t  mscEdpIn;
	tlkusb_stdEndpointDesc_t  mscEdpOut;
}tlkusb_mscConfigDesc_t;





#endif //#if (TLK_USB_MSC_ENABLE)

#endif //TLKUSB_MSC_DESC_H


