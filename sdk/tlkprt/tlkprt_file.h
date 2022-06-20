/********************************************************************************************************
 * @file     tlkprt_file.h
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

#ifndef TLKPRT_FILE_H
#define TLKPRT_FILE_H


typedef enum{
	TLKPRT_FILE_TYPE_OTA  = 0x01,
	TLKPRT_FILE_TYPE_MP3  = 0x05,
	TLKPRT_FILE_TYPE_TONE = 0x06,
}TLKPRT_FILE_TYPE_ENUM;
typedef enum{
	TLKPRT_FILE_STATUS_SOMPLETE = 0x00,
	TLKPRT_FILE_STATUS_SUCCESS = 0x00,
	TLKPRT_FILE_STATUS_FAILURE = 0x01,
}TLKPRT_FILE_STATUS_ENUM;
typedef enum{
	TLKPRT_FILE_REASON_NONE = 0x00,
	TLKPRT_FILE_REASON_REJECT  = 0x80,
	TLKPRT_FILE_REASON_TIMEOUT,
	TLKPRT_FILE_REASON_ERR_PARAM,
	TLKPRT_FILE_REASON_ERR_DIGEST,
	TLKPRT_FILE_REASON_NOT_SUPPORT,	
}TLKPRT_FILE_REASON_ENUM;


typedef struct{
	uint08 minePort;
	uint08 fileType;
	uint08 waitNumb;
	uint08 nameLens;
	uint16 interval; //Unit:100us
	uint16 unitLens; //Single Packet Length
	uint32 fileSize;
	uint08 digest[16];
	uint08 fileName[64];
}tlkprt_file_startCmd_t;
typedef struct{
	uint08 peerPort;
	uint08 minePort;
	uint16 interval; //Unit:100us
	uint16 unitLens;
}tlkprt_file_startRsp_t;

typedef struct{
	uint08 dbport;
}tlkprt_file_pauseCmd_t;
typedef struct{
	uint08 dbport;
	uint08 status;
	uint08 reason;
}tlkprt_file_pauseRsp_t;

typedef struct{
	uint08 dbport;
	uint08 status;
	uint08 digest[16];
}tlkprt_file_closeCmd_t;
typedef struct{
	uint08 dbport;
	uint08 status;
	uint08 reason;
}tlkprt_file_closeRsp_t;


typedef struct{
	uint08 dbport;
	uint08 status;
	uint08 reason;
}tlkprt_file_closeEvt_t;
typedef struct{
	uint08 dbport;
	uint32 curNumb;
	uint32 getNumb;
}tlkprt_file_shakeEvt_t;




#endif //TLKPRT_FILE_H

