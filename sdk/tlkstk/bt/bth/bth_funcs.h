/********************************************************************************************************
 * @file     bth_funcs.h
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
#ifndef BTH_FUNC_SET_H
#define BTH_FUNC_SET_H

#if (TLK_STK_BTH_ENABLE)


typedef struct{
	uint08 type; //funcType
	uint16 funID;
	int(*Func)(uint08 *pData, uint16 dataLen);
}bth_func_item_t;

typedef enum{
	TLK_FUNC_TYPE_BTH = 0,
	TLK_FUNC_TYPE_BTH_ACL,
	TLK_FUNC_TYPE_BTH_SCO,
	TLK_FUNC_TYPE_BTH_DEV,
	TLK_FUNC_TYPE_BTH_CMD, //HCI-CMD
	TLK_FUNC_TYPE_BTH_EVT, //HCI-EVT
	TLK_FUNC_TYPE_BTH_L2C,
	TLK_FUNC_TYPE_BTH_SIG,
	TLK_FUNC_TYPE_BTH_MAX,
}TLK_FUNC_BTH_TYPE_ENUM;



int bth_FuncAcl_Conn(uint08 *pData, uint16 dataLen);
int bth_FuncAcl_ConnCancel(uint08 *pData,uint16 dataLen);
int bth_FuncAcl_ConnCancelComplete(uint08 *pData,uint16 dataLen);
int bth_FuncAcl_Disc(uint08 *pData, uint16 dataLen);
int bth_FuncAcl_DiscByAddr(uint08 *pData, uint16 dataLen);
int bth_FuncSco_Conn(uint08 *pData, uint16 dataLen);
int bth_FuncSco_Disc(uint08 *pData,uint16 dataLen);







int bth_scoConnectCall(uint08 pData, uint16 dataLen);


int bth_callFunc(uint08 funcType, uint08 funcID, uint08 *pData, uint16 dataLen);



#endif //#if (TLK_STK_BTH_ENABLE)

#endif //BTH_FUNC_SET_H

