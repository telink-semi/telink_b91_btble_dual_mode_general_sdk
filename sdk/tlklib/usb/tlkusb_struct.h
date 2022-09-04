/********************************************************************************************************
 * @file     tlkusb_struct.h
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

#ifndef TLKUSB_STRUCT_H
#define TLKUSB_STRUCT_H




typedef struct
{
	uint08 bReqType; /**< Type of the request. */
	uint08 bRequest; /**< Request command code. */
	uint16 wValue; /**< wValue parameter of the request. */
	uint16 wIndex; /**< wIndex parameter of the request. */
	uint16 wLength; /**< Length of the data to transfer in bytes. */
}tlkusb_setup_req_t;


/** \brief Standard USB String Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard descriptors, the length
 *  of the descriptor for placement in the descriptor header must be determined by the \ref USB_STRING_LEN()
 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the supported language IDs for
 *  the device as an array.
 *
 *  This structure uses the relevant standard's given element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_String_t for the version of this type with with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint16 bString[]; /**< String data, as unicode characters (alternatively, string language IDs).
	                     *   If normal ASCII characters are to be used, they must be added as an array
	                     *   of characters rather than a normal C string so that they are widened to
	                     *   Unicode size.
	                     *
	                     *   Under GCC, strings prefixed with the "L" character (before the opening string
	                     *   quotation mark) are considered to be Unicode strings, and may be used instead
	                     *   of an explicit array of ASCII characters.
	                     */
}__attribute__((packed)) tlkusb_stdStringDesc_t;


/** \brief Standard USB Device Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength;
	uint08 bDescriptorType;
	uint16 bcdUSB; /**< BCD of the supported USB specification. */
	uint08 bDeviceClass; /**< USB device class. */
	uint08 bDeviceSubClass; /**< USB device subclass. */
	uint08 bDeviceProtocol; /**< USB device protocol. */
	uint08 bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	uint16 idVendor; /**< Vendor ID for the USB product. */
	uint16 idProduct; /**< Unique product ID for the USB product. */
	uint16 bcdDevice; /**< Product release (version) number. */
	uint08 iManufacturer; /**< String index for the manufacturer's name.*/
	uint08 iProduct; /**< String index for the product name/details.*/
	uint08 iSerialNumber; /**< String index for the product's globally unique hexadecimal serial number, in uppercase Unicode ASCII.*/
	uint08 bNumConfigurations; /**< Total number of configurations supported by the device. */
}__attribute__((packed)) tlkusb_stdDeviceDesc_t;

/** \brief Standard USB Device Qualifier Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Qualifier Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_DeviceQualifier_t for the version of this type with non-standard LUFA specific element names.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint16 bcdUSB; /**< BCD of the supported USB specification. */
	uint08 bDeviceClass; /**< USB device class. */
	uint08 bDeviceSubClass; /**< USB device subclass. */
	uint08 bDeviceProtocol; /**< USB device protocol. */
	uint08 bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	uint08 bNumConfigurations; /**< Total number of configurations supported by the device.*/
	uint08 bReserved; /**< Reserved for future use, must be 0. */
}__attribute__((packed)) tlkusb_stdQualifierDesc_t;

/** \brief Standard USB Configuration Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint16 wTotalLength; /**< Size of the configuration descriptor header, and all sub descriptors inside the configuration.*/
	uint08 bNumInterfaces; /**< Total number of interfaces in the configuration. */
	uint08 bConfigurationValue; /**< Configuration index of the current configuration. */
	uint08 iConfiguration; /**< Index of a string descriptor describing the configuration. */
	uint08 bmAttributes; /**< Configuration attributes, comprised of a mask of \c USB_CONFIG_ATTR_* masks.*/
	uint08 bMaxPower; /**< Maximum power consumption of the device while in the current configuration, calculated by the \ref USB_CONFIG_POWER_MA() macro.*/
}__attribute__((packed)) tlkusb_stdConfigureDesc_t;

/** \brief Standard USB Interface Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref tlkusb_stdInterfaceDesc_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint08 bInterfaceNumber; /**< Index of the interface in the current configuration. */
	uint08 bAlternateSetting; /**< Alternate setting for the interface number.*/
	uint08 bNumEndpoints; /**< Total number of endpoints in the interface. */
	uint08 bInterfaceClass; /**< Interface class ID. */
	uint08 bInterfaceSubClass; /**< Interface subclass ID. */
	uint08 bInterfaceProtocol; /**< Interface protocol ID. */
	uint08 iInterface; /**< Index of the string descriptor describing the interface.*/
}__attribute__((packed)) tlkusb_stdInterfaceDesc_t;

/** \brief Standard USB Endpoint Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  \see \ref tlkusb_stdEndpointDesc_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint08 bEndpointAddress; /**< Logical address of the endpoint within the device for the current configuration, including direction mask.*/
	uint08 bmAttributes; /**< Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*) and attributes (ENDPOINT_ATTR_*) masks. */
	uint16 wMaxPacketSize; /**< Size of the endpoint bank, in bytes. This indicates the maximum packet size that the endpoint can receive at a time.*/
	uint08 bInterval; /**< Polling interval in milliseconds for the endpoint if it is an INTERRUPT or ISOCHRONOUS type.*/
}__attribute__((packed)) tlkusb_stdEndpointDesc_t;

/** \brief Standard USB Interface Association Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
 *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
 *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_Descriptor_Interface_Association_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 bFirstInterface; /**< Index of the first associated interface. */
	uint08 bInterfaceCount; /**< Total number of associated interfaces. */
	uint08 bFunctionClass; /**< Interface class ID. */
	uint08 bFunctionSubClass; /**< Interface subclass ID. */
	uint08 bFunctionProtocol; /**< Interface protocol ID. */
	uint08 iFunction; /**< Index of the string descriptor describing the interface association. */
}__attribute__((packed)) tlkusb_stdAssociateDesc_t;

typedef struct
{
	uint08 Size;
    uint08 Type;
	uint16 HIDSpec;
	uint08 CountryCode;
	uint08 TotalReportDescriptors;
	uint08 HIDReportType;
	uint16 HIDReportLength;
}tlkusb_HidEndpointDesc_t;


typedef struct
{
	uint08 bLength; 
	uint08 bDescriptorType;
	uint08 Subtype;
	uint08 ACSpecification[2];
	uint08 TotalLength[2];
	uint08 InCollection;
	uint08 InterfaceNumber;
}__attribute__((packed)) tlkusb_audInterfaceAcDesc_t;
typedef struct
{
	uint08 bLength; 
	uint08 bDescriptorType;
	uint08 Subtype;
	uint08 ACSpecification[2];
	uint08 TotalLength[2];
	uint08 InCollection;
	uint08 InterfaceNumber_spk;
	uint08 InterfaceNumber_mic;
}__attribute__((packed)) tlkusb_audInterfaceAcTLDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 TerminalLink;
	uint08 Delay;
	uint08 AudioFormat[2];//uint16 FormatTag;
}__attribute__((packed)) tlkusb_audInterfaceAsDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 TerminalID;
	uint16 TerminalType;
	uint08 AssocTerminal;
	uint08 TotalChannels;
	uint16 ChannelConfig;
	uint08 ChannelStrIndex;
	uint08 TerminalStrIndex;
}__attribute__((packed)) tlkusb_audInputDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 TerminalID;
	uint16 TerminalType;
	uint08 AssocTerminal;
	uint08 SourceID;
	uint08 Terminal;
}__attribute__((packed)) tlkusb_audOutputDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 UnitID;
	uint08 SourceID;
	uint08 ControlSize;
	uint08 MAControls[2];
	uint08 FeatureUnitStrIndex;
}__attribute__((packed)) tlkusb_audSingleFeatureDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 UnitID;
	uint08 SourceID;
	uint08 ControlSize;
	uint08 MAControls[3];
	uint08 FeatureUnitStrIndex;
}__attribute__((packed)) tlkusb_audDoubleFeatureDesc_t;

typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 FormatType;
	uint08 NrChannels;
	uint08 SubFrameSize;
	uint08 BitResolution;
	uint08 SampleFrequencyType;
	uint08 tSamFreq[3];
}__attribute__((packed)) tlkusb_audFormatDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 EndpointAddress;
	uint08 MAttributes;
	uint16 MaxPacketSize;
	uint08 Interval;
	uint08 Refresh;
	uint08 SynchAddress;
}__attribute__((packed)) tlkusb_audStdEndpointDesc_t;
typedef struct
{
	uint08 Length;
	uint08 DescriptorType;
	uint08 DescriptorSubtype;
	uint08 MAttributes;
	uint08 LockDelayUnits;
	uint08 LockDelay[2]; //uint16 LockDelay;
}__attribute__((packed)) tlkusb_audSpcEndpointDesc_t;





#endif //TLKUSB_STRUCT_H

