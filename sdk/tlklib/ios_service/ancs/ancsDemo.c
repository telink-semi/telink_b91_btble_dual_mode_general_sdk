/********************************************************************************************************
 * @file     ancsDemo.c
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     12. 11, 2019
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

#include "ancsDemo.h"
_attribute_data_retention_ u8 appBundIdNum = 0;

/*
 * For ANCS initialization, confirm the size of appBundleId [] by looking 
 * up the APP ID (appId_null) marked in appBundleId.
 */
void ancs_setBundleIdLen(){
	u32 i;
	for(i = 0; i < 128; i++){
		if(memcmp(appBundleId[i], appId_null, 5) == 0){
			break;
		}
	}

	appBundIdNum = i;
}


u8 ancs_findAppBundleId(u8 *notifyId, u16 len){
	if(!len)	return 0;

	for(u32 i = 0; i < appBundIdNum; i++){
		if(memcmp(appBundleId[i], notifyId, len) == 0){
			return i + 1;
		}
	}

	return 0;
}


void ancs_initial(u8 attsIdType){

	ancsInit(attsIdType);
	///must be set the total length of the array appBundleId[] during the ANCS initialization
	ancs_setBundleIdLen();
}

ancs_call_ev_t  call_ev_handler(void)
{
	u8 ev = 0;
	ancs_notifySrc_type_t *ptr = (ancs_notifySrc_type_t *) ancs_getLastNotifyCmdId(0,0);

	if( ptr->CategroyID == CategroyIDIncomingCall)
	{
		tlkapi_trace(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"incoming call");
		ev = CALL_EV_INCOMING_CALL;
	}
	else if( ptr->CategroyID == CategroyIDMissCall )
	{
		tlkapi_trace(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"master hang up the call");
		ev = CALL_EV_MASTER_HANG_UP_CALL;
	}

	return ev;
}

/*
 * It is used for ANCS to set up services and to process notifications 
 * that ANCS received.
 */
void ancs_mainLoopTask(){

	_attribute_data_retention_ static u32 tick_ancsTask = 1;
	extern u16 ancs_getConnInterval();
	u32 connInterval = ancs_getConnInterval() * 1250;

	if((tick_ancsTask & 1) && (!clock_time_exceed(tick_ancsTask, 1 * connInterval))){
		return;
	}else{
		tick_ancsTask = clock_time() | 1;
	}

	ancsEventHandle();///should call first

	if(ancs_getNewsReceivedStatus() == ANCS_HAVE_NEW_NOTICE){
		///notice the user that has a new notice coming, need to get the appId, title, subTitle, message, etc.
		//like by bellowed

		ancs_attId_AppId_Title* appId 			= (ancs_attId_AppId_Title*) ancs_getNewsAppId();
#if 1
		ancs_attId_AppId_Title* title			= (ancs_attId_AppId_Title*) ancs_getNewsTitle();
		ancs_attId_AppId_Title*	subTitle		= (ancs_attId_AppId_Title*) ancs_getNewsSubTitle();
		ancs_attId_Message*	message				= (ancs_attId_Message*)		ancs_getNewsMessage();
		ancs_attId_Message_Size* messageSize 	= (ancs_attId_Message_Size*)ancs_getNewsMessageSize();
		ancs_attId_Date* date					= (ancs_attId_Date*)ancs_getNewsDate();

		ancs_attId_Postive_Action_Label* positiveActionLabel	= (ancs_attId_Postive_Action_Label*) ancs_getNewsPositiveActionLabel();
		ancs_attId_Negative_Action_Label* negativeActionLabel	= (ancs_attId_Negative_Action_Label*)ancs_getNewsNegativeActionLabel();
#endif
		u8 ret = ancs_findAppBundleId(appId->data, appId->len);

		if(ret){
			switch(ret){
				case BUNDLE_ID_NULL:{
					break;
				}
				case BUNDLE_ID_CALL:{
					call_ev_handler();
					break;
				}
				case BUNDLE_ID_QQ:{
					break;
				}
				case BUNDLE_ID_SMS:{
					break;
				}
				case BUNDLE_ID_WECHAT:{
					break;
				}
				case BUNDLE_ID_WHATSAPP:{
					break;
				}

				default:
					break;
			}
		}

		if(ret == BUNDLE_ID_CALL){
			///message length must be set 0
		}
		
		tlkapi_trace(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN, "------------ANCS------------");

		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"APP ID", appId->data, appId->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"title", title->data, title->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"subTitle", subTitle->data, subTitle->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"message", message->data, message->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"messageSize", messageSize->data, messageSize->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"date", date->data, date->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"positiveActionLabel", positiveActionLabel->data, positiveActionLabel->len);
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"negativeActionLabel", negativeActionLabel->data, negativeActionLabel->len);

		ancs_setNewsReceivedStatus(ANCS_SET_NO_NEW_NOTICE);//clear state
	}else{
		if(ancs_isIncomingCallNeedRemoved() == INCOMING_CALL_NEED_REMOVED){
			tlkapi_trace(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"incoming call need removed now");
		}
	}

}





