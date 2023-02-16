/********************************************************************************************************
 * @file     btp_avrcp.h
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

#ifndef BTP_AVRCP_H
#define BTP_AVRCP_H




/* Define PASSTHROUGH OP_ID */
typedef enum{
	BTP_AVRCP_KEYID_SELECT        = 0x00,
    BTP_AVRCP_KEYID_UP            = 0x01,
    BTP_AVRCP_KEYID_DOWN          = 0x02,
    BTP_AVRCP_KEYID_LEFT          = 0x03,
    BTP_AVRCP_KEYID_RIGHT         = 0x04,
    BTP_AVRCP_KEYID_RIGHT_UP      = 0x05,
    BTP_AVRCP_KEYID_RIGHT_DOWN    = 0x06,
    BTP_AVRCP_KEYID_LEFT_UP       = 0x07,
    BTP_AVRCP_KEYID_LEFT_DOWN     = 0x08,
    BTP_AVRCP_KEYID_ROOT_MENU     = 0x09,
    BTP_AVRCP_KEYID_SETUP_MENU    = 0x0A,
    BTP_AVRCP_KEYID_CONTENTS_MENU = 0x0B,
    BTP_AVRCP_KEYID_FAVORITE_MENU = 0x0C,
    BTP_AVRCP_KEYID_EXIT          = 0x0D,
    BTP_AVRCP_KEYID_RESERVED_1    = 0x0E,
    BTP_AVRCP_KEYID_0             = 0x20,
    BTP_AVRCP_KEYID_1             = 0x21,
    BTP_AVRCP_KEYID_2             = 0x22,
    BTP_AVRCP_KEYID_3             = 0x23,
    BTP_AVRCP_KEYID_4             = 0x24,
    BTP_AVRCP_KEYID_5             = 0x25,
    BTP_AVRCP_KEYID_6             = 0x26,
    BTP_AVRCP_KEYID_7             = 0x27,
    BTP_AVRCP_KEYID_8             = 0x28,
    BTP_AVRCP_KEYID_9             = 0x29,
    BTP_AVRCP_KEYID_DOT           = 0x2A,
    BTP_AVRCP_KEYID_ENTER         = 0x2B,
    BTP_AVRCP_KEYID_CLEAR         = 0x2C,
    BTP_AVRCP_KEYID_RESERVED_2    = 0x2D,
    BTP_AVRCP_KEYID_CHANNEL_UP    = 0x30,
    BTP_AVRCP_KEYID_CHANNEL_DOWN  = 0x31,
    BTP_AVRCP_KEYID_PREVIOUS_CHANNEL = 0x32,
    BTP_AVRCP_KEYID_SOUND_SELECT  = 0x33,
    BTP_AVRCP_KEYID_INPUT_SELECT  = 0x34,
    BTP_AVRCP_KEYID_DISPLAY_INFORMATION = 0x35,
    BTP_AVRCP_KEYID_HELP          = 0x36,
    BTP_AVRCP_KEYID_PAGE_UP       = 0x37,
    BTP_AVRCP_KEYID_PAGE_DOWN     = 0x38,
    BTP_AVRCP_KEYID_RESERVED_3    = 0x39,
    BTP_AVRCP_KEYID_SKIP          = 0x3C,
    BTP_AVRCP_KEYID_POWER         = 0x40,
    BTP_AVRCP_KEYID_VOLUME_UP     = 0x41,
    BTP_AVRCP_KEYID_VOLUME_DOWN   = 0x42,
    BTP_AVRCP_KEYID_MUTE          = 0x43,
    BTP_AVRCP_KEYID_PLAY          = 0x44,
    BTP_AVRCP_KEYID_STOP          = 0x45,
    BTP_AVRCP_KEYID_PAUSE         = 0x46,
    BTP_AVRCP_KEYID_RECORD        = 0x47,
    BTP_AVRCP_KEYID_REWIND        = 0x48,
    BTP_AVRCP_KEYID_FAST_FORWARD  = 0x49,
    BTP_AVRCP_KEYID_EJECT         = 0x4A,
    BTP_AVRCP_KEYID_FORWARD       = 0x4B,
    BTP_AVRCP_KEYID_BACKWARD      = 0x4C,
    BTP_AVRCP_KEYID_RESERVED_4    = 0x4D,
    BTP_AVRCP_KEYID_ANGLE         = 0x50,
    BTP_AVRCP_KEYID_SUBPICTURE    = 0x51,
    BTP_AVRCP_KEYID_RESERVED_5    = 0x52,
    BTP_AVRCP_KEYID_F1            = 0x71,
    BTP_AVRCP_KEYID_F2            = 0x72,
    BTP_AVRCP_KEYID_F3            = 0x73,
    BTP_AVRCP_KEYID_F4            = 0x74,
    BTP_AVRCP_KEYID_F5            = 0x75,
    BTP_AVRCP_KEYID_RESERVED_6    = 0x76,
}BTP_AVRCP_KEYID_ENUM;

typedef enum{
	BTP_AVRCP_PLAY_STATE_STOPPED = 0,
	BTP_AVRCP_PLAY_STATE_PLAYING,
	BTP_AVRCP_PLAY_STATE_PAUSED,
	BTP_AVRCP_PLAY_STATE_FWD_SEEK,
	BTP_AVRCP_PLAY_STATE_REV_SEEK,
}BTP_AVRCP_PLAY_STATE_ENUM;


//#endif
#define AVRCP_BTN_PUSHED	0x00
#define AVRCP_BTN_RELEASED	0x80

#define AVRCP_EVENT_PLAYBACK_STATUS_STOPPED         0x00
#define AVRCP_EVENT_PLAYBACK_STATUS_PLAYING         0x01
#define AVRCP_EVENT_PLAYBACK_STATUS_PAUSE           0x02


typedef void (*BtpAvrcpKeyChangeCallback)(uint16 aclHandle, uint08 keyID, uint08 isPress);
typedef void (*BtpAvrcpVolumeChangeCallback)(uint16 aclHandle, uint08 volume);



#define BTP_AVRCP_TIMEOUT         200000 //100ms
#define BTP_AVRCP_CONN_TIMEOUT    (5000000/BTP_AVRCP_TIMEOUT)
#define BTP_AVRCP_DISC_TIMEOUT    (5000000/BTP_AVRCP_TIMEOUT)


/******************************************************************************
 * Function: AVRCP init interface
 * Descript: This interface be used by user to initial the avrcp resource
 *           of client/server before create a connection between the entity.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_init(void);

/******************************************************************************
 * Function: AVRCP Trigger Connect interface
 * Descript: Defines trigger the avrcp connect cmd to peer avrcp entity and setup
 *           a connection with devices.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @usrId--The service user id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_connect(uint16 aclHandle, uint08 usrID);

/******************************************************************************
 * Function: AVRCP Trigger Disonnect interface
 * Descript: Defines trigger the avrcp disconnect cmd to peer avrcp entity and
 *           tear up a connection which specify by aclhandle and usrid.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @usrId--The service user id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_disconn(uint16 aclHandle, uint08 usrID);

/******************************************************************************
 * Function: AVRCP Send destroy interface
 * Descript: Defines trigger to release the resource which allocated.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 * Return: null
*******************************************************************************/
void btp_avrcp_destroy(uint16 aclHandle);


/******************************************************************************
 * Function: AVRCP get play/stop status interface
 * Descript: Defines trigger the avrcp cmd check the music status of peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 * Return: true(success)/false(fail)
*******************************************************************************/
bool btp_avrcp_remoteIsPlaying(uint16 aclHandle);

/******************************************************************************
 * Function: AVRCP query set volume Command interface
 * Descript: Defines trigger query the peer avrcp whether support set volume or not.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 * Return: true(success)/false(fail)
*******************************************************************************/
bool btp_avrcp_isSupportSetVolume(uint16 aclHandle);

/******************************************************************************
 * Function: AVRCP set Volume Command interface
 * Descript: Defines trigger the avrcp change the music volume to peer entity.
 * Params:
 *        @volume--The volume value.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_avrcp_setVolume(uint16 aclHandle, uint08 volume, bool isSrc);

/******************************************************************************
 * Function: AVRCP Set music state interface
 * Descript: Defines trigger the avrcp cmd change the music of peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @playsate--The state of music to be set.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_setPlayState(uint16 aclHandle, uint08 playState);

/******************************************************************************
 * Function: AVRCP Notify music sate Command interface
 * Descript: Defines trigger the avrcp of CT notify the music state 
 *           changing to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @playState--The service channel id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_notifyPlayState(uint16 aclHandle, uint08 playState);

/******************************************************************************
 * Function: AVRCP Send Key interface
 * Descript: Defines trigger the avrcp key press cmd to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @keyID--The key id which reflect to a special command.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_sendKeyPress(uint16 aclHandle, uint08 keyID);

/******************************************************************************
 * Function: AVRCP Send Key Release interface
 * Descript: Defines trigger the avrcp key release cmd to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @keyID--The key id which reflect to a special command.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int  btp_avrcp_sendKeyRelease(uint16 aclHandle, uint08 keyID);

/******************************************************************************
 * Function: AVRCP Register to listen the Music change interface
 * Descript: Defines register a callback to listen the music player state changing
 *           which operate by avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @cb--The play changed callback.
 * Return: null
*******************************************************************************/
void btp_avrcp_regKeyChangeCB(BtpAvrcpKeyChangeCallback cb);

/******************************************************************************
 * Function: AVRCP Register Volume Change interface
 * Descript: Defines register a volume change of music player.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @cb--The volume changed callback.
 * Return: null
*******************************************************************************/
void btp_avrcp_regVolumeChangeCB(BtpAvrcpVolumeChangeCallback cb);




#endif /* BTP_AVRCP_H */

