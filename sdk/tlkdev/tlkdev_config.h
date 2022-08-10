/********************************************************************************************************
 * @file     tlkdev_config.h
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

#ifndef TLKDV_CONFIG_H
#define TLKDV_CONFIG_H




#define TLKDEV_SYS_BAT_ENABLE         1


#define TLKDEV_EXT_PUYA_P25Q32H_ENABLE         0


#ifndef TLK_DEV_XT26G0X_ENABLE
#define TLK_DEV_XT26G0X_ENABLE       (0 && TLK_CFG_DEV_ENABLE) //not support
#endif



#endif //TLKDV_CONFIG_H

