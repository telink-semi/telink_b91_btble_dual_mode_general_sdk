/********************************************************************************************************
 * @file     timer_event.h
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

#ifndef TLKSTK_TIMER_H
#define TLKSTK_TIMER_H

typedef int (*ev_timer_callback_t)(uint32_t data);

/**
 *  @brief Definition for timer event
 */
typedef struct ev_timer_event {
	struct ev_timer_event *next;
    ev_timer_callback_t cb; //!< Callback function when expire, this must be specified
    uint32_t t;         //!< Used internal
    uint32_t interval;  //!< Used internal
    uint32_t data;             //!< Callback function arguments.
    uint8_t link_id;
} ev_timer_event_t;

typedef ev_timer_event_t * ev_timer_list_t;

extern void timer_event_init(void);
extern ev_timer_event_t *ev_on_timer(uint8_t link_id,ev_timer_callback_t cb, uint32_t data, uint32_t t_us, ev_timer_list_t *event_pend_list);
extern void ev_unon_timer(uint8_t link_id,ev_timer_callback_t cb, ev_timer_list_t *event_pend_list);
extern void ev_process_timer(ev_timer_list_t *event_pend_list);


#endif /* TLKSTK_TIMER_H */
