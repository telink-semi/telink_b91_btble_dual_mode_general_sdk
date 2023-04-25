/********************************************************************************************************
 * @file     tlkdev_serial.c
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
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkdrv/ext/serial/tlkdrv_serial.h"
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys/tlksys_pm.h"
#endif


static uint08 sTlkDevSerialPort = 0xFF;
static uint08 sTlkDevSerialIsOpen = false;

extern void tlkcfg_setSerialBaudrate(uint32 baudrate);


int tlkdev_serial_init(void)
{
	sTlkDevSerialPort = 0xFF;
	tlkdrv_serial_init();
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkdev_serial_isBusy, "tlkdev_serial");
	tlksys_pm_appendLeaveSleepCB(tlkdev_serial_wakeup);
	#endif

	return TLK_ENONE;
}

int tlkdev_serial_mount(uint08 port, uint32 baudRate, uint16 txPin,
	uint16 rxPin, uint08 txDma, uint08 rxDma, uint16 rtsPin, uint16 ctsPin)
{	
	int ret;

	if(sTlkDevSerialPort != 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_mount: failure - already mount");
		return -TLK_EBUSY;
	}
	
	ret = tlkdrv_serial_mount(port, baudRate, txPin, rxPin, txDma, rxDma,
		  rtsPin, ctsPin);
	if(ret == TLK_ENONE){
		sTlkDevSerialPort = port;
		sTlkDevSerialIsOpen = false;
	}
	return ret;
}
int tlkdev_serial_unmount(void)
{
	int ret;
	
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_unmount: failure - not mount");
		return -TLK_ENOOBJECT;
	}
	
	ret = tlkdrv_serial_unmount(sTlkDevSerialPort);
	if(ret == TLK_ENONE){
		sTlkDevSerialPort = 0xFF;
		sTlkDevSerialIsOpen = false;
	}
	return ret;
}

int tlkdev_serial_setBaudrate(uint32 baudRate)
{
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_setBaudrate: failure - not mount");
		return -TLK_EBUSY;
	}
	return tlkdrv_serial_setBaudrate(sTlkDevSerialPort, baudRate);
}
uint32 tlkdev_serial_getBuadrate()
{
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkDev_serial_getBaudrate: failure - not mount");
		return -TLK_EBUSY;
	}
	return tlkdrv_serial_getBaudrate(sTlkDevSerialPort);
}

int tlkdev_serial_setRxFifo(uint08 *pBuffer, uint16 buffLen)
{
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_setRxFifo: failure - not mount");
		return -TLK_EBUSY;
	}
	return tlkdrv_serial_setRxFifo(sTlkDevSerialPort, pBuffer, buffLen);
}
int tlkdev_serial_setTxQFifo(uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint16 buffLen)
{
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_setTxQFifo: failure - not mount");
		return -TLK_EBUSY;
	}
	return tlkdrv_serial_setTxQFifo(sTlkDevSerialPort, fnumb, fsize, pBuffer, buffLen);
}
int tlkdev_serial_setRxQFifo(uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint16 buffLen)
{
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_setRxQFifo: failure - not mount");
		return -TLK_EBUSY;
	}
	return tlkdrv_serial_setRxQFifo(sTlkDevSerialPort, fnumb, fsize, pBuffer, buffLen);
}

int tlkdev_serial_open(void)
{
	int ret;

	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdrv_serial_open: failure - not mount");
		return -TLK_ENOREADY;
	}
	if(sTlkDevSerialIsOpen){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdrv_serial_open: failure - already open");
		return -TLK_EREPEAT;
	}
	
	ret = tlkdrv_serial_open(sTlkDevSerialPort);
	if(ret == TLK_ENONE){
		sTlkDevSerialIsOpen = true;
	}
	if(sTlkDevSerialIsOpen == true){
		tlkcfg_setSerialBaudrate(tlkdrv_serial_getBaudrate(sTlkDevSerialPort));
	}
	return ret;
}
int tlkdev_serial_close(void)
{
	int ret;
	
	if(sTlkDevSerialPort == 0xFF){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_close: failure - not mount");
		return -TLK_ENOREADY;
	}
	if(!sTlkDevSerialIsOpen){
		tlkapi_error(TLKDEV_SYS_DBG_FLAG, TLKDEV_SYS_DBG_SIGN, "tlkdev_serial_close: failure - not open");
		return -TLK_ESTATUS;
	}
	
	ret = tlkdrv_serial_close(sTlkDevSerialPort);
	if(ret == TLK_ENONE){
		sTlkDevSerialIsOpen = false;
	}
	return ret;
}

int tlkdev_serial_read(uint08 *pBuff, uint16 buffLen)
{
	if(sTlkDevSerialPort == 0xFF || !sTlkDevSerialIsOpen) return -TLK_ENOREADY;
	return tlkdrv_serial_read(sTlkDevSerialPort, pBuff, buffLen);
}
_attribute_ram_code_sec_
int tlkdev_serial_send(uint08 *pData, uint16 dataLen)
{
	if(sTlkDevSerialPort == 0xFF || !sTlkDevSerialIsOpen) return -TLK_ENOREADY;
	return tlkdrv_serial_send(sTlkDevSerialPort, pData, dataLen);
}

bool tlkdev_serial_isBusy(void)
{
	if(sTlkDevSerialPort == 0xFF || !sTlkDevSerialIsOpen) return false;
	return tlkdrv_serial_isBusy(sTlkDevSerialPort);
}
void tlkdev_serial_wakeup(uint wakeSrc)
{
	if(sTlkDevSerialPort != 0xFF && sTlkDevSerialIsOpen){
		tlkdrv_serial_wakeup(sTlkDevSerialPort);
	}
}

void tlkdev_serial_clear(void)
{
	if(sTlkDevSerialPort == 0xFF) return;
	tlkdrv_serial_clear(sTlkDevSerialPort);
}

void tlkdev_serial_regCB(TlkDevSerialRecvCB cb)
{
	if(sTlkDevSerialPort == 0xFF) return;
	tlkdrv_serial_regCB(sTlkDevSerialPort, (TlkDrvSerialRecvCB)cb);
}


bool tlkdev_serial_sfifoIsMore60(uint16 dataLen)
{
	if(sTlkDevSerialPort == 0xFF) return false;
	return tlkdrv_serial_sfifoIsMore60(sTlkDevSerialPort, dataLen);
}
bool tlkdev_serial_sfifoIsMore80(uint16 dataLen)
{
	if(sTlkDevSerialPort == 0xFF) return false;
	return tlkdrv_serial_sfifoIsMore85(sTlkDevSerialPort, dataLen);
}

_attribute_ram_code_sec_
void tlkdev_serial_handler(void)
{
	if(sTlkDevSerialPort == 0xFF) return;
	tlkdrv_serial_handler(sTlkDevSerialPort);
}




#endif //#if (TLK_DEV_SERIAL_ENABLE)

