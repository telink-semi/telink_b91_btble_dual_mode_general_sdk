/********************************************************************************************************
 * @file	app.c
 *
 * @brief	This is the source file for B91m 2.4G
 *
 * @author	Telink
 * @date	Aug 2, 2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
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
 *
 *******************************************************************************************************/

#include "tlkapp_config.h"

static tpll_payload_t rx_payload;

static tpll_payload_t tx_payload = TPLL_CREATE_PAYLOAD(
		TPLL_PIPE0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20);

static volatile uint32_t irq_cnt_tt = 0;
static volatile uint32_t irq_cnt_ds = 0;
static volatile uint32_t irq_cnt_retry = 0;
static volatile uint32_t irq_cnt_dr = 0;
static volatile uint8_t rx_flag = 1;
static volatile uint8_t maxretry_flag = 1;

static volatile uint32_t tx_cnt = 0;

static volatile uint32_t evt_tx_finish_cnt, evt_tx_failed_cnt, evt_rx_cnt;

_attribute_ram_code_sec_ __attribute__((optimize("-Os"))) void tpll_event_handler(tpll_event_id_t evt_id)
{
    switch (evt_id)
    {
    case TPLL_EVENT_TX_SUCCESS:
        evt_tx_finish_cnt++;
        break;
    case TPLL_EVENT_TX_FAILED:
        evt_tx_failed_cnt++;
        break;
    case TPLL_EVENT_RX_RECEIVED:
        tpll_read_rx_payload(&rx_payload);
        evt_rx_cnt++;
        break;
    }
}

void rf_irq_handler(void)
{
    tpll_event_handler_t m_event_handler = tpll_get_event_handler();

    irq_cnt_tt++;

    if (rf_get_irq_status(FLD_RF_IRQ_RX))
    {
        reg_rf_irq_status = FLD_RF_IRQ_RX;
        if (m_event_handler) tpll_rx_irq_handler(m_event_handler);
        rx_flag = 1;
    }

    if (rf_get_irq_status(FLD_RF_IRQ_TX))
    {
        reg_rf_irq_status = FLD_RF_IRQ_TX;
        if (m_event_handler) m_event_handler(TPLL_EVENT_TX_SUCCESS);
    }

    if (rf_get_irq_status(FLD_RF_IRQ_RX_DR))
    {
        reg_rf_irq_status = FLD_RF_IRQ_RX_DR;
        irq_cnt_dr++;
    }

    if (rf_get_irq_status(FLD_RF_IRQ_TX_DS))
    {
        reg_rf_irq_status = FLD_RF_IRQ_TX_DS;
        irq_cnt_ds++;
    }

    if (rf_get_irq_status(FLD_RF_IRQ_INVALID_PID))
    {
        reg_rf_irq_status = FLD_RF_IRQ_INVALID_PID;
    }

    if (rf_get_irq_status(FLD_RF_IRQ_TX_RETRYCNT))
    {
        reg_rf_irq_status = FLD_RF_IRQ_TX_RETRYCNT;
        irq_cnt_retry++;
        maxretry_flag = 1;


        tpll_update_tx_rptr(PTX_CHANNEL);
        if (m_event_handler) m_event_handler(TPLL_EVENT_TX_FAILED);
    }

    plic_interrupt_complete(IRQ15_ZB_RT);
    rf_clr_irq_status(0xffff);
}

void tpll_gpio_init(void)
{
    gpio_function_en(BLUE_LED | GREEN_LED | WHITE_LED | RED_LED);
    gpio_output_en(BLUE_LED | GREEN_LED | WHITE_LED | RED_LED);
    gpio_input_dis(BLUE_LED | GREEN_LED | WHITE_LED | RED_LED);

}

uint32_t tpll_init(void)
{
    uint32_t err_code = 0;

    uint8_t tx_address[5] = {0xE7, 0xE7, 0xE7, 0xE7,0xE7};

    tpll_config_t tpll_config = TPLL_DEFAULT_CONFIG;

    tpll_config.mode = TPLL_MODE_PTX;
    tpll_config.event_handler = tpll_event_handler;
    tpll_config.bitrate = TPLL_BITRATE_2MBPS;
    tpll_config.tx_output_power_idx = RF_POWER_INDEX_P0p01dBm;
    tpll_config.retransmit_delay = 150;
    tpll_config.retransmit_count = 3;
    tpll_config.tx_settle = 118;
    tpll_config.rx_settle = 85;
    tpll_config.rx_timeout = 500;
    tpll_config.payload_length = 32;
    tpll_config.pipe = PTX_CHANNEL;
    tpll_config.irq_mask =
        FLD_RF_IRQ_RX | FLD_RF_IRQ_TX | FLD_RF_IRQ_TX_DS | FLD_RF_IRQ_RX_DR | FLD_RF_IRQ_TX_RETRYCNT;

    err_code = tpll_config_init(&tpll_config);
    VERIFY_SUCCESS(err_code);

    tpll_set_address_length(TPLL_ADDRESS_WIDTH_5BYTES);

    tpll_set_address(PTX_CHANNEL,tx_address);

    tpll_set_tx_pipe(PTX_CHANNEL);

    return SUCCESS;
}

static void tpll_ptx_process(void)
{
    if (rx_flag || maxretry_flag)
    {
        maxretry_flag = rx_flag = 0;

        tx_payload.data[3]++;
        delay_ms(10);
        if (tpll_write_payload(&tx_payload) == SUCCESS)
            tpll_start_tx();

        tx_cnt++;
        if (tx_cnt % 50 == 0)
            gpio_toggle(BLUE_LED);
    }
}

void tpll_run_test(void)
{

    while (1)
    {
        tpll_ptx_process();
    }
}
