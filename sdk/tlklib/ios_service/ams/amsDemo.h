/********************************************************************************************************
 * @file     amsDemo.h
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     15. 04, 2021
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 ********************************************************************************************************/


#ifndef _AMSDEMO_H_
#define _AMSDEMO_H_

#include "ams.h"
#include "amsDef.h"

void amsDemo_getTrackInfo();
void amsDemo_getQueueInfo();
void amsDemo_sendRemoteIdControl();
void ams_mainLoopTask();

#endif /* _AMSDEMO_H_ */
