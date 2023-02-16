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

static tpll_payload_t ack_payload = TPLL_CREATE_PAYLOAD(
    TPLL_PIPE0, 0xaa, 0xbb, 0xcc, 0xdd, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x00, 0x11, 0x22, 0x33, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f);

static volatile uint32_t irq_cnt_tx = 0;
static volatile uint32_t irq_cnt_rx = 0;
static volatile uint32_t irq_cnt_ds = 0;
static volatile uint32_t irq_cnt_dr = 0;
static volatile uint8_t  rx_flag = 0;
static volatile uint32_t irq_cnt_invalid = 0;
static volatile uint32_t rx_cnt = 0;

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
        ack_payload.data[4]++;
        ack_payload.pipe = rx_payload.pipe;
        tpll_flush_tx(ack_payload.pipe);
        tpll_write_payload(&ack_payload);

        rx_flag = 1;
        evt_rx_cnt++;
        break;
    }
}

_attribute_ram_code_sec_ void rf_irq_handler(void)
{
    tpll_event_handler_t m_event_handler = tpll_get_event_handler();

    if (rf_get_irq_status(FLD_RF_IRQ_RX))
    {
        reg_rf_irq_status = FLD_RF_IRQ_RX;
        irq_cnt_rx++;
        if (m_event_handler) tpll_rx_irq_handler(m_event_handler);
    }

    if (rf_get_irq_status(FLD_RF_IRQ_RX_DR))
    {
        irq_cnt_dr++;
        reg_rf_irq_status = FLD_RF_IRQ_RX_DR;
    }

    if (rf_get_irq_status(FLD_RF_IRQ_TX_DS))
    {
        irq_cnt_ds++;
        reg_rf_irq_status = FLD_RF_IRQ_TX_DS;
    }

    if (rf_get_irq_status(FLD_RF_IRQ_TX))
    {
        irq_cnt_tx++;
        reg_rf_irq_status = FLD_RF_IRQ_TX;
        if (m_event_handler) m_event_handler(TPLL_EVENT_TX_SUCCESS);
    }

    if (rf_get_irq_status(FLD_RF_IRQ_INVALID_PID))
    {
        irq_cnt_invalid++;
        reg_rf_irq_status = FLD_RF_IRQ_INVALID_PID;
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

    uint8_t rx_address[5] = {0xE7, 0xE7, 0xE7, 0xE7,0xE7};

    tpll_config_t tpll_config = TPLL_DEFAULT_CONFIG;

    tpll_config.mode = TPLL_MODE_PRX;
    tpll_config.event_handler = tpll_event_handler;
    tpll_config.bitrate = TPLL_BITRATE_2MBPS;
    tpll_config.tx_output_power_idx = RF_POWER_INDEX_P0p01dBm;
    tpll_config.retransmit_delay = 150;
    tpll_config.tx_settle = 118;
    tpll_config.rx_settle = 85;
    tpll_config.rx_timeout = 500;
    tpll_config.payload_length = 32;
    tpll_config.pipe = TPLL_PIPE0;
    tpll_config.irq_mask =
        FLD_RF_IRQ_RX | FLD_RF_IRQ_TX | FLD_RF_IRQ_TX_DS | FLD_RF_IRQ_RX_DR | FLD_RF_IRQ_TX_RETRYCNT;

    err_code = tpll_config_init(&tpll_config);
    VERIFY_SUCCESS(err_code);

    tpll_set_address_length(TPLL_ADDRESS_WIDTH_5BYTES);

    tpll_set_address(TPLL_PIPE0,rx_address);

    return SUCCESS;
}

void tpll_run_test(void)
{

    tpll_start_rx();

    while (1)
    {
        if (rx_flag)
        {
            rx_flag = 0;

            rx_cnt++;
            if (rx_cnt % 50 == 0)
            {
                gpio_toggle(GREEN_LED);
            }
        }
    }
}
