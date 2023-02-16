/********************************************************************************************************
 * @file     tlkmdi_iap.h
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
#ifndef TLKMDI_IAP_H
#define TLKMDI_IAP_H

#if (TLK_MDI_BTIAP_ENABLE)



typedef struct{
	uint08 detFlag;
	uint16 aclHandle;
	uint08 nameLen;
	uint08 name[32];
	uint08 bdaddr[6];
}tlkmdi_btiap_t;


int tlkmdi_btiap_init(void);

void tlkmdi_btiap_setAddr(uint08 bdaddr[6]);
void tlkmdi_btiap_setName(uint08 *pName, uint08 nameLen);
void tlkmdi_btiap_setAclHandle(bool isConn, uint16 aclHandle);



void tlkmdi_btiap_handler(void);




#endif //#if (TLK_MDI_BTIAP_ENABLE)

#endif //TLKMDI_IAP_H

