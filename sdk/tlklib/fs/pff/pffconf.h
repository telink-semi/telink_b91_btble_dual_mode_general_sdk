/********************************************************************************************************
 * @file     pffconf.h
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

/*---------------------------------------------------------------------------/
/  Petit fatFs - Configuration file
/---------------------------------------------------------------------------*/

#ifndef PFCONF_DEF
#define PFCONF_DEF 8088	/* Revision ID */
#if (TLK_DEV_XT2602E_ENABLE)
/*---------------------------------------------------------------------------/
/ Function Configurations (0:Disable, 1:Enable)
/---------------------------------------------------------------------------*/

#define	PF_USE_READ		1	/* pf_read() function */
#define	PF_USE_DIR		1	/* pf_opendir() and pf_readdir() function */
#define	PF_USE_LSEEK	1	/* pf_lseek() function */
#define	PF_USE_WRITE	0	/* pf_write() function */

#define PF_FS_FAT12		1	/* FAT12 */
#define PF_FS_FAT16		1	/* FAT16 */
#define PF_FS_FAT32		0	/* FAT32 */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define PF_USE_LCC		0	/* Allow lower case ASCII and non-ASCII chars */

#define	PF_CODE_PAGE	437
/* The PF_CODE_PAGE specifies the code page to be used on the target system.
/  SBCS code pages with PF_USE_LCC == 1 requiers a 128 byte of case conversion
/  table. It might occupy RAM on some platforms, e.g. avr-gcc.
/  When PF_USE_LCC == 0, PF_CODE_PAGE has no effect.
/
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS)
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
*/


#endif /* PF_CONF */
#endif
