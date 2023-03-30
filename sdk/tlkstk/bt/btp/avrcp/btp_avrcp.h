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

typedef enum{
	BTP_AVRCP_STATUS_CODE_INVALID_CMD              = 0x00, //Invalid command, sent if TG received a PDU that it did not understand.
	BTP_AVRCP_STATUS_CODE_INVALID_PARAMTER         = 0x01, //Invalid parameter, sent if the TG received a PDU with a parameter ID that it did not understand.
	BTP_AVRCP_STATUS_CODE_PARAMTER_CONTENT_ERROR   = 0x02, //Parameter content error. Sent if the parameter ID is understood, but content is wrong or corrupted.
	BTP_AVRCP_STATUS_CODE_INTERNAL_ERROR           = 0x03, //Internal Error - sent if there are error conditions not covered by a more specific error code.
	BTP_AVRCP_STATUS_CODE_OPERATE_WITHOUT_ERROR    = 0x04, //Operation completed without error. This is the status that should be returned if the operation was successful.
	BTP_AVRCP_STATUS_CODE_UID_CHANGE               = 0x05, //UID Changed. The UIDs on the device have changed
	BTP_AVRCP_STATUS_CODE_RESERVED                 = 0x06,
	BTP_AVRCP_STATUS_CODE_INVALID_DIRECTION        = 0x07, //Invalid Direction. The Direction parameter is invalid
	BTP_AVRCP_STATUS_CODE_NOT_A_DIRECTORY          = 0x08, //Not a Directory. The UID provided does not refer to a folder item
	BTP_AVRCP_STATUS_CODE_DOES_NOT_EXIST           = 0x09, //Does Not Exist. The UID provided does not refer to any currently valid item
	BTP_AVRCP_STATUS_CODE_INVALID_SCOPE            = 0x0A, //Invalid Scope. The scope parameter is invalid
	BTP_AVRCP_STATUS_CODE_RANGE_OUT_OF_BOUNDS      = 0x0B, //Range Out of Bounds. The start of range provided is not valid
	BTP_AVRCP_STATUS_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE = 0x0C, //Folder Item is not playable. The UID provided refers to a folder item which cannot be handled by this media player
	BTP_AVRCP_STATUS_CODE_MEDIA_IN_USE             = 0x0D, //Media in Use. The media is not able to be used for this operation at this time
	BTP_AVRCP_STATUS_CODE_NOW_PLAYING_LIST_FULL    = 0x0E, //Now Playing List Full. No more items can be added to the Now Playing List
	BTP_AVRCP_STATUS_CODE_SEARCH_NOT_SUPPORTED     = 0x0F, //Search Not Supported. The Browsed Media Player does not support search
	BTP_AVRCP_STATUS_CODE_SEARCH_IN_PROGRESS       = 0x10, //Search in Progress. A search operation is already in progress
	BTP_AVRCP_STATUS_CODE_INVALID_PLAYER_ID        = 0x11, //Invalid Player Id. The specified Player Id does not refer to a valid player
	BTP_AVRCP_STATUS_CODE_PLAY_NOT_BROWSABLE       = 0x12, //Player Not Browsable. The Player Id supplied refers to a Media Player which does not support browsing.
	BTP_AVRCP_STATUS_CODE_PLAY_NOT_ADDRESSED       = 0x13, //Player Not Addressed. The Player Id supplied refers to a player which is not currently addressed, and the command is not able to be performed if the player is not set as addressed.
	BTP_AVRCP_STATUS_CODE_NO_VALID_SEARCHRESULTS   = 0x14, //No valid Search Results. The Search result list does not contain valid entries, e.g. after being invalidated due to change of browsed player
	BTP_AVRCP_STATUS_CODE_NO_AVAILABLE_PLAYERS     = 0x15, //No available players
	BTP_AVRCP_STATUS_CODE_ADDRESSED_PLAYER_CHANGED = 0x16, //Addressed Player Changed
}BTP_AVRCP_STATUS_CODE_ENUM;

typedef enum{
	BTP_AVRCP_ATTRID_NOT_USED          = 0x00,
	BTP_AVRCP_ATTRID_TITLE             = 0x01, //Text field representing the title, song name or content  description coded per  specified character set.
	BTP_AVRCP_ATTRID_ARTIST_NAME       = 0x02, //Text field representing artist(s), performer(s) or group coded per specified character set.
	BTP_AVRCP_ATTRID_ALBUM_NAME        = 0x03, //Text field representing the title of the recording (source) from which the audio in the file is taken.
	BTP_AVRCP_ATTRID_TRACK_NUMBER      = 0x04, //Numeric ASCII string containing the order number of the audio-file on its original recording.
	BTP_AVRCP_ATTRID_TOTAL_NUMBER_OF_TRACKS  = 0x05, //Numeric ASCII string containing the total number of tracks or elements on the original recording.
	BTP_AVRCP_ATTRID_GENRE             = 0x06, //Text field representing the category of the composition characterized by a particular style.
	BTP_AVRCP_ATTRID_PLAYING_TIME      = 0x07, //Numeric ASCII string containing the length of the audio file in milliseconds. (E.g. 02:30 = 150000)
	BTP_AVRCP_ATTRID_DEFAULT_COVER_ART = 0x08, //BIP Image Handle
}BTP_AVRCP_ATTRID_ENUM;

typedef enum{
	BTP_AVRCP_PLAYER_APP_SET_ATTR_ILLEGAL = 0x00, //
	BTP_AVRCP_PLAYER_APP_SET_ATTR_EQUALIZER_ON_OFF_STATUS = 0x01,
	BTP_AVRCP_PLAYER_APP_SET_ATTR_REPEAT_MODE_STATUS = 0x02,
	BTP_AVRCP_PLAYER_APP_SET_ATTR_SHUFFLE_ON_OFF_STATUS = 0x03,
	BTP_AVRCP_PLAYER_APP_SET_ATTR_SCAN_ON_OFF_STATUS = 0x04,
}BTP_AVRCP_PLAYER_APP_SET_ATTR_ENUM;



typedef void (*BtpAvrcpKeyChangeCallback)(uint16 aclHandle, uint08 keyID, uint08 isPress);
typedef void (*BtpAvrcpVolumeChangeCallback)(uint16 aclHandle, uint08 volume);



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
int btp_avrcp_setDefaultVolume(uint08 volume);
int btp_avrcp_setTrackValue(uint16 aclHandle, uint32 valueH, uint32 valudL);

int btp_avrcp_sendEventNoty(uint16 aclHandle, uint08 eventID, uint08 *pData, uint16 dataLen);
int btp_avrcp_sendRegEventNotyCmd(uint16 aclHandle, uint08 eventID);

/******************************************************************************
 * Function: AVRCP Set music state interface
 * Descript: Defines trigger the avrcp cmd change the music of peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @playsate--The state of music to be set. Refer BTP_AVRCP_PLAY_STATE_ENUM.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_avrcp_setPlayState(uint16 aclHandle, uint08 playState);

/******************************************************************************
 * Function: AVRCP Notify music sate Command interface
 * Descript: Defines trigger the avrcp of CT notify the music state 
 *           changing to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @playState--Refer BTP_AVRCP_PLAY_STATE_ENUM.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_avrcp_notifyPlayState(uint16 aclHandle, uint08 playState);

/******************************************************************************
 * Function: AVRCP Send Key interface
 * Descript: Defines trigger the avrcp key press cmd to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @keyID--The key id which reflect to a special command.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_avrcp_sendKeyPress(uint16 aclHandle, uint08 keyID);

/******************************************************************************
 * Function: AVRCP Send Key Release interface
 * Descript: Defines trigger the avrcp key release cmd to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @keyID--The key id which reflect to a special command.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int btp_avrcp_sendKeyRelease(uint16 aclHandle, uint08 keyID);

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


extern int btp_avrcp_browseConnect(uint16 aclHandle);
extern int btp_avrcp_browseDisconn(uint16 aclHandle);



#endif /* BTP_AVRCP_H */

