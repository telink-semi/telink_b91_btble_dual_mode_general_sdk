/********************************************************************************************************
 * @file     tlkmmi_phoneStatus.h
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

#ifndef TLKMMI_PHONE_STATUS_H
#define TLKMMI_PHONE_STATUS_H

#if (TLKMMI_PHONE_ENABLE)



/******************************************************************************
 * Function: tlkmmi_phone_statusInit
 * Descript: Register the phone call event callback.
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_statusInit(void);





#endif //#if (TLKMMI_PHONE_ENABLE)

#endif //TLKMMI_PHONE_STATUS_H

