/********************************************************************************************************
 * @file	tpll.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "drivers.h"

#include "tpll.h"
#include "string.h"

#define TX_FIFO_DEP 2   // fifo num = 2^FIFO_DEP
#define TX_FIFO_SIZE 64 // must 64B align

#define TPLL_PIPE_NUM 6
#define TPLL_PIPE_TX_FIFO_SIZE 64
#define TPLL_PIPE_TX_FIFO_NUM 4
#define TPLL_PIPE_RX_FIFO_SIZE 128
#define TPLL_PIPE_RX_FIFO_NUM 4




#define RADIO_CRCCNF_LEN_DISABLED 0 /** CRC length is zero and CRC calculation is disabled */
#define RADIO_CRCCNF_LEN_ONE 1      /** CRC length is one byte and CRC calculation is enabled */
#define RADIO_CRCCNF_LEN_TWO 2      /** CRC length is two bytes and CRC calculation is enabled */
#define RADIO_CRCCNF_LEN_THREE 3    /** CRC length is three bytes and CRC calculation is enabled */

/** @brief Macro to calculate the DMA length of a packet */
#define RF_TX_PACKET_DMA_LEN(rf_data_len) (((rf_data_len) + 3) / 4) | (((rf_data_len) % 4) << 22)

#define PIPE_TX_FIFO_SIZE 64 /** The size of the transmission first-in, first-out buffer */
#define PIPE_TX_FIFO_NUM 4   /** The number of the transmission first-in, first-out buffer */

static tpll_config_t m_config_local;
bool m_tpll_initialized = false;
static tpll_event_handler_t m_event_handler;

static uint8_t rx_fifo[TPLL_PIPE_RX_FIFO_SIZE * TPLL_PIPE_RX_FIFO_NUM] __attribute__((aligned(4))) = {};

void init_dma_default_buff();
void tpll_dma_init();

void tpll_trx_set_enable(rf_mode_e rf_mode);
void tpll_rx_set(void);
void tpll_tx_set(void);
void tpll_trx_settle_time(uint16_t tx_stl_period_us, uint16_t rx_stl_period_us);
void tpll_SetTxMI(rf_mi_value_e mi_value);
void tpll_SetRxMI(rf_mi_value_e mi_value);
void tpll_EnableAckPayload(uint8_t enable);
void tpll_EnableNoAck(uint8_t enable);
void tpll_set_auto_retry(uint8_t retry_times, uint16_t retry_delay);
void tpll_rx_timeout_set(uint16_t period_us);
void tpll_enable_crcfilter(uint8_t enable);

/** An enum describing the TPLL mode state-machine status.
 *
 */
typedef enum
{
    TPLL_STATE_IDLE = 0,  /**< idle */
    TPLL_STATE_ACTIVE,    /**< active */
    TPLL_STATE_TX_SETTLE, /**< tx settle*/
    TPLL_STATE_TX,        /**< tx */
    TPLL_STATE_RX_WAIT,   /**< rx wait */
    TPLL_STATE_RX,        /**< rx */
    TPLL_STATE_TX_WAIT,   /**< tx wait */
} tpll_mainstate_t;

typedef enum
{
    TPLL_ADDR_UPDATE_MASK_BASE0 = BIT(0),
    TPLL_ADDR_UPDATE_MASK_BASE1 = BIT(1),
    TPLL_ADDR_UPDATE_MASK_PREFIX = BIT(2),
} tpll_addr_update_mask_t;

/** @brief  Primary LinkLayer transmission modes. */
typedef enum
{
    TPLL_TXMODE_AUTO,
    TPLL_TXMODE_MANUAL
} tpll_tx_mode_t;

/** @brief  Primary LinkLayer protocols. */
typedef enum
{
    TPLL_PROTOCOL_TPLL,    /**< Primary LinkLayer with fixed payload length.     */
    TPLL_PROTOCOL_TPLL_DPL /**< Primary LinkLayer with dynamic payload length.   */
} tpll_protocol_t;

/** @brief  Primary LinkLayer CRC modes. */
typedef enum
{
    TPLL_CRC_16BIT = RADIO_CRCCNF_LEN_TWO,   /**< Use two-byte CRC. */
    TPLL_CRC_8BIT = RADIO_CRCCNF_LEN_ONE,    /**< Use one-byte CRC. */
    TPLL_CRC_OFF = RADIO_CRCCNF_LEN_DISABLED /**< Disable CRC.      */
} tpll_crc_t;

typedef struct
{
    uint8_t base_address_0[ADDR_MAX_LEN - 1];
    uint8_t base_address_1[ADDR_MAX_LEN - 1];
    uint8_t pipe_prefixes[TPLL_PIPE_COUNT];
    uint8_t addr_length;
    uint8_t pipe_nums;
} tpll_address_t;
tpll_address_t m_tpll_addr = TPLL_ADDR_DEFAULT;
uint8_t m_addr_0[5] = {0};
uint8_t m_addr_1[5] = {0};

#if 0
/**
 *	@brief	  	This function servers to set multi byte write.
 *   @param[in]      reg_start - register start.
 *   @param[in]      buf - store bytes, which need to be write.
 *   @param[in]      len - the length of byte.
 *	@return	 	rf_ed:0x00~0xff
 */
static void multi_byte_reg_write(uint32_t reg_start, uint8_t *buf, int len)
{
    int i = 0;
    for (i = 0; i < len; i++, reg_start++)
    {
        write_reg8(reg_start, buf[i]);
    }
}
#endif

/** @brief Macro for verifying statement to be true. It will cause the exterior function to return
 *        err_code if the statement is not true.
 *
 * @param[in]   statement   Statement to test.
 * @param[in]   err_code    Error value to return if test was invalid.
 *
 * @retval      nothing, but will cause the exterior function to return @p err_code if @p statement
 *              is false.
 */
#define VERIFY_TRUE(statement, err_code) \
    do                                   \
    {                                    \
        if (!(statement))                \
        {                                \
            return err_code;             \
        }                                \
    } while (0)

/** @brief Macro for verifying statement to be false. It will cause the exterior function to return
 *        err_code if the statement is not false.
 *
 * @param[in]   statement   Statement to test.
 * @param[in]   err_code    Error value to return if test was invalid.
 *
 * @retval      nothing, but will cause the exterior function to return @p err_code if @p statement
 *              is true.
 */
#define VERIFY_FALSE(statement, err_code) \
    do                                    \
    {                                     \
        if ((statement))                  \
        {                                 \
            return err_code;              \
        }                                 \
    } while (0)

/** @brief Macro for verifying that the module is initialized. It will cause the exterior function to
 *        return if not.
 *
 * @param[in] param  The variable to check if is NULL.
 */
#define VERIFY_PARAM_NOT_NULL(param) VERIFY_FALSE(((param) == NULL), ERROR_NULL)

#define VERIFY_PAYLOAD_LENGTH(p)                                       \
    do                                                                 \
    {                                                                  \
        if (p->length == 0 || p->length > TPLL_MAX_PAYLOAD_LENGTH) \
        {                                                              \
            return ERROR_INVALID_LENGTH;                          \
        }                                                              \
    } while (0)

/*
Judge whether the sync word is good or not
*/
uint8_t sync_word_is_valid(uint8_t *sync_word, uint8_t len, uint8_t th)
{
    uint8_t last_bit = 0xff;
    uint8_t cnt = 0;
    uint8_t tmp = 0;

    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tmp = (sync_word[i] >> j) & 0x01;
            if (last_bit != tmp)
            {
                last_bit = tmp;
                cnt = 1;
            }
            else
            {
                cnt++;
            }

            if (cnt >= th)
            {
                return 0;
            }
        }
    }

    return 1;
}

/**
 * @brief   Check for TX FIFO empty.
 *
 * @details Use this function to check if TX FIFO is empty.
 *
 * @param[in]   pipe_id             specify the pipe
 *
 * @return  TX FIFO empty bit
 * @retval  FALSE TX FIFO NOT empty
 * @retval  RUE TX FIFO empty
 *
 */
uint8_t tpll_tx_fifo_empty(uint8_t pipe_id)
{
    return (read_reg8((REG_DMA_TX_FIFO_WPTR_PIPE0 + (pipe_id << 1)))) ==
           (read_reg8((REG_DMA_TX_FIFO_RPTR_PIPE0 + (pipe_id << 1))));
}

/** @brief Function for setting the radio bitrate.
 *
 * @param[in]   bitrate                         Radio bitrate.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_set_bitrate(tpll_bitrate_t bitrate)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    if (bitrate == TPLL_BITRATE_1MBPS)
    {
        rf_set_pri_1M_mode();
        tpll_trx_set_enable(RF_MODE_PRIVATE_1M); // 1m
    }
    else if (bitrate == TPLL_BITRATE_2MBPS)
    {

        rf_set_pri_2M_mode();
        tpll_trx_set_enable(RF_MODE_PRIVATE_2M); // 2m
    }

    return SUCCESS;
}

/** @brief Function for enable modem continuous mode.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_enable_cont_mode(void)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    BM_SET(read_reg8(MODEM_MODE_CFG_RX1_0), BIT(3));

    return SUCCESS;
}

uint32_t tpll_config_init(tpll_config_t const *p_config)
{
    if (p_config == NULL)
        return ERROR_NULL;

    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    memcpy(&m_config_local, p_config, sizeof(tpll_config_t));

    m_event_handler = p_config->event_handler;

    rf_mode_init(); // driver api

    tpll_set_bitrate(p_config->bitrate);
    tpll_set_tx_power(p_config->tx_output_power_idx);

    tpll_dma_init();

    tpll_enable_cont_mode(); // request by qiangkai 2022/8/12

    if (p_config->mode == TPLL_MODE_PTX)
    {
        tpll_tx_set();
        tpll_set_auto_retry(p_config->retransmit_count, p_config->retransmit_delay);
        tpll_rx_timeout_set(p_config->rx_timeout);
    }
    else if (p_config->mode == TPLL_MODE_PRX)
    {
        tpll_rx_set();
        tpll_enable_crcfilter(1);
    }

    tpll_trx_settle_time(p_config->tx_settle, p_config->rx_settle);

    write_reg8(TIM_ALIGN_1, 0x38);//RX_DIS_PDET_BLANK
    write_reg8(LL_TXWAIT_L, 0x05);//tx wait

    core_enable_interrupt();
    plic_interrupt_enable(IRQ15_ZB_RT);
    rf_set_irq_mask(p_config->irq_mask);

    m_tpll_initialized = true;

    return SUCCESS;
}

/**
 * @brief   This function serves to get tpll event handler function
 * @return  event handler function pointer
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) tpll_event_handler_t
tpll_get_event_handler(void)
{
    if (m_event_handler == NULL || m_tpll_initialized == false)
        return NULL;
    return m_event_handler;
}

/**
 * @brief       Get the width of the address.
 * @param       none.
 * @return      Width of the TPLL address (in bytes).
 */
uint8_t tpll_get_address_length(void)
{
    return read_reg8(REG_BASEBAND_ACCESS_CODE_LEN) & 0x07;
}

// static uint8_t m_ack_packet[32] __attribute__ ((aligned (4))) = {0};
static void tpll_init_fifos(void)
{
    // rx fifo init
    write_reg8(REG_BB_LL_BASE_ADDR, 0x80);  // stop first
    write_reg8(REG_DMA_RX_FIFO_RPTR, 0x20); // reset rptr

    // reset the wptr and rptr of all pipes' tx-fifos
    int i = 0;
    for (i = 0; i < TPLL_PIPE_NUM; i++)
    {
        write_reg8((REG_DMA_TX_FIFO_WPTR_PIPE0 + i), 0);
        write_reg8((REG_DMA_TX_FIFO_RPTR_PIPE0 + i), 0x20); // bit5, rptr clear
    }
}

/**
 * @brief     This function serves to rx dma setting.
 * @param[in] buff 		 	  - The buffer that store received packet.
 * @param[in] fifo_num  	  - DMA fifo num.
 * @param[in] fifo_byte_size  - The length of one dma fifo.
 * @return	  none.
 */
void tpll_dma_init()
{
    tpll_init_fifos();
    init_dma_default_buff();
    rf_set_tx_dma(TX_FIFO_DEP, TX_FIFO_SIZE);
    rf_set_rx_dma(rx_fifo, TPLL_PIPE_RX_FIFO_NUM - 1, TPLL_PIPE_RX_FIFO_SIZE);
}

/** @brief Function for setting the channel to use for the radio.
 *
 * The module must be in an idle state to call this function. As a PTX, the
 * application must wait for an idle state and as a PRX, the application must stop RX
 * before changing the channel. After changing the channel, operation can be resumed.
 *
 * @param[in]   channel                         Channel to use for radio.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_INVALID_STATE             If the module is not initialized.
 * @retval  ERROR_BUSY                      If the module was not in idle state.
 * @retval  ERROR_INVALID_PARAM             If the channel is invalid (larger than 100).
 */
uint32_t tpll_set_rf_channel(uint32_t channel)
{
    uint8_t cmd = 0;
    write_reg8(REG_BB_LL_BASE_ADDR, 0x80); // stop first

    cmd = read_reg8(REG_BB_LL_BASE_ADDR); // if not have this step, 0x140a24 sometime will not change to IDLE
    if (cmd != 0x80)                      // just for compile error, meanwhile double confirm the BB_LL cmd reg
        write_reg8(REG_BB_LL_BASE_ADDR, 0x80);

    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    VERIFY_TRUE(channel <= 100, ERROR_INVALID_PARAM);
    rf_set_chn(channel);

    return SUCCESS;
}

/**
 * @brief   This function serves to set RF channel 0 access command.
 * @param[in]  acc - the command.
 * @param[in]  pipe_id -  channel id
 * @return  none.
 */
void tpll_chn_access_code_comm(uint32_t pipe_id, uint32_t acc)
{

    uint8_t i = 0;

    uint8_t acc_len = read_reg8(ACCLEN) & 0x07;
    switch (pipe_id)
    {
    case 0:
    case 1:
        for (i = 0; i < acc_len; i++)
        {
            write_reg8(ACCESS_CODE_BASE_PIPE0 + i + (pipe_id * 8), (acc >> (i * 8)) & 0xff);
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        for (i = 0; i < acc_len; i++)
        {
            write_reg8(ACCESS_CODE_BASE_PIPE0 + i + 8, (acc >> (i * 8)) & 0xff);
        }
        write_reg8((ACCESS_CODE_BASE_PIPE2 + (pipe_id - 2)), (uint8_t)(acc & 0xff));
        break;
    default:
        break;
    }
    //	reg_rf_acclen |= 0x80;//setting access code needs to writ 0x405 access code trigger bit 1 to enable in long range
    // mode,,and the mode of  BLE1M and BLE2M need not.
}

/**
 * @brief   This function serves to set RF channel 0 access command.
 * @param[in]  pipe_id -  channel id
 * @param[in]  addr - the pointer of address.
 * @return  none.
 */
void tpll_set_address(tpll_pipeid_t pipe_id, uint8_t *addr)
{
    rf_set_pipe_access_code(pipe_id, addr);
}

/**
 * @brief   	This function serves to set access code.
 * @param[in]   pipe_id   - The pipe id that you want to set access code.
 * @param[in]   acc   - That you want to set the access code.
 * @return  	none.
 */
void tpll_set_access_code(uint32_t pipe_id, uint32_t acc)
{
    tpll_chn_access_code_comm(pipe_id, acc);
}

/** @brief Function for setting the length of the address.
 *
 * @param[in]       address_width           Length of the TPLL address (in bytes).
 *
 * @retval  SUCCESS                      If the address length was set successfully.
 * @retval  ERROR_INVALID_PARAM          If the address length was invalid.
 * @retval  ERROR_BUSY                   If the function failed because the radio is busy.
 */
uint32_t tpll_set_address_length(tpll_address_width_t address_width)
{
    if (address_width < ADDR_MIN_LEN || address_width > ADDR_MAX_LEN)
        return ERROR_INVALID_PARAM;

    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    uint8_t tmp = read_reg8(REG_BASEBAND_ACCESS_CODE_LEN) & 0xf8; // 0x800405[2:0], acc_len
    tmp |= address_width;
    write_reg8(REG_BASEBAND_ACCESS_CODE_LEN, tmp);

    tmp = read_reg8(MODEM_RX_CTRL_0) & 0xf8;
    tmp |= address_width;
    write_reg8(MODEM_RX_CTRL_0, tmp);

    write_reg8(REG_BASEBAND_ACCESS_CODE_SYNC_THRESHOLD, address_width * 8); // set the length of address code sync
                                                                            // window
    return SUCCESS;
}

/**
 * @brief       Close one or all pipes.
 * @param       pipe_id Radio pipes to close.
 * @return      none.
 */
void tpll_close_pipe(tpll_pipeid_t pipe_id)
{
    uint8_t tmp = read_reg8(REG_BASEBAND_RX_PIPE_EN);
    switch (pipe_id)
    {
    case TPLL_PIPE0:
    case TPLL_PIPE1:
    case TPLL_PIPE2:
    case TPLL_PIPE3:
    case TPLL_PIPE4:
    case TPLL_PIPE5:
        tmp &= (~BIT(pipe_id));
        break;

    case TPLL_PIPE_ALL:
        tmp &= 0xc0;
        break;

    case TPLL_TX:
    default:
        break;
    }
    write_reg8(REG_BASEBAND_RX_PIPE_EN, tmp);
}

/** @brief Function for enabling pipes.
 *
 * @param   pipe_id                      the pipe id that need to enable(tpll_pipeid_t锛�
 *
 * @retval  SUCCESS                     If the pipes were enabled and disabled successfully.
 * @retval  ERROR_BUSY                  If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM         If the function failed because the address given was too close to a zero
 * address.
 */
uint32_t tpll_enable_pipes(tpll_pipeid_t pipe_id)
{
    uint8_t tmp = read_reg8(REG_BASEBAND_RX_PIPE_EN);

    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    if (pipe_id >= TPLL_PIPE_NUM)
        return ERROR_INVALID_PARAM;

    switch (pipe_id)
    {
    case TPLL_PIPE0:
    case TPLL_PIPE1:
    case TPLL_PIPE2:
    case TPLL_PIPE3:
    case TPLL_PIPE4:
    case TPLL_PIPE5:
        tmp |= BIT(pipe_id);
        break;

    case TPLL_PIPE_ALL:
        tmp |= 0x3f;
        break;

    case TPLL_TX:
    default:
        break;
    }
    write_reg8(REG_BASEBAND_RX_PIPE_EN, tmp);

    return SUCCESS;

    // write_reg8(REG_BASEBAND_RX_PIPE_EN, (read_reg8(REG_BASEBAND_RX_PIPE_EN) & 0xc0) | (1 << pipe_id)); // RXCHN_EN
    // [5:0]
}

static void update_tpll_address_params(uint8_t update_mask)
{
    if (false == m_tpll_initialized)
        tpll_close_pipe(TPLL_PIPE_ALL);

    if ((update_mask & TPLL_ADDR_UPDATE_MASK_BASE0) != 0)
    {
        memcpy(m_addr_0, m_tpll_addr.base_address_0, (m_tpll_addr.addr_length - 1));
        m_addr_0[m_tpll_addr.addr_length - 1] = m_tpll_addr.pipe_prefixes[0];
        tpll_set_address(TPLL_PIPE0, m_addr_0);
        tpll_enable_pipes(TPLL_PIPE0);
    }

    if ((update_mask & TPLL_ADDR_UPDATE_MASK_BASE1) != 0)
    {
        memcpy(m_addr_1, m_tpll_addr.base_address_1, (m_tpll_addr.addr_length - 1));
        m_addr_1[m_tpll_addr.addr_length - 1] = m_tpll_addr.pipe_prefixes[1];
        tpll_set_address(TPLL_PIPE1, m_addr_1);
        tpll_enable_pipes(TPLL_PIPE1);
    }

    if ((update_mask & TPLL_ADDR_UPDATE_MASK_PREFIX) != 0)
    {
        uint8_t i;
        for (i = 0; i < m_tpll_addr.pipe_nums; i++)
        {
            switch (i)
            {
            case TPLL_PIPE0:
                m_addr_0[m_tpll_addr.addr_length - 1] = m_tpll_addr.pipe_prefixes[0];
                tpll_set_address(TPLL_PIPE0, m_addr_0);
                break;
            case TPLL_PIPE1:
                m_addr_1[m_tpll_addr.addr_length - 1] = m_tpll_addr.pipe_prefixes[1];
                tpll_set_address(TPLL_PIPE1, m_addr_1);
                break;
            case TPLL_PIPE2:
            case TPLL_PIPE3:
            case TPLL_PIPE4:
            case TPLL_PIPE5:
                write_reg8((reg_rf_access_code_base_pipe2 + (i - 2)), m_tpll_addr.pipe_prefixes[i]);
                break;
            default:
                break;
            }
            tpll_enable_pipes(i);
        }
    }
}

/**
 *  @brief  Function to enable/disable each access_code channel for RF Rx terminal.
 *
 *  @param[in]  pipe                Bit0~bit5 correspond to channel 0~5, respectively.
 *
 *                      If is set as 0x3f (i.e. 00111111),
 *                      all access_code channels (0~5) are enabled.
 */
static inline void rf_rx_acc_code_enable(rf_channel_e pipe)
{
    write_reg8(MODEM_RX_CTRL_1, (read_reg8(MODEM_RX_CTRL_1) & 0xc0) | (1 << pipe)); // RXCHN_EN [5:0]
}

/** @brief Function for setting the base address for pipe 0.
 *
 * @param[in]       p_addr      Pointer to the address data.
 *
 * @retval  SUCCESS                     If the base address was set successfully.
 * @retval  ERROR_BUSY                  If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM         If the function failed because the address given was too close to a zero
 * address.
 * @retval  ERROR_NULL                  If the required parameter was NULL.
 */
uint32_t tpll_set_base_address_0(uint8_t const *p_addr)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    VERIFY_PARAM_NOT_NULL(p_addr);

    m_tpll_addr.addr_length = tpll_get_address_length();
    memcpy(m_tpll_addr.base_address_0, &p_addr[0], m_tpll_addr.addr_length - 1);
    update_tpll_address_params(TPLL_ADDR_UPDATE_MASK_BASE0);
    return SUCCESS;
}

/** @brief Function for setting the base address for pipe 1 to pipe 5.
 *
 * @param[in]       p_addr      Pointer to the address data.
 *
 * @retval  SUCCESS                     If the base address was set successfully.
 * @retval  ERROR_BUSY                  If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM         If the function failed because the address given was too close to a zero
 * address.
 * @retval  ERROR_NULL                  If the required parameter was NULL.
 */
uint32_t tpll_set_base_address_1(uint8_t const *p_addr)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    VERIFY_PARAM_NOT_NULL(p_addr);

    m_tpll_addr.addr_length = tpll_get_address_length();
    memcpy(m_tpll_addr.base_address_1, &p_addr[0], m_tpll_addr.addr_length - 1);
    update_tpll_address_params(TPLL_ADDR_UPDATE_MASK_BASE1);
    return SUCCESS;
}

/**
 * @brief   Function for configures the number of available pipes's prefiex and enables the pipes.
 *
 * @param   *p_prefixes            for appointed pipe.
 * @param   num_pipes                number of pipes
 *
 * @return  error code for result.
 * @retval  SUCCESS                0:  success.
 * @retval  ERROR_NULL             1:  bad prefix
 * @retval  ERROR_INVALID_PARAM    2:  bad pipe nums
 */
uint32_t tpll_set_prefixes(uint8_t *p_prefixes, uint8_t num_pipes)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    VERIFY_PARAM_NOT_NULL(p_prefixes);

    if (num_pipes > TPLL_PIPE_COUNT)
        return ERROR_INVALID_PARAM;

    memcpy(&m_tpll_addr.pipe_prefixes, p_prefixes, num_pipes);
    m_tpll_addr.pipe_nums = num_pipes;
    update_tpll_address_params(TPLL_ADDR_UPDATE_MASK_PREFIX);
    return SUCCESS;
}

/**
 * @brief   Function for set one pipe as a TX pipe.
 *
 * @param   pipe_id                 Pipe to be set as a TX pipe.
 *
 * @retval  SUCCESS                If set tx pipe success.
 * @retval  ERROR_BUSY             If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM    bad pipe nums
 */
uint32_t tpll_set_tx_pipe(tpll_pipeid_t pipe_id)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    VERIFY_TRUE(pipe_id <= TPLL_PIPE5, ERROR_INVALID_PARAM);

    // enable PTX in the specified pipe
    uint8_t tmp = read_reg8(LL_CTRL2);
    tmp &= 0xf8;

    tmp |= pipe_id;

    tmp |= 0x10;
    write_reg8(LL_CTRL2, tmp);

    return SUCCESS;
}

/**
 * @brief   This function serves to set RF TPLL  TRX mode  .
 * @param   rf_mode - rf mode.
 * @return  none.
 */
void tpll_trx_set_enable(rf_mode_e rf_mode)
{
    if (rf_mode == RF_MODE_PRIVATE_1M)
        write_reg8(FORMAT, 0x9a); // 1m 9a //enable  ack flag
    else if (rf_mode == RF_MODE_PRIVATE_2M)
        write_reg8(FORMAT, 0x8a); // 2m,8a

    write_reg8(PREAMBLE_TRAIL, 0x42); // set preamble_len [4:0] preamble_len , [7:5] trailer_len
    write_reg8(LL_CTRL2, 0xd0); // txchn_man_en, rxirq_report_all, rep_sn_pid_en

    // auto mode
    write_reg8(LL_CMD, 0x80);                       // stop cmd
    write_reg8(LL_CTRL3, 0x29);                       // reg0xf16 pll_en_man and tx_en_dly_en  enable
    write_reg8(RXMODE, read_reg8(RXMODE) & 0xfe); // rx disable
    write_reg8(LL_CTRL0, read_reg8(LL_CTRL0) & 0xce); // reg0xf02 disable rx_en_man and tx_en_man
}

/**
 * @brief       Set one pipe as a TX pipe.
 * @param       pipe_id Pipe to be set as a TX pipe.
 * @return      none.
 */
void TPLL_SetTXPipe(tpll_pipeid_t pipe_id)
{
    if (pipe_id <= TPLL_PIPE5)
    {
        // enable PTX in the specified pipe
        uint8_t tmp = read_reg8(LL_CTRL2);
        tmp &= 0xf8;

        tmp |= pipe_id;

        tmp |= 0x10;
        write_reg8(LL_CTRL2, tmp);
    }
}

/**
 * @brief       Get the current TX pipe.
 * @param       none.
 * @return      The pipe set as a TX pipe.
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint8_t tpll_get_tx_pipe(void)
{
    return (read_reg8(LL_CTRL2) & 0x07);
}

/**
 * @brief   Update the read pointer of the TX FIFO.
 *
 * @param[in]   pipe_id             Pipe id.
 *
 * @retval  SUCCESS                If the operation completed successfully.
 * @retval  ERROR_BUSY             If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM    bad pipe nums
 */
uint32_t tpll_update_tx_rptr(tpll_pipeid_t pipe_id)
{
    // adjust rptr manually

    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    VERIFY_TRUE(pipe_id < TPLL_PIPE_COUNT, ERROR_INVALID_PARAM);

    write_reg8(REG_DMA_TX_FIFO_RPTR_PIPE0 + (pipe_id << 1), 0x40);

    return SUCCESS;
}

/**
 * @brief   This function serves to set TPLL RF mode rx .
 * @return  none.
 */
void tpll_rx_set(void)
{
    write_reg8(LL_CTRL1, 0x30);             // rx timeout off
    write_reg8(LL_RESET_PID, 0x3f);             // reset pid
    write_reg8(LL_CTRL2, 0xc0);             // chn tx_manual off
    write_reg8(REG_DMA_RX_FIFO_RPTR, 0x20); // reset rptr
}

/**
 * @brief   This function serves to set TPLL RF mode tx .
 * @return  none.
 */
void tpll_tx_set(void)
{
    write_reg8(LL_CTRL0, read_reg8(LL_CTRL0) & 0xfe); // md_en
    write_reg8(LL_CTRL1, read_reg8(LL_CTRL1) & 0xf7); // crc_en
    int i = 0;
    for (i = 0; i < 6; i++)
    {
        write_reg8((REG_DMA_TX_FIFO_WPTR_PIPE0 + (i << 1)), 0);
        write_reg8((REG_DMA_TX_FIFO_RPTR_PIPE0 + (i << 1)), 0x20); // bit4, rptr clear
    }
}

/**
 * @brief   Function to set retransmit and retransmit delay parameters.
 *
 * @param[in]   retry_times         Number of retransmit, 0 equals retransmit OFF.
 * @param[in]   retry_delay         Retransmit delay
 */
void tpll_set_auto_retry(uint8_t retry_times, uint16_t retry_delay)
{
    retry_times &= 0x0f; // accommodate with N_Europe chips
    write_reg8(LL_MAX_RETRY_CNT, retry_times);

    retry_delay &= 0x0fff;
    uint16_t tmp = read_reg16(LL_ARD_L);
    tmp &= 0xf000;
    tmp |= retry_delay;
    write_reg16(LL_ARD_L, tmp);
}

/** @brief Function for setting the packet retransmit delay.
 *
 * @param[in]   delay                           Delay between retransmissions.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_set_retransmit_delay(uint16_t delay)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    delay &= 0x0fff;
    uint16_t tmp = read_reg16(LL_ARD_L);
    tmp &= 0xf000;
    tmp |= delay;
    write_reg16(LL_ARD_L, tmp);
    return SUCCESS;
}

/** @brief Function for setting the number of retransmission attempts.
 *
 * @param[in]   count                           Number of retransmissions.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_set_retransmit_count(uint16_t count)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    count &= 0x0f; // accommodate with N_Europe chips
    write_reg8(LL_MAX_RETRY_CNT, count);
    return SUCCESS;
}

/**
 * @brief   This function serves to set RF tx settle time and rx settle time.
 *
 * @param[in] tx_stl_period_us  tx settle time
 * @param[in] rx_stl_period_us  rx settle time
 *
 * @return  none.
 */
void tpll_trx_settle_time(uint16_t tx_stl_period_us, uint16_t rx_stl_period_us)
{
    tx_stl_period_us &= 0x0fff;
    rx_stl_period_us &= 0x0fff;

    write_reg8(LL_TXSTL_L, (read_reg8(LL_TXSTL_L) & 0xf000) | tx_stl_period_us); // txxstl 112us
    write_reg8(LL_RXSTL_L, (read_reg8(LL_RXSTL_L) & 0xf000) | rx_stl_period_us); // rxstl 85us
}

/**
 * @brief   This function serves to set RF tx settle time.
 *
 * @param[in] tx_stl_period_us  tx settle time
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_set_tx_settle_time(uint16_t tx_stl_period_us)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    tx_stl_period_us &= 0x0fff;
    write_reg8(LL_TXSTL_L, (read_reg8(LL_TXSTL_L) & 0xf000) | tx_stl_period_us); // txxstl 112us

    return SUCCESS;
}

/**
 * @brief   This function serves to set RF rx settle time.
 *
 * @param[in] rx_stl_period_us  rx settle time
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_set_rx_settle_time(uint16_t rx_stl_period_us)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    rx_stl_period_us &= 0x0fff;
    write_reg8(LL_RXSTL_L, (read_reg8(LL_RXSTL_L) & 0xf000) | rx_stl_period_us); // rxstl 85us

    return SUCCESS;
}


/**
 * @brief	  	This function is used to  set the modulation index of the receiver.
 *              This function is common to all modes,the order of use requirement:configure mode first,then set the the modulation index,default is 0.5 in drive,
 *              both sides need to be consistent otherwise performance will suffer,if don't specifically request,don't need to call this function.
 * @param[in]	mi	- the value of modulation_index*100.
 * @return	 	none.
 */
static void tpll_set_rx_modulation_index(rf_mi_value_e mi_value)
{
	unsigned char modulation_index_high;
	unsigned char modulation_index_low;
	unsigned char kvm_trim;
	unsigned char tmep_mi = 0;
	tmep_mi = (unsigned int)((mi_value * 128)/100);
	modulation_index_low = tmep_mi%256;

	modulation_index_high = (tmep_mi%512)>>8;

	reg_rf_modem_rxc_mi_flex_ble_0 = modulation_index_low;
	reg_rf_modem_rxc_mi_flex_ble_1 |= modulation_index_high;


	if(reg_rf_mode_cfg_tx1_0 & 0x01)//2MBPS mode
	{
		if ((mi_value >= 75)&&(mi_value <= 100))
			kvm_trim = 3;
		else if (mi_value > 100)
			kvm_trim = 7;
		else
			kvm_trim = 1;
	}
	else
	{

		if ((mi_value >= 75)&&(mi_value <= 100))
			kvm_trim = 1;
		else if ((mi_value > 100)&&(mi_value <= 150))
			kvm_trim = 3;
		else if (mi_value > 150)
			kvm_trim = 7;
		else
			kvm_trim = 0;
	}
	reg_rf_mode_cfg_tx1_0=(reg_rf_mode_cfg_tx1_0&~(FLD_RF_VCO_TRIM_KVM))|(kvm_trim<<1);
}


/**
 * @brief	  	This function is used to set the modulation index of the sender.
 *              This function is common to all modes,the order of use requirement:configure mode first,then set the the modulation index,default is 0.5 in drive,
 *              both sides need to be consistent otherwise performance will suffer,if don't specifically request,don't need to call this function.
 * @param[in]	mi	- the value of modulation_index*100.
 * @return	 	none.
 */
static void tpll_set_tx_modulation_index(rf_mi_value_e mi_value)
{
	unsigned char modulation_index_high;
	unsigned char modulation_index_low;
	unsigned char kvm_trim;
	unsigned char tmep_mi = 0;
	//mi_value = (unsigned int)(mi_value * 1.28);
	tmep_mi = (unsigned int)((mi_value * 128)/100);
	modulation_index_low = tmep_mi%256;

	modulation_index_high = (tmep_mi%512)>>8;
	reg_rf_mode_cfg_rx2_0 = modulation_index_low;
	reg_rf_mode_cfg_rx2_1 |= modulation_index_high;

	if(reg_rf_mode_cfg_tx1_0 & 0x01)//2MBPS mode
	{
		if ((mi_value >= 75)&&(mi_value <= 100))
			kvm_trim = 3;
		else if (mi_value > 100)
			kvm_trim = 7;
		else
			kvm_trim = 1;
	}
	else
	{

		if ((mi_value >= 75)&&(mi_value <= 100))
			kvm_trim = 1;
		else if ((mi_value > 100)&&(mi_value <= 150))
			kvm_trim = 3;
		else if (mi_value > 150)
			kvm_trim = 7;
		else
			kvm_trim = 0;
	}
	reg_rf_mode_cfg_tx1_0=(reg_rf_mode_cfg_tx1_0&~(FLD_RF_VCO_TRIM_KVM))|(kvm_trim<<1);

}

/**
 * @brief       Set the frequency deviation of the transmitter, which follows the equation below.
 *              frequency deviation = bitrate/(modulation index)^2
 *
 * @param       mi_value    Modulation index.
 *
 * @return      none.
 */
void tpll_SetTxMI(rf_mi_value_e mi_value)
{
    tpll_set_tx_modulation_index(mi_value);
}


/**
 * @brief       Set the frequency deviation of the transmitter, which follows the equation below.
 *              frequency deviation = bitrate/(modulation index)^2
 * @param       mi_value    Modulation index.
 * @return      none.
 */
void tpll_SetRxMI(rf_mi_value_e mi_value)
{
    tpll_set_rx_modulation_index(mi_value);
    
}

/**
 * @brief   Enables the ACK payload feature
 *
 * @param   enable                  Whether to enable or disable ACK payload
 */
void tpll_EnableAckPayload(uint8_t enable)
{
    if (!enable)
    {
        reg_rf_ll_ctrl_2 |= BIT(5);
    }
    else
    {
        reg_rf_ll_ctrl_2 &= ~BIT(5);
    }
}

/**
 * @brief   Enables the W_TX_PAYLOAD_NOACK command
 *
 * @param   enable                  Whether to enable or disable NoAck option in 9-bit Packet control field
 */
void tpll_EnableNoAck(uint8_t enable)
{
    if (enable)
    {
        write_reg8(FORMAT, read_reg8(FORMAT) | 0x40); // set reg_0x140804's bit6 to enable tx_noack
    }
    else
    {
        write_reg8(FORMAT, read_reg8(FORMAT) & 0xbf); // clear reg_0x140804's bit6 to disable tx_noack
    }
}

uint8_t ptx_buffer[(TPLL_PIPE_COUNT * PIPE_TX_FIFO_SIZE * PIPE_TX_FIFO_NUM) + 64]
    __attribute__((aligned(4))) = {};

void init_dma_default_buff()
{
#define TX_PKT_PAYLOAD 0
    u8 rf_data_len = TX_PKT_PAYLOAD + 1;
    ptx_buffer[4] = TX_PKT_PAYLOAD;
    u32 rf_tx_dma_len = RF_TX_PACKET_DMA_LEN(rf_data_len);
    ptx_buffer[3] = (rf_tx_dma_len >> 24) & 0xff;
    ptx_buffer[2] = (rf_tx_dma_len >> 16) & 0xff;
    ptx_buffer[1] = (rf_tx_dma_len >> 8) & 0xff;
    ptx_buffer[0] = rf_tx_dma_len & 0xff;

    reg_dma_src_addr(DMA0) = convert_ram_addr_cpu2bus(ptx_buffer);
}

/** @brief this function to write a packet of TX payload into the radio.
 *
 *  @param[in]  pipe_id -  channel id
 *  @param *tx_pload  pointer to buffer in which TX payload are present
 *  @param length     number of bytes to write
 *
 *  @retval    the data length added to the tx buffer
 */
uint8_t *p_tx_fifo;
volatile uint8_t pipe_pid[6] = {0, 0, 0, 0, 0, 0};

/** @brief Function for writing a payload for transmission or acknowledgement.
 *
 * @details This function writes a payload that is added to the tx fifo.
 *
 * @param[in]   p_payload     Pointer to the structure that contains information and state of the payload.
 *
 * @retval  SUCCESS                     If the payload was successfully queued for writing.
 * @retval  ERROR_NULL                  If the required parameter was NULL.
 * @retval  ERROR_INVALID_STATE         If the module is not initialized.
 * @retval  ERROR_NO_MEM                If the TX FIFO is full.
 * @retval  ERROR_INVALID_LENGTH        If the payload length was invalid (zero or larger than the allowed
 * maximum).
 */
uint32_t tpll_write_payload(tpll_payload_t const *p_payload)
{
    uint8_t tmp, wptr, rptr;
    VERIFY_TRUE(m_tpll_initialized, ERROR_INVALID_STATE);
    VERIFY_PARAM_NOT_NULL(p_payload);
    VERIFY_PAYLOAD_LENGTH(p_payload);

    VERIFY_TRUE(p_payload->pipe < TPLL_PIPE_COUNT, ERROR_INVALID_PARAM);

    wptr = read_reg8((REG_DMA_TX_FIFO_WPTR_PIPE0 + (p_payload->pipe << 1)));
    rptr = read_reg8((REG_DMA_TX_FIFO_RPTR_PIPE0 + (p_payload->pipe << 1)));

    if (((wptr + 1) % PIPE_TX_FIFO_NUM) == (rptr % PIPE_TX_FIFO_NUM))
    { // if the tx-fifo is full, return  immediately
        return ERROR_NO_MEM;

    }

    tpll_set_tx_pipe(p_payload->pipe);

    p_tx_fifo = ptx_buffer + 64 + (p_payload->pipe * PIPE_TX_FIFO_NUM + (wptr % 4)) * PIPE_TX_FIFO_SIZE;

    u8 rf_data_len = p_payload->length + 1;
    p_tx_fifo[4] = p_payload->length;
    u32 rf_tx_dma_len = rf_tx_packet_dma_len(rf_data_len);
    p_tx_fifo[3] = (rf_tx_dma_len >> 24) & 0xff;
    p_tx_fifo[2] = (rf_tx_dma_len >> 16) & 0xff;
    p_tx_fifo[1] = (rf_tx_dma_len >> 8) & 0xff;
    p_tx_fifo[0] = rf_tx_dma_len & 0xff;

    memcpy(&p_tx_fifo[5], &p_payload->data[0], p_payload->length);

    wptr = (wptr + 1) % 32;

    write_reg8(REG_DMA_TX_FIFO_WPTR_PIPE0 + (p_payload->pipe << 1), wptr); // update wptr

    //		set and update pid
    tmp = read_reg8(LL_CTRL1);
    tmp &= 0x3f;
    tmp |= (pipe_pid[p_payload->pipe] << 6);
    write_reg8(LL_CTRL1, tmp);
    pipe_pid[p_payload->pipe] = (pipe_pid[p_payload->pipe] + 1) % 4;

    return SUCCESS;
}

/**
 * @brief      This function serves to optimize RF performance
 * @details This function must be called every time rx is turned on,
 *          and is called by an internal function.
 *          If there are other requirements that need to be called,
 *          turn off rx first, then call it again to make sure the Settings take effect
 */
inline void rf_set_rxpara(void)
{
    uint8_t reg_calibration = 0;
    reg_calibration = ((read_reg8(LDOT_RDBK2_1) & 0xf) << 2) | ((read_reg8(LDOT_RDBK2_0) & 0xc0) >> 6);
    if (reg_calibration > 7)
        reg_calibration -= 7;
    write_reg8(LDOT_DBG2_1, (read_reg8(LDOT_DBG2_1) & 0xc0) | reg_calibration);
}

/** @brief Function for starting to transmit data from the FIFO buffer.
 *
 * @retval  SUCCESS                     If the transmission was started successfully.
 * @retval  ERROR_BUSY                  If the function failed because the radio is busy.
 */
uint32_t tpll_start_rx(void)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);
    write_reg8(REG_BB_LL_BASE_ADDR, 0x80); // stop first

    if (TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS))
    {
        write_reg8(REG_BB_LL_BASE_ADDR, 0x84); // trig
        while (TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS))
            ;
    }

    return SUCCESS;
}

/** @brief Function for reading an RX payload.
 *
 * @param[in,out]   p_payload   Pointer to the structure that contains information and state of the payload.
 *
 * @retval  SUCCESS                     If the data was read successfully.
 * @retval  ERROR_NULL                  If the required parameter was NULL.
 * @retval  ERROR_INVALID_STATE         If the module is not initialized.
 */
static uint32_t m_timestamp = 0;
uint32_t tpll_read_rx_payload(tpll_payload_t *p_payload)
{
    uint8_t rptr = read_reg8(REG_DMA_RX_FIFO_RPTR);
    uint8_t wptr = read_reg8(REG_DMA_RX_FIFO_WPTR);
    uint8_t *p_rxfifo = NULL;

    if (NULL == p_payload)
    {
        return ERROR_NULL;
    }

    VERIFY_TRUE(m_tpll_initialized == true, ERROR_INVALID_STATE);

    p_rxfifo = rx_fifo + (rptr % TPLL_PIPE_RX_FIFO_NUM) * TPLL_PIPE_RX_FIFO_SIZE;

    p_payload->length = p_rxfifo[4] & 0x3f;
    if (p_payload->length > TPLL_MAX_PAYLOAD_LENGTH)
        return ERROR_INVALID_PARAM;

    memcpy(&p_payload->data[0], &p_rxfifo[5], p_payload->length);
    m_timestamp = p_rxfifo[5 + p_payload->length + 2] | (p_rxfifo[5 + p_payload->length + 3] << 8) |
                  (p_rxfifo[5 + p_payload->length + 4] << 16) | (p_rxfifo[5 + p_payload->length + 5] << 24);
    p_payload->rssi = p_rxfifo[5 + p_payload->length + 8] - 110;
    p_payload->pipe = (p_rxfifo[(p_rxfifo[4] & 0x3f) + 12] & 0x70) >> 4;

    /*
     * it is a new way to adjust rptr of rx fifo
     * it make read same pid different crc packet possible
     */
    if ((rptr % TPLL_PIPE_RX_FIFO_NUM) != (wptr % TPLL_PIPE_RX_FIFO_NUM))
        write_reg8(REG_DMA_RX_FIFO_RPTR, 0x40); // set bit6 to increase by one
    return SUCCESS;
}

/** @brief Enable interrupts globally in the system.
 *  @details    This macro must be used when the initialization phase is over and the interrupts
 *              can start being handled by the system.
 */
//void core_enable_interrupt(void)
//{
//    set_csr(NDS_MSTATUS, 1 << 3);
//    set_csr(NDS_MIE, 1 << 11 | 1 << 7 | 1 << 3);
//    plic_set_feature(FLD_FEATURE_VECTOR_MODE_EN); // enable vectored in PLIC
//}

/**
 * @brief   This function serves to set RF rx timeout.
 * @return  none.
 */
void tpll_rx_timeout_set(uint16_t period_us)
{
    uint16_t tmp;

    period_us &= 0x0fff;
    tmp = read_reg16(REG_LINKLAYER_PTX_RX_TIME) & 0xf000;
    tmp |= period_us;
    write_reg16(REG_LINKLAYER_PTX_RX_TIME, tmp);
}

/** @brief Function for starting transmission.
 *
 * @retval  SUCCESS                     If the TX started successfully.
 * @retval  ERROR_BUFFER_EMPTY          If the TX did not start because the FIFO buffer is empty.
 * @retval  ERROR_BUSY                  If the function failed because the radio is busy.
 */
uint32_t tpll_start_tx(void)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    uint8_t ret = false;
    for (int pipe_id = 0; pipe_id < TPLL_PIPE_COUNT; pipe_id++)
    {
        if (!tpll_tx_fifo_empty(pipe_id))
            ret = true;
    }
    if (!ret)
        return ERROR_BUFFER_EMPTY;

    write_reg8(REG_BB_LL_BASE_ADDR, 0x80); // stop first
    if (TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS))
    {
        write_reg8(REG_BB_LL_BASE_ADDR, 0x83); // trig
        while (TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS));
    }

    return SUCCESS;
}

/** @brief Function for disabling the Primary LinkLayer module.
 *
 * @details Calling this function disables the Primary LinkLayer module immediately.
 *          Doing so might stop ongoing communications.
 *
 *
 * @retval  SUCCESS             If Primary LinkLayer was disabled.
 */
uint32_t tpll_disable(void)
{
    write_reg8(REG_BB_LL_BASE_ADDR, 0x80);
    return SUCCESS;
}

/** @brief Function for removing remaining items from the TX buffer.
 *
 * @details This function clears the TX FIFO buffer.
 *
 * @param[in]   pipe_id                     pipe for which need flush tx buffer.
 *
 * @retval  SUCCESS                     If pending items in the TX buffer were successfully cleared.
 * @retval  ERROR_INVALID_STATE         If the module is not initialized.
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint32_t tpll_flush_tx(
    tpll_pipeid_t pipe_id)
{
    VERIFY_TRUE(m_tpll_initialized == true, ERROR_INVALID_STATE);

    write_reg8((REG_DMA_TX_FIFO_RPTR_PIPE0 + pipe_id), 0x80);

    return SUCCESS;
}

/** @brief Function for removing remaining items from the RX buffer.
 *
 * @retval  SUCCESS                     If the pending items in the RX buffer were successfully cleared.
 * @retval  ERROR_INVALID_STATE         If the module is not initialized.
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint32_t tpll_flush_rx(void)
{
    VERIFY_TRUE(m_tpll_initialized == true, ERROR_INVALID_STATE);

    write_reg8(REG_DMA_RX_FIFO_RPTR, 0x80);

    return SUCCESS;
}

/**
 * @brief   Reuse the last transmitted payload for the next packet.
 *
 * @param[in]   pipe_id             pipe id.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_reuse_pid(tpll_pipeid_t pipe_id)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    uint8_t wptr = read_reg8(REG_DMA_TX_FIFO_WPTR_PIPE0 + pipe_id) & 0x0f;
    uint8_t rptr = (wptr + 31) % 32; // move rptr one step backward from wptr
    rptr |= 0x20;                          // set bit5
    write_reg8((REG_DMA_TX_FIFO_RPTR_PIPE0 + pipe_id), rptr);

    return SUCCESS;
}

/** @brief Function for setting the radio output power.
 *
 * @param[in]   tx_output_power_idx                 Output power index.
 *
 * @retval  SUCCESS                         If the operation completed successfully.
 * @retval  ERROR_BUSY                      If the function failed because the radio is busy.
 */
uint32_t tpll_set_tx_power(rf_power_level_index_e tx_output_power_idx)
{
    VERIFY_TRUE(TPLL_STATE_IDLE == read_reg8(REG_LINKLAYER_STATE_MACHINE_STATUS), ERROR_BUSY);

    rf_set_power_level_index(tx_output_power_idx);

    return SUCCESS;
}

/* Disable / enable sending an ACK packet when a crc error occurs  */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) void tpll_enable_crcfilter(
    uint8_t enable)
{
    if (enable)
        BM_SET(read_reg8(RXTCRCPKT), BIT(0));
    else
        BM_CLR(read_reg8(RXTCRCPKT), BIT(0));
}

/* Get the packet received */
static __attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint8_t *tpll_get_rx_packet(
    void)
{
    uint8_t *rx_packet =
        rx_fifo + (read_reg8(REG_DMA_RX_FIFO_RPTR) % TPLL_PIPE_RX_FIFO_NUM) * TPLL_PIPE_RX_FIFO_SIZE;
    return rx_packet;
}

/* Get the pid of the received packet */
static __attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint8_t tpll_get_rx_packet_id(
    uint8_t *rx_packet)
{
    return (rx_packet[4] >> 6);
}

/* Get the crc of the received packet */
static __attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint16_t tpll_get_rx_packet_crc(
    uint8_t *rx_packet)
{
    return ((rx_packet[(rx_packet[4] & 0x3f) + 6] << 8) + rx_packet[(rx_packet[4] & 0x3f) + 5]);
}

/* Check if the crc of the received packet is valid */
static __attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint8_t tpll_is_crc_valid(
    uint8_t *rx_packet)
{
    return ((rx_packet[(rx_packet[4] & 0x3f) + 14] & 0x01) == 0x00);
}

static __attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint8_t
tpll_is_packet_len_valid(uint8_t *rx_packet)
{
    return (rx_packet[rx_packet[4] & 0x3f] <= 0x3e); // for compile error
}

/* Check if the payload length is zero */
static __attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) uint8_t tpll_is_packet_empty(
    uint8_t *rx_packet)
{
    return ((rx_packet[4] & 0x3f) == 0x00);
}

typedef struct
{
    uint8_t rx_pid;
    uint8_t local_pid;
    uint16_t rx_crc;
    uint16_t local_crc;
} tpll_packet_t;

tpll_packet_t tpll_packet = {0, 3, 0, 0};
volatile uint32_t debug_total_packets_cnt, debug_filtered_packets_cnt, debug_new_packets_cnt,
    debug_repeated_packets_cnt = 0, debug_empty_packets_cnt = 0, debug_crc_err_packets_cnt = 0;
static uint8_t m_wptr, m_rptr = 0;
static uint8_t *rx_packet = NULL;

/**
 * @brief   TPLL rx irq handler
 *
 * @param   p_event_handler         event handler
 *
 * @retval  none
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) void tpll_rx_irq_handler(
    tpll_event_handler_t p_event_handler)
{
    if (NULL == p_event_handler)
        return;

    rx_packet = tpll_get_rx_packet();
    tpll_packet.rx_pid = tpll_get_rx_packet_id(rx_packet);
    tpll_packet.rx_crc = tpll_get_rx_packet_crc(rx_packet);

    if (tpll_is_crc_valid(rx_packet) && tpll_is_packet_len_valid(rx_packet) &&
        !tpll_is_packet_empty(rx_packet)) // valid not-empty packet
    {
        if ((tpll_packet.local_pid == tpll_packet.rx_pid) && (tpll_packet.local_crc == tpll_packet.rx_crc))
        {
            // ignore repeated packets
            debug_repeated_packets_cnt++;
        }
        else // handle new packets
        {
            debug_new_packets_cnt++;
            p_event_handler(TPLL_EVENT_RX_RECEIVED);
        }
    }
    else if (tpll_is_crc_valid(rx_packet) && tpll_is_packet_len_valid(rx_packet) &&
             tpll_is_packet_empty(rx_packet)) // valid empty packet, it do not need to read
    {
        m_wptr = read_reg8(REG_DMA_RX_FIFO_WPTR) % TPLL_PIPE_RX_FIFO_NUM;
        m_rptr = read_reg8(REG_DMA_RX_FIFO_RPTR) % TPLL_PIPE_RX_FIFO_SIZE;
        if ((m_wptr % TPLL_PIPE_RX_FIFO_NUM) != (m_rptr % TPLL_PIPE_RX_FIFO_NUM))
            write_reg8(REG_DMA_RX_FIFO_RPTR, 0x40); // set bit6 to increase by one
        debug_empty_packets_cnt++;
    }
    else if (!tpll_is_crc_valid(rx_packet))
    {
        debug_crc_err_packets_cnt++;
    }
    else
    {
        debug_filtered_packets_cnt++;
    }
    debug_total_packets_cnt++;
    if(tpll_is_crc_valid(rx_packet))
    {
		tpll_packet.local_pid = tpll_packet.rx_pid;
		tpll_packet.local_crc = tpll_packet.rx_crc;
    }
}
