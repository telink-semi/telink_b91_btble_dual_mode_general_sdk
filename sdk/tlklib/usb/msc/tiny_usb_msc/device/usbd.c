/********************************************************************************************************
 * @file     usbd.c
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

#include "../tusb_option.h"

#include "../tusb.h"
#include "usbd.h"
#include "usbd_pvt.h"
#include "dcd.h"

#ifndef CFG_TUD_TASK_QUEUE_SZ
#define CFG_TUD_TASK_QUEUE_SZ   16
#endif

#ifndef CFG_TUD_EP_MAX
#define CFG_TUD_EP_MAX          9
#endif
static void mark_interface_endpoint(uint8_t ep2drv[][2], uint8_t const* p_desc, uint16_t desc_len, uint8_t driver_id);
static bool process_set_config(uint8_t rhport);
//--------------------------------------------------------------------+
// Device Data
//--------------------------------------------------------------------+

// Invalid driver ID in itf2drv[] ep2drv[][] mapping
enum { DRVID_INVALID = 0xFFu };

typedef struct
{
  struct
  {
    volatile uint8_t connected    : 1;
    volatile uint8_t addressed    : 1;
    volatile uint8_t suspended    : 1;

    uint8_t remote_wakeup_en      : 1; // enable/disable by host
    uint8_t remote_wakeup_support : 1; // configuration descriptor's attribute
    uint8_t self_powered          : 1; // configuration descriptor's attribute
  };

  volatile uint8_t cfg_num; // current active configuration (0x00 is not configured)
  uint8_t speed;

  uint8_t itf2drv[16];     // map interface number to driver (0xff is invalid)
  uint8_t ep2drv[CFG_TUD_EP_MAX][2]; // map endpoint to driver ( 0xff is invalid )

  struct
  {
    volatile bool busy    : 1;
    volatile bool stalled : 1;
    volatile bool claimed : 1;

    // TODO merge ep2drv here, 4-bit should be sufficient
  }ep_status[CFG_TUD_EP_MAX][2];

}usbd_device_t;

static usbd_device_t _usbd_dev;

//--------------------------------------------------------------------+
// Class Driver
//--------------------------------------------------------------------+
#if CFG_TUSB_DEBUG >= 2
  #define DRIVER_NAME(_name)    .name = _name,
#else
  #define DRIVER_NAME(_name)
#endif

// Built-in class drivers
static usbd_class_driver_t const _usbd_driver[] =
{
  {
    DRIVER_NAME("MSC")
    .init             = mscd_init,
    .reset            = mscd_reset,
    .open             = mscd_open,
    .xfer_cb          = mscd_xfer_cb,
    .sof              = NULL
  }
};
enum { BUILTIN_DRIVER_COUNT = TU_ARRAY_SIZE(_usbd_driver) };

// Additional class drivers implemented by application
static usbd_class_driver_t const * _app_driver = NULL;
static uint8_t _app_driver_count = 0;

// virtually joins built-in and application drivers together.
// Application is positioned first to allow overwriting built-in ones.
static inline usbd_class_driver_t const * get_driver(uint8_t drvid)
{
  // Application drivers
  if ( usbd_app_driver_get_cb )
  {
    if ( drvid < _app_driver_count ) return &_app_driver[drvid];
    drvid -= _app_driver_count;
  }

  // Built-in drivers
  if (drvid < BUILTIN_DRIVER_COUNT) return &_usbd_driver[drvid];

  return NULL;
}

#define TOTAL_DRIVER_COUNT    (_app_driver_count + BUILTIN_DRIVER_COUNT)

//--------------------------------------------------------------------+
// DCD Event
//--------------------------------------------------------------------+

static bool _usbd_initialized = false;

// Event queue
// OPT_MODE_DEVICE is used by OS NONE for mutex (disable usb isr)
OSAL_QUEUE_DEF(OPT_MODE_DEVICE, _usbd_qdef, CFG_TUD_TASK_QUEUE_SZ, dcd_event_t);
static osal_queue_t _usbd_q;


bool tud_mounted(void)
{
  return _usbd_dev.cfg_num ? true : false;
}


//--------------------------------------------------------------------+
// USBD Task
//--------------------------------------------------------------------+
bool tud_inited(void)
{
  return _usbd_initialized;
}

bool tud_init ()
{
  // skip if already initialized
  if (_usbd_initialized) return _usbd_initialized;

//  TU_LOG2("USBD init\r\n");

  tu_varclr(&_usbd_dev);


  // Init device queue & task
  _usbd_q = osal_queue_create(&_usbd_qdef);

  // Get application driver if available
  if ( usbd_app_driver_get_cb )
  {
    _app_driver = usbd_app_driver_get_cb(&_app_driver_count);
  }

  // Init class drivers
  for (uint8_t i = 0; i < TOTAL_DRIVER_COUNT; i++)
  {
    usbd_class_driver_t const * driver = get_driver(i);
//    TU_LOG2("%s init\r\n");
    driver->init();
  }

  // Init device controller driver
  //dcd_init();
 // dcd_int_enable();
  memset(_usbd_dev.itf2drv, DRVID_INVALID, sizeof(_usbd_dev.itf2drv));
  process_set_config(0);
  _usbd_initialized = true;

  return true;
}


/* USB Device Driver task
 * This top level thread manages all device controller event and delegates events to class-specific drivers.
 * This should be called periodically within the mainloop or rtos thread.
 *
   @code
    int main(void)
    {
      application_init();
      tusb_init();

      while(1) // the mainloop
      {
        application_code();
        tud_task(); // tinyusb device task
      }
    }
    @endcode
 */
void tud_task (void)
{
  // Skip if stack is not initialized
  if ( !tusb_inited() )
	  {
	  //tusb_init();
	  return;
	  }

  // Loop until there is no more events in the queue
  while (1)
  {
    dcd_event_t event;

    if ( !osal_queue_receive(_usbd_q, &event) ) return;

    switch ( event.event_id )
    {
      case DCD_EVENT_XFER_COMPLETE:
      {
        // Invoke the class callback associated with the endpoint address
        uint8_t const ep_addr = event.xfer_complete.ep_addr;
        uint8_t const epnum   = tu_edpt_number(ep_addr);
        uint8_t const ep_dir  = tu_edpt_dir(ep_addr);

//        TU_LOG2("on EP %02X with %u bytes\r\n", ep_addr, (unsigned int) event.xfer_complete.len);

        _usbd_dev.ep_status[epnum][ep_dir].busy = false;
        _usbd_dev.ep_status[epnum][ep_dir].claimed = 0;

         usbd_class_driver_t const * driver = get_driver( _usbd_dev.ep2drv[epnum][ep_dir] );

//         TU_LOG2("  %s xfer callback\r\n");
         driver->xfer_cb(event.rhport, ep_addr, (xfer_result_t)event.xfer_complete.result, event.xfer_complete.len);
      }
      break;
      default:
        TU_BREAKPOINT();
      break;
    }
  }
}

//--------------------------------------------------------------------+
// Control Request Parser & Handling
//--------------------------------------------------------------------+


//--------------------------------------------------------------------+
// DCD Event Handler
//--------------------------------------------------------------------+
void dcd_event_handler(dcd_event_t const * event, bool in_isr)
{

   osal_queue_send(_usbd_q, event, in_isr);

}

void dcd_event_xfer_complete (uint8_t rhport, uint8_t ep_addr, uint32_t xferred_bytes, uint8_t result, bool in_isr)
{
  dcd_event_t event = { .rhport = rhport, .event_id = DCD_EVENT_XFER_COMPLETE };

  event.xfer_complete.ep_addr = ep_addr;
  event.xfer_complete.len     = xferred_bytes;
  event.xfer_complete.result  = result;

  dcd_event_handler(&event, in_isr);
}

//--------------------------------------------------------------------+
// USBD API For Class Driver
//--------------------------------------------------------------------+

// Parse consecutive endpoint descriptors (IN & OUT)
bool usbd_open_edpt_pair(uint8_t rhport, uint8_t const* p_desc, uint8_t ep_count, uint8_t xfer_type, uint8_t* ep_out, uint8_t* ep_in)
{
  for(int i=0; i<ep_count; i++)
  {
    tusb_desc_endpoint_t const * desc_ep = (tusb_desc_endpoint_t const *) p_desc;

    TU_ASSERT(TUSB_DESC_ENDPOINT == desc_ep->bDescriptorType && xfer_type == desc_ep->bmAttributes.xfer);
    TU_ASSERT(usbd_edpt_open(rhport, desc_ep));

    if ( tu_edpt_dir(desc_ep->bEndpointAddress) == TUSB_DIR_IN )
    {
      (*ep_in) = desc_ep->bEndpointAddress;
    }else
    {
      (*ep_out) = desc_ep->bEndpointAddress;
    }

    p_desc = tu_desc_next(p_desc);
  }

  return true;
}

//--------------------------------------------------------------------+
// USBD Endpoint API
//--------------------------------------------------------------------+

bool usbd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * desc_ep)
{
//  TU_LOG2("  Open EP %02X with Size = %u\r\n", desc_ep->bEndpointAddress, desc_ep->wMaxPacketSize.size);

  switch (desc_ep->bmAttributes.xfer)
  {
    case TUSB_XFER_ISOCHRONOUS:
    {
      uint16_t const max_epsize = (_usbd_dev.speed == TUSB_SPEED_HIGH ? 1024 : 1023);
      TU_ASSERT(desc_ep->wMaxPacketSize.size <= max_epsize);
    }
    break;

    case TUSB_XFER_BULK:
      if (_usbd_dev.speed == TUSB_SPEED_HIGH)
      {
        // Bulk highspeed must be EXACTLY 512
        TU_ASSERT(desc_ep->wMaxPacketSize.size == 512);
      }else
      {
        // TODO Bulk fullspeed can only be 8, 16, 32, 64
        TU_ASSERT(desc_ep->wMaxPacketSize.size <= 64);
      }
    break;

    case TUSB_XFER_INTERRUPT:
    {
      uint16_t const max_epsize = (_usbd_dev.speed == TUSB_SPEED_HIGH ? 1024 : 64);
      TU_ASSERT(desc_ep->wMaxPacketSize.size <= max_epsize);
    }
    break;

    default: return false;
  }

  return dcd_edpt_open(rhport, desc_ep);
}

bool usbd_edpt_claim(uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);


  // can only claim the endpoint if it is not busy and not claimed yet.
  bool const ret = (_usbd_dev.ep_status[epnum][dir].busy == 0) && (_usbd_dev.ep_status[epnum][dir].claimed == 0);
  if (ret)
  {
    _usbd_dev.ep_status[epnum][dir].claimed = 1;
  }


  return ret;
}

bool usbd_edpt_release(uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);



  // can only release the endpoint if it is claimed and not busy
  bool const ret = (_usbd_dev.ep_status[epnum][dir].busy == 0) && (_usbd_dev.ep_status[epnum][dir].claimed == 1);
  if (ret)
  {
    _usbd_dev.ep_status[epnum][dir].claimed = 0;
  }


  return ret;
}

bool usbd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);

//  TU_LOG2("  Queue EP %02X with %u bytes ... ", ep_addr, total_bytes);

  // Attempt to transfer on a busy endpoint, sound like an race condition !
  TU_ASSERT(_usbd_dev.ep_status[epnum][dir].busy == 0);

  // Set busy first since the actual transfer can be complete before dcd_edpt_xfer() could return
  // and usbd task can preempt and clear the busy
  _usbd_dev.ep_status[epnum][dir].busy = true;

  if ( dcd_edpt_xfer(rhport, ep_addr, buffer, total_bytes) )
  {
//    TU_LOG2("OK\r\n");
    return true;
  }else
  {
    // DCD error, mark endpoint as ready to allow next transfer
    _usbd_dev.ep_status[epnum][dir].busy = false;
    _usbd_dev.ep_status[epnum][dir].claimed = 0;
//    TU_LOG2("failed\r\n");
    TU_BREAKPOINT();
    return false;
  }
}

bool usbd_edpt_busy(uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);

  return _usbd_dev.ep_status[epnum][dir].busy;
}

void usbd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);

  dcd_edpt_stall(rhport, ep_addr);
  _usbd_dev.ep_status[epnum][dir].stalled = true;
  _usbd_dev.ep_status[epnum][dir].busy = true;
}

void usbd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);

  dcd_edpt_clear_stall(rhport, ep_addr);
  _usbd_dev.ep_status[epnum][dir].stalled = false;
  _usbd_dev.ep_status[epnum][dir].busy = false;
}

bool usbd_edpt_stalled(uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  uint8_t const epnum = tu_edpt_number(ep_addr);
  uint8_t const dir   = tu_edpt_dir(ep_addr);

  return _usbd_dev.ep_status[epnum][dir].stalled;
}

/**
 * usbd_edpt_close will disable an endpoint.
 *
 * In progress transfers on this EP may be delivered after this call.
 *
 */
void usbd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
//  TU_LOG2("  CLOSING Endpoint: 0x%02X\r\n", ep_addr);

  dcd_edpt_close(rhport, ep_addr);

  return;
}
extern uint8_t const desc_fs_configuration[];
// Process Set Configure Request
// This function parse configuration descriptor & open drivers accordingly
static bool process_set_config(uint8_t rhport)
{
  tusb_desc_configuration_t const * desc_cfg = (tusb_desc_configuration_t const *) desc_fs_configuration; // index is cfg_num-1
  TU_ASSERT(desc_cfg != NULL && desc_cfg->bDescriptorType == TUSB_DESC_CONFIGURATION);

  // Parse configuration descriptor
  _usbd_dev.remote_wakeup_support = (desc_cfg->bmAttributes & TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP) ? 1 : 0;
  _usbd_dev.self_powered = (desc_cfg->bmAttributes & TUSB_DESC_CONFIG_ATT_SELF_POWERED) ? 1 : 0;

  // Parse interface descriptor
  uint8_t const * p_desc   = ((uint8_t const*) desc_cfg) + sizeof(tusb_desc_configuration_t);
  uint8_t const * desc_end = ((uint8_t const*) desc_cfg) + desc_cfg->wTotalLength;

  while( p_desc < desc_end )
  {
    tusb_desc_interface_assoc_t const * desc_itf_assoc = NULL;

    // Class will always starts with Interface Association (if any) and then Interface descriptor
    if ( TUSB_DESC_INTERFACE_ASSOCIATION == tu_desc_type(p_desc) )
    {
      desc_itf_assoc = (tusb_desc_interface_assoc_t const *) p_desc;
      p_desc = tu_desc_next(p_desc); // next to Interface
    }

    TU_ASSERT( TUSB_DESC_INTERFACE == tu_desc_type(p_desc) );

    tusb_desc_interface_t const * desc_itf = (tusb_desc_interface_t const*) p_desc;
    uint16_t const remaining_len = desc_end-p_desc;

    uint8_t drv_id;
    for (drv_id = 0; drv_id < TOTAL_DRIVER_COUNT; drv_id++)
    {
      usbd_class_driver_t const *driver = get_driver(drv_id);
      uint16_t const drv_len = driver->open(rhport, desc_itf, remaining_len);

      if ( drv_len > 0 )
      {
        // Open successfully, check if length is correct
        TU_ASSERT( sizeof(tusb_desc_interface_t) <= drv_len && drv_len <= remaining_len);

        // Interface number must not be used already
        TU_ASSERT(DRVID_INVALID == _usbd_dev.itf2drv[desc_itf->bInterfaceNumber]);

//        TU_LOG2("  %s opened\r\n");
        _usbd_dev.itf2drv[desc_itf->bInterfaceNumber] = drv_id;

        // If IAD exist, assign all interfaces to the same driver
        if (desc_itf_assoc)
        {
          // IAD's first interface number and class should match with opened interface
          TU_ASSERT(desc_itf_assoc->bFirstInterface == desc_itf->bInterfaceNumber &&
                    desc_itf_assoc->bFunctionClass  == desc_itf->bInterfaceClass);

          for(uint8_t i=1; i<desc_itf_assoc->bInterfaceCount; i++)
          {
            _usbd_dev.itf2drv[desc_itf->bInterfaceNumber+i] = drv_id;
          }
        }

        mark_interface_endpoint(_usbd_dev.ep2drv, p_desc, drv_len, drv_id); // TODO refactor

        p_desc += drv_len; // next interface

        break;
      }
    }

    // Failed if cannot find supported driver
    TU_ASSERT(drv_id < TOTAL_DRIVER_COUNT);
  }

  // invoke callback
 // if (tud_mount_cb) tud_mount_cb();

  return true;
}

// Helper marking endpoint of interface belongs to class driver
static void mark_interface_endpoint(uint8_t ep2drv[][2], uint8_t const* p_desc, uint16_t desc_len, uint8_t driver_id)
{
  uint16_t len = 0;

  while( len < desc_len )
  {
    if ( TUSB_DESC_ENDPOINT == tu_desc_type(p_desc) )
    {
      uint8_t const ep_addr = ((tusb_desc_endpoint_t const*) p_desc)->bEndpointAddress;

      ep2drv[tu_edpt_number(ep_addr)][tu_edpt_dir(ep_addr)] = driver_id;
    }

    len   = (uint16_t)(len + tu_desc_len(p_desc));
    p_desc = tu_desc_next(p_desc);
  }
}
