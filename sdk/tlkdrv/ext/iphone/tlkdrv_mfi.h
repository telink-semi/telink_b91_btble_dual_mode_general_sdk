/********************************************************************************************************
 * @file	tlkdrv_mfi.h
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
#ifndef TLKDRV_MFI_H
#define TLKDRV_MFI_H


#define TLKDRV_MFI_DBG_FLAG        ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_MFI_DBG_SIGN        "[CODEC]"

#define TLKDRV_MFI_I2C_ADDR        0x20
#define TLKDRV_MFI_I2C_SCL_PIN     GPIO_PD2
#define TLKDRV_MFI_I2C_SDA_PIN     GPIO_PD3

#define TLKDRV_MFI_CHIP_ID	       0x00000300




enum HAPMFiHWAuthRegister{
	/**
     * Device Version.
     *
     * - Block: 0
     * - Length: 1 byte
     * - Power-Up Value: See HAPMFiHWAuthDeviceVersion
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_DeviceVersion = 0x00,

    /**
     * Authentication Revision / Firmware Version (2.0C).
     *
     *  - Block: 0
     *  - Length: 1 byte
     *  - Power-Up Value: 0x01
     *  - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AuthenticationRevision = 0x01,

    /**
     * Authentication Protocol Major Version.
     *
     * - Block: 0
     * - Length: 1 byte
     * - Power-Up Value: 0x03 / 0x02 (2.0C)
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AuthenticationProtocolMajorVersion = 0x02,

    /**
     * Authentication Protocol Minor Version.
     *
     * - Block: 0
     * - Length: 1 byte
     * - Power-Up Value: 0x00
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AuthenticationProtocolMinorVersion = 0x03,

    /**
     * Device ID.
     *
     * - Block: 0
     * - Length: 4 bytes
     * - Power-Up Value: 0x00000300 / 0x00000200 (2.0C)
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_DeviceID = 0x04,

    /**
     * Error Code.
     *
     * - Block: 0
     * - Length: 1 byte
     * - Power-Up Value: 0x00
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_ErrorCode = 0x05,

    /**
     * Authentication Control and Status.
     *
     * - Block: 1
     * - Length: 1 byte
     * - Power-Up Value: 0x00
     * - Access: Read/Write
     */
    kHAPMFiHWAuthRegister_AuthenticationControlAndStatus = 0x10,

    /**
     * Challenge Response Data Length.
     *
     * - Block: 1
     * - Length: 2 bytes
     * - Power-Up Value: 0 / 128 (2.0C)
     * - Access: Read-only / Read/Write (2.0C)
     */
    kHAPMFiHWAuthRegister_ChallengeResponseDataLength = 0x11,

    /**
     * Challenge Response Data.
     *
     * - Block: 1
     * - Length: 64 bytes / 128 bytes (2.0C)
     * - Power-Up Value: Undefined
     * - Access: Read-only / Read/Write (2.0C)
     */
    kHAPMFiHWAuthRegister_ChallengeResponseData = 0x12,

    /**
     * Challenge Data Length.
     *
     * - Block: 2
     * - Length: 2 bytes
     * - Power-Up Value: 0 / 20 (2.0C)
     * - Access: Read-only / Read/Write (2.0C)
     */
    kHAPMFiHWAuthRegister_ChallengeDataLength = 0x20,

    /**
     * Challenge Data.
     *
     * - Block: 2
     * - Length: 32 bytes / 128 bytes (2.0C)
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     */
    kHAPMFiHWAuthRegister_ChallengeData = 0x21,

    /**
     * Accessory Certificate Data Length.
     *
     * - Block: 3
     * - Length: 2 bytes
     * - Power-Up Value: 607-609 / <= 1280 (2.0C)
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateDataLength = 0x30,

    /**
     * Accessory Certificate Data 1.
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData1 = 0x31,

    /**
     * Accessory Certificate Data 2.
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData2 = 0x32,

    /**
     * Accessory Certificate Data 3.
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData3 = 0x33,

    /**
     * Accessory Certificate Data 4.
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData4 = 0x34,

    /**
     * Accessory Certificate Data 5.
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData5 = 0x35,

    /**
     * Accessory Certificate Data 6 (2.0C).
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData6 = 0x36,

    /**
     * Accessory Certificate Data 7 (2.0C).
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData7 = 0x37,

    /**
     * Accessory Certificate Data 8 (2.0C).
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData8 = 0x38,

    /**
     * Accessory Certificate Data 9 (2.0C).
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData9 = 0x39,

    /**
     * Accessory Certificate Data 10 (2.0C).
     *
     * - Block: 3
     * - Length: 128 bytes
     * - Power-Up Value: Certificate
     * - Access: Read-only
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateData10 = 0x3A,

    /**
     * Self-Test Status / Self-Test Control and Status (2.0C).
     *
     * - Block: 4
     * - Length: 1 byte
     * - Power-Up Value: 0x00
     * - Access: Read-only / Read/Write (2.0C)
     */
    kHAPMFiHWAuthRegister_SelfTestStatus = 0x40,

    /**
     * System Event Counter (SEC) (2.0C).
     *
     * - Block: 4
     * - Length: 1 byte
     * - Power-Up Value: Undefined
     * - Access: Read-only
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_SystemEventCounter = 0x4D,

    /**
     * Device Certificate Serial Number / Accessory Certificate Serial Number (2.0C).
     *
     * - Block: 4
     * - Length: 32 bytes / 31 bytes (2.0C)
     * - Power-Up Value: Certificate / Null-terminated string (2.0C)
     * - Access: Read-only
     */
    kHAPMFiHWAuthRegister_AccessoryCertificateSerialNumber = 0x4E,

    /**
     * Apple Device Certificate Data Length (2.0C).
     *
     * - Block: 5
     * - Length: 2 bytes
     * - Power-Up Value: 0x0000
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateDataLength = 0x50,

    /**
     * Apple Device Certificate Data 1 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData1 = 0x51,

    /**
     * Apple Device Certificate Data 2 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData2 = 0x52,

    /**
     * Apple Device Certificate Data 3 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData3 = 0x53,

    /**
     * Apple Device Certificate Data 4 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData4 = 0x54,

    /**
     * Apple Device Certificate Data 5 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData5 = 0x55,

    /**
     * Apple Device Certificate Data 6 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData6 = 0x56,

    /**
     * Apple Device Certificate Data 7 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData7 = 0x57,

    /**
     * Apple Device Certificate Data 8 (2.0C).
     *
     * - Block: 5
     * - Length: 128 bytes
     * - Power-Up Value: Undefined
     * - Access: Read/Write
     *
     * @remark Obsolete since R30.
     */
    kHAPMFiHWAuthRegister_AppleDeviceCertificateData8 = 0x58,

    /**
     * Sleep.
     *
     * - Block: 1
     * - Length: 1 byte
     * - Power-Up Value: Undefined
     * - Access: Write-only
     */
    kHAPMFiHWAuthRegister_Sleep = 0x60
};



typedef struct{
	uint08 isInit;
	uint08 isOpen;
}tlkdrv_mfi_t;


int tlkdrv_mfi_init(void);

bool tlkdrv_mfi_isOpen(void);

int tlkdrv_mfi_open(void);
int tlkdrv_mfi_close(void);

int tlkdrv_mfi_read(uint08 reg, uint08 *pBuff, uint08 readLen);
int tlkdrv_mfi_write(uint08 reg, uint08 *pData, uint08 dataLen);




#endif //TLKDRV_CODEC_H

