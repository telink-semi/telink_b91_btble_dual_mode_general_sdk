/********************************************************************************************************
 * @file	tlkdrv_mmc5603MemsicCompass.h
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
/*****************************************************************************
* Copyright (C), 2018, MEMSIC Inc.
* File Name		: MemsicAlgo.h
* Author		: MEMSIC AE Algoritm Team
* Description	: This file is the head file of MemsicAlgo.lib. It provides the 
* 			      interface function declarations of the lib. 
* History		: 1. Data			: 20180424
* 			  	  2. Author			: Yan Guopu
*			  	  3. Modification	: 	
*****************************************************************************/
#ifndef _MEMSIC_COMPASS_H_
#define _MEMSIC_COMPASS_H_

#if TLKDRV_GSENSOR_MMC5603_ENABLE



#define ANDROID_PLATFORM	1
#define EVB_PLATFORM		2
#define WINDOWS_PLATFORM	3
#define PLATFORM			EVB_PLATFORM

#if((PLATFORM == ANDROID_PLATFORM)||(PLATFORM == EVB_PLATFORM))
#define DLL_API
#define _stdcall
#elif(PLATFORM == WINDOWS_PLATFORM)
#define DLL_API extern "C" __declspec(dllexport)
#endif

#if(PLATFORM == ANDROID_PLATFORM)
#include <utils/Log.h>	//ALOGE
#endif


/*******************************************************************************************
* Function Name	: InitialCompassAlgo
* Description	: Initial the filter coeficient for the compass filter.
* Input			: fc -- fliter = (1-fc)*old + fc*new; the fc smaller, the filter stronger.
* Output		: None.
* Return		: 1 --- succeed.
*				 -1 --- fail. 
********************************************************************************************/
DLL_API int _stdcall InitialCompassAlgo(float fc);

/*******************************************************************************************
* Function Name	: GetCalOri
* Description	: Get the MEMSIC calibrated orientation vector.
* Input			: acc[0-2] --- Accelerometer data of three axis, unit is g.
*               : mag[0-2] --- Magnetic calibrated data of three axis, unit is gauss.
* Output		: co[0-2]  --- orientation data, azimuth, pitch, roll, unit is degree.
* Return		: 1 --- succeed.
*				 -1 --- fail.
********************************************************************************************/
DLL_API int _stdcall GetCalOri(float *acc, float *mag, float *co);



#endif //#if TLKDRV_GSENSOR_MMC5603_ENABLE

#endif

