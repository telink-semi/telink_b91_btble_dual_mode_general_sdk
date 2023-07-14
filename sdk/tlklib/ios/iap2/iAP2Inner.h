/********************************************************************************************************
 * @file     iAP2Packet.c
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

#ifndef IAP2_INNER_H
#define IAP2_INNER_H

#if (TLKBTP_CFG_IAP_ENABLE)

#ifdef __cplusplus
extern "C" {
#endif
    

#define BTP_IAP_DBG_FLAG       ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_IAP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_IAP_DBG_SIGN       nullptr



#ifdef __cplusplus
}
#endif

#endif //#if (TLKBTP_CFG_IAP_ENABLE)

#endif /* #ifndef iAP2Link_iAP2Packet_h */
