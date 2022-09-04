/********************************************************************************************************
 * @file     btp_a2dp.h
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

#ifndef BTP_A2DP_H
#define BTP_A2DP_H


typedef	void(*BtpA2dpRecvDataCallback)(uint08 *pData, uint16 dataLen);


typedef enum{
	BTP_A2DP_STATUS_CLOSED = 0,
	BTP_A2DP_STATUS_OPENED,
	BTP_A2DP_STATUS_PAUSED,
	BTP_A2DP_STATUS_STREAM,
}BTP_A2DP_STATUS_ENUM;

typedef enum{
	BTP_A2DP_CODEC_SBC    = 0x00,
	BTP_A2DP_CODEC_MPEG12 = 0x01,
	BTP_A2DP_CODEC_MPEG24 = 0x02,
	BTP_A2DP_CODEC_ATRAC  = 0x04,
	BTP_A2DP_CODEC_VENDOR = 0xff,
}BTP_A2DP_CODEC_ENUM;
typedef enum{
	BTP_A2DP_CHN_MODE_MONO         = 0x08,
	BTP_A2DP_CHN_MODE_DUAL         = 0x04,
	BTP_A2DP_CHN_MODE_STEREO       = 0x02,
	BTP_A2DP_CHN_MODE_JOINT_STEREO = 0x01,
}BTP_A2DP_CHN_MODE_ENUM;




#define A2DP_BLOCK_LENGTH_4		(1 << 3)
#define A2DP_BLOCK_LENGTH_8		(1 << 2)
#define A2DP_BLOCK_LENGTH_12		(1 << 1)
#define A2DP_BLOCK_LENGTH_16		1

#define A2DP_SUBBANDS_4			(1 << 1)
#define A2DP_SUBBANDS_8			1

#define A2DP_ALLOCATION_SNR		(1 << 1)
#define A2DP_ALLOCATION_LOUDNESS	1


/******************************************************************************
 * Function: A2DP initial interface
 * Descript: Defines trigger the initial flow of A2DP sink and A2DP source
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value is returned means the initial process fail.
*******************************************************************************/
extern int btp_a2dp_init(void);

/******************************************************************************
 * Function: A2DP connect interface
 * Descript: Defines the format of the user trigger connect interface.
 * Params:
 *     @aclHandle[IN]--The ACL link's handle.
 *     @usrID[IN]--The user id use to verify whether user is client or server
 *                  to trigger the connect process
 * Return: Returning TLK_ENONE(0x00) means the connect process success.
 *         If others value is returned means the connect process fail.
*******************************************************************************/
extern int btp_a2dp_connect(uint16 aclHandle, uint08 usrID);

/******************************************************************************
 * Function: A2DP disconnect interface
 * Descript: Defines the format of the user trigger disconnect interface.
 * Params:
 *     @aclHandle[IN]--The ACL link's handle.
 *     @usrID[IN]--The user id use to verify whether user is client or server
 *                  to trigger the disconnect process
 * Return: Returning TLK_ENONE(0x00) means the disconnect process success.
 *         If others value is returned means the disconnect process fail.
*******************************************************************************/
extern int btp_a2dp_disconn(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP destroy interface
 * Descript: Defines the format of the user trigger destroy interface.
 * Params:
 *     @aclHandle[IN]--The ACL link's handle.
 * Return: null
*******************************************************************************/
extern void btp_a2dp_destroy(uint16 aclHandle);

extern bool btp_a2dp_isSnk(uint16 aclHandle);
extern bool btp_a2dp_isSrc(uint16 aclHandle);

extern bool btp_a2dp_isStart(uint16 aclHandle);

extern uint16 btp_a2dp_getSrcHandle(void);
extern uint16 btp_a2dp_getSnkHandle(void);


/******************************************************************************
 * The a2dp source interface
 ******************************************************************************/

/******************************************************************************
 * Function: A2DP source send media data interface
 * Descript: Defines the format of the A2dp source send media data to sink device.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 *      @packet[IN]--the stream data from audio part.
 *      @size[IN]--the stream data size.
 *      @marker[IN]--the marker.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 * 		If others value is returned means the send process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_sendMediaData(uint16 aclHandle, uint16 seqNumber, uint32 timestamp, uint08 *pPayload, uint16 payloadSize);

/******************************************************************************
 * Function: A2DP source start interface
 * Descript: Defines the format of the A2dp source trigger start stream.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the start process success.
 * 		If others value is returned means the start process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_start(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source suspend interface
 * Descript: Defines the format of the A2dp source trigger suspend stream.
 * Params:
 *   	@aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the suspend process success.
 * 		If others value is returned means the suspend process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_suspend(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source stop interface
 * Descript: Defines the format of the A2dp source trigger stop stream.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the stop process success.
 * 		If others value is returned means the stop process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_close(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source abort interface
 * Descript: Defines the format of the A2dp source trigger abort the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the abort process success.
 * 		If others value is returned means the abort process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_abort(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source reconfig interface
 * Descript: Defines the format of the A2dp source trigger reconfig 
 *           the codec's paramter.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 *      @sampleRate[IN]--16K, 32K, 44.1K, 48K
 * Return: Returning TLK_ENONE(0x00) means the reconfig process success.
 * 		If others value is returned means the reconfig process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_reconfig(uint16 aclHandle, uint32 sampleRate);

/******************************************************************************
 * Function: A2DP source get A2DP connection status
 * Descript: Defines the format of the A2dp source trigger get status 
 *           of the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the get process success.
 * 		If others value is returned means the get process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_getStatus(uint16 aclHandle);
extern int btp_a2dpsnk_getStatus(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source verify a2dp stream status
 * Descript: Defines the format of the A2dp source trigger to get whether 
 *           the a2dp is stream or not.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning true(0x00) means the a2dp in streaming.
 * 		If false is returned means the a2dp in others status.
 *******************************************************************************/
extern bool btp_a2dpsrc_isInStream(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source set a2dp samplerate
 * Descript: Defines the format of the A2dp source trigger to set the samplerate
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * 	    @sampleRate[IN]--The codec's samplerate
 * Return: Returning TLK_ENONE(0x00) means set success.
 * 		If other's value is returned means set fail.
 *******************************************************************************/
extern int btp_a2dpsrc_setSampleRate(uint16 aclHandle, uint32 sampleRate);

/******************************************************************************
 * Function: A2DP sink suspend interface
 * Descript: Defines the format of the A2dp sink trigger suspend the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) or -TLK_EBUSY means the suspend process success.
 * 		If others value is returned means the abort process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_suspend(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP sink start interface
 * Descript: Defines the format of the A2dp source trigger start stream.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the start process success.
 * 		If others value is returned means the start process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_start(uint16 aclHandle);

/******************************************************************************
 * Function: btp_a2dpsnk_getSampleRate
 * Descript: Get Sample Rate for Codec.
 * Params:
 * 	    @pA2dp[IN]--The A2dp control bolck which convey the paramter.
 * Return: SampleRate.
 *******************************************************************************/
extern uint btp_a2dpsrc_getSampleRate(uint16 aclHandle);
#if TLK_CFG_PTS_ENABLE
extern uint btp_a2dpsrc_getbitpool(uint16 aclHandle);
extern uint btp_a2dpsrc_getblock(uint16 aclHandle);
extern int  btp_a2dpsrc_sendDiscoveryCmd(uint16 aclHandle);
#endif

/********************************************************************************************
 * The a2dp Sink Interface 
 *******************************************************************************************/

/******************************************************************************
 * Function: btp_a2dpsnk_getSampleRate
 * Descript: Get Sample Rate for Codec.
 * Params:
 * 	    @pA2dp[IN]--The A2dp control bolck which convey the paramter.
 * Return: SampleRate.
 *******************************************************************************/
extern uint btp_a2dpsnk_getSampleRate(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP sink send reconfig service's paramters interface
 * Descript: Defines the format of the A2dp sink trigger to reconfig 
 *           a2dp codec frequency interface.
 * Params:
 * 	@aclHandle[IN]--The ACL link's handle.
 *  @type[IN]--The codec's frequency's type;
 *             0 is 16K, 1 is 32K, 2 is 44.1K, default is 48K
 * Return: Returning TLK_ENONE(0x00) means the reconfig process success.
 * 		   If others value is returned means the reconfig process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_reconfigCodec(uint16 aclHandle, uint08 type);

/******************************************************************************
 * Function: A2DP sink data callback
 * Descript: Defines the format of the A2dp sink application register data 
 *           channel's callback to upper layer.
 * Params:
 * 	    @datCallback[IN]--The a2dp sink data callback function.
 * Return:null.
 * 
 *******************************************************************************/
extern void btp_a2dpsnk_regRecvDataCB(BtpA2dpRecvDataCallback datCallback);

/******************************************************************************
 * Function: A2DP sink send delay report
 * Descript: Defines the delay report function for audio control 
 *           the A2DP stream delay to peer device, becasue there exists 
 *           some time delay including decoding, buffer or else some other reason
 * Params:
 * 	    @aclHandle[IN]--The a2dp sink handle.
 *      @delay[IN]--The delay time.
 * Return:null.
 *******************************************************************************/
extern int btp_a2dpsnk_delayreport(uint16 aclHandle, uint16 delay);


#endif /* BTP_A2DP_H */

