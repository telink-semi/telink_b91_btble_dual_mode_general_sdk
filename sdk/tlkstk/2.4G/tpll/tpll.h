/********************************************************************************************************
 * @file	tpll.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef _TPLL_H_
#define _TPLL_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_BASE_NUM (0x0)                           /** Global error base */
#define SUCCESS (ERROR_BASE_NUM + 0)              /** Successful command */
#define ERROR_NULL (ERROR_BASE_NUM + 1)           /** Null Pointer */
#define ERROR_INVALID_PARAM (ERROR_BASE_NUM + 2)  /** Invalid Parameter */
#define ERROR_BUSY (ERROR_BASE_NUM + 3)           /** Busy */
#define ERROR_INVALID_STATE (ERROR_BASE_NUM + 4)  /** Invalid state, operation disallowed in this state */
#define ERROR_BUFFER_EMPTY (ERROR_BASE_NUM + 5)   /** FIFO is empty */
#define ERROR_NO_MEM (ERROR_BASE_NUM + 6)         /** No Memory for operation */
#define ERROR_INVALID_LENGTH (ERROR_BASE_NUM + 7) /** Invalid Length */

#define ADDR_MIN_LEN 3   /** Minimum address length */
#define ADDR_MAX_LEN 5   /** Maximum address length */
#define TPLL_PIPE_COUNT 6 /** Maximum pipe count */

#define TPLL_MAX_PAYLOAD_LENGTH 64 /** The maximum size of the payload */

/** @brief Macro for verifying that a function returned SUCCESS. It will cause the exterior
 *        function to return error code of statement if it is not @ref SUCCESS.
 *
 * @param[in] statement     Statement to check against SUCCESS.
 */
#define VERIFY_SUCCESS(statement)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        uint32_t _err_code = (uint32_t)(statement);                                                                    \
        if (_err_code != SUCCESS)                                                                                 \
        {                                                                                                              \
            return _err_code;                                                                                          \
        }                                                                                                              \
    } while (0)

/** @brief Primary LinkLayer pipe IDs. */
typedef enum
{
    TPLL_PIPE0 = 0,      /** Select pipe0               */
    TPLL_PIPE1,          /** Select pipe1               */
    TPLL_PIPE2,          /** Select pipe2               */
    TPLL_PIPE3,          /** Select pipe3               */
    TPLL_PIPE4,          /** Select pipe4               */
    TPLL_PIPE5,          /** Select pipe5               */
    TPLL_TX,             /** Refer to TX address        */
    TPLL_PIPE_ALL = 0xFF /** Close or open all pipes    */
} tpll_pipeid_t;

/** @brief Primary LinkLayer bitrate modes. */
typedef enum
{
    TPLL_BITRATE_2MBPS = RF_MODE_PRIVATE_2M, /** 2 Mb radio mode. */
    TPLL_BITRATE_1MBPS = RF_MODE_PRIVATE_1M, /** 1 Mb radio mode. */
} tpll_bitrate_t;

/** @brief  Primary LinkLayer modes. */
typedef enum
{
    TPLL_MODE_PTX, /** Primary transmitter mode. */
    TPLL_MODE_PRX  /** Primary receiver mode.    */
} tpll_mode_t;

/** @brief Primary LinkLayer event IDs used to indicate the type of the event. */
typedef enum
{
    TPLL_EVENT_TX_SUCCESS,  /** Event triggered on TX success   */
    TPLL_EVENT_TX_FAILED,   /** Event triggered on TX failure   */
    TPLL_EVENT_RX_RECEIVED, /** Event triggered on RX received  */
} tpll_event_id_t;

/** @brief Definition of the event handler for the module. */
typedef void (*tpll_event_handler_t)(tpll_event_id_t evt_id);

/** @brief Primary LinkLayer payload.
 *
 * @details This payload is used both for transmissions and for acknowledging a received packet with a payload.
 */
typedef struct
{
    uint8_t length; /** Length of the packet (maximum value is @ref TPLL_MAX_PAYLOAD_LENGTH) */
    uint8_t pipe;   /** Pipe used for this payload */
    uint8_t noack; /** Flag indicating that this packet will not be acknowledgement. Flag is ignored when selective auto
                      ack is enabled */
    uint8_t pid;   /** PID assigned during communication */
    int8_t rssi;   /** RSSI for the received packet */
    uint8_t data[TPLL_MAX_PAYLOAD_LENGTH]; /** The payload data */
} tpll_payload_t;

/** @brief  Main configuration structure for the module. */
typedef struct
{
    tpll_mode_t mode;                   /** Primary LinkLayer mode */
    tpll_event_handler_t event_handler; /** Primary LinkLayer event handler */

    // General RF parameters
    tpll_bitrate_t bitrate;                          /** Primary LinkLayer bitrate mode */
    rf_power_level_index_e tx_output_power_idx; /** Primary LinkLayer radio transmission power idx */
    uint16_t retransmit_delay;                  /** The delay between each retransmission of unacknowledged packets */
    uint16_t retransmit_count;                  /** The number of retransmission attempts before transmission fail */

    uint16_t tx_settle;  /** TX settle time */
    uint16_t rx_settle;  /** RX settle time */
    uint16_t rx_timeout; /** RX timeout time */

    // Control settings
    uint8_t payload_length; /** Length of the payload (maximum length depends on the platforms that are used on each
                               side) */
    tpll_pipeid_t pipe; /** Pipe ID */
    rf_irq_e irq_mask;      /** IRQ mask */
} tpll_config_t;

/** @brief Primary LinkLayer address width. */
typedef enum
{
    TPLL_ADDRESS_WIDTH_3BYTES = 3, /** Set address width to 3 bytes */
    TPLL_ADDRESS_WIDTH_4BYTES,     /** Set address width to 4 bytes */
    TPLL_ADDRESS_WIDTH_5BYTES      /** Set address width to 5 bytes */
} tpll_address_width_t;

/** @brief Default address configuration for Primary LinkLayer. */
#define TPLL_ADDR_DEFAULT                                                                                          \
    {                                                                                                                  \
        .base_address_0 = {0xe7, 0xe7, 0xe7, 0xe7}, .base_address_1 = {0xc2, 0xc2, 0xc2, 0xc2},                        \
        .pipe_prefixes = {0xe7, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6}, .addr_length = 5, .pipe_nums = TPLL_PIPE_COUNT      \
    }

/** @brief  Default radio parameters.*/
#define TPLL_DEFAULT_CONFIG                                                                                        \
    {                                                                                                                  \
        .mode = TPLL_MODE_PTX, .bitrate = TPLL_BITRATE_2MBPS, .tx_output_power_idx = RF_POWER_INDEX_P0p01dBm,      \
        .tx_settle = 118, .rx_settle = 85, .retransmit_delay = 150, .retransmit_count = 3, .payload_length = 32,       \
        .pipe = TPLL_PIPE0, .irq_mask = 0,                                                                         \
    }

/** @brief Implementation details for TRF_NUM_VA_ARGS */
#define TRF_LEN_VA_ARGS_CAL(                                                                                           \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, \
    _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, \
    _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, \
    _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, \
    _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113,  \
    _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, N, ...)                        \
    N

/**@brief Macro to get the number of arguments in a call variadic macro call
 *
 * param[in]    ...     List of arguments
 *
 * @retval  Number of variadic arguments in the argument list
 */
#define TRF_NUM_VA_ARGS(...)                                                                                           \
    TRF_LEN_VA_ARGS_CAL(__VA_ARGS__, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113,   \
                        112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93,   \
                        92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70,    \
                        69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47,    \
                        46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,    \
                        23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/** @brief Macro to create an initializer for a TX data packet.
 *
 * @details This macro generates an initializer. Using the initializer is more efficient
 *          than setting the individual parameters dynamically.
 *
 * @param[in]   _pipeid The pipe to use for the data packet.
 * @param[in]   ...     Comma separated list of character data to put in the TX buffer.
 *
 * @return  Initializer that sets up the pipe, length, and byte array for content of the TX data.
 */
#define TPLL_CREATE_PAYLOAD(_pipeid, ...)                                                                          \
    {.pipe = _pipeid, .length = TRF_NUM_VA_ARGS(__VA_ARGS__), .data = {__VA_ARGS__}};

/** @brief  Function for setting RF tx settle time.
 *
 * @param[in]   tx_stl_period_us                Tx settle time.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_tx_settle_time(uint16_t tx_stl_period_us);

/** @brief  Function for setting RF rx settle time.
 *
 * @param[in]   rx_stl_period_us                Rx settle time.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_rx_settle_time(uint16_t rx_stl_period_us);

/** @brief  Function for setting the radio bitrate.
 *
 * @param[in]   bitrate                         Radio bitrate.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_bitrate(tpll_bitrate_t bitrate);

/** @brief  Function for initializing the Primary LinkLayer module.
 *
 * @param   p_config                            Parameters for initializing the module.
 *
 * @retval  SUCCESS                        If initialization was successful.
 * @retval  ERROR_NULL                     If the @p p_config argument was NULL.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_config_init(tpll_config_t const *p_config);

/**
 * @brief   Function for getting Primary LinkLayer event handler function
 *
 * @return  event handler function pointer
 */
tpll_event_handler_t tpll_get_event_handler(void);

/** @brief  Function for setting the channel to use for the radio.
 *
 * @details The module must be in an idle state to call this function. As a PTX, the
 *          application must wait for an idle state and as a PRX, the application must stop RX
 *          before changing the channel. After changing the channel, operation can be resumed.
 *
 * @param[in]   channel                         Channel to use for radio.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_INVALID_STATE            If the module is not initialized.
 * @retval  ERROR_BUSY                     If the module was not in idle state.
 * @retval  ERROR_INVALID_PARAM            If the channel is invalid (larger than 100).
 */
uint32_t tpll_set_rf_channel(uint32_t channel);

/** @brief  Function for setting the length of the address.
 *
 * @param[in]   address_width                   Length of the Primary LinkLayer address (in bytes).
 *
 * @retval  SUCCESS                        If the address length was set successfully.
 * @retval  ERROR_INVALID_PARAM            If the address length was invalid.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_address_length(tpll_address_width_t address_width);

/** @brief  Function for setting the address for a specific pipe.
 *
 * @param[in]   pipe_id                         The pipe channel id that you want to set access code.
 * @param[in]   addr                            The pointer of address that you want to set the access code.
 */
void tpll_set_address(tpll_pipeid_t pipe_id, uint8_t *addr);

/** @brief  Function for setting the base address for pipe 0.
 *
 * @param[in]   p_addr                          Pointer to the address data.
 *
 * @retval  SUCCESS                        If the base address was set successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM            If the function failed because the address given was too close to a zero
 * address.
 * @retval  ERROR_NULL                     If the required parameter was NULL.
 */
uint32_t tpll_set_base_address_0(uint8_t const *p_addr);

/** @brief  Function for setting the base address for pipe 1 to pipe 5.
 *
 * @param[in]   p_addr                          Pointer to the address data.
 *
 * @retval  SUCCESS                        If the base address was set successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM            If the function failed because the address given was too close to a zero
 * address.
 * @retval  ERROR_NULL                     If the required parameter was NULL.
 */
uint32_t tpll_set_base_address_1(uint8_t const *p_addr);

/**
 * @brief   Function for setting the number of pipes and the pipe prefix addresses.
 *
 * @details This function configures the number of available pipes, enables the pipes,
 *          and sets their prefix addresses.
 *
 * @param[in]   p_prefixes                      The pointer to a char array that contains the prefix for each pipe.
 * @param[in]   num_pipes                       Number of pipes. Must be less than or equal to @ref TPLL_PIPE_COUNT.
 *
 * @retval  SUCCESS                        If the prefix addresses were set successfully.
 * @retval  ERROR_NULL                     If the required parameter was NULL.
 * @retval  ERROR_INVALID_PARAM            If an invalid number of pipes was given or if the address given was too
 * close to a zero address.
 */
uint32_t tpll_set_prefixes(uint8_t *p_prefixes, uint8_t num_pipes);

/**
 * @brief   Function for getting the width of the address.
 *
 * @return  length of the TPLL address (in bytes).
 */
uint8_t tpll_get_address_length(void);

/** @brief  Function for enabling pipes.
 *
 * @details The @p pipe_id parameter must contain the same number of pipes as has been configured
 * with @ref tpll_set_prefixes. This number may not be greater than the number defined by
 * @ref TPLL_PIPE_COUNT
 *
 * @param   pipe_id                             The pipe id that need to enable( @ref tpll_pipeid_t锛�
 *
 * @retval  SUCCESS                        If the pipes were enabled and disabled successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM            If the function failed because the address given was too close to a zero
 * address.
 */
uint32_t tpll_enable_pipes(tpll_pipeid_t pipe_id);

/** @brief  Function for setting one pipe as a TX pipe.
 *
 * @param   pipe_id                             Pipe to be set as a TX pipe.
 *
 * @retval  SUCCESS                        If set tx pipe success.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM            If the given pipe number was invalid.
 */
uint32_t tpll_set_tx_pipe(tpll_pipeid_t pipe_id);

/**
 * @brief   TPLL rx irq handler
 *
 * @param   p_event_handler                     Event handler for RX Irq
 */
void tpll_rx_irq_handler(tpll_event_handler_t p_event_handler);

/** @brief  Function for writing a payload for transmission or acknowledgement.
 *
 * @details This function writes a payload that is added to the tx fifo.
 *
 * @param[in]   p_payload                       The pointer to the structure that contains information and state of the
 * payload.
 *
 * @retval  SUCCESS                        If the payload was successfully queued for writing.
 * @retval  ERROR_NULL                     If the required parameter was NULL.
 * @retval  ERROR_INVALID_STATE            If the module is not initialized.
 * @retval  ERROR_NO_MEM                   If the TX FIFO is full.
 * @retval  ERROR_INVALID_LENGTH           If the payload length was invalid (zero or larger than the allowed
 * maximum).
 */
uint32_t tpll_write_payload(tpll_payload_t const *p_payload);

/** @brief  Function for reading an RX payload.
 *
 * @param[in,out]   p_payload                   The pointer to the structure that contains information and state of the
 * payload.
 *
 * @retval  SUCCESS                        If the data was read successfully.
 * @retval  ERROR_NULL                     If the required parameter was NULL.
 * @retval  ERROR_INVALID_STATE            If the module is not initialized.
 */
uint32_t tpll_read_rx_payload(tpll_payload_t *p_payload);

/** @brief Function for starting transmission.
 *
 * @retval  SUCCESS                        If the TX started successfully.
 * @retval  ERROR_BUFFER_EMPTY             If the TX did not start because the FIFO buffer is empty.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_start_tx(void);

/** @brief  Function for starting to transmit data from the FIFO buffer.
 *
 * @retval  SUCCESS                        If the transmission was started successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_start_rx(void);

/**
 * @brief   Function for setting the packet retransmit delay.
 *
 * @param[in]   delay                           Delay between retransmissions.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_retransmit_delay(uint16_t delay);

/** @brief  Function for setting the number of retransmission attempts.
 *
 * @param[in]   count                           Number of retransmissions.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_retransmit_count(uint16_t count);

/** @brief  Function for disabling the Primary LinkLayer module.
 *
 * @details Calling this function disables the Primary LinkLayer module immediately.
 *          Doing so might stop ongoing communications.
 *
 * @retval  SUCCESS                        If Primary LinkLayer was disabled.
 */
uint32_t tpll_disable(void);

/** @brief Function for removing remaining items from the TX buffer.
 *
 * @details This function clears the specific TX FIFO buffer.
 *
 * @param[in]   pipe_id                         The pipe for which need flush tx buffer.
 *
 * @retval  SUCCESS                        If pending items in the TX buffer were successfully cleared.
 * @retval  ERROR_INVALID_STATE            If the module is not initialized.
 */
uint32_t tpll_flush_tx(tpll_pipeid_t pipe_id);

/** @brief  Function for removing remaining items from the RX buffer.
 *
 * @retval  SUCCESS                        If the pending items in the RX buffer were successfully cleared.
 * @retval  ERROR_INVALID_STATE            If the module is not initialized.
 */
uint32_t tpll_flush_rx(void);

/** @brief  Function for setting the radio output power.
 *
 * @param[in]   tx_output_power_idx                 Output power index.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 */
uint32_t tpll_set_tx_power(rf_power_level_index_e tx_output_power_idx);

/**
 * @brief   Function for updating the read pointer of the TX FIFO.
 *
 * @param[in]   pipe_id                         Pipe id.
 *
 * @retval  SUCCESS                        If the operation completed successfully.
 * @retval  ERROR_BUSY                     If the function failed because the radio is busy.
 * @retval  ERROR_INVALID_PARAM            If the given pipe number was invalid.
 */
uint32_t tpll_update_tx_rptr(tpll_pipeid_t pipe_id);

#ifdef __cplusplus
}
#endif

#endif /*_TPLL_H_*/
