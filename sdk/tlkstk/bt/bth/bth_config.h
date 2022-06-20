/********************************************************************************************************
 * @file     bth_config.h
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

#ifndef BTH_CONFIG_H
#define BTH_CONFIG_H



#define BTH_L2CAP_MTU_SIZE                656//676


#define BTH_CFG_CMD_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)
#define BTH_CFG_EVT_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)
#define BTH_CFG_ACL_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)
#define BTH_CFG_SCO_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)
#define BTH_CFG_L2C_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)
#define BTH_CFG_SIG_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)

#define BTH_CFG_DEV_DBG_ENABLE            (1 && TLKBTH_CFG_DBG_ENABLE)






#endif //BTH_CONFIG_H

