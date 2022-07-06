/********************************************************************************************************
 * @file     btp_spp.h
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

#ifndef BTP_SPP_H
#define BTP_SPP_H



typedef void (*BtpSppRecvDataCB)(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);


typedef struct{
	uint08 state;
	uint08 rfcHandle;
	uint16 aclHandle;
	uint08 credit;
	uint08 resv001;
    uint16 mtuSize;
}btp_spp_item_t;
typedef struct{
    btp_spp_item_t item[TLK_BT_SPP_MAX_NUMB];
}btp_spp_ctrl_t;


/******************************************************************************
 * Function: SPP Init interface
 * Descript: This interface be used by User to initial spp resource.
 * Params:
 * Return:
*******************************************************************************/
int btp_spp_init(void);

/******************************************************************************
 * Function: SPP Register Callback interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
void btp_spp_regDataCB(BtpSppRecvDataCB dataCB);

/******************************************************************************
 * Function: SPP send Connect interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
int  btp_spp_connect(uint16 aclHandle, uint08 channel);


/******************************************************************************
 * Function: SPP send DisConnect interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
int  btp_spp_disconn(uint16 aclHandle);

/******************************************************************************
 * Function: SPP send Destroy interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
void btp_spp_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: SPP send Data interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
int btp_spp_sendData(uint16 aclHandle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: SPP get Idle count interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
uint08 btp_spp_getIdleCount(void);

/******************************************************************************
 * Function: SPP get Used count interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
uint08 btp_spp_getUsedCount(void);

/******************************************************************************
 * Function: SPP get Connect count interface
 * Descript:
 * Params:
 * Return:
*******************************************************************************/
uint08 btp_spp_getConnCount(void);

/******************************************************************************
 * Function: SPP get Idle interface
 * Descript:
 * Params:
 * Return:The spp idle control block.
*******************************************************************************/
btp_spp_item_t *btp_spp_getIdleItem(void);

/******************************************************************************
 * Function: SPP get used interface
 * Descript:
 * Params:
 * Return:The spp used control block.
*******************************************************************************/
btp_spp_item_t *btp_spp_getUsedItem(uint16 aclHandle);

/******************************************************************************
 * Function: SPP get Connect interface
 * Descript:
 * Params:
 * Return:The spp connect control block.
*******************************************************************************/
btp_spp_item_t *btp_spp_getConnItem(uint16 aclHandle);



#endif /* BTP_SPP_H */
