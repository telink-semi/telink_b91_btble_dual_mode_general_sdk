/********************************************************************************************************
 * @file     bth_funcs.c
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
#include "types.h"
#include "tlksys/prt/tlkpto_comm.h"
#if (TLK_STK_BTH_ENABLE)
#include "bth_stdio.h"
#include "bth_l2cap.h"
#include "bth_signal.h"
#include "bth_funcs.h"
#include "bth_acl.h"
#include "bth_sco.h"

static const bth_func_item_t scBthFunSet[] = {
	//acl
	{TLK_FUNC_TYPE_BTH_ACL,0x01,bth_FuncAcl_Conn},
	{TLK_FUNC_TYPE_BTH_ACL,0x02,bth_FuncAcl_ConnCancel},
	{TLK_FUNC_TYPE_BTH_ACL,0x03,bth_FuncAcl_ConnCancelComplete},
	{TLK_FUNC_TYPE_BTH_ACL,0X04,bth_FuncAcl_Disc},
	{TLK_FUNC_TYPE_BTH_ACL,0X05,bth_FuncAcl_DiscByAddr},
	//sco
	{TLK_FUNC_TYPE_BTH_SCO,0x01,bth_FuncSco_Conn},
	{TLK_FUNC_TYPE_BTH_SCO,0x02,bth_FuncSco_Disc},
};
int bth_FuncAcl_Conn(uint08 *pData, uint16 dataLen)
{
	int ret = 0;
	uint08 btAddr[6] = {0};
	uint32 devClass = 0;
	uint08 initRole;
	uint16 timeout;
	if(pData == nullptr || dataLen < 13)
	{
		return -TLK_EPARAM;
	}

	tmemcpy(btAddr, pData, 6);
	
	devClass |= (pData[9] << 24) & 0xFF000000;
	devClass |= (pData[8] << 16) & 0xFF0000;
	devClass |= (pData[7] <<  8) & 0xFF00;
	devClass |= pData[6] & 0xFF;
	initRole = pData[10];
	timeout = (((uint16)pData[12] << 8) | pData[11])*100;

	ret = bth_acl_connect(btAddr, devClass, initRole,timeout);

	return ret;
}
int bth_FuncAcl_ConnCancel(uint08 *pData,uint16 dataLen)
{
	int ret = 0;
	uint08 btAddr[6] = {0};
	if(pData == nullptr || dataLen < 6)
		return -TLK_EPARAM;
	tmemcpy(btAddr,pData,6);
	ret = bth_acl_connectCancel(pData);

	return ret;
}
int bth_FuncAcl_ConnCancelComplete(uint08 *pData,uint16 dataLen)
{
	int ret = 0;
	uint08 status;
	uint08 btAddr[6] = {0};
	if(pData == nullptr || dataLen < 7)
		return -TLK_EPARAM;

	status = pData[0];
	tmemcpy(btAddr, pData+1, 6);

	bth_acl_connCancelComplete(status, btAddr);

	return ret;
}
int bth_FuncAcl_Disc(uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 handle;

	if(pData == nullptr || dataLen < 2)
		return -TLK_EPARAM;

	handle = ((uint16)pData[1]<<8) | pData[0];
	ret = bth_acl_disconn(handle);

	return ret;
}
int bth_FuncAcl_DiscByAddr(uint08 *pData, uint16 dataLen)
{
	int ret = 0;
	uint08 btAddr[6] = {0};
	if(pData == nullptr || dataLen < 6)
		return -TLK_EPARAM;
	tmemcpy(btAddr,pData,6);
	ret = bth_acl_disconnByAddr(btAddr);
	return ret;
}
int bth_FuncSco_Conn(uint08 *pData, uint16 dataLen)
{
	int ret = 0;
	uint16 handle;
	uint16 linkType;
	uint08 airMode;
	if(pData == nullptr || dataLen < 5)
		return -TLK_EPARAM;
	
	handle = ((uint16)pData[1]<<8 | pData[0]);
	linkType = ((uint16)pData[3]<<8 | pData[2]);
	airMode = pData[4];
	ret = bth_sco_connect(handle,linkType, airMode);
	return ret;
}
int bth_FuncSco_Disc(uint08 *pData,uint16 dataLen)
{
	int ret = 0;
	uint16 handle;
	uint08 reason;
	if(pData == nullptr || dataLen < 3)
		return -TLK_EPARAM;
	handle = ((uint16)pData[1]<<8 | pData[0]);
	reason = pData[2];

	ret = bth_sco_disconn(handle,reason);

	return ret;
}
int bth_getSetNum()
{
	return sizeof(scBthFunSet)/sizeof(scBthFunSet[0]);
}
int bth_FuncCall(uint08 funcType, uint08 funcID, uint08 *pData, uint16 dataLen)
{
	int ret = 0;
	if(funcID > TLK_FUNC_TYPE_BTH_MAX)
		return -TLK_EPARAM;
	for(int i = 0;i<bth_getSetNum();i++)
	{
		if(scBthFunSet[i].type == funcType && scBthFunSet[i].funID == funcID)ret = scBthFunSet[i].Func(pData,dataLen);
	}
	return ret;
}



#endif //#if (TLK_STK_BTH_ENABLE)

