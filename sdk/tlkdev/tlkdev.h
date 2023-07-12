/********************************************************************************************************
 * @file	tlkdev.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef TLKDEV_H
#define TLKDEV_H


#define TLKDEV_DBG_FLAG         ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_DBG_SIGN         "[CDEV]"

#define TLKDEV_SYS_DBG_FLAG     ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV_SYS << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_SYS_DBG_SIGN     "[SDEV]"

#define TLKDEV_EXT_DBG_FLAG     ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_EXT_DBG_SIGN     "[EDEV]"

#define TLKDEV_USR_DBG_FLAG     ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV_USR << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_USR_DBG_SIGN     "[UDEV]"


int tlkdev_init(void);



#endif //TLKDEV_H

