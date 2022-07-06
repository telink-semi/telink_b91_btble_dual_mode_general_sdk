/********************************************************************************************************
 * @file     tusb.c
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

#include "tusb_option.h"



#include "tusb.h"

#include "device/usbd_pvt.h"
#include "common/tusb_error.h"

bool tusb_init(void)
{
  TU_ASSERT ( tud_init() ); // init device stack
  return true;
}

bool tusb_inited(void)
{
  bool ret = false;

  ret = ret || tud_inited();

  return ret;
}

/*------------------------------------------------------------------*/
/* Debug
 *------------------------------------------------------------------*/
#if CFG_TUSB_DEBUG
#include <ctype.h>

char const* const tusb_strerr[TUSB_ERROR_COUNT] = { ERROR_TABLE(ERROR_STRING) };

static void dump_str_line(uint8_t const* buf, uint16_t count)
{
  tu_printf("  |");

  // each line is 16 bytes
  for(uint16_t i=0; i<count; i++)
  {
    const char ch = buf[i];
    tu_printf("%c", isprint(ch) ? ch : '.');
  }

  tu_printf("|\r\n");
}

/* Print out memory contents
 *  - buf   : buffer
 *  - count : number of item
 *  - indent: prefix spaces on every line
 */
void tu_print_mem(void const *buf, uint32_t count, uint8_t indent)
{
  uint8_t const size = 1; // fixed 1 byte for now

  if ( !buf || !count )
  {
    tu_printf("NULL\r\n");
    return;
  }

  uint8_t const *buf8 = (uint8_t const *) buf;

  char format[] = "%00X";
  format[2] += 2*size;

  const uint8_t item_per_line  = 16 / size;

  for(unsigned int i=0; i<count; i++)
  {
    unsigned int value=0;

    if ( i%item_per_line == 0 )
    {
      // Print Ascii
      if ( i != 0 )
      {
        dump_str_line(buf8-16, 16);
      }

      for(uint8_t s=0; s < indent; s++) tu_printf(" ");

      // print offset or absolute address
      tu_printf("%04X: ", 16*i/item_per_line);
    }

    memcpy(&value, buf8, size);
    buf8 += size;

    tu_printf(" ");
    tu_printf(format, value);
  }

  // fill up last row to 16 for printing ascii
  const uint32_t remain = count%16;
  uint8_t nback = (remain ? remain : 16);

  if ( remain )
  {
    for(uint32_t i=0; i< 16-remain; i++)
    {
      tu_printf(" ");
      for(int j=0; j<2*size; j++) tu_printf(" ");
    }
  }

  dump_str_line(buf8-nback, nback);
}

#endif