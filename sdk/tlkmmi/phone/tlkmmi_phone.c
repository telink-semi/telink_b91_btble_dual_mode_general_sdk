/********************************************************************************************************
 * @file     tlkmmi_phone.c
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
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_PHONE_ENABLE)
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneComm.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"
#include "tlkmmi/phone/tlkmmi_phoneStatus.h"




/******************************************************************************
 * Function: tlkmmi_phone_init
 * Descript: Trigger the phone to initial, including the phone control block, 
 *           and phone status,phone book and comm. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_init(void)
{
	tlkmmi_phone_ctrlInit();
	tlkmmi_phone_commInit();
	tlkmmi_phone_bookInit();
	tlkmmi_phone_statusInit();
	

	return TLK_ENONE;
}




#endif //#if (TLKMMI_PHONE_ENABLE)

