/********************************************************************************************************
 * @file     tlkmmi.h
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

#ifndef TLKMMI_H
#define TLKMMI_H



/******************************************************************************
 * Function: tlkmmi_init
 * Descript: 
 * Params:
 * Return: TLK_NONE means success,others means fail.
 * Others: None.
*******************************************************************************/
int  tlkmmi_init(void);

/******************************************************************************
 * Function: tlkmmi_process
 * Descript: 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_process(void);

/******************************************************************************
 * Function: tlkmmi_isbusy
 * Descript: 
 * Params:
 * Return: true/false.
 * Others: None.
*******************************************************************************/
bool tlkmmi_pmIsbusy(void);



#endif //TLKMMI_H

