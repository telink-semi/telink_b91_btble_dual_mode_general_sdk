/********************************************************************************************************
 * @file     tusb_option.h
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
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#pragma once

#define TUSB_VERSION_MAJOR     0
#define TUSB_VERSION_MINOR     10
#define TUSB_VERSION_REVISION  0
/** \defgroup group_mcu Supported MCU
 * \ref CFG_TUSB_MCU must be defined to one of these
 *  @{ */

#define OPT_MCU_NONE                0

// LPC
#define OPT_MCU_LPC11UXX            1 ///< NXP LPC11Uxx
#define OPT_MCU_LPC13XX             2 ///< NXP LPC13xx
#define OPT_MCU_LPC15XX             3 ///< NXP LPC15xx
#define OPT_MCU_LPC175X_6X          4 ///< NXP LPC175x, LPC176x
#define OPT_MCU_LPC177X_8X          5 ///< NXP LPC177x, LPC178x
#define OPT_MCU_LPC18XX             6 ///< NXP LPC18xx
#define OPT_MCU_LPC40XX             7 ///< NXP LPC40xx
#define OPT_MCU_LPC43XX             8 ///< NXP LPC43xx
#define OPT_MCU_LPC51UXX            9 ///< NXP LPC51U6x
#define OPT_MCU_LPC54XXX           10 ///< NXP LPC54xxx
#define OPT_MCU_LPC55XX            11 ///< NXP LPC55xx

// NRF
#define OPT_MCU_NRF5X             100 ///< Nordic nRF5x series

// SAM
#define OPT_MCU_SAMD21            200 ///< MicroChip SAMD21
#define OPT_MCU_SAMD51            201 ///< MicroChip SAMD51
#define OPT_MCU_SAMG              202 ///< MicroChip SAMDG series
#define OPT_MCU_SAME5X            203 ///< MicroChip SAM E5x
#define OPT_MCU_SAMD11            204 ///< MicroChip SAMD11
#define OPT_MCU_SAML22            205 ///< MicroChip SAML22

// STM32
#define OPT_MCU_STM32F0           300 ///< ST STM32F0
#define OPT_MCU_STM32F1           301 ///< ST STM32F1
#define OPT_MCU_STM32F2           302 ///< ST STM32F2
#define OPT_MCU_STM32F3           303 ///< ST STM32F3
#define OPT_MCU_STM32F4           304 ///< ST STM32F4
#define OPT_MCU_STM32F7           305 ///< ST STM32F7
#define OPT_MCU_STM32H7           306 ///< ST STM32H7
#define OPT_MCU_STM32L0           307 ///< ST STM32L0
#define OPT_MCU_STM32L1           308 ///< ST STM32L1
#define OPT_MCU_STM32L4           309 ///< ST STM32L4

// Sony
#define OPT_MCU_CXD56             400 ///< SONY CXD56

// TI MSP430
#define OPT_MCU_MSP430x5xx        500 ///< TI MSP430x5xx

// ValentyUSB eptri
#define OPT_MCU_VALENTYUSB_EPTRI  600 ///< Fomu eptri config

// NXP iMX RT
#define OPT_MCU_MIMXRT10XX        700 ///< NXP iMX RT10xx

// Nuvoton
#define OPT_MCU_NUC121            800
#define OPT_MCU_NUC126            801
#define OPT_MCU_NUC120            802
#define OPT_MCU_NUC505            803

// Espressif
#define OPT_MCU_ESP32S2           900 ///< Espressif ESP32-S2
#define OPT_MCU_ESP32S3           901 ///< Espressif ESP32-S3

// Dialog
#define OPT_MCU_DA1469X          1000 ///< Dialog Semiconductor DA1469x

// Raspberry Pi
#define OPT_MCU_RP2040           1100 ///< Raspberry Pi RP2040

// NXP Kinetis
#define OPT_MCU_MKL25ZXX         1200 ///< NXP MKL25Zxx

// Silabs
#define OPT_MCU_EFM32GG          1300 ///< Silabs EFM32GG
#define OPT_MCU_EFM32GG11        1301 ///< Silabs EFM32GG11
#define OPT_MCU_EFM32GG12        1302 ///< Silabs EFM32GG12

// Renesas RX
#define OPT_MCU_RX63X            1400 ///< Renesas RX63N/631

// Telink TLSR9
#define OPT_MCU_TLSR9x1x       	 1500 ///< Telink TLSR9x1x
/** @} */

/** \defgroup group_supported_os Supported RTOS
 *  \ref CFG_TUSB_OS must be defined to one of these
 *  @{ */
#define OPT_OS_NONE       1  ///< No RTOS
#define OPT_OS_FREERTOS   2  ///< FreeRTOS
#define OPT_OS_MYNEWT     3  ///< Mynewt OS
#define OPT_OS_CUSTOM     4  ///< Custom OS is implemented by application
#define OPT_OS_PICO       5  ///< Raspberry Pi Pico SDK
#define OPT_OS_RTTHREAD   6  ///< RT-Thread
/** @} */

// Allow to use command line to change the config name/location
  #include "tusb_config.h"

/** \addtogroup group_configuration
 *  @{ */


//--------------------------------------------------------------------
// RootHub Mode Configuration
// CFG_TUSB_RHPORTx_MODE contains operation mode and speed for that port
//--------------------------------------------------------------------

#define OPT_MODE_DEVICE       0x01 ///< Device Mode

//--------------------------------------------------------------------+
// COMMON OPTIONS
//--------------------------------------------------------------------+

#define CFG_TUSB_OS             OPT_OS_NONE







/** @} */
