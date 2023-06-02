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
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysMsgSys.h"
#include "tlkmmi_sysMsgInner.h"
#include "tlkmmi_sysAdapt.h"
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/sys/tlkdev_store.h"
#endif
#include "tlklib/dbg/tlkdbg_config.h"

#if (TLK_CFG_USB_ENABLE)
extern bool tlkusb_setModule(uint08 modtype); //TLKUSB_MODTYPE_ENUM
extern void tklcfg_setUsbMode(uint08 umode);
void tlkcfg_setSerialBaudrate(uint32 baudrate);
#endif
extern int bth_hci_sendResetCmd(void);
extern uint tlkcfg_getWorkMode(void);
extern void tlkcfg_setSerialBaudrate(uint32 baudrate);
#if (TLK_CFG_COMM_ENABLE)
extern void delay_ms(unsigned int millisec);
extern int tlkmdi_comm_setBaudrate(uint32 baudrate);
extern uint32 tlkmdi_comm_getBaudrate();
#endif
#if (TLK_CFG_TEST_ENABLE)
extern void tlkcfg_setWorkMode(TLK_WORK_MODE_ENUM wmode);
extern void core_reboot(void);
#endif

static void tlkmmi_sys_recvGetVersionCmdDeal(void);
static void tlkmmi_sys_recvRebootCmdDeal(void);
static void tlkmmi_sys_recvPowerOffCmdDeal(void);
static void tlkmmi_sys_recvSetHeartCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvLoadDbgCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvUpdateDbgMajorCmdDeal(uint08 *pData,uint16 dataLen);
static void tlkmmi_sys_recvUpdateDbgMinorCmdDeal(uint08 *pData,uint16 dataLen);
static void tlkmmi_sys_recvGetWorkModeCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvSetWorkModeCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvFormatUDiskCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvUpdateVcdCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvSetUSBModeCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvSetbaudrateCmdDeal(uint08 *pData, uint08 dataLen);


#if (TLK_CFG_DBG_ENABLE)
static tlkapi_timer_t gTlkMmiSysDbgLoadTimer;
static int tlkmmi_sys_sendDbgItemInfoEvt(TLK_DEBUG_MAJOR_ID_ENUM majorId, uint32 sign);
static bool tlkmmi_sys_loadDbgTimer(tlkapi_timer_t *pTimer,uint32 userArg);
#endif

#if (TLKDBG_CFG_HPU_VCD_ENABLE)
extern void tlkdbg_hpuvcd_setOpen(bool isOpen);
#endif
int tlkmmi_sys_sysMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_sysMsgHandler:msgID-%d", msgID);
	if(msgID == TLKPRT_COMM_CMDID_SYS_VERSION){
		tlkmmi_sys_recvGetVersionCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_REBOOT){
		tlkmmi_sys_recvRebootCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_POWER_OFF){
		tlkmmi_sys_recvPowerOffCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_HEART){
		tlkmmi_sys_recvSetHeartCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_LOAD){
		tlkmmi_sys_recvLoadDbgCmdDeal(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE){
		tlkmmi_sys_recvUpdateDbgMajorCmdDeal(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE){
		tlkmmi_sys_recvUpdateDbgMinorCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_VCD_CFG){
		tlkmmi_sys_recvUpdateVcdCmdDeal(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_GET_WORK_MODE){
		tlkmmi_sys_recvGetWorkModeCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE){
		tlkmmi_sys_recvSetWorkModeCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_FORMAT_U_DISK){
		tlkmmi_sys_recvFormatUDiskCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_USB_MODE){
		tlkmmi_sys_recvSetUSBModeCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_BAUDRATE){
		tlkmmi_sys_recvSetbaudrateCmdDeal(pData,dataLen);
	}
	return TLK_ENONE;
}

static void tlkmmi_sys_recvUpdateDbgMajorCmdDeal(uint08 *pData,uint16 dataLen)
{
#if (TLK_CFG_DBG_ENABLE)
	uint32 mask = 0;
	uint08 majorID;

	if(dataLen < 6){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}

	majorID = pData[1];
	if(majorID >= TLK_MAJOR_DBGID_MAX){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return ;
	}
	mask |= pData[5];
	mask |= (pData[4] << 8);
	mask |= (pData[3] << 16);
	mask |= (pData[2] << 24);
	if(tlk_debug_setDbgMask(pData[1], mask)){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
#else
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#endif
}
static void tlkmmi_sys_recvUpdateDbgMinorCmdDeal(uint08 *pData,uint16 dataLen)
{
#if (TLK_CFG_DBG_ENABLE)
	bool isSucc;
	uint08 majorID;
	uint08 minorID;
	
	if(dataLen < 4){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	majorID = pData[1];
	minorID = pData[2];
	if(majorID >= TLK_MAJOR_DBGID_MAX){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return ;
	}

	if(pData[3]) isSucc = tlk_debug_enableDbgItem(majorID, minorID);
	else isSucc = tlk_debug_disableDbgItem(majorID, minorID);
	if(isSucc){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MAJOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
#else
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_MINOR_UPDATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#endif
}
static void tlkmmi_sys_recvLoadDbgCmdDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_CFG_DBG_ENABLE)
	uint32 loadSign;
	
	if(dataLen < 1){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_DBG_LOAD, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(tlkmmi_sys_adaptIsHaveTimer(&gTlkMmiSysDbgLoadTimer))
	{
		tlkapi_trace(TLKMMI_SYS_DBG_FLAG,TLKMMI_SYS_DBG_SIGN,"tlkmmi_sys_recvLoadDbgCmdDeal: Timer is busy");
		return;
	}
	loadSign = 0;
	if(pData[0] == 1) loadSign = 1;
	else if(pData[0] == 2) loadSign = 2;
	tlkmmi_sys_adaptInitTimer(&gTlkMmiSysDbgLoadTimer, tlkmmi_sys_loadDbgTimer, loadSign, TLKMMI_SYS_TIMEOUT);
	tlkmmi_sys_adaptInsertTimer(&gTlkMmiSysDbgLoadTimer);
#endif
}
static void tlkmmi_sys_recvGetVersionCmdDeal(void)
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
static void tlkmmi_sys_recvRebootCmdDeal(void)
{
	bth_hci_sendResetCmd();
	tlkmmi_sys_setPowerParam(300000, true);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_REBOOT, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_recvPowerOffCmdDeal(void)
{
	bth_hci_sendResetCmd();
	tlkmmi_sys_setPowerParam(300000, false);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_POWER_OFF, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_recvSetHeartCmdDeal(uint08 *pData, uint08 dataLen)
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
static void tlkmmi_sys_recvGetWorkModeCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 data[4];
	data[0] = tlkcfg_getWorkMode();
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_GET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, data, 1);
}
static void tlkmmi_sys_recvSetWorkModeCmdDeal(uint08 *pData, uint08 dataLen)
{
#if !(TLK_CFG_TEST_ENABLE)
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#else
	if(dataLen == 0){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		tlkcfg_setWorkMode(pData[0]);
		core_reboot();
	}
#endif
}
static void tlkmmi_sys_recvFormatUDiskCmdDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_DEV_STORE_ENABLE)
	if(!tlkdev_store_isOpen()){
		tlkdev_store_open();
	}
	tlkdev_store_format();
#endif
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_FORMAT_U_DISK, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}

static void tlkmmi_sys_recvUpdateVcdCmdDeal(uint08 *pData, uint08 dataLen)
{
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	if(dataLen < 1)
	{
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgHpuVcdUpdateDeal: length error - %d", dataLen);
		return;
	}
	tlkdbg_hpuvcd_setOpen(pData[0]);
#endif
}


#if (TLK_CFG_DBG_ENABLE)
static bool tlkmmi_sys_loadDbgTimer(tlkapi_timer_t *pTimer,uint32 userArg)
{
	#if (TLK_CFG_DBG_ENABLE)
	static int majorID = 0;

	int unitCnt = 0;
	if(majorID == TLK_MAJOR_DBGID_MAX){
		majorID = 0;
		return false;
	}
	unitCnt = tlk_debug_getItemNumb(majorID);
	if(unitCnt == 0){
		majorID++;
		return true;
	}
	if(tlkmmi_sys_sendDbgItemInfoEvt(majorID, userArg) >= unitCnt)
	{
		majorID++;
	}
	#endif //#if (TLK_CFG_DBG_ENABLE)
	return true;
}

static int tlkmmi_sys_sendDbgItemInfoEvt(TLK_DEBUG_MAJOR_ID_ENUM majorId, uint32 sign)
{
	static int index = 0;
	uint08 total;
	uint08 minorID;
	uint08 buffLen;
	uint08 buffer[128];
	uint08 nameLen = 0;

	total = tlk_debug_getItemNumb(majorId);
	
	minorID = index;
	buffLen = 1;
	memset(buffer,0,sizeof(buffer)/sizeof(buffer[0]));
	if(sign == 1) buffer[buffLen++] = 1; //type=1:DBG, Type=2:VCD
	else if(sign == 2) buffer[buffLen++] = 2;
	buffer[buffLen++] = majorId; //majorId
	buffer[buffLen++] = minorID; //minorId
	//data:isOPen
	if(sign == 1)
	{
		if(tlk_debug_dbgItemIsEnable(majorId, minorID)) buffer[buffLen++] = 1;
		else buffer[buffLen++] = 0;
	}
	else if(sign == 2)
	{
		if(tlk_debug_vcdItemIsEnable(majorId, minorID)) buffer[buffLen++] = 1;
		else buffer[buffLen++] = 0;
	}
	//nameLen+name
	nameLen = strlen(tlk_debug_getItemSign(majorId, minorID));
	
	buffer[buffLen++] = nameLen;
	if(nameLen != 0){
		tmemcpy(buffer+buffLen, tlk_debug_getItemSign(majorId, minorID), nameLen);
		buffLen += nameLen;
	}	
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
static void tlkmmi_sys_recvSetUSBModeCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 1){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgSetUSBModeDeal: length error - %d", dataLen);
		return;
	}
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgSetUSBModeDeal: ", pData, dataLen);
	#if (TLK_CFG_USB_ENABLE)
	tlkusb_setModule(pData[0]);
	tklcfg_setUsbMode(pData[0]);
	#endif
}
static void tlkmmi_sys_recvSetbaudrateCmdDeal(uint08 * pData, uint08 dataLen)
{
	if(dataLen < 4){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvSetbaudrateCmdDeal: length error - %d", dataLen);
		return;
	}
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvSetbaudrateCmdDeal: ", pData, dataLen);
	#if (TLK_CFG_COMM_ENABLE)
	uint32 baudrate = 0;
	baudrate |= (pData[0] & 0xFF);
	baudrate |= ((pData[1] & 0xFF) << 8);
	baudrate |= ((pData[2] & 0xFF) << 16);
	baudrate |= ((pData[3] & 0xFF) << 24);
	if(baudrate < 9600 || baudrate == tlkmdi_comm_getBaudrate()){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_BAUDRATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}else{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_BAUDRATE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, pData, dataLen);
	}
	delay_ms(100);
	tlkmdi_comm_setBaudrate(baudrate);
	tlkcfg_setSerialBaudrate(baudrate);
	
	#endif
}

#endif //#if (TLKMMI_SYSTEM_ENABLE)

