/********************************************************************************************************
 * @file     tlkmmi_btmgrAcl.c
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

#include "string.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkmdi/tlkmdi_stdio.h"
#include "tlkmmi/tlkmmi_stdio.h"
#if (TLKMMI_BTMGR_BTACL_ENABLE)
#include "tlkmdi/tlkmdi_btacl.h"
#include "tlkmdi/tlkmdi_btinq.h"
#include "tlkmdi/tlkmdi_btrec.h"
#include "tlkmdi/tlkmdi_bthfp.h"
#include "tlkmmi/tlkmmi_adapt.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrComm.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrAcl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrInq.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrRec.h"


extern int bth_acl_enableSniffSet(uint16 aclHandle, bool enable);

#if (TLK_MDI_BTINQ_ENABLE)
static int tlkmmi_btmgr_reportCB(uint32 devClass, uint08 rssi, uint08 nameLen, uint08 *pBtaddr, uint08 *pBtName);
static void tlkmmi_btmgr_completeCB(void);
#endif
static void tlkmmi_btmgr_aclConnectCB(uint16 handle, uint08 status, uint08 *pBtAddr);
static void tlkmmi_btmgr_aclEncryptCB(uint16 handle, uint08 status, uint08 *pBtAddr);
static void tlkmmi_btmgr_aclDisconnCB(uint16 handle, uint08 reason, uint08 *pBtAddr);
static void tlkmmi_btmgr_aclProfConnCB(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr);
static void tlkmmi_btmgr_aclProfDiscCB(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr);
static bool tlkmmi_btmgr_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmmi_btmgr_procs(tlkmmi_btmgr_acl_t *pCtrl);

static void tlkmmi_btmgr_appendProfile(uint16 aclHandle);
static void tlkmmi_btmgr_aclReset(void);


static tlkmmi_btmgr_acl_t sTlkMmiBtMgrAcl;
static TlkMmiBtMgrAclConnectCallback sTlkMmiBtMgrAclConnCB = nullptr;
static TlkMmiBtMgrAclConnectCallback sTlkMmiBtMgrAclDiscCB = nullptr;
static TlkMmiBtMgrProfileConnectCallback sTlkMmiBtMgrProfileConnCB = nullptr;
static TlkMmiBtMgrProfileDisconnCallback sTlkMmiBtMgrProfileDiscCB = nullptr;

/******************************************************************************
 * Function: tlkmmi_btmgr_aclInit
 * Descript: Handle bt ACL initial including register acl callback to listening 
 *           each acl status changing, and reset the acl manager resource. 
 * Params:
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_aclInit(void)
{
	tmemset(&sTlkMmiBtMgrAcl, 0, sizeof(tlkmmi_btmgr_acl_t));
	tlkmmi_adapt_initTimer(&sTlkMmiBtMgrAcl.timer, tlkmmi_btmgr_timer, (uint32)&sTlkMmiBtMgrAcl, TLKMMI_BTMGR_TIMEOUT);
	
	tlkmdi_btacl_regConnectCB(tlkmmi_btmgr_aclConnectCB);
	tlkmdi_btacl_regEncryptCB(tlkmmi_btmgr_aclEncryptCB);
	tlkmdi_btacl_regDisconnCB(tlkmmi_btmgr_aclDisconnCB);
	tlkmdi_btacl_regProfileConnectCB(tlkmmi_btmgr_aclProfConnCB);
	tlkmdi_btacl_regProfileDisconnCB(tlkmmi_btmgr_aclProfDiscCB);
		
	return TLK_ENONE;
}
/******************************************************************************
 * Function: tlkmmi_btmgr_aclReset
 * Descript: reset the acl manager resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkmmi_btmgr_aclReset(void)
{
	tlkmmi_adapt_removeTimer(&sTlkMmiBtMgrAcl.timer);
	tmemset(&sTlkMmiBtMgrAcl, 0, sizeof(tlkmmi_btmgr_acl_t));
	tlkmmi_adapt_initTimer(&sTlkMmiBtMgrAcl.timer, tlkmmi_btmgr_timer, (uint32)&sTlkMmiBtMgrAcl, TLKMMI_BTMGR_TIMEOUT);
}
/******************************************************************************
 * Function: tlkmmi_btmgr_aclIsBusy
 * Descript: verify acl is busy. 
 * Params:
 * Return: true is busy, false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmmi_btmgr_aclIsBusy(void)
{
	if(sTlkMmiBtMgrAcl.busys != TLKMMI_BTMGR_BUSY_NONE) return true;
	return false;
}

/******************************************************************************
 * Function: tlkmdi_btmgr_regAclConnectCB
 * Descript: Register a acl connect callback.. 
 * Params:
 *        @connCB[IN]--The callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
void tlkmdi_btmgr_regAclConnectCB(TlkMmiBtMgrAclConnectCallback connCB)
{
	sTlkMmiBtMgrAclConnCB = connCB;
}

/******************************************************************************
 * Function: tlkmdi_btmgr_regAclDisconnCB
 * Descript: Register the acl link disconnect callback. 
 * Params:
 *        @discCB[IN]--The Acl Disconnect callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
void tlkmdi_btmgr_regAclDisconnCB(TlkMmiBtMgrAclDisconnCallback discCB)
{
	sTlkMmiBtMgrAclDiscCB = discCB;
}

/******************************************************************************
 * Function: tlkmdi_btmgr_regProfileConnectCB
 * Descript: Register a profile connect callback.. 
 * Params:
 *        @connCB[IN]--The callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
void tlkmdi_btmgr_regProfileConnectCB(TlkMmiBtMgrProfileConnectCallback connCB)
{
	sTlkMmiBtMgrProfileConnCB = connCB;
}

/******************************************************************************
 * Function: tlkmdi_btmgr_regProfileDisconnCB
 * Descript: Register the profile disconnect callback. 
 * Params:
 *        @discCB[IN]--The Acl Disconnect callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
void tlkmdi_btmgr_regProfileDisconnCB(TlkMmiBtMgrProfileDisconnCallback discCB)
{
	sTlkMmiBtMgrProfileDiscCB = discCB;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_connect
 * Descript: Connect the acl link and set a timeout to handle acl connect timeout. 
 * Params:
 *         @btaddr[IN]--The BT address.
 *         @timeout[IN]--The timeout value. Unit:ms.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_connect(uint08 btaddr[6], uint32 timeout)
{
	tlkmdi_btacl_item_t *pBtAcl;
	tlkmdi_btinq_item_t *pBtInq;
	bth_device_item_t *pBtDev;

	if(sTlkMmiBtMgrAcl.busys != TLKMMI_BTMGR_BUSY_NONE){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: failure - busy");
		return -TLK_EBUSY;
	}

	if(timeout < 15000) timeout = 15000;
	sTlkMmiBtMgrAcl.connTime = timeout;
	sTlkMmiBtMgrAcl.timeout  = timeout/TLKMMI_BTMGR_TIMEOUT_MS;
	tmemcpy(sTlkMmiBtMgrAcl.btaddr, btaddr, 6);
	
	pBtAcl = tlkmdi_btacl_searchUsedItem(btaddr);
	if(pBtAcl != nullptr){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: failure - exist");
		if(pBtAcl->state == TLK_STATE_CONNECT) return TLK_ENONE;
		else return -TLK_EBUSY;
	}
	if(tlkmdi_btacl_getIdleCount() == 0){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: failure - no quota");
		return -TLK_EQUOTA;
	}

	pBtDev = bth_device_getItem(btaddr, nullptr);
	if(pBtDev != nullptr){
		int ret;
		sTlkMmiBtMgrAcl.devClass = pBtDev->devClass;
		tmemcpy(sTlkMmiBtMgrAcl.btaddr,pBtDev->devAddr,6);
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: exist in peer - dev class - 0x%06x", pBtDev->devClass);
		if(!tlkmmi_btmgr_recIsBusy()){
			ret = tlkmdi_btacl_connect(btaddr, pBtDev->devClass, timeout);
			if(ret == TLK_ENONE || ret == -TLK_EBUSY){
				tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: success - start connect");
				sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_WAIT_CONN;
				tlkmmi_adapt_insertTimer(&sTlkMmiBtMgrAcl.timer);
			}else{
				tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: failure - by mdi-acl");
			}
		}else{
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: busy - wait close reconnect");
			ret = -TLK_EBUSY;
			sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_WAIT_REC;
			sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_OPEN_CONN;
			tlkmmi_btmgr_recClose();
			tlkmmi_adapt_insertTimer(&sTlkMmiBtMgrAcl.timer);
		}
		return ret;
	}
	
	#if !(TLK_MDI_BTINQ_ENABLE)
	return -TLK_EFAIL;
	#else

	if(tlkmmi_btmgr_recIsBusy()){
		tlkmmi_btmgr_recClose();
		sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_WAIT_REC;
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: execute - wait reconnect close");
	}
	pBtInq = tlkmdi_btinq_getUsedItem(btaddr);
	if(pBtInq != nullptr){
		int ret = -TLK_EBUSY;
		sTlkMmiBtMgrAcl.devClass = pBtInq->devClass;
		tmemcpy(sTlkMmiBtMgrAcl.btaddr,pBtInq->btaddr,6);
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: success - exist in inquiry");
		sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_OPEN_CONN;
		tlkmmi_adapt_insertTimer(&sTlkMmiBtMgrAcl.timer);
		return ret;//tlkmdi_btacl_connect(btaddr, pBtInq->devClass, timeout);
	}
	if(tlkmdi_btinq_isBusy()){
		tlkmdi_btinq_close();
		sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_WAIT_INQ;
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: execute - wait inquiry over");
	}
	sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_OPEN_INQ;
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: busy - wait device");	
	tlkmmi_adapt_insertTimer(&sTlkMmiBtMgrAcl.timer);
	#endif

	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_connect: timeout-%d, busys-0x%04x", 
		sTlkMmiBtMgrAcl.timeout, sTlkMmiBtMgrAcl.busys);
	
	return -TLK_EBUSY;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_disconn
 * Descript: Disconnect the acl link which specify by bt handle. 
 * Params:
 *        @handle[IN]--The acl handle.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_disconn(uint16 handle)
{
	return tlkmdi_btacl_disconn(handle, 0x13);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_disconnByAddr
 * Descript: Disconnect the acl link which specify by bt address. 
 * Params:
 *        @handle[IN]--The Bt address.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_disconnByAddr(uint08 btaddr[6])
{
	tlkmdi_btacl_item_t *pBtAcl;
	
	pBtAcl = tlkmdi_btacl_searchUsedItem(btaddr);
	if(pBtAcl == nullptr){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_disconnByAddr: no acl device");
		return TLK_ENONE;
	} 
	
	if(pBtAcl->handle == 0){
		tlkmdi_btacl_cancel(btaddr);
		return TLK_ENONE;
	}else{
		return tlkmdi_btacl_disconn(pBtAcl->handle, 0x13);
	}
}

#if (TLK_MDI_BTINQ_ENABLE)
static int tlkmmi_btmgr_reportCB(uint32 devClass, uint08 rssi, uint08 nameLen, uint08 *pBtaddr, uint08 *pBtName)
{
	if((sTlkMmiBtMgrAcl.busys & TLKMMI_BTMGR_BUSY_WAIT_DEV) != 0){
		if(tmemcmp(pBtaddr, sTlkMmiBtMgrAcl.btaddr, 6) == 0){
			int ret;
			sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_NONE;
			tlkmdi_btinq_close();
			tlkmdi_btinq_regCallback(nullptr, nullptr);
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_reportCB: execute - find device 0x%08x 0x%06x %d", 
				*(uint32*)(sTlkMmiBtMgrAcl.btaddr), devClass, sTlkMmiBtMgrAcl.connTime);
			ret = tlkmdi_btacl_connect(sTlkMmiBtMgrAcl.btaddr, devClass, sTlkMmiBtMgrAcl.connTime);
			if(ret == TLK_ENONE || ret == -TLK_EBUSY){
				sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_WAIT_CONN;
			}else{
				tlkmmi_btmgr_sendAclConnectEvt(0, TLK_EFAIL, sTlkMmiBtMgrAcl.btaddr);
			}
		}
	}
	return TLK_ENONE;
}
static void tlkmmi_btmgr_completeCB(void)
{
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_completeCB");
	if((sTlkMmiBtMgrAcl.busys & TLKMMI_BTMGR_BUSY_WAIT_DEV) != 0){
		sTlkMmiBtMgrAcl.busys &= ~TLKMMI_BTMGR_BUSY_WAIT_DEV;
		sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_NONE;
		tlkmdi_btinq_regCallback(nullptr, nullptr);
		tlkmmi_btmgr_sendAclConnectEvt(0, TLK_ETIMEOUT, sTlkMmiBtMgrAcl.btaddr);
	}
}
#endif

static void tlkmmi_btmgr_aclConnectCB(uint16 handle, uint08 status, uint08 *pBtAddr)
{
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_aclConnectCB: 0x%x %d, 0x%08x", handle, status, *(uint32*)pBtAddr);
	if(tmemcmp(sTlkMmiBtMgrAcl.btaddr, pBtAddr, 6) == 0){
		tlkmmi_adapt_removeTimer(&sTlkMmiBtMgrAcl.timer);
		if((sTlkMmiBtMgrAcl.busys & TLKMMI_BTMGR_BUSY_WAIT_DEV) != 0){
			tlkmdi_btinq_close();
			tlkmdi_btinq_regCallback(nullptr, nullptr);
		}
		sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_NONE;
		tlkmmi_btmgr_aclReset();
	}
	tlkmmi_btmgr_sendAclConnectEvt(handle, status, pBtAddr);
	if(sTlkMmiBtMgrAclConnCB != nullptr){
		sTlkMmiBtMgrAclConnCB(handle, status, pBtAddr);
	}
}
static void tlkmmi_btmgr_aclEncryptCB(uint16 handle, uint08 status, uint08 *pBtAddr)
{
	bth_acl_enableSniffSet(handle, true);
}
static void tlkmmi_btmgr_aclDisconnCB(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	if(tmemcmp(sTlkMmiBtMgrAcl.btaddr, pBtAddr, 6) == 0){
		sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_NONE;
		if((sTlkMmiBtMgrAcl.busys & TLKMMI_BTMGR_BUSY_WAIT_DEV) != 0){
			tlkmdi_btinq_close();
			tlkmdi_btinq_regCallback(nullptr, nullptr);
			sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_NONE;
		}
	}
	tlkmmi_btmgr_sendAclDisconnEvt(handle, reason, pBtAddr);
	#if (TLK_MDI_BTHFP_ENABLE)
	tlkmdi_hfp_destroy(handle);
	#endif
	if(sTlkMmiBtMgrAclDiscCB != nullptr){
		sTlkMmiBtMgrAclDiscCB(handle, reason, pBtAddr);
	}
}
static void tlkmmi_btmgr_aclProfConnCB(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	if(sTlkMmiBtMgrProfileConnCB != nullptr){
		sTlkMmiBtMgrProfileConnCB(handle, status, ptype, usrID);
	}
	if(ptype != BTP_PTYPE_SDP && ptype != BTP_PTYPE_RFC){
		tlkmmi_btmgr_sendProfConnectEvt(handle, status, ptype, usrID, pBtAddr);
	}
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_aclProfConnCB:{ptype-%d,usrID-%d,handle-%d,reason-%d} ", 
		ptype, usrID, handle, status);
}
static void tlkmmi_btmgr_aclProfDiscCB(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	if(sTlkMmiBtMgrProfileDiscCB != nullptr){
		sTlkMmiBtMgrProfileDiscCB(handle, reason, ptype, usrID);
	}
	if(ptype != BTP_PTYPE_SDP && ptype != BTP_PTYPE_RFC){
		tlkmmi_btmgr_sendProfDisconnEvt(handle, reason, ptype, usrID, pBtAddr);
	}
	
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_aclProfDiscCB:{ptype-%d,usrID-%d,handle-%d,reason-%d} ", 
		ptype, usrID, handle, reason);
	if(ptype == BTP_PTYPE_SDP && usrID == BTP_USRID_CLIENT){
		tlkmmi_btmgr_appendProfile(handle);
	}
}


static bool tlkmmi_btmgr_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkmmi_btmgr_acl_t *pCtrl;
	
	pCtrl = (tlkmmi_btmgr_acl_t*)userArg;
//	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_timer: 0x%x %d 0x%x", pCtrl, pCtrl->timeout, pCtrl->busys);
	
	if(pCtrl->timeout != 0) pCtrl->timeout --;
	if(pCtrl->timeout != 0){
		tlkmmi_btmgr_procs(pCtrl);
	}else{
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_timer: over");
		if((pCtrl->busys & TLKMMI_BTMGR_BUSY_WAIT_DEV) != 0){
			tlkmdi_btinq_close();
			tlkmdi_btinq_regCallback(nullptr, nullptr);
		}
		pCtrl->busys = TLKMMI_BTMGR_BUSY_NONE;
		tlkmmi_btmgr_sendAclConnectEvt(0, TLK_ETIMEOUT, sTlkMmiBtMgrAcl.btaddr);
	}
//	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_timer 01: %d 0x%x", pCtrl->timeout, pCtrl->busys);
	if(pCtrl->busys != 0) return true;
	return false;
}
static void tlkmmi_btmgr_procs(tlkmmi_btmgr_acl_t *pCtrl)
{
	if((pCtrl->busys & TLKMMI_BTMGR_BUSY_WAIT_REC) != 0){
		if(!tlkmmi_btmgr_recIsBusy()){
			pCtrl->busys &= ~TLKMMI_BTMGR_BUSY_WAIT_REC;
		}
	}else if((pCtrl->busys & TLKMMI_BTMGR_BUSY_WAIT_INQ) != 0){
		if(!tlkmdi_btinq_isBusy()){
			pCtrl->busys &= ~TLKMMI_BTMGR_BUSY_WAIT_INQ;
			sTlkMmiBtMgrAcl.busys |= TLKMMI_BTMGR_BUSY_OPEN_INQ;
		}
	}else if((sTlkMmiBtMgrAcl.busys & TLKMMI_BTMGR_BUSY_OPEN_INQ) != 0){
		uint16 inqTime = pCtrl->connTime/1000;
		pCtrl->busys &= ~TLKMMI_BTMGR_BUSY_OPEN_INQ;
		if(tlkmdi_btinq_start(0, TLKMMI_BTMGR_INQ_RSSI, 0, inqTime, 1, false) == TLK_ENONE){
			pCtrl->busys |= TLKMMI_BTMGR_BUSY_WAIT_DEV;
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_procs[inq]: success - start inquiry");
			tlkmdi_btinq_regCallback(tlkmmi_btmgr_reportCB, tlkmmi_btmgr_completeCB);
		}else{
			pCtrl->busys = TLKMMI_BTMGR_BUSY_NONE;
			tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_procs[inq]: failure - start inquiry");
			tlkmdi_btinq_regCallback(nullptr, nullptr);
			tlkmmi_btmgr_sendAclConnectEvt(0, TLK_EFAIL, pCtrl->btaddr);
		}
	}else if((pCtrl->busys & TLKMMI_BTMGR_BUSY_OPEN_CONN) != 0){
		int ret;
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_procs: execute - find device");
		ret = tlkmdi_btacl_connect(pCtrl->btaddr, pCtrl->devClass, pCtrl->connTime);
		if(ret == TLK_ENONE || ret == -TLK_EBUSY){
			sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_WAIT_CONN;
		}else{
			sTlkMmiBtMgrAcl.busys = TLKMMI_BTMGR_BUSY_NONE;
			tlkmmi_btmgr_sendAclConnectEvt(0, TLK_EFAIL, pCtrl->btaddr);
		}
	}else if((pCtrl->busys & TLKMMI_BTMGR_BUSY_WAIT_DEV) != 0){
		
	}else if((pCtrl->busys & TLKMMI_BTMGR_BUSY_WAIT_CONN) != 0){
		
	}
}


static void tlkmmi_btmgr_appendProfile(uint16 aclHandle)
{
	#if !(TLK_CFG_PTS_ENABLE)
	int ret;
	uint08 dtype;
	uint16 delayMs;
	tlkmdi_btacl_item_t *pItem;

	pItem = tlkmdi_btacl_getConnItem(aclHandle);
	if(pItem == nullptr){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: fault exist");
		return;
	}
	
	delayMs = pItem->active ? 0 : 2000;
	dtype = bth_devClassToDevType(pItem->devClass);
	if(tlkmdi_btacl_isHaveRfc(aclHandle)){
		ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_RFC, 0, delayMs);
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append rfc success");
		}else{
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append rfc failure");
		}
		if(tlkmdi_btacl_isFindHfp(aclHandle)){
			ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_HFP, BTP_USRID_NONE, delayMs+200);
			if(ret == TLK_ENONE){
				tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append hfp success");
			}else{
				tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append hfp failure");
			}
		}
		#if (TLKMMI_PHONE_BOOK_ENABLE)
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: pbap - %d %d %d %d", 
			aclHandle, dtype, tlkmdi_btacl_isFindPbap(aclHandle), pItem->devClass);
		if(dtype != BTH_REMOTE_DTYPE_HEADSET && tlkmdi_btacl_isFindPbap(aclHandle)){
			ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_PBAP, BTP_USRID_CLIENT, delayMs+500);
			if(ret == TLK_ENONE){
				tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append pbap success");
			}else{
				tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append pbap failure");
			}
		}
		#endif
		delayMs += 800;
	}
	
	if(dtype == BTH_REMOTE_DTYPE_HEADSET){
		ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_A2DP, BTP_USRID_SERVER, delayMs);
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append a2dp-src success");
		}else{
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append a2dp-src failure");
		}
	}else if(dtype == BTH_REMOTE_DTYPE_COMPUTER || dtype == BTH_REMOTE_DTYPE_PHONE){
		ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_A2DP, BTP_USRID_CLIENT, delayMs);
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append a2dp-snk success");
		}else{
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append a2dp-snk failure");
		}
	}else{
		ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_A2DP, BTP_USRID_NONE, delayMs);
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append a2dp-none success");
		}else{
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append a2dp-none failure");
		}
	}
	
	ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_AVRCP, BTP_USRID_NONE, delayMs+500);
	if(ret == TLK_ENONE){
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append avrcp success");
	}else{
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append avrcp failure");
	}
    if(dtype != BTH_REMOTE_DTYPE_HEADSET){
	    ret = tlkmdi_btacl_appendProf(aclHandle, BTP_PTYPE_HID, BTP_USRID_SERVER, delayMs);
	    if(ret == TLK_ENONE){
		    tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append hid success");
	    }else{
		    tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_appendProfile: append hid failure");
	    }
	}
	#endif
}


#endif //#if (TLKMMI_BTMGR_BTACL_ENABLE)

