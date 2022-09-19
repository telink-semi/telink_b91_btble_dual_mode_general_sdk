/********************************************************************************************************
 * @file     tlkdev_stdio.h
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

#ifndef TLKDEV_STDIO_H
#define TLKDEV_STDIO_H



#include "tlk_config.h"
#include "tlkdev/tlkdev_config.h"
#include "tlkdev/tlkdev_define.h"

//Device
#include "tlkdev/sys/tlkdev_bat.h"
#include "tlkdev/sys/tlkdev_codec.h"




extern int  tlkdev_init(void);
extern void tlkdev_process(void);
extern bool tlkdev_pmIsBusy(void);


#endif //TLKDEV_STDIO_H

