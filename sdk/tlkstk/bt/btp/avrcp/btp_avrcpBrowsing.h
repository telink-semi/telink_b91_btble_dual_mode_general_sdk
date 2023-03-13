/********************************************************************************************************
 * @file     btp_avrcpBrowsing.h
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
#ifndef BTP_AVRCP_BROWSING_H
#define BTP_AVRCP_BROWSING_H

#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)



int btp_avrcp_browseInit(void);

int btp_avrcp_browseConnect(uint16 aclHandle);
int btp_avrcp_browseDisconn(uint16 aclHandle);
void btp_avrcp_browseDestroy(uint16 aclHandle);



#endif //#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)

#endif //BTP_AVRCP_BROWSING_H

