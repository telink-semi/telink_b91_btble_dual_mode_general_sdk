/********************************************************************************************************
 * @file     tlkmdi_hfp
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
#ifndef TLKMDI_HFP_H
#define TLKMDI_HFP_H

#if (TLK_MDI_HFP_ENABLE)


#define TLKMDI_HFPHF_TIMEOUT                100000
#define TLKMDI_HFPHF_CALL_WAIT_TIMEOUT      (3000000/TLKMDI_HFPHF_TIMEOUT)

#define TLKMDI_HFPHF_NUMBER_MAX_LEN					64


typedef enum{
	TLKMDI_HFPHF_EVTID_NONE = 0,
	TLKMDI_HFPHF_EVTID_CALL_CLOSE,
	TLKMDI_HFPHF_EVTID_CALL_START,
	TLKMDI_HFPHF_EVTID_CALL_ALART,
	TLKMDI_HFPHF_EVTID_CALL_ACTIVE,
}TLKMDI_HFPHF_EVTID_ENUM;

typedef enum{
	TLKMDI_HFPHF_CALL_DIR_NONE = 0,
	TLKMDI_HFPHF_CALL_DIR_INCOMING,
	TLKMDI_HFPHF_CALL_DIR_OUTGOING,
}TLKMDI_HFPHF_CALL_DIR_ENUM;
typedef enum{
	TLKMDI_HFPHF_CALL_STATUS_NONE = 0,
	TLKMDI_HFPHF_CALL_STATUS_START,
	TLKMDI_HFPHF_CALL_STATUS_ALART,
	TLKMDI_HFPHF_CALL_STATUS_ACTIVE,
}TLKMDI_HFPHF_CALL_STATUS_ENUM;

typedef enum{
	TLKMDI_HFPHF_CALL_BUSY_NONE = 0x00,
	TLKMDI_HFPHF_CALL_BUSY_WAIT_NUMBER = 0x01,
}TLKMDI_HFPHF_CALL_BUSYS_ENUM;
typedef enum{
	TLKMDI_HFPHF_CALL_FLAG_NONE   = 0x00,
	TLKMDI_HFPHF_CALL_FLAG_CLOSE  = 0x01,
	TLKMDI_HFPHF_CALL_FLAG_START  = 0x02,
	TLKMDI_HFPHF_CALL_FLAG_ALART  = 0x04,
	TLKMDI_HFPHF_CALL_FLAG_ACTIVE = 0x08,
	TLKMDI_HFPHF_CALL_FLAG_NUMBER = 0x10,
	
	TLKMDI_HFPHF_CALL_FLAG_REPORT_START  = 0x20,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE = 0x40,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE  = 0x80,

	TLKMDI_HFPHF_CALL_FLAG_STATUS_MASK = TLKMDI_HFPHF_CALL_FLAG_START | TLKMDI_HFPHF_CALL_FLAG_ALART 
		| TLKMDI_HFPHF_CALL_FLAG_ACTIVE | TLKMDI_HFPHF_CALL_FLAG_CLOSE,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK = TLKMDI_HFPHF_CALL_FLAG_REPORT_START | TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE
		| TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE,
}TLKMDI_HFPHF_CALL_FLAGS_ENUM;


typedef struct{
	uint08 status;
	uint16 handle;
	uint08 timeout;
	uint08 numbLen;
	uint08 callDir;
	uint08 callBusy;
	uint08 callFlag;
	uint08 reserved;
	uint16 callHandle;
	tlkapi_timer_t timer;
	uint08 number[TLKMDI_HFPHF_NUMBER_MAX_LEN];
}tlkmdi_hfphf_ctrl_t;

typedef struct{
	uint08 codec;
	uint16 handle;
}tlkmdi_hfphf_codecEvt_t;
typedef struct{
	uint16 handle;
	uint08 callDir;
	uint08 numbLen;
}tlkmdi_hfphf_statusEvt_t;


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_hfp_init(void);

/******************************************************************************
 * Function: tlkmdi_hfphf_destroy.
 * Descript: Reset the HF control block adn release resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_hfp_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: tlkmdi_hfphf_getCodecID.
 * Descript: Get the Sco's codec id.
 * Params: None.
 * Return: Return codec id.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_hfphf_getCodecID(void);

/******************************************************************************
 * Function: tlkmdi_hfphf_getCallStatus.
 * Descript: Get the phone call statuss.
 * Params: None.
 * Return: Return the status.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_hfphf_getCallStatus(void);

/******************************************************************************
 * Function: tlkmdi_hfphf_getCallNumber.
 * Descript: Get the call number.
 * Params: None.
 * Return: Return call number.
 * Others: None.
*******************************************************************************/
uint08 *tlkmdi_hfphf_getCallNumber(void);


#endif //#if (TLK_MDI_HFP_ENABLE)

#endif //TLKMDI_HFP_H

