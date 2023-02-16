/********************************************************************************************************
 * @file     tlktsk_define.h
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
#ifndef TLKTSK_DEFINE_H
#define TLKTSK_DEFINE_H



typedef enum{
	TLKTSK_PROCID_SYSTEM = 0, //System processes must come first
	TLKTSK_PROCID_AUDIO,
	TLKTSK_PROCID_FILEM,
	TLKTSK_PROCID_STACK,
	TLKTSK_PROCID_TEST,
	TLKTSK_PROCID_VIEW,
	TLKTSK_PROCID_MAX,
}TLKTSK_PROCID_ENUM;

typedef enum{
	TLKTSK_TASKID_NONE = 0,
	TLKTSK_TASKID_SYSTEM, //System task must come first
	TLKTSK_TASKID_STACK,
	TLKTSK_TASKID_AUDIO,
	TLKTSK_TASKID_BTMGR,
	TLKTSK_TASKID_LEMGR,
	TLKTSK_TASKID_LEMST,
	TLKTSK_TASKID_PHONE,
	TLKTSK_TASKID_FILE,
	TLKTSK_TASKID_TEST,
	TLKTSK_TASKID_VIEW,
	TLKTSK_TASKID_MAX,
}TLKTSK_TASKID_ENUM;


#define TLKTSK_PROC_AUDIO_SIZE        512
#define TLKTSK_PROC_FILEM_SIZE        512
#define TLKTSK_PROC_STACK_SIZE        1500
#define TLKTSK_PROC_SYSTEM_SIZE       512
#define TLKTSK_PROC_TEST_SIZE         1024
#define TLKTSK_PROC_VIEW_SIZE         1024



#endif //TLKTSK_DEFINE_H

