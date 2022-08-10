/********************************************************************************************************
 * @file     tlkmmi_fileDfu.h
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
#ifndef TLKMMI_FILE_DFU_H
#define TLKMMI_FILE_DFU_H

#if (TLKMMI_FILE_DFU_ENABLE)



#define TLKMMI_FILE_DFU_MAX_SIZE                 (0xBE000)



#define TLKMMI_FILE_DFU_SAVE_INNER               1
#define TLKMMI_FILE_DFU_SAVE_OUTER               2
#define TLKMMI_FILE_DFU_SAVE_METHOD              TLKMMI_FILE_DFU_SAVE_INNER
#if (TLKMMI_FILE_DFU_SAVE_METHOD == TLKMMI_FILE_DFU_SAVE_INNER)
#define TLKMMI_FILE_DFU_SAVE_OFFSET              0x100000
#else
#define TLKMMI_FILE_DFU_SAVE_OFFSET              0x1000
#endif

#define TLKMMI_FILE_DFU_VERSION_CHECK_ENABLE    1

#define TLKMMI_FILE_DFU_FORCE_AUTH_ENABLE        0
#define TLKMMI_FILE_DFU_FORCE_CRYP_ENABLE        0


#define TLKMMI_FILE_DFU_SERIAL_UNIT_LENS         64
#define TLKMMI_FILE_DFU_SERIAL_WAIT_INTV         (4096/TLKMMI_FILE_DFU_SERIAL_UNIT_LENS)
#define TLKMMI_FILE_DFU_SERIAL_TRAN_INTV         10 //10*100us=1ms     
#define TLKMMI_FILE_DFU_SERIAL_SAVE_INTV         (TLKMMI_FILE_DFU_SERIAL_WAIT_INTV << 2)

#define TLKMMI_FILE_DFU_BT_SPP_UNIT_LENS         512
#define TLKMMI_FILE_DFU_BT_SPP_WAIT_INTV         (8192/TLKMMI_FILE_DFU_BT_SPP_UNIT_LENS)
#define TLKMMI_FILE_DFU_BT_SPP_TRAN_INTV         10 //10*100us=1ms  
#define TLKMMI_FILE_DFU_BT_SPP_SAVE_INTV         (TLKMMI_FILE_DFU_BT_SPP_WAIT_INTV << 2)

#define TLKMMI_FILE_DFU_BT_ATT_UNIT_LENS         512
#define TLKMMI_FILE_DFU_BT_ATT_WAIT_INTV         (8192/TLKMMI_FILE_DFU_BT_ATT_UNIT_LENS)
#define TLKMMI_FILE_DFU_BT_ATT_TRAN_INTV         10 //10*100us=1ms  
#define TLKMMI_FILE_DFU_BT_ATT_SAVE_INTV         (TLKMMI_FILE_DFU_BT_ATT_WAIT_INTV << 2)

#define TLKMMI_FILE_DFU_LE_ATT_UNIT_LENS         32
#define TLKMMI_FILE_DFU_LE_ATT_WAIT_INTV         (4096/TLKMMI_FILE_DFU_LE_ATT_UNIT_LENS)
#define TLKMMI_FILE_DFU_LE_ATT_TRAN_INTV         10 //10*100us=1ms  
#define TLKMMI_FILE_DFU_LE_ATT_SAVE_INTV         (TLKMMI_FILE_DFU_LE_ATT_WAIT_INTV << 2)



#define TLKMMI_FILE_DFU_START_FLAG               0x4B
#define TLKMMI_FILE_DFU_START_OFFSET             0x20

#define TLKMMI_FILE_DFU_BK_RESUME_ENABLE         1



#if (TLK_CFG_FLASH_CAP < 0x200000 && (TLKMMI_FILE_DFU_SAVE_METHOD == TLKMMI_FILE_DFU_SAVE_INNER))
	#error "The inner flash is too small to store the firmware in it."
#endif


bool tlkmmi_file_dfuIsStart(void);



#endif //TLKMMI_FILE_ENABLE

#endif //TLKMMI_FILE_DFU_H

