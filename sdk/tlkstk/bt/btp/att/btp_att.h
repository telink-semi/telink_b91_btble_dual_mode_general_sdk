/********************************************************************************************************
 * @file	btp_att.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef BTP_ATT_H
#define BTP_ATT_H

#if (TLKBTP_CFG_ATT_ENABLE)


#include "tlkstk/bt/btp/att/btp_attStdio.h"


typedef struct{
	uint08 attNum;
	uint08 permit;
	uint08 uuidLen;
	uint16 attrLen;
	uint08 *pUUID;
	uint08 *pAttr;
	uint08(*Read)(uint16 handle, uint16 chnID, uint08 *pData, uint16 dataLen);
	uint08(*Write)(uint16 handle, uint16 chnID, uint08 *pData, uint16 dataLen);
}btp_attItem_t, btp_gattItem_t;



extern int btp_att_init(void);


int btp_att_connect(uint16 handle, uint08 usrID);
int btp_att_disconn(uint16 handle, uint08 usrID);
void btp_att_destroy(uint16 handle);


extern int  btp_attsrv_setTable(const btp_attItem_t *pTable, uint16 count);
extern uint btp_attsrv_getChnID(uint16 aclHandle);



#endif //#if (TLKBTP_CFG_ATT_ENABLE)

#endif /* BTP_ATT_H */

