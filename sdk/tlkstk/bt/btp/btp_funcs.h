/********************************************************************************************************
 * @file     btp_funcs.h
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
#ifndef BTP_FUNCS_H
#define BTP_FUNCS_H

#if (TLK_STK_BTP_ENABLE)



typedef struct{
	uint08 type; //funcType
	uint16 funID;
	int(*Func)(uint08 *pData, uint16 dataLen);
}btp_func_item_t;

typedef enum{
	TLK_FUNC_TYPE_BTP = 0,
	TLK_FUNC_TYPE_BTP_SDP,
	TLK_FUNC_TYPE_BTP_SDPC,
	TLK_FUNC_TYPE_BTP_SDPS,
	TLK_FUNC_TYPE_BTP_RFCOMM,
	TLK_FUNC_TYPE_BTP_IAP,
	TLK_FUNC_TYPE_BTP_SPP,
	TLK_FUNC_TYPE_BTP_ATT,
	TLK_FUNC_TYPE_BTP_A2DP,
	TLK_FUNC_TYPE_BTP_A2DPS,
	TLK_FUNC_TYPE_BTP_A2DPC,
	TLK_FUNC_TYPE_BTP_HID,
	TLK_FUNC_TYPE_BTP_HIDS,
	TLK_FUNC_TYPE_BTP_HIDC,
	TLK_FUNC_TYPE_BTP_HFP,
	TLK_FUNC_TYPE_BTP_HFPAG,
	TLK_FUNC_TYPE_BTP_HFPHF,
	TLK_FUNC_TYPE_BTP_AVRCP,
	TLK_FUNC_TYPE_BTP_OBEX,
	TLK_FUNC_TYPE_BTP_PBAP,
	TLK_FUNC_TYPE_BTP_MAX,
}TLK_FUNC_BTP_TYPE_ENUM;



int btp_FuncCall(uint08 funcType, uint08 funcID, uint08 *pData, uint16 dataLen);


#endif //#if (TLK_STK_BTP_ENABLE)

#endif //BTP_FUNCS_H

