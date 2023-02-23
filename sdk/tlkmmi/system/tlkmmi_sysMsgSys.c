/********************************************************************************************************
 * @file     tlkmmi_sysMsgSys.c
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/tsk/tlktsk_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysMsgSys.h"
#include "tlkmmi_sysMsgInner.h"
#include "tlkmmi_sysAdapt.h"

extern int bth_hci_sendResetCmd(void);

static void tlkmmi_sys_commGetVersionDeal(void);
static void tlkmmi_sys_commRebootDeal(void);
static void tlkmmi_sys_commPowerOffDeal(void);
static void tlkmmi_sys_commSetHeartDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_commDbgLoad();
static void tlkmmi_sys_commDbgMajorUpdate(uint08 *pData,uint16 dataLen);
static void tlkmmi_sys_commDbgMinorUpdate(uint08 *pData,uint16 dataLen);


extern tlkapi_timer_t gTlkMmiSysDbgTimer;
extern unsigned long *tlk_debug_get_dbgMask();
extern tlk_debug_info_t **tlk_debug_get_dbgInfo();

int tlkmmi_sys_sysMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_sysMsgHandler:msgID-%d", msgID);
	if(msgID == TLKPRT_COMM_CMDID_SYS_VERSION){
		tlkmmi_sys_commGetVersionDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_REBOOT){
		tlkmmi_sys_commRebootDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_POWER_OFF){
		tlkmmi_sys_commPowerOffDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_HEART){
		tlkmmi_sys_commSetHeartDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_LOAD){
		tlkmmi_sys_commDbgLoad();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE){
		tlkmmi_sys_commDbgMajorUpdate(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE){
		tlkmmi_sys_commDbgMinorUpdate(pData, dataLen);
	}
	return TLK_ENONE;
}

static void tlkmmi_sys_commDbgMajorUpdate(uint08 *pData,uint16 dataLen)
{
#if TLK_CFG_DBG_ENABLE
	int majorID = pData[1];
	uint32 flags = 0;
	if((majorID >= TLK_MAJOR_DBGID_MAX) || (tlk_debug_get_dbgInfo()[majorID] == nullptr)){
		return ;
	}
	flags |= pData[5];
	flags |= (pData[4] << 8);
	flags |= (pData[3] << 16);
	flags |= (pData[2] << 24);
	tlk_debug_get_dbgMask()[majorID] = flags;
#endif
}
static void tlkmmi_sys_commDbgMinorUpdate(uint08 *pData,uint16 dataLen)
{
	if(dataLen < 4)
	{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	int majorID = pData[1];
	int minorID = pData[2];

	if(pData[3]) tlk_debug_get_dbgMask()[majorID] |= (1<<minorID);
	else tlk_debug_get_dbgMask()[majorID] &= ~(1<<minorID);
}
static void tlkmmi_sys_commDbgLoad()
{
#if (TLK_CFG_DBG_ENABLE)
	tlkmmi_sys_adaptInsertTimer(&gTlkMmiSysDbgTimer);
#endif
}
int tlkmmi_sys_commDbgItemEvtSend(TLK_DEBUG_MAJOR_ID_ENUM majorId)
{
#if (TLK_CFG_DBG_ENABLE)
	static int index = 0;
	
	int total = tlk_debug_get_dbgInfo()[majorId]->unitCnt;

	uint08  buffLen;
	uint08 buffer[128];
	uint08 nameLen = 0;
	
	buffLen = 1;
	memset(buffer,0,sizeof(buffer)/sizeof(buffer[0]));
	//majorId
	buffer[buffLen++] = majorId;
	//minorId
	buffer[buffLen++] = tlk_debug_get_dbgInfo()[majorId]->unit[index].minorID;
	//data:isOPen
	if(tlk_debug_get_dbgMask()[majorId] & (1 << index))buffer[buffLen++] = 1;
	else buffer[buffLen++] = 0;
	//nameLen+name
	nameLen = strlen(tlk_debug_get_dbgInfo()[majorId]->unit[index].pDbgSign);

	buffer[buffLen++] = nameLen;
	tmemcpy(buffer+buffLen,tlk_debug_get_dbgInfo()[majorId]->unit[index].pDbgSign,nameLen);
	buffLen += nameLen;
	buffer[0] = buffLen;

	tlkmmi_sys_sendCommEvt(TLKPRT_COMM_EVTID_SYS_DBG_LOAD,buffer,buffLen);
	index++;

	if(index >= total)
	{
		index = 0;
		return total;
	}
	return index;
#else
	return TLK_ENONE;
#endif
}

static void tlkmmi_sys_commGetVersionDeal(void)
{
	uint08 buffLen;
	uint08 buffer[12];
	uint16 prtVersion;
	uint32 libVersion;
	uint32 appVersion;

	libVersion = TLK_LIB_VERSION;
	prtVersion = TLK_PRT_VERSION;
	appVersion = TLK_APP_VERSION;
	
	buffLen = 0;
	buffer[buffLen++] = (libVersion & 0xFF000000) >> 24;
	buffer[buffLen++] = (libVersion & 0x00FF0000) >> 16;
	buffer[buffLen++] = (libVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (libVersion & 0x000000FF);
	buffer[buffLen++] = (prtVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (prtVersion & 0x000000FF);
	buffer[buffLen++] = (appVersion & 0xFF000000) >> 24;
	buffer[buffLen++] = (appVersion & 0x00FF0000) >> 16;
	buffer[buffLen++] = (appVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (appVersion & 0x000000FF);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_VERSION, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_sys_commRebootDeal(void)
{
	bth_hci_sendResetCmd();
	tlkmmi_sys_setPowerParam(300000, true);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_REBOOT, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_commPowerOffDeal(void)
{
	bth_hci_sendResetCmd();
	tlkmmi_sys_setPowerParam(300000, false);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_POWER_OFF, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_commSetHeartDeal(uint08 *pData, uint08 dataLen)
{
	uint08 enable;
	uint16 timeout;
	
	enable = pData[0];
	timeout = pData[1];
	if(timeout == 0) timeout = 3;
	if(dataLen < 2 || timeout > 100){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_HEART, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(enable) tlkmmi_sys_setHeartParam(timeout*1000000);
	else tlkmmi_sys_setHeartParam(0);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_HEART, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, &enable, 1);
}



#endif //#if (TLKMMI_SYSTEM_ENABLE)

