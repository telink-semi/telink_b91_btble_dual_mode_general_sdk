/********************************************************************************************************
 * @file	btp_browsingInner.h
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
#ifndef BTP_BROWSING_INNER_H
#define BTP_BROWSING_INNER_H


#define BTP_BROWSING_DBG_FLAG     ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_BROWSE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_BROWSING_DBG_SIGN     "[BROWSE]"


typedef enum{
	BTP_BROWSING_FLAG_NONE = 0x0000,
	BTP_BROWSING_BUSY_WAIT_SET_PLAYER_RSP = 0x0001,
}BTP_BROWSING_FLAGS_ENUM;
typedef enum{
	BTP_BROWSING_BUSY_NONE = 0x0000,
	BTP_BROWSING_BUSY_SEND_GENERAL_REJECT = 0x0001,
	BTP_BROWSING_BUSY_SEND_SET_PLAYER_CMD = 0x0002,
}BTP_BROWSING_BUSYS_ENUM;


typedef struct{
	uint08 state;
	uint08 stage;
	uint08 usrID;
	uint08 ecode;
	uint16 busys;
	uint16 flags;

	uint08 fscIsEn;
	uint08 cmdTrid;
	uint08 rspTrid;
	uint08 rtnMode;
	uint16 ehnCtrl;
	
	uint16 chnID;
	uint16 timeout;
	uint16 aclHandle;

	uint16 setPlayerID;
	
	tlkapi_timer_t timer;
}btp_browsing_item_t;

typedef struct{
    btp_browsing_item_t item[TLK_BT_BROWSING_MAX_NUMB];
}btp_browsing_ctrl_t;



int btp_browsing_innerInit(void);

/******************************************************************************
 * Function: BROWSING reset interface
 * Descript: This interface be used to reset the sepcify browsing control block.
 * Params:
 * Return:
*******************************************************************************/
void btp_browsing_resetNode(btp_browsing_item_t *pAvrcp);

/******************************************************************************
 * Function: BROWSING get a unused browsing control block num interface
 * Descript: This interface be used to get the total unused control block
 *           num before a new connection need to setup.
 * Params:
 * Return: Returning value means the number of unused browsing control block.
*******************************************************************************/
uint08 btp_browsing_getIdleCount(void);

/******************************************************************************
 * Function: BROWSING get the number of used browsing control block interface
 * Descript: This interface be used to get the total used control block num.
 * Params:
 * Return: Returning value means the number of used browsing control block.
*******************************************************************************/
uint08 btp_browsing_getUsedCount(void);

/******************************************************************************
 * Function: BROWSING get the number of connected browsing control block interface
 * Descript: This interface be used to get the total connected control block num.
 * Params:
 * Return: Returning value means the number of connected browsing control block.
*******************************************************************************/
uint08 btp_browsing_getConnCount(void);

/******************************************************************************
 * Function: BROWSING get the browsing control block index interface
 * Descript: This interface be used to get the index connected control block.
 *           which specify by index.
 * Params:
 * Return: Returning value means the index of browsing control block.
*******************************************************************************/
uint08 btp_browsing_getNodeIndex(btp_browsing_item_t *pAvrcp);

/******************************************************************************
 * Function: BROWSING get the idle browsing control block interface
 * Descript: This interface be used to get the idle browsing control block.
 * Params:
 * Return: Returning a unused browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getNode(uint08 index);

/******************************************************************************
 * Function: BROWSING get the idle browsing control block interface
 * Descript: This interface be used to get the idle browsing control block.
 * Params:
 * Return: Returning a unused browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getIdleNode(void);

/******************************************************************************
 * Function: BROWSING get the used browsing control block interface
 * Descript: This interface be used to get the used browsing control block 
 *           by aclHandle.
 * Params:
 *        @aclhandle[IN]--The acl link handle.
 * Return: Returning the used browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getUsedNode(uint16 aclHandle);

/******************************************************************************
 * Function: BROWSING get the connected browsing control block interface
 * Descript: This interface be used to get the connected browsing control block 
 *           by aclHandle.
 * Params:
 *        @aclhandle[IN]--The acl link handle.
 * Return: Returning the connected browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getConnNode(uint16 aclHandle);

/******************************************************************************
 * Function: BROWSING get the connected browsing control block interface
 * Descript: This interface be used to get the connected browsing control block 
 *           by scid.
 * Params:
 *        @scid[IN]--The psm id.
 * Return: Returning the connected browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getConnNodeByScid(uint16 scid);

/******************************************************************************
 * Function: BROWSING get the connected browsing control block interface
 * Descript: This interface be used to get the connected browsing control block 
 *           by scid.
 * Params:
 *        @scid[IN]--The psm id.
 * Return: Returning the connected browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getConnNodeByNone(uint08 offset);

/******************************************************************************
 * Function: BROWSING get the connected browsing control block interface
 * Descript: This interface be used to get the connected browsing control block 
 *           by index.
 * Params:
 *        @index[IN]--The index of a browsing control block.
 * Return: Returning the connected browsing control block.
*******************************************************************************/
btp_browsing_item_t *btp_browsing_getConnNodeByIndex(uint08 index);


#endif //BTP_BROWSING_INNER_H

