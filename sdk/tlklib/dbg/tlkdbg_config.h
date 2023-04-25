/********************************************************************************************************
 * @file     tlkdbgConfig.h
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
#ifndef TLKDBG_CONFIG_H
#define TLKDBG_CONFIG_H



#define TLKDBG_CFG_USB_LOG_ENABLE         (1 && TLK_CFG_DBG_ENABLE && TLK_CFG_USB_ENABLE)
#define TLKDBG_CFG_USB_VCD_ENABLE         (0 && TLK_CFG_VCD_ENABLE && TLK_CFG_USB_ENABLE)
#define TLKDBG_CFG_USB_DWN_ENABLE         (0 && TLK_CFG_DBG_ENABLE && TLK_CFG_USB_ENABLE)

//GSU - GPIO Simulate UART
#define TLKDBG_CFG_GSU_LOG_ENABLE         (0 && TLK_CFG_DBG_ENABLE)
#define TLKDBG_CFG_GSU_VCD_ENABLE         (0 && TLK_CFG_VCD_ENABLE)

//HPU - Hardware Protocol UART
#define TLKDBG_CFG_HPU_LOG_ENABLE         (1 && TLK_CFG_DBG_ENABLE && TLK_CFG_COMM_ENABLE)
#define TLKDBG_CFG_HPU_VCD_ENABLE         (0 && TLK_CFG_VCD_ENABLE && TLK_CFG_COMM_ENABLE)
#define TLKDBG_CFG_HPU_DWN_ENABLE         (1 && TLK_CFG_DBG_ENABLE && TLK_CFG_COMM_ENABLE) //DWN-Download

//HWU - Hardware UART
#define TLKDBG_CFG_HWU_LOG_ENABLE         (0 && TLK_CFG_DBG_ENABLE)
#define TLKDBG_CFG_HWU_VCD_ENABLE         (0 && TLK_CFG_VCD_ENABLE)



#endif //TLKDBG_CONFIG_H

