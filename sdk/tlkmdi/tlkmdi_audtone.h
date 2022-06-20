/********************************************************************************************************
 * @file     tlkmdi_tone.h
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

#ifndef TLKMDI_AUDTONE_H
#define TLKMDI_AUDTONE_H

#if (TLKMDI_CFG_AUDTONE_ENABLE)


typedef enum{
	TLKMDI_TONE_TYPE_CONNECT = 0x0,		
	TLKMDI_TONE_TYPE_DISCONN,
	TLKMDI_TONE_TYPE_CALL_RING,
	TLKMDI_TONE_TYPE_BI,
	TLKMDI_TONE_TYPE_BO,
	TLKMDI_TONE_TYPE_DING, 
	TLKMDI_TONE_TYPE_DING_DING,
	TLKMDI_TONE_TYPE_MAX
}TLKMDI_TONE_TYPE_ENUM;

/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int  tlkmdi_audtone_init(void);

int tlkmdi_audtone_start(uint16 handle, uint32 param);
int tlkmdi_audtone_close(uint16 handle);


/******************************************************************************
 * Function: tlkmdi_audtone_isBusy
 * Descript: Is the Tone Ctrl is enable. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
bool tlkmdi_audtone_isBusy(void);


/******************************************************************************
 * Function: tlkmdi_audtone_intval
 * Descript: Get Tone Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audtone_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audtone_switch
 * Descript: Change Tone status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_audtone_irqProc
 * Descript: Call Tone mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_irqProc(void);

/******************************************************************************
 * Function: tlkmdi_tone_start
 * Descript: Start tone to playing. 
 * Params:
 *         @index[IN]--The music index.
 * Return: Return TLK_ENONE is success other value is false.
*******************************************************************************/
bool tlkmdi_tone_start(uint16 index);

/******************************************************************************
 * Function: tlkmdi_tone_close
 * Descript: Stop tone to playing. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_close(void);

/******************************************************************************
 * Function: tlkmdi_tone_setParam
 * Descript: Set the playing paramter. 
 * Params: 
 *        @playIndex[IN]--Theplay index.
 *        @playcount[IN]--The play count.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_setParam(uint16 playIndex, uint08 playCount);



#endif //#if (TLKMDI_CFG_AUDTONE_ENABLE)

#endif //TLKMDI_AUDTONE_H

