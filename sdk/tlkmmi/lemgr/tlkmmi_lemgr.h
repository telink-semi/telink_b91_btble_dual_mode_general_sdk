/********************************************************************************************************
 * @file     tlkmmi_lemgr.h
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

#ifndef TLKMMI_LEMGR_H
#define TLKMMI_LEMGR_H

#if (TLKMMI_LEMGR_ENABLE)


#define TLKMMI_LEMGR_DBG_FLAG         (TLKMMI_LEMGR_DBG_ENABLE | TLKAPI_DBG_FLAG_ALL)
#define TLKMMI_LEMGR_DBG_SIGN         "[MMI]"



int tlkmmi_lemgr_init(void);





#endif //TLKMMI_LEMGR_ENABLE

#endif //TLKMMI_LEMGR_H

