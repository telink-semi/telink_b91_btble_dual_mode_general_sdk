/********************************************************************************************************
 * @file     tlkapp_system.h
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

#ifndef TLKAPP_SYSTEM_H
#define TLKAPP_SYSTEM_H



#define APP_SYSTEM_BTNAME_LENS     32


typedef struct{
	uint08 isBoot;
	uint08 resv00;
	uint16 heartNumb;
	uint32 heartIntv; //HeartBeat Interval
	uint32 heartTimer;
	uint32 powerTimer;
}tlkapp_system_ctrl_t;



/******************************************************************************
 * Function: tlkapp_system_init
 * Descript: This function is used to initialize system parameters and configuration.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
int  tlkapp_system_init(void);

/******************************************************************************
 * Function: tlkapp_system_handler
 * Descript: This function to control the mechine power off or restart 
 *           and register the heart beat.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_system_handler(void);


/******************************************************************************
 * Function: tlkapp_system_poweroff
 * Descript: This Function for power off the meachine.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_system_poweroff(void);





#endif //TLKAPP_SYSTEM_H

