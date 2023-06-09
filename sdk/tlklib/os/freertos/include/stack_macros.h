/********************************************************************************************************
 * @file     stack_macros.h
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

/*
 * FreeRTOS Kernel V10.4.2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef STACK_MACROS_H
#define STACK_MACROS_H

#if (TLK_OS_FREERTOS_ENABLE)

/*
 * Call the stack overflow hook function if the stack of the task being swapped
 * out is currently overflowed, or looks like it might have overflowed in the
 * past.
 *
 * Setting configCHECK_FOR_STACK_OVERFLOW to 1 will cause the macro to check
 * the current stack state only - comparing the current top of stack value to
 * the stack limit.  Setting configCHECK_FOR_STACK_OVERFLOW to greater than 1
 * will also cause the last few stack bytes to be checked to ensure the value
 * to which the bytes were set when the task was created have not been
 * overwritten.  Note this second test does not guarantee that an overflowed
 * stack will always be recognised.
 */

/*-----------------------------------------------------------*/

#if ( ( configCHECK_FOR_STACK_OVERFLOW == 1 ) && ( portSTACK_GROWTH < 0 ) )

/* Only the current stack state is to be checked. */
    #define taskCHECK_FOR_STACK_OVERFLOW()                                                            \
    {                                                                                                 \
        /* Is the currently saved stack pointer within the stack limit? */                            \
        if( pxCurrentTCB->pxTopOfStack <= pxCurrentTCB->pxStack )                                     \
        {                                                                                             \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB, pxCurrentTCB->pcTaskName ); \
        }                                                                                             \
    }

#endif /* configCHECK_FOR_STACK_OVERFLOW == 1 */
/*-----------------------------------------------------------*/

#if ( ( configCHECK_FOR_STACK_OVERFLOW == 1 ) && ( portSTACK_GROWTH > 0 ) )

/* Only the current stack state is to be checked. */
    #define taskCHECK_FOR_STACK_OVERFLOW()                                                            \
    {                                                                                                 \
                                                                                                      \
        /* Is the currently saved stack pointer within the stack limit? */                            \
        if( pxCurrentTCB->pxTopOfStack >= pxCurrentTCB->pxEndOfStack )                                \
        {                                                                                             \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB, pxCurrentTCB->pcTaskName ); \
        }                                                                                             \
    }

#endif /* configCHECK_FOR_STACK_OVERFLOW == 1 */
/*-----------------------------------------------------------*/

#if ( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) && ( portSTACK_GROWTH < 0 ) )

    #define taskCHECK_FOR_STACK_OVERFLOW()                                                            \
    {                                                                                                 \
        const uint32_t * const pulStack = ( uint32_t * ) pxCurrentTCB->pxStack;                       \
        const uint32_t ulCheckValue = ( uint32_t ) 0xa5a5a5a5;                                        \
                                                                                                      \
        if( ( pulStack[ 0 ] != ulCheckValue ) ||                                                      \
            ( pulStack[ 1 ] != ulCheckValue ) ||                                                      \
            ( pulStack[ 2 ] != ulCheckValue ) ||                                                      \
            ( pulStack[ 3 ] != ulCheckValue ) )                                                       \
        {                                                                                             \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB, pxCurrentTCB->pcTaskName ); \
        }                                                                                             \
    }

#endif /* #if( configCHECK_FOR_STACK_OVERFLOW > 1 ) */
/*-----------------------------------------------------------*/

#if ( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) && ( portSTACK_GROWTH > 0 ) )

    #define taskCHECK_FOR_STACK_OVERFLOW()                                                                                                \
    {                                                                                                                                     \
        int8_t * pcEndOfStack = ( int8_t * ) pxCurrentTCB->pxEndOfStack;                                                                  \
        static const uint8_t ucExpectedStackBytes[] = { tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE }; \
                                                                                                                                          \
                                                                                                                                          \
        pcEndOfStack -= sizeof( ucExpectedStackBytes );                                                                                   \
                                                                                                                                          \
        /* Has the extremity of the task stack ever been written over? */                                                                 \
        if( memcmp( ( void * ) pcEndOfStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) != 0 )                     \
        {                                                                                                                                 \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB, pxCurrentTCB->pcTaskName );                                     \
        }                                                                                                                                 \
    }

#endif /* #if( configCHECK_FOR_STACK_OVERFLOW > 1 ) */
/*-----------------------------------------------------------*/

/* Remove stack overflow macro if not being used. */
#ifndef taskCHECK_FOR_STACK_OVERFLOW
    #define taskCHECK_FOR_STACK_OVERFLOW()
#endif


#endif //#if (TLK_OS_FREERTOS_ENABLE)

#endif /* STACK_MACROS_H */
