/********************************************************************************************************
 * @file     tlkmdi.h
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

#ifndef TLKMDI_H
#define TLKMDI_H




/******************************************************************************
 * Function: tlkmdi_init
 * Descript: Trigger to initial the comm(serial) block and adapt and event. 
 *           It is  also initial the acl and inquiry mp3 and so on.This is 
 *           the main interface to initial all the Bt basic function.
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_init(void);

/******************************************************************************
 * Function: tlkmdi_isbusy
 * Descript: Trigger to check is adapt is busy.
 * Params:
 * Return: Return true is success, other value is failure.
 * Others: None.
*******************************************************************************/
bool tlkmdi_isbusy(void);


bool tlkmdi_pmIsbusy(void);




#endif //TLKMDI_H

