/********************************************************************************************************
 * @file     tlkmdi_bthfp.h
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
#ifndef TLKMDI_BTHFP_H
#define TLKMDI_BTHFP_H

#if (TLK_MDI_BTHFP_ENABLE)


#define TLKMDI_HFPHF_TIMEOUT                100000
#define TLKMDI_HFPHF_CALL_WAIT_TIMEOUT      (3000000/TLKMDI_HFPHF_TIMEOUT)

#define TLKMDI_HFPHF_NUMBER_MAX_LEN					32

#define TLKMDI_HFPHF_THREE_WAY_CALL_ENABLE          1



typedef enum{
	TLKMDI_HFPHF_FLAG_NONE   = 0x00,
		
	TLKMDI_HFPHF_FLAG_START  = 0x01,
	TLKMDI_HFPHF_FLAG_ALART  = 0x02,
	TLKMDI_HFPHF_FLAG_ACTIVE = 0x04,
	TLKMDI_HFPHF_FLAG_PAUSED = 0x08,
	TLKMDI_HFPHF_FLAG_CLOSE  = 0x10,
}TLKMDI_HFPHF_FLAGS_ENUM;

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
	TLKMDI_HFPHF_CALL_BUSY_REPORT_START  = 0x01,
	TLKMDI_HFPHF_CALL_BUSY_REPORT_ACTIVE = 0x02,
	TLKMDI_HFPHF_CALL_BUSY_REPORT_PAUSED = 0x04,
	TLKMDI_HFPHF_CALL_BUSY_REPORT_RESUME = 0x08,
	TLKMDI_HFPHF_CALL_BUSY_REPORT_WAIT   = 0x10,
	TLKMDI_HFPHF_CALL_BUSY_REPORT_CLOSE  = 0x20,
}TLKMDI_HFPHF_CALL_BUSYS_ENUM;
typedef enum{
	TLKMDI_HFPHF_CALL_FLAG_NONE   = 0x00,

	TLKMDI_HFPHF_CALL_FLAG_READY  = 0x01,	
	
	TLKMDI_HFPHF_CALL_FLAG_REPORT_START  = 0x02,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE = 0x04,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED = 0x08,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_WAIT   = 0x10,
	TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE  = 0x20,
	
	TLKMDI_HFPHF_CALL_FLAG_REPORT_MASK = TLKMDI_HFPHF_CALL_FLAG_REPORT_START | TLKMDI_HFPHF_CALL_FLAG_REPORT_ACTIVE
		| TLKMDI_HFPHF_CALL_FLAG_REPORT_PAUSED | TLKMDI_HFPHF_CALL_FLAG_REPORT_CLOSE | TLKMDI_HFPHF_CALL_FLAG_REPORT_WAIT,
}TLKMDI_HFPHF_CALL_FLAGS_ENUM;


typedef struct{
	uint08 numbLen;
	uint08 callDir;
	uint08 callStat;
	uint08 callBusy; //TLKMDI_HFPHF_CALL_BUSYS_ENUM
	uint08 callFlag; //TLKMDI_HFPHF_CALL_FLAGS_ENUM
	uint08 number[TLKMDI_HFPHF_NUMBER_MAX_LEN];
}tlkmdi_hfphf_unit_t;
typedef struct{
	uint08 flags; //TLKMDI_HFPHF_FLAGS_ENUM
	uint08 resv0;
	uint16 handle;
	tlkmdi_hfphf_unit_t unit[2];
	tlkmdi_hfphf_unit_t temp[2];
}tlkmdi_hfphf_ctrl_t;

typedef struct{
	uint08 codec;
	uint16 handle;
}tlkmdi_hfphf_codecEvt_t;
typedef struct{
	uint16 handle;
	uint08 callNum; // 0 or 1
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
int tlkmdi_bthfp_init(void);

/******************************************************************************
 * Function: tlkmdi_hfphf_destroy.
 * Descript: Reset the HF control block adn release resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_bthfp_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: tlkmdi_bthfp_getHfCallNumber.
 * Descript: Get the call number.
 * Params: None.
 * Return: Return call number.
 * Others: None.
*******************************************************************************/
uint08 *tlkmdi_bthfp_getHfCallNumber(uint08 callNum);


int tlkmdi_bthfp_rejectHfWaitAndKeepActive(void);
int tlkmdi_bthfp_acceptHfWaitAndHoldActive(void);
int tlkmdi_bthfp_hungupHfActiveAndResumeHold(void);



#endif //#if (TLK_MDI_BTHFP_ENABLE)

#endif //TLKMDI_BTHFP_H

