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
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/sys/tlkdev_store.h"
#endif


extern int bth_hci_sendResetCmd(void);
extern uint tlkcfg_getWorkMode(void);
extern void tlkcfg_setWorkMode(TLK_WORK_MODE_ENUM wmode);
extern void start_reboot(void);

static void tlkmmi_sys_commGetVersionDeal(void);
static void tlkmmi_sys_commRebootDeal(void);
static void tlkmmi_sys_commPowerOffDeal(void);
static void tlkmmi_sys_commSetHeartDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_commDbgLoad(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_commDbgMajorUpdate(uint08 *pData,uint16 dataLen);
static void tlkmmi_sys_commDbgMinorUpdate(uint08 *pData,uint16 dataLen);
static void tlkmmi_sys_commGetWorkMode(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_commSetWorkMode(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_commFormatUDisk(uint08 *pData, uint08 dataLen);
static bool tlkmmi_dbgload_timer(tlkapi_timer_t *pTimer,uint32 userArg);


#if (TLK_CFG_DBG_ENABLE)
tlkapi_timer_t gTlkMmiSysDbgLoadTimer;
int tlkmmi_sys_commDbgItemEvtSend(TLK_DEBUG_MAJOR_ID_ENUM majorId,uint32 sign);

extern unsigned long *tlk_debug_get_dbgMask();
extern unsigned long *tlk_debug_get_vcdMask();
extern tlk_debug_info_t **tlk_debug_get_dbgInfo();
#endif

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
		tlkmmi_sys_commDbgLoad(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE){
		tlkmmi_sys_commDbgMajorUpdate(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE){
		tlkmmi_sys_commDbgMinorUpdate(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_GET_WORK_MODE){
		tlkmmi_sys_commGetWorkMode(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE){
		tlkmmi_sys_commSetWorkMode(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_FORMAT_U_DISK){
		tlkmmi_sys_commFormatUDisk(pData, dataLen);
	}
	return TLK_ENONE;
}

static void tlkmmi_sys_commDbgMajorUpdate(uint08 *pData,uint16 dataLen)
{
#if (TLK_CFG_DBG_ENABLE)
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
#if (TLK_CFG_DBG_ENABLE)
	if(dataLen < 4)
	{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	int majorID = pData[1];
	int minorID = pData[2];

	if(pData[3]) tlk_debug_get_dbgMask()[majorID] |= (1<<minorID);
	else tlk_debug_get_dbgMask()[majorID] &= ~(1<<minorID);
#endif
}
static void tlkmmi_sys_commDbgLoad(uint08 *pData, uint08 dataLen)
{
#if (TLK_CFG_DBG_ENABLE)
	if(dataLen < 1)
	{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_LOAD, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(tlkmmi_sys_adaptIsHaveTimer(&gTlkMmiSysDbgLoadTimer))
	{
		tlkapi_trace(TLKMMI_SYS_DBG_FLAG,TLKMMI_SYS_DBG_SIGN,"tlkmmi_sys_commDbgLoad: Timer is busy");
		return;
	}
	uint32 loadSign = 0;
	if(pData[0] == 1) loadSign = 1;
	else if(pData[0] == 2) loadSign = 2;
	tlkmmi_sys_adaptInitTimer(&gTlkMmiSysDbgLoadTimer, tlkmmi_dbgload_timer, loadSign, TLKMMI_SYS_TIMEOUT);
	tlkmmi_sys_adaptInsertTimer(&gTlkMmiSysDbgLoadTimer);
#endif
}
static bool tlkmmi_dbgload_timer(tlkapi_timer_t *pTimer,uint32 userArg)
{
	#if (TLK_CFG_DBG_ENABLE)
	static int majorID = 0;

	int unitCnt = 0;
	if(majorID == TLK_MAJOR_DBGID_MAX)
	{
		majorID = 0;
		return false;
	}
	if(tlk_debug_get_dbgInfo()[majorID] == nullptr)
	{
		majorID++;
		return true;
	}
	unitCnt = tlk_debug_get_dbgInfo()[majorID]->unitCnt;
	if( tlkmmi_sys_commDbgItemEvtSend(majorID,userArg) >= unitCnt)
	{
		majorID++;
	}
	#endif //#if (TLK_CFG_DBG_ENABLE)
	return true;
}
void tlkmmi_dbgLoad_timer_start()
{
#if (TLK_CFG_DBG_ENABLE)
	tlkmmi_sys_adaptInsertTimer(&gTlkMmiSysDbgLoadTimer);
#endif
}
void tlkmmi_dbgLoad_timer_close()
{
#if (TLK_CFG_DBG_ENABLE)
	tlkmmi_sys_adaptRemoveTimer(&gTlkMmiSysDbgLoadTimer);
#endif
}


#if (TLK_CFG_DBG_ENABLE)
int tlkmmi_sys_commDbgItemEvtSend(TLK_DEBUG_MAJOR_ID_ENUM majorId,uint32 sign)
{
	static int index = 0;
	
	int total = tlk_debug_get_dbgInfo()[majorId]->unitCnt;

	uint08  buffLen;
	uint08 buffer[128];
	uint08 nameLen = 0;
	
	buffLen = 1;
	memset(buffer,0,sizeof(buffer)/sizeof(buffer[0]));
	if(sign == 1) buffer[buffLen++] = 1;
	else if(sign == 2) buffer[buffLen++] = 2;
	//majorId
	buffer[buffLen++] = majorId;
	//minorId
	buffer[buffLen++] = tlk_debug_get_dbgInfo()[majorId]->unit[index].minorID;
	//data:isOPen
	if(sign == 1)
	{
		if(tlk_debug_get_dbgMask()[majorId] & (1 << index))buffer[buffLen++] = 1;
		else buffer[buffLen++] = 0;
	}
	else if(sign == 2)
	{
		if(tlk_debug_get_vcdMask()[majorId] & (1 << index))buffer[buffLen++] = 1;
		else buffer[buffLen++] = 0;
	}
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
}
#endif //#if (TLK_CFG_DBG_ENABLE)

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
static void tlkmmi_sys_commGetWorkMode(uint08 *pData, uint08 dataLen)
{
	uint08 data[4];
	data[0] = tlkcfg_getWorkMode();
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_GET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, data, 1);
}
static void tlkmmi_sys_commSetWorkMode(uint08 *pData, uint08 dataLen)
{
#if !(TLK_CFG_DBG_ENABLE)
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#else
	if(dataLen == 0){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		tlkcfg_setWorkMode(pData[0]);
		start_reboot();
	}
#endif
}
static void tlkmmi_sys_commFormatUDisk(uint08 *pData, uint08 dataLen)
{
#if (TLK_DEV_STORE_ENABLE)
	if(!tlkdev_store_isOpen()){
		tlkdev_store_open();
	}
	tlkdev_store_format();
#endif
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_FORMAT_U_DISK, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}



#endif //#if (TLKMMI_SYSTEM_ENABLE)

