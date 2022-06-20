/********************************************************************************************************
 * @file     tlkmmi_fileCtrl.h
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
#ifndef TLKMMI_FILE_CTRL_H
#define TLKMMI_FILE_CTRL_H

#if (TLKMMI_FILE_ENABLE)


#define TLKMMI_FILE_TIMEOUT           100000 //100ms
#define TLKMMI_FILE_CONN_TIMEOUT      (3000000/TLKMMI_FILE_TIMEOUT)
#define TLKMMI_FILE_IDLE_TIMEOUT      (3000000/TLKMMI_FILE_TIMEOUT)


#define TLKMMI_FILE_UNIT_COUNT        (TLKMMI_FILE_P2M_COUNT+TLKMMI_FILE_M2P_COUNT)

#define TLKMMI_FILE_WAITNUMB_MAX      16
#define TLKMMI_FILE_WAITNUMB_DEF      8
#define TLKMMI_FILE_INTERVAL_MIN      20 //2ms, unit:100us
#define TLKMMI_FILE_UNITLENS_DEF      64
#define TLKMMI_FILE_UNITLENS_MAX      64 
#define TLKMMI_FILE_UNITLENS_MIN      16 


typedef enum{
	TLKMMI_FILE_ATTR_DIR_M2P = 0x01, //0x00-P2M
}TLKMMI_FILE_ATTRS_ENUM;
typedef enum{
	TLKMMI_FILE_BUSY_NONE = 0x00,
	TLKMMI_FILE_BUSY_SEND_START_RSP = 0x01,
	TLKMMI_FILE_BUSY_SEND_CLOSE_RSP = 0x02,
	TLKMMI_FILE_BUSY_SEND_PAUSE_RSP = 0x04,
	TLKMMI_FILE_BUSY_SEND_CLOSE_EVT = 0x08,
	TLKMMI_FILE_BUSY_SEND_SHAKE_EVT = 0x10,
}TLKMMI_FILE_BUSYS_ENUM;





int tlkmmi_file_ctrlInit(void);


void tlkmmi_file_start(uint08 peerPort, uint08 fileType, uint08 waitNumb, uint16 interval, uint16 unitLens,
	uint32 fileSize, uint08 *pDigest, uint08 *pFileName, uint08 nameLens);
void tlkmmi_file_pause(uint08 peerPort);
void tlkmmi_file_close(uint08 peerPort, uint08 status, uint08 *pDigest);



#endif //TLKMMI_FILE_ENABLE

#endif //TLKMMI_FILE_CTRL_H

