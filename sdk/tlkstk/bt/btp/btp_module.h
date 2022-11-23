/********************************************************************************************************
 * @file     btp_module.h
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

#ifndef BTP_MODULE_H
#define BTP_MODULE_H

#if (TLK_STK_BTP_ENABLE)



typedef struct{
	int(*Connect)(uint16 aclHandle, uint08 usrID);
	int(*Disconn)(uint16 aclHandle, uint08 usrID);
}btp_modinf_t;



int btp_module_connect(uint16 aclHandle, uint08 ptype, uint08 usrID, uint08 channel);
int btp_module_disconn(uint16 aclHandle, uint08 ptype, uint08 usrID);


#endif //#if (TLK_STK_BTP_ENABLE)

#endif //BTP_MODULE_H

