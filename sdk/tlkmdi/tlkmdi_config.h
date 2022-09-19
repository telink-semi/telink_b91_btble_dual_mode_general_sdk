/********************************************************************************************************
 * @file     tlkmdi_config.h
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

#ifndef TLKMDI_CONFIG_H
#define TLKMDI_CONFIG_H



#define TLKMDI_BTACL_DBG_ENABLE      (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_BTREC_DBG_ENABLE      (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_BTINQ_DBG_ENABLE      (1 && TLKMDI_CFG_DBG_ENABLE)

#define TLKMDI_FILE_DBG_ENABLE       (1 && TLKMDI_CFG_DBG_ENABLE)

#define TLKMDI_AUD_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_MP3_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_SCO_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_SRC_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_SNK_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_HF_DBG_ENABLE         (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_AG_DBG_ENABLE         (1 && TLKMDI_CFG_DBG_ENABLE)

#define TLKMDI_PLAY_DBG_ENABLE       (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_TONE_DBG_ENABLE       (1 && TLKMDI_CFG_DBG_ENABLE)

#define TLKMDI_HFP_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_HID_DBG_ENABLE        (1 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_KEY_DBG_ENABLE        (0 && TLKMDI_CFG_DBG_ENABLE)
#define TLKMDI_LED_DBG_ENABLE        (0 && TLKMDI_CFG_DBG_ENABLE)


#define TLKMDI_EVENT_UNIT_NUMB       64
#define TLKMDI_EVENT_UNIT_SIZE       16

#define TLKMDI_AUDIO_VOLUME_DEF               60
#define TLKMDI_AUDIO_VOLUME_STEP              6
#define TLKMDI_AUDIO_VOLUME_EXPAND            7

/******************************************************************************
 * Macro: TLKMDI_CFG_xxxx_ENABLE
 * Descr: 
*******************************************************************************/
#define TLKMDI_CFG_AUDPLAY_ENABLE             (1 && TLK_MDI_AUDIO_ENABLE && TLK_MDI_MP3_ENABLE)
#define TLKMDI_CFG_AUDTONE_ENABLE             (1 && TLK_MDI_AUDIO_ENABLE && TLK_MDI_MP3_ENABLE)
#define TLKMDI_CFG_AUDSRC_ENABLE              (1 && TLK_MDI_AUDIO_ENABLE && TLK_MDI_MP3_ENABLE)
#define TLKMDI_CFG_AUDSNK_ENABLE              (1 && TLK_MDI_AUDIO_ENABLE)
#define TLKMDI_CFG_AUDSCO_ENABLE              (1 && TLK_MDI_AUDIO_ENABLE)
#define TLKMDI_CFG_AUDHFP_ENABLE              (1 && TLK_MDI_AUDIO_ENABLE)
#define TLKMDI_CFG_VOL_GRAD_INC_ENABLE        (1 && TLK_MDI_AUDIO_ENABLE)       
#define TLKMDI_CFG_REUSE_SPKBUFF_ENABLE       (0 && TLK_MDI_AUDIO_ENABLE)

#define TLKMDI_CFG_MP3_NAME_SIZE              48


#endif //TLKMDI_CONFIG_H

