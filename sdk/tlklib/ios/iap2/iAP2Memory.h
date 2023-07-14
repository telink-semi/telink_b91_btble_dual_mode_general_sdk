/********************************************************************************************************
 * @file     iAP2Memory.h
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
#ifndef IAP2_MEMORY_H
#define IAP2_MEMORY_H

#if (TLKBTP_CFG_IAP_ENABLE)


#define IAP2_MEMORY_BUFF_SIZE       (20*1024)


void iap2_memInit(void);

void *iap2_malloc(uint32 size);
void *iap2_calloc(uint32 size);
void iap2_free(void *ptr);



#endif //#if (TLKBTP_CFG_IAP_ENABLE)

#endif //IAP2_MEMORY_H

