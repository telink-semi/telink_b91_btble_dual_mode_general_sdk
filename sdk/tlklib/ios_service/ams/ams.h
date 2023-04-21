/*********************************************************************************************

***********
 * @file     ams.h
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     15.04.2021
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary

property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor

(Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms

described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information

in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND

is provided.
 *


**********************************************************************************************

*********/
#ifndef _AMS_H_
#define _AMS_H_
///type

#include "amsDef.h"
///all the string format was 'UTF-8'
void amsInit(u16 attsIdType);
void amsEventHandle ();
void amsStackCallback(u8 *p);
void amsFuncSetEnable(u8 enable);

u8 amsFuncIsEn();
bool ams_findAmsService();
ams_sevice_state_t amsGetConnState();

/*
 * get current ams library version
 *
 */
u8 *getAmsVersion();

/*
 * translate the packet format with BLE-Stack interface as AMS old packet format
 */
rf_packet_l2cap_req_t_special_ams* ams_attPktTrans(u8 *p);

/*
 * send the entity id packet to MS
 * reference the AMS protocol to send or the demo code
 * parm.entityId and parm.attsIds can be followed as: entityId + attributeId or entityId + attributeId + attributeId...
 */
void ams_sendEntityIdRequestPkt(ams_sentEntityIdPkt_parm parm);

/*
 * send the remote command id packet to MS
 * reference the AMS protocol to send or the demo code
 */
void ams_sendRemoteCmdIdRequestPkt(u8 remoteCmdId);


/*
 * getting the flag about info update,
 * for notice that info would updated
 * after it received the newest notice from MS
 */
u8 ams_getInfoUpdateFlag();

/*
 * setting the info update flag,
 * for notice that info would updated
 * after it received the newest notice from MS
 */
void ams_setInfoUpdateFlag(u8 flag);

/*
 * getting current player shuffle mode state
 */
u8 ams_getQueueShuffleModeState();

/*
 * getting current player repeat mode state
 */
u8 ams_getQueueRepeatMoedState();

/*
 * getting current track artist string
 */
ams_storageFormat ams_getTrackArtist();

/*
 * getting current track album string
 */
ams_storageFormat ams_getTrackAlbum();

/*
 * getting current track title string
 */
ams_storageFormat ams_getTrackTitle();

/*
 * getting current track duration
 */
ams_storageFormat ams_getTrackDuration();

/*
 * getting current player APP name
 */
ams_storageFormat ams_getPlayerAppName();

/*
 * getting current volume value
 */
ams_storageFormat ams_getPlayerVolume();

/*
 * getting the player information, current player application name, playing state or others
 */
ams_playerPlaybackInfo_t ams_getPlayerPlaybackInfo();

#endif /* _AMS_H_ */
