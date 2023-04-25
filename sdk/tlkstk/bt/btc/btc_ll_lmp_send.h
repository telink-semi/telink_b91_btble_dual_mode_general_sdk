/********************************************************************************************************
 * @file     btc_ll_lmp_send.h
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
#ifndef BTC_LL_LMP_SEND_H
#define BTC_LL_LMP_SEND_H



int btc_lmp_send(uint08 linkId, uint08 *pData, uint08 dataLen);

int lmp_accept_opcode_req(uint8_t link_id, uint8_t opcode, uint8_t errorcode);
int lmp_accept_ext_opcode_req(uint8_t link_id, uint8_t esc, uint8_t code, uint8_t errorcode);
int lmp_send_timing_accuracy_req(uint8_t link_id);
int lmp_send_timing_accuracy_res(uint8_t link_id);
int lmp_send_auto_rate(uint8_t link_id);
int lmp_send_preferred_rate(uint8_t link_id, preferred_rate_t data_rate);

int lmp_send_min_pwr(uint8_t link_id, uint8_t tid);
int lmp_send_max_pwr(uint8_t link_id, uint8_t tid);
int lmp_send_incr_pwr(uint8_t link_id);
int lmp_send_decr_pwr(uint8_t link_id);
int lmp_send_pwr_ctr_req(uint8_t link_id,uint8_t adjust);
int lmp_send_pwr_ctr_res(uint8_t link_id,uint8_t adjust);

int lmp_send_detach_req(uint8_t link_id,uint8_t reason);

int lmp_send_encryption_mode_req(uint8_t link_id, uint8_t tid, uint8_t encrypt_mode);
int lmp_send_encryption_keysize_req(uint8_t link_id, uint8_t tid, unsigned char keysize);

int lmp_send_pause_enc_req(uint8_t link_id, uint8_t tid);
int lmp_send_stop_enc_req(uint8_t link_id,uint8_t tid);
int lmp_send_resume_enc_req(uint8_t link_id,uint8_t tid);
int lmp_send_start_enc_req(uint8_t link_id, uint8_t *rand, uint8_t tid);

int lmp_send_set_afh_req(uint8_t link_id,uint8_t afh_mode,uint32_t afh_instant,struct chnl_map * chn_map);
int lmp_send_ch_classification_req(uint8_t link_id, uint8_t afh_rpt_mode, uint16_t afh_min_interval, uint16_t afh_max_interval);
int lmp_send_ch_classification(uint8_t link_id,struct chnl_map ch_class);
int btc_authen_sendLmpRandNumber(int linkId, uint8_t * rand_number);
int btc_authen_sendLmpSres(uint8_t linkId, uint8_t* sres);

int lmp_send_max_slot(int Lid, unsigned char max_slot);
int lmp_send_cmd_max_slot_req(int Lid, unsigned char max_slot);
int lmp_send_cmd_ptt_req(int Lid, unsigned char ptt);


int lmp_send_setup_complete(int Lid,uint8_t tid);
int lmp_send_host_con_req(int Lid);

int lmp_send_in_rand_number(uint08 linkId, uint08 number[16]);
int lmp_send_comb_key(uint08 linkId, uint08 combkey[16]);
int lmp_send_io_cap_ext_req(uint08 linkId,uint08 io_cap,uint08 oob_data_present,uint08 authen_req);
int lmp_send_io_cap_ext_res(uint08 linkId);
int lmp_send_encaps_hdr(uint08 linkId,uint08 major,uint08 minor,uint08 enc_payload_len);
int lmp_send_encaps_payload(uint08 linkId, uint08 key[16]);
int lmp_send_sp_number(uint08 linkId, uint08 *number);
int lmp_send_sp_number_cfm(uint08 linkId, uint08 *cfm);
int lmp_send_dhkey_check(uint08 linkId,uint08 *check);
int lmp_send_numeric_comparsion_failed(uint08 linkId);
int lmp_send_passkey_entry_failed(uint08 linkId);
int lmp_send_oob_failed(uint08 linkId);


int lmp_send_cmd_qos_req(uint8_t link_id, unsigned short poll_interval, unsigned char nbc);
int lmp_send_qos(uint8_t link_id);
int lmp_send_qos_req(uint8_t link_id);

int lmp_send_slot_offset(uint8_t link_id, uint16_t slot_offset, uint8_t tid);
int lmp_send_switch_req(uint8_t link_id, uint32_t instant, uint8_t tid);

int lmp_send_lsto(uint8_t link_id,uint16_t timeout);

int lmp_send_version_req(uint8_t link_id, uint8_t version, uint16_t comp_id, uint16_t sub_version);
int lmp_send_version_res(uint8_t link_id, uint8_t version, uint16_t comp_id, uint16_t sub_version);
int lmp_send_name_req(uint8_t link_id, uint8_t name_offset);
int lmp_send_name_res(uint8_t link_id, uint8_t name_offset);

int lmp_send_clock_off_req(uint8_t link_id);
int lmp_send_clk_off_res(uint8_t link_id, uint16_t clock_off);

void lmp_send_sniff_req(uint8_t link_id, uint8_t timing_ctrl_flags, uint16_t Dsniff, uint16_t Tsniff, uint16_t sniff_attempt, uint16_t sniff_timeout, uint08 initiator);
void lmp_send_unsniff_req(uint8_t link_id);



#endif /* BTC_LL_LMP_SEND_H */

