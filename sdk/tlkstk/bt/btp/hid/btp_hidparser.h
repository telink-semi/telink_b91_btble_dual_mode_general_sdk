/********************************************************************************************************
 * @file	btp_hidparser.h
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
#ifndef BTP_HID_PARSER_H
#define BTP_HID_PARSER_H

#if 0//(TLKBTP_CFG_HIDH_ENABLE)



typedef enum {
    Main=0,
    Global,
    Local,
    Reserved
} TagType;

typedef enum {
    Input=8,
    Output,
    Coll,
    Feature,
    EndColl
} MainItemTag;

typedef enum {
    UsagePage,
    LogicalMinimum,
    LogicalMaximum,
    PhysicalMinimum,
    PhysicalMaximum,
    UnitExponent,
    Unit,
    ReportSize,
    ReportID,
    ReportCount,
    Push,
    Pop
} GlobalItemTag;

typedef enum {
    Usage,
    UsageMinimum,
    UsageMaximum,
    DesignatorIndex,
    DesignatorMinimum,
    DesignatorMaximum,
    StringIndex,
    StringMinimum,
    StringMaximum,
    Delimiter 
} LocalItemTag;

typedef struct  {
    uint32  item_value;    
    uint16 item_size; 
    uint08  item_type;
    uint08  item_tag;
    uint08  data_size;
} hid_descriptor_item_t;

typedef enum {
    BTP_HID_PARSER_SCAN_FOR_REPORT_ITEM,
    BTP_HID_PARSER_USAGES_AVAILABLE,
    BTP_HID_PARSER_COMPLETE,
} btp_hid_parser_state_t;


typedef struct {

    // Descriptor
    uint08        * descriptor;
    uint16        descriptor_len;

    // Report
    uint16       report_type;
    uint08       * report;
    uint16        report_len;

    // State
    btp_hid_parser_state_t state;

    hid_descriptor_item_t descriptor_item;

    uint16        descriptor_pos;
    uint16        report_pos_in_bit;

    // usage pos and usage_page after last main item, used to find next usage
    uint16        usage_pos;
    uint16        usage_page;

    // usage generator
    uint32        usage_minimum;
    uint32        usage_maximum;
    uint16        available_usages;    
    uint08         required_usages;
    uint08         active_record;
    uint08         have_usage_min;
    uint08         have_usage_max;

    // global
    uint32         global_logical_minimum;
    uint32         global_logical_maximum;
    uint16        global_usage_page; 
    uint08         global_report_size;
    uint08         global_report_count;
    uint08         global_report_id;
} btp_hid_parser_t;

void btp_hid_parser_init(btp_hid_parser_t * parser, uint08 * hid_descriptor, uint16 hid_descriptor_len,
		uint16 hid_report_type, uint08 * hid_report, uint16 hid_report_len);


int  btp_hid_parser_has_more(btp_hid_parser_t * parser);


void btp_hid_parser_get_field(btp_hid_parser_t * parser, uint16 * usage_page, uint16 * usage, uint32 * value);

void btp_hid_parse_descriptor_item(hid_descriptor_item_t * item, const uint08 * hid_descriptor, uint16 hid_descriptor_len);

int btstack_hid_get_report_size_for_id(int report_id, uint16 report_type, uint16 hid_descriptor_len, uint08 * hid_descriptor);

uint16 btp_hid_id_valid(int report_id, uint16 hid_descriptor_len, uint08 * hid_descriptor);

int btp_hid_report_id_declared(uint16 hid_descriptor_len, uint08 * hid_descriptor);




#endif //#if (TLKBTP_CFG_HIDH_ENABLE)

#endif //BTP_HID_PARSER_H
