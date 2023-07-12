/********************************************************************************************************
 * @file	tlkdrv_rtl2108.h
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
#ifndef TLKDRV_RTL2108_H
#define TLKDRV_RTL2108_H

#if (TLKDRV_CODEC_RTL2108_ENABLE)


#define TLKDRV_RTL2108_GPIO_SYS               GPIO_PB3//output high active
#define TLKDRV_RTL2108_GPIO_RESET             GPIO_PE7//output high active
#define TLKDRV_RTL2108_GPIO_INIT_READY        GPIO_PD5//input 500ms init 
#define TLKDRV_RTL2108_GPIO_I2S_READY         GPIO_PD4//output high actice
#define TLKDRV_RTL2108_GPIO_POWER_SWITCH      GPIO_PA4//input for power off


#define TLKDRV_RTL2108_CMD_HEADER1       0xaa
#define TLKDRV_RTL2108_CMD_HEADER2       0x55

typedef enum {
    TLKDRV_RTL2108_CMD_GET_VERSION       = 0x01,
	TLKDRV_RTL2108_CMD_GET_CURRENT_STATE = 0x02,
    TLKDRV_RTL2108_CMD_SET_MICGAIN       = 0x03,
    TLKDRV_RTL2108_CMD_GET_MICGAIN       = 0x04,
    TLKDRV_RTL2108_CMD_SET_MICMUTE       = 0x05,
    TLKDRV_RTL2108_CMD_GET_MICMUTE       = 0x06,
    TLKDRV_RTL2108_CMD_SET_MUSICEQ       = 0x0A,
    TLKDRV_RTL2108_CMD_GET_MUSICEQ       = 0x0B,
    TLKDRV_RTL2108_CMD_SET_CALL_STATE    = 0x0F,
    TLKDRV_RTL2108_CMD_SET_MIC_ENABLE    = 0x80,
    TLKDRV_RTL2108_CMD_FACTORY_RESET     = 0x86,
}TLKDRV_RTL2108_CMD_ENUM;

typedef enum {
    TLKDRV_RTL2108_CMD_EQ_FLAT    = 0x00,
	TLKDRV_RTL2108_CMD_EQ_POP     = 0x01,
	TLKDRV_RTL2108_CMD_EQ_JAZZ    = 0x02,
	TLKDRV_RTL2108_CMD_EQ_CLASSIC = 0x03,
	TLKDRV_RTL2108_CMD_EQ_ROCK    = 0x04,
}TLKDRV_RTL2108_EQ_ENUM;



typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint08 isMute;
	uint08 micVol;
	uint08 micIsEn;
	uint08 spkIsEn;
	uint08 channel;
	uint08 bitDepth;
	uint32 sampleRate;
}tlkdrv_rtl2108_t;





void tlkdrv_rtl2108_getFwVersion(void);
void tlkdrv_rtl2108_setCurrentState(uint08 state);
void tlkdrv_rtl2108_setMicGain(uint08 volume);
void tlkdrv_rtl2108_getMicGain(void);
void tlkdrv_rtl2108_setMicMuteStatus(uint08 status);
void tlkdrv_rtl2108_getMicMuteStatus(void);
void tlkdrv_rtl2108_setMusicEq(uint08 eqType);
void tlkdrv_rtl2108_getMusicEq(void);
void tlkdrv_rtl2108_setCallStatus(uint08 status);
void tlkdrv_rtl2108_setMicEnable(void);

void tlkdrv_rtl2108_factoryReset(void);




#endif //#if (TLKDRV_CODEC_RTL2108_ENABLE)

#endif //TLKDRV_RTL2108_H

