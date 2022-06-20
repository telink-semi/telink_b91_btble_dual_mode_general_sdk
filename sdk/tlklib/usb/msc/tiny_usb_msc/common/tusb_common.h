/********************************************************************************************************
 * @file     tusb_common.h
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

/** \ingroup Group_Common
 *  \defgroup Group_CommonH common.h
 *  @{ */
#pragma once
//--------------------------------------------------------------------+
// Includes
//--------------------------------------------------------------------+

// Standard Headers
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>


#define TU_ATTR_WEAK __attribute__((weak))


#define TU_BSWAP16(u16) (__builtin_bswap16(u16))
#define TU_BSWAP32(u32) (__builtin_bswap32(u32))

#define tu_htons(u16)  (TU_BSWAP16(u16))
#define tu_ntohs(u16)  (TU_BSWAP16(u16))

#define tu_htonl(u32)  (TU_BSWAP32(u32))
#define tu_ntohl(u32)  (TU_BSWAP32(u32))

//--------------------------------------------------------------------+
// Macros Helper
//--------------------------------------------------------------------+
#define TU_ARRAY_SIZE(_arr)   ( sizeof(_arr) / sizeof(_arr[0]) )
#define TU_MIN(_x, _y)        ( ( (_x) < (_y) ) ? (_x) : (_y) )
#define TU_MAX(_x, _y)        ( ( (_x) > (_y) ) ? (_x) : (_y) )

#define TU_U16_HIGH(u16)      ((uint8_t) (((u16) >> 8) & 0x00ff))
#define TU_U16_LOW(u16)       ((uint8_t) ((u16)       & 0x00ff))
#define U16_TO_U8S_BE(u16)    TU_U16_HIGH(u16), TU_U16_LOW(u16)
#define U16_TO_U8S_LE(u16)    TU_U16_LOW(u16), TU_U16_HIGH(u16)

#define TU_U32_BYTE3(u32)     ((uint8_t) ((((uint32_t) u32) >> 24) & 0x000000ff)) // MSB
#define TU_U32_BYTE2(u32)     ((uint8_t) ((((uint32_t) u32) >> 16) & 0x000000ff))
#define TU_U32_BYTE1(u32)     ((uint8_t) ((((uint32_t) u32) >>  8) & 0x000000ff))
#define TU_U32_BYTE0(u32)     ((uint8_t) (((uint32_t)  u32)        & 0x000000ff)) // LSB

#define U32_TO_U8S_BE(u32)    TU_U32_BYTE3(u32), TU_U32_BYTE2(u32), TU_U32_BYTE1(u32), TU_U32_BYTE0(u32)
#define U32_TO_U8S_LE(u32)    TU_U32_BYTE0(u32), TU_U32_BYTE1(u32), TU_U32_BYTE2(u32), TU_U32_BYTE3(u32)

#define TU_BIT(n)             (1U << (n))



//------------- Mem -------------//
#define tu_memclr(buffer, size)  memset((buffer), 0, (size))
#define tu_varclr(_var)          tu_memclr(_var, sizeof(*(_var)))

//------------- Bytes -------------//
static inline uint32_t tu_u32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
  return ( ((uint32_t) b3) << 24) | ( ((uint32_t) b2) << 16) | ( ((uint32_t) b1) << 8) | b0;
}

static inline uint16_t tu_u16(uint8_t high, uint8_t low)
{
  return (uint16_t) ((((uint16_t) high) << 8) | low);
}

static inline uint8_t tu_u32_byte3(uint32_t u32) { return TU_U32_BYTE3(u32); }
static inline uint8_t tu_u32_byte2(uint32_t u32) { return TU_U32_BYTE2(u32); }
static inline uint8_t tu_u32_byte1(uint32_t u32) { return TU_U32_BYTE1(u32); }
static inline uint8_t tu_u32_byte0(uint32_t u32) { return TU_U32_BYTE0(u32); }

static inline uint8_t tu_u16_high(uint16_t u16) { return TU_U16_HIGH(u16); }
static inline uint8_t tu_u16_low (uint16_t u16) { return TU_U16_LOW(u16); }

//------------- Bits -------------//
static inline uint32_t tu_bit_set  (uint32_t value, uint8_t pos) { return value | TU_BIT(pos);                  }
static inline uint32_t tu_bit_clear(uint32_t value, uint8_t pos) { return value & (~TU_BIT(pos));               }
static inline bool     tu_bit_test (uint32_t value, uint8_t pos) { return (value & TU_BIT(pos)) ? true : false; }

//------------- Min -------------//
static inline uint8_t  tu_min8  (uint8_t  x, uint8_t y ) { return (x < y) ? x : y; }
static inline uint16_t tu_min16 (uint16_t x, uint16_t y) { return (x < y) ? x : y; }
static inline uint32_t tu_min32 (uint32_t x, uint32_t y) { return (x < y) ? x : y; }

//------------- Max -------------//
static inline uint8_t  tu_max8  (uint8_t  x, uint8_t y ) { return (x > y) ? x : y; }
static inline uint16_t tu_max16 (uint16_t x, uint16_t y) { return (x > y) ? x : y; }
static inline uint32_t tu_max32 (uint32_t x, uint32_t y) { return (x > y) ? x : y; }

//------------- Align -------------//
static inline uint32_t tu_align(uint32_t value, uint32_t alignment)
{
  return value & ((uint32_t) ~(alignment-1));
}

static inline uint32_t tu_align16 (uint32_t value) { return (value & 0xFFFFFFF0UL); }
static inline uint32_t tu_align32 (uint32_t value) { return (value & 0xFFFFFFE0UL); }
static inline uint32_t tu_align4k (uint32_t value) { return (value & 0xFFFFF000UL); }
static inline uint32_t tu_offset4k(uint32_t value) { return (value & 0xFFFUL); }

//------------- Mathematics -------------//
static inline uint32_t tu_abs(int32_t value) { return (uint32_t)((value < 0) ? (-value) : value); }

/// inclusive range checking TODO remove
static inline bool tu_within(uint32_t lower, uint32_t value, uint32_t upper)
{
  return (lower <= value) && (value <= upper);
}

// log2 of a value is its MSB's position
// TODO use clz TODO remove
static inline uint8_t tu_log2(uint32_t value)
{
  uint8_t result = 0;
  while (value >>= 1) { result++; }
  return result;
}

// MCU that could access unaligned memory natively
static inline uint32_t tu_unaligned_read32  (const void* mem           ) { return *((uint32_t*) mem);  }
static inline uint16_t tu_unaligned_read16  (const void* mem           ) { return *((uint16_t*) mem);  }

static inline void     tu_unaligned_write32 (void* mem, uint32_t value ) { *((uint32_t*) mem) = value; }
static inline void     tu_unaligned_write16 (void* mem, uint16_t value ) { *((uint16_t*) mem) = value; }

// To be removed
//------------- Binary constant -------------//

#define TU_BIN8(x)               ((uint8_t)  (0b##x))
#define TU_BIN16(b1, b2)         ((uint16_t) (0b##b1##b2))
#define TU_BIN32(b1, b2, b3, b4) ((uint32_t) (0b##b1##b2##b3##b4))

//--------------------------------------------------------------------+
// Debug Function
//--------------------------------------------------------------------+

// CFG_TUSB_DEBUG for debugging
// 0 : no debug
// 1 : print when there is error
// 2 : print out log
#define CFG_TUSB_DEBUG 0
#if CFG_TUSB_DEBUG

#define tu_printf    printf

#else
#define tu_printf
#endif

static inline
void tu_print_var(uint8_t const* buf, uint32_t bufsize)
{
#if CFG_TUSB_DEBUG
	for(uint32_t i=0; i<bufsize; i++) tu_printf("%02X ", buf[i]);
#endif
}

// Log with debug level 1
#define TU_LOG1               tu_printf
#define TU_LOG1_VAR(_x)       tu_print_var((uint8_t const*)(_x), sizeof(*(_x)))
#define TU_LOG1_INT(_x)       tu_printf(#_x " = %ld\n", (uint32_t) (_x) )
#define TU_LOG1_HEX(_x)       tu_printf(#_x " = %lX\n", (uint32_t) (_x) )
#define TU_LOG1_LOCATION()    tu_printf("%s: %d:\r\n", __PRETTY_FUNCTION__, __LINE__)
#define TU_LOG1_FAILED()      tu_printf("%s: %d: Failed\r\n", __PRETTY_FUNCTION__, __LINE__)


  #define TU_LOG2             TU_LOG1
  #define TU_LOG2_VAR         TU_LOG1_VAR
  #define TU_LOG2_INT         TU_LOG1_INT
  #define TU_LOG2_HEX         TU_LOG1_HEX
  #define TU_LOG2_LOCATION()  TU_LOG1_LOCATION()



typedef struct
{
  uint32_t key;
  const char* data;
} tu_lookup_entry_t;

typedef struct
{
  uint16_t count;
  tu_lookup_entry_t const* items;
} tu_lookup_table_t;

static inline const char* tu_lookup_find(tu_lookup_table_t const* p_table, uint32_t key)
{
  for(uint16_t i=0; i<p_table->count; i++)
  {
    if (p_table->items[i].key == key) return p_table->items[i].data;
  }

  return NULL;
}
 // CFG_TUSB_DEBUG



/**  @} */
