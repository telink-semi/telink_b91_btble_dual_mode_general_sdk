/********************************************************************************************************
 * @file     iAP2Time.c
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
/*
 *	File: iAP2Time.c
 *	Package: iAP2Utility
 *	Abstract: n/a 
 *
 *	Disclaimer: IMPORTANT: This Apple software is supplied to you, by Apple
 * 	Inc. ("Apple"), in your capacity as a current, and in good standing,
 *	Licensee in the MFi Licensing Program. Use of this Apple software is
 *	governed by and subject to the terms and conditions of your MFi License,
 *	including, but not limited to, the restrictions specified in the provision
 *	entitled "Public Software", and is further subject to your agreement to
 *	the following additional terms, and your agreement that the use,
 *	installation, modification or redistribution of this Apple software
 * 	constitutes acceptance of these additional terms. If you do not agree with
 * 	these additional terms, please do not use, install, modify or redistribute
 *	this Apple software.
 *
 *	In consideration of your agreement to abide by the following terms, and
 *	subject to these terms, Apple grants you a personal, non-exclusive
 *	license, under Apple's copyrights in this original Apple software (the
 *	"Apple Software"), to use, reproduce, and modify the Apple Software in
 *	source form, and to use, reproduce, modify, and redistribute the Apple
 *	Software, with or without modifications, in binary form. While you may not
 *	redistribute the Apple Software in source form, should you redistribute
 *	the Apple Software in binary form, in its entirety and without
 *	modifications, you must retain this notice and the following text and
 *	disclaimers in all such redistributions of the Apple Software. Neither the
 *	name, trademarks, service marks, or logos of Apple Inc. may be used to
 *	endorse or promote products derived from the Apple Software without
 *	specific prior written permission from Apple. Except as expressly stated
 *	in this notice, no other rights or licenses, express or implied, are
 *	granted by Apple herein, including but not limited to any patent rights
 *	that may be infringed by your derivative works or by other works in which
 *	the Apple Software may be incorporated.
 *	
 *	The Apple Software is provided by Apple on an "AS IS" basis. APPLE MAKES
 *	NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
 *	IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
 *	PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
 *	ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 *
 *	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
 *	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *	INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 *	MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND
 *	WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT
 *	LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY
 *	OF SUCH DAMAGE.
 *
 *	Copyright (C) 2012 Apple Inc. All Rights Reserved.
 *
 */
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_STK_BTP_ENABLE)
#include "tlkstk/bt/btp/btp_stdio.h"
#if (TLKBTP_CFG_IAP_ENABLE)
#include "iAP2Inner.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "drivers.h"
#include "iAP2LinkConfig.h"
#include "iAP2Defines.h"
#include "iAP2Time.h"
#include "iAP2TimeImplementation.h"
#include "iAP2ListArray.h"
#include "iAP2Log.h"


/*
****************************************************************
**
** iAP2Time local functions
**
****************************************************************
*/

static uint8_t _iAP2TimeGetFreeTimeoutID (iAP2Timer_t* timer);
void _iAP2TimeCheckTimeouts (iAP2Timer_t* timer);
void _iAP2TimeHandleExpired (iAP2Timer_t* timer, uint32_t curTime);
static uint32_t _iAP2TimeCompareTimeout (void* a, void* b, uint8_t dataSize);
static uint32_t _iAP2TimeCompareID (void* a, void* b, uint8_t dataSize);

bool _iAP2TimeCallbackAfter (iAP2Timer_t* timer,
                             uint32_t     delayMs,
                             iAP2TimeCB_t callback)
{
	return false;
}

void _iAP2TimeCancelCallback (iAP2Timer_t* timer)
{
}


/*
****************************************************************
**
**  _iAP2TimeGetFreeTimeoutID
**
**  Input:
**      timer:      timer
**
**  Output:
**      None
**
**  Return:
**      uint8_t     next free timeoutID (returne timeoutID is marked reseved)
**
****************************************************************
*/
uint8_t _iAP2TimeGetFreeTimeoutID (iAP2Timer_t* timer)
{
    uint8_t result = timer->nextID++;
    if (result == INVALID_TIMEOUT_ID)
    {
        result = timer->nextID++;
    }
    return result;
}


/*
****************************************************************
**
**  _iAP2TimeCompareID
**
**  Input:
**      a:          first item
**      b:          second item
**      dataSize:   item size
**
**  Output:
**      None
**
**  Return:
**      long    0   if the two items have the same ID
**              < 0 if the first item ID < second item ID
**              > 0 if the first item ID > second item ID
**
****************************************************************
*/
uint32_t _iAP2TimeCompareID (void* a, void* b, uint8_t dataSize)
{
    iAP2Timeout_t* aTime = (iAP2Timeout_t*) a;
    iAP2Timeout_t* bTime = (iAP2Timeout_t*) b;
    return aTime->id - bTime->id;
}


/*
****************************************************************
**
**  _iAP2TimeCompareTimeout
**
**  Input:
**      a:          first item
**      b:          second item
**      dataSize:   item size
**
**  Output:
**      None
**
**  Return:
**      long    0   if the two items have the same timeout
**              < 0 if the first item timeout < second item ID
**              > 0 if the first item timeout > second item timeout
**
****************************************************************
*/
uint32_t _iAP2TimeCompareTimeout (void* a, void* b, uint8_t dataSize)
{
    iAP2Timeout_t* aTime = (iAP2Timeout_t*) a;
    iAP2Timeout_t* bTime = (iAP2Timeout_t*) b;
    return aTime->timeout - bTime->timeout;
}


/*
****************************************************************
**
**  _iAP2TimeHandleExpired
**
**  Input:
**      timer:      timer
**      curTime:    current time in ms
**
**  Output:
**      None
**
**  Return:
**      None
**
****************************************************************
*/
void _iAP2TimeHandleExpired (iAP2Timer_t* timer, uint32_t curTime)
{
    uint8_t index;
    iAP2Timeout_t* timeout;
    index = iAP2ListArrayGetFirstItemIndex (timer->listBuffer);
    timeout = (iAP2Timeout_t*) iAP2ListArrayItemForIndex (timer->listBuffer, index);
    timer->runningTimeOut = INVALID_TIMEOUT_VAL;
    while (timeout && curTime >= timeout->timeout)
    {
        tlkapi_trace(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "_iAP2TimeHandleExpired");
        iAP2TimeCancelTimer(timer,index);
        
        iAP2TimeExpireCB_t  expiredCB = timer->expiredCB;
        uint8_t         timeoutID = timeout->id;
        uint8_t         timeoutType = timeout->type;
        iAP2ListArrayDeleteItem (timer->listBuffer, index, NULL);
        expiredCB (timer, timeoutID, timeoutType, curTime);
        index = iAP2ListArrayGetFirstItemIndex (timer->listBuffer);
        timeout = (iAP2Timeout_t*) iAP2ListArrayItemForIndex (timer->listBuffer, index);
    }
    _iAP2TimeCheckTimeouts (timer);
}


/*
****************************************************************
**
**  _iAP2TimeCheckTimeouts
**
**  Input:
**      timer:      timer
**
**  Output:
**      None
**
**  Return:
**      None
**
****************************************************************
*/
#if 0
void _iAP2TimeCheckTimeouts (iAP2Timer_t* timer)
{
    uint8_t index = iAP2ListArrayGetFirstItemIndex (timer->listBuffer);
    iAP2Timeout_t* timeout = (iAP2Timeout_t*) iAP2ListArrayItemForIndex (timer->listBuffer, index);
    if (timeout && (timeout->timeout < timer->runningTimeOut || timer->runningTimeOut == INVALID_TIMEOUT_VAL))
    {
        uint32_t curTime;
        if (timer->runningTimeOut != INVALID_TIMEOUT_VAL)
        {
            timer->runningTimeOut = INVALID_TIMEOUT_VAL;
            _iAP2TimeCancelCallback (timer);
        }
        curTime = iAP2TimeGetCurTimeMs();
        timer->runningTimeOut = timeout->timeout;
        _iAP2TimeCallbackAfter (timer,
                                timeout->timeout - curTime,
                                _iAP2TimeHandleExpired);
    }
}
#endif
void _iAP2TimeCheckTimeouts (iAP2Timer_t* timer)
{
    return;
}


/*
****************************************************************
**
**  iAP2TimeGetCurTimeMs
**
**  Input:
**      None
**
**  Output:
**      None
**
**  Return:
**      uint32_t    current time in ms.
**
****************************************************************
*/
uint32_t iAP2TimeGetCurTimeMs (void)
{
    uint32_t curtick;

    curtick = stimer_get_tick()/SYSTEM_TIMER_TICK_1US;
    curtick = curtick/1000;
    //tlkapi_trace(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "curtick=%d", curtick);
    return curtick;
}


/*
****************************************************************
**
**  iAP2TimeInit
**
**  Input:
**      None
**
**  Output:
**      None
**
**  Return:
**      None
**
**  Note: Must be called before using any other functions.
**        OBSOLETE!!!!
**
****************************************************************
*/
void iAP2TimeInit (void)
{
    /* Do nothing */
}


/*
****************************************************************
**
**  iAP2TimeCleanup
**
**  Input:
**      None
**
**  Output:
**      None
**
**  Return:
**      None
**
**  Note: Called when iAP2Time functions are no longer required.
**        OBSOLETE!!!!
**
****************************************************************
*/
void iAP2TimeCleanup (void)
{
    /* Do nothing */
}


/*
****************************************************************
**
**  iAP2TimeGetBuffSize
**
**  Input:
**      maxTimeouts:    max number of timers that can be "active" at once.
**
**  Output:
**      None
**
**  Return:
**      uint32_t    minimum size of timeBuff required for proper operation
**
****************************************************************
*/
uint32_t iAP2TimeGetBuffSize (uint8_t maxTimeouts)
{
    uint32_t result = sizeof(iAP2Timer_t) + iAP2ListArrayGetBuffSize (maxTimeouts, sizeof(iAP2Timeout_t));
    return result;
}


/*
****************************************************************
**
**  iAP2TimeCreate
**
**  Input:
**      context:        context to use on calls to user implemented functions.
**                          (should be pointer to link)
**      expiredCB:      function to callback on timer expired
**      cancelCB:       function to callback on timer cancel
**      maxTimeouts:    max number of timers that will be "active" at once.
**
**  Output:
**      Passed in timeBuff is initialized.
**
**  Return:
**      iAP2Timer_t*    passed in timeBuff
**
**  Note:   Only one iAP2TimeInit call and one iAP2Timer_t instance is
**          required per link.
**          One iAP2Timer_t can handle multiple timeOuts upto maxTimeouts.
**
****************************************************************
*/
iAP2Timer_t* iAP2TimeCreate (void*               context,
                             iAP2TimeExpireCB_t  expiredCB,
                             iAP2TimeCancelCB_t  cancelCB,
                             uint8_t             maxTimeouts,
                             uint8_t*            timeBuff)
{
    iAP2Timer_t* timer;
    uint8_t* timeBuffNextFree = timeBuff;

    timer = (iAP2Timer_t*) timeBuffNextFree;
    timeBuffNextFree += sizeof(iAP2Timer_t);
    timer->link         = context;
    timer->context3     = NULL;
    timer->expiredCB    = expiredCB;
    timer->cancelCB     = cancelCB;
    timer->maxTimeOuts  = maxTimeouts;
    timer->runningTimeOut = INVALID_TIMEOUT_VAL;
    timer->nextID       = 0;
    timer->listBuffer   = timeBuffNextFree;
    timeBuffNextFree += iAP2ListArrayGetBuffSize(timer->maxTimeOuts, sizeof(iAP2Timeout_t));
    iAP2ListArrayInit (timer->listBuffer,
                       timer->maxTimeOuts,
                       sizeof(iAP2Timeout_t));
    timer->timeBuff     = timeBuff;
    timer->timeBuffNextFree = timeBuffNextFree;

    /*
    ** Following are for compatibility with previous implementation...
    ** can be removed once all dependent code is fixed.
    */
    timer->origTime     = 0;
    timer->timeout      = 0;
    timer->tag          = "mainTimer";
    timer->tagData      = 0;
    timer->packet       = NULL;
    timer->inReleaseList = FALSE;

    return timer;
}


/*
****************************************************************
**
**  iAP2TimeDelete
**
**  Input:
**      timer:  pointer to timer to cancel
**
**  Output:
**      None
**
**  Return:
**      None
**
****************************************************************
*/
void iAP2TimeDelete (iAP2Timer_t* timer)
{
    timer->runningTimeOut = INVALID_TIMEOUT_VAL;
    _iAP2TimeCancelCallback (timer);
    iAP2ListArrayCleanup (timer->listBuffer, NULL);
    _iAP2TimeCleanupCallback (timer);
}


/*
****************************************************************
**
**  iAP2TimeGetContext
**
**  Input:
**      timer:  pointer to timer
**
**  Output:
**      None
**
**  Return:
**      void* context associated with the timer
**
****************************************************************
*/
void* iAP2TimeGetContext (iAP2Timer_t* timer)
{
    return timer->link;
}


/*
****************************************************************
**
**  iAP2TimeCallbackAfter
**
**  Input:
**      timer:      timer
**      type:           user defined timeout type
**      delayMs:    amount of time to wait before callback is called
**
**  Output:
**      None
**
**  Return:
**      uint8_t     return INVALID_TIMEOUT_ID if error, else returns the timeoutID
**                  of registered timeOut
**
**  Note:   After call to this function, expireCB function registered in
**          iAP2TimeInit will be called after specified delayMs.
**
****************************************************************
*/
uint8_t iAP2TimeCallbackAfter (iAP2Timer_t* timer,
                               uint8_t      type,
                               uint32_t     delayMs)
{
    uint8_t timeoutID = _iAP2TimeGetFreeTimeoutID(timer);
    iAP2Timeout_t timeout;
    timeout.timeout = iAP2TimeGetCurTimeMs() + delayMs;
    timeout.id      = timeoutID;
    timeout.type    = type;
    tlkapi_trace(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "iAP2TimeCallbackAfter id=%d, type=%d, timeout=%d",
		timeout.id, timeout.type, delayMs);
    iAP2ListArrayAddItemInOrder (timer->listBuffer, &timeout, (piAP2ListArrayCompareFunc)_iAP2TimeCompareTimeout);
    _iAP2TimeCheckTimeouts (timer);
    return timeoutID;
}


/*
****************************************************************
**
**  iAP2TimeCancelTimer
**
**  Input:
**      timer:      timer
**      timeoutID:  timeoutID to cancel
**
**  Output:
**      None
**
**  Return:
**      None
**
**  Note:   This must be implemented by user of the library.
**
****************************************************************
*/
void iAP2TimeCancelTimer (iAP2Timer_t* timer,
                          uint8_t      timeoutID)
{
   
    uint8_t index;
    iAP2Timeout_t temp;
    temp.id = timeoutID;
    index = iAP2ListArrayFindItem (timer->listBuffer, &temp, (piAP2ListArrayCompareFunc)_iAP2TimeCompareID);
    
    if (index != kiAP2ListArrayInvalidIndex)
    {
		tlkapi_trace(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "iAP2TimeCancelTimer id=%d", timeoutID);
		tlkapi_trace(BTP_IAP_DBG_FLAG, BTP_IAP_DBG_SIGN, "iAP2TimeCancelTimer index=%d", index);
        iAP2ListArrayDeleteItem (timer->listBuffer, index, NULL);
        _iAP2TimeCheckTimeouts (timer);
    }
}


/*
****************************************************************
**
**  iAP2TimeGetType
**
**  Input:
**      timer:      timer
**      timeoutID:  timeoutID to type for
**
**  Output:
**      None
**
**  Return:
**      None
**
**  Note:   This must be implemented by user of the library.
**
****************************************************************
*/
uint8_t iAP2TimeGetType (iAP2Timer_t* timer,
                         uint8_t      timeoutID)
{
    uint8_t type = INVALID_TIMEOUT_TYPE;
    uint8_t index;
    iAP2Timeout_t temp;
    temp.id = timeoutID;
    index = iAP2ListArrayFindItem (timer->listBuffer, &temp, (piAP2ListArrayCompareFunc)_iAP2TimeCompareID);
    if (index != kiAP2ListArrayInvalidIndex)
    {
        iAP2Timeout_t* timeout;
        timeout = (iAP2Timeout_t*) iAP2ListArrayItemForIndex (timer->listBuffer, index);
        type = timeout->type;
    }
    return type;
}


/*
****************************************************************
**
**  __printTimeOutItem
**
****************************************************************
*/

static void __printTimeOutItem (void* item)
{
    iAP2Timeout_t* timeout = (iAP2Timeout_t*) item;
    iAP2Log("        entry(%p): id=%u type=%u tiemout=%u\n", timeout, timeout->id, timeout->type, timeout->timeout);
}


/*
****************************************************************
**
**  iAP2TimePrintInfo
**
**  Input:
**      timer:      timer
**
**  Output:
**      None
**
**  Return:
**      None
**
****************************************************************
*/
void iAP2TimePrintInfo (iAP2Timer_t* timer, BOOL needStartStop)
{
    if (needStartStop)
    {
        iAP2LogStart();
    }
    iAP2Log("TIMER info (%p): link=%p context3=%p\n", timer, timer->link, timer->context3);
    iAP2Log("    expiredCB=%p cancelCB=%p timeBuff=%p timeBuffNextFree=%p\n", timer->expiredCB, timer->cancelCB, timer->timeBuff, timer->timeBuffNextFree);
    iAP2Log("    nextID=%u runningTimeOut=%u\n", timer->nextID, timer->runningTimeOut);
    iAP2Log("    LIST (%p):  maxTimeOuts=%u count=%u\n", timer->listBuffer, timer->maxTimeOuts, iAP2ListArrayGetCount(timer->listBuffer));
    iAP2ListArrayForEach(timer->listBuffer, __printTimeOutItem);
    if (needStartStop)
    {
        iAP2LogStop();
    }
}


#endif //#if (TLKBTP_CFG_IAP_ENABLE)

#endif //#if (TLK_STK_BTP_ENABLE)

