/********************************************************************************************************
 * @file     tlkmmi_audioMsgInner.h
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
#ifndef TLKMMI_AUDIO_MSG_INNER_H
#define TLKMMI_AUDIO_MSG_INNER_H

#if (TLKMMI_AUDIO_ENABLE)


int tlkmmi_audio_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkmmi_audio_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmmi_audio_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen);

int tlkmmi_audio_innerMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen);



#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_MSG_INNER_H

