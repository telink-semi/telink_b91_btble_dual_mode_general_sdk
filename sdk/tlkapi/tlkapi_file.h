/********************************************************************************************************
 * @file	tlkapi_file.h
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
#ifndef TLKAPI_FILE_H
#define TLKAPI_FILE_H


#if (TLK_CFG_FS_ENABLE)



//File Mode
#define	TLKAPI_FM_READ                  0x01
#define	TLKAPI_FM_WRITE                 0x02
#define	TLKAPI_FM_OPEN_EXISTING         0x00
#define	TLKAPI_FM_CREATE_NEW            0x04
#define	TLKAPI_FM_CREATE_ALWAYS         0x08
#define	TLKAPI_FM_OPEN_ALWAYS           0x10
#define	TLKAPI_FM_OPEN_APPEND           0x30


#if (TLK_FS_FAT_ENABLE)
	#define FCHAR      uint16
#else
	#define FCHAR      char
	#define FIL        void
#endif



/******************************************************************************
 * Function: tlkapi_file_size
 * Descript: Get the File size.
 * Params: None.
 * Return: Return the size of file.
 * Others: None.
*******************************************************************************/
int tlkapi_file_size(FIL *pFile);
/******************************************************************************
 * Function: tlkapi_file_strlen
 * Descript: Get the string length.
 * Params: None.
 * Return: Return the length of String.
 * Others: None.
*******************************************************************************/
int tlkapi_file_strlen(FCHAR *pPath);

/******************************************************************************
 * Function: tlkapi_file_open
 * Descript: open the File.
 * Params: 
 *      @path[IN]--The file path.
 *      @mode[IN]--The file mode.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_open(FIL *pFile, const FCHAR *pPath, uint08 mode);				/* Open or create a file */

/******************************************************************************
 * Function: tlkapi_file_close
 * Descript: close the File.
 * Params: 
 *      @pFile[IN]--The file.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_close(FIL *pFile);											/* Close an open file object */

/******************************************************************************
 * Function: tlkapi_file_seek
 * Descript: locate the position of File.
 * Params: 
 *      @pFile[IN]--The file.
 *      @ofs[IN]--The offset need to seek.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_seek(FIL *pFile, uint32 ofs);								/* Move file pointer of the file object */

/******************************************************************************
 * Function: tlkapi_file_read
 * Descript: read data from FIle.
 * Params: 
 *      @pFile[IN]--The file.
 *      @buff[IN]--The buff stored the read data.
 *      @btr[IN]--Num of bytes to read.
 *      @bt[IN]--Num of bytes actually to read.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_read(FIL *pFile, void* buff, uint32 btr, uint32* br);			/* Read data from the file */

/******************************************************************************
 * Function: tlkapi_file_write
 * Descript: write data to FIle.
 * Params: 
 *      @pFile[IN]--The file.
 *      @buff[IN]--The buff stored the write data.
 *      @btr[IN]--Num of bytes to write.
 *      @bt[IN]--Num of bytes actually to write.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_write(FIL *pFile, const void* buff, uint32 btw, uint32* bw);	/* Write data to the file */

/******************************************************************************
 * Function: tlkapi_file_opendir
 * Descript: Open the dir.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @path[IN]--The path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_opendir(DIR *pDir, const FCHAR* path);						/* Open a directory */

/******************************************************************************
 * Function: tlkapi_file_closedir
 * Descript: Close the dir.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @path[IN]--The path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_closedir(DIR *pDir);										/* Close an open directory */

/******************************************************************************
 * Function: tlkapi_file_readdir
 * Descript: read the dir item.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @fno[IN]--The file info.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_readdir(DIR *pDir, FILINFO* fno);							/* Read a directory item */

/******************************************************************************
 * Function: tlkapi_file_findfirst
 * Descript: find the first file in dir.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @fno[IN]--The file info.
 *      @path[IN]--The file path.
 *      @pattern[IN]--The file pattern.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_findfirst(DIR *pDir, FILINFO* fno, const FCHAR* path, const FCHAR* pattern);	/* Find first file */

/******************************************************************************
 * Function: tlkapi_file_findnext
 * Descript: Find the next file.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @fno[IN]--The file info.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_findnext(DIR *pDir, FILINFO* fno);							/* Find next file */

/******************************************************************************
 * Function: tlkapi_file_mkdir
 * Descript: Create a sub directory.
 * Params: 
 *        @path[IN]--The file path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_mkdir(const FCHAR* path);								/* Create a sub directory */

/******************************************************************************
 * Function: tlkapi_file_unlink
 * Descript: Delete an existing file or directory .
 * Params: 
 *      @path[IN]--The file path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_unlink(const FCHAR* path);								/* Delete an existing file or directory */

/******************************************************************************
 * Function: tlkapi_file_rename
 * Descript: Rename/Move a file or directory.
 * Params: 
 *      @path_old[IN]--The old file path.
 *      @path_new[IN]--The new file path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_rename(const FCHAR* path_old, const FCHAR* path_new);	/* Rename/Move a file or directory */

/******************************************************************************
 * Function: tlkapi_file_mount
 * Descript: Mount/Unmount a logical drive.
 * Params: 
 *      @path[IN]--The file path.
 *      @opt[IN]--The cmd of mount/unmount.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_mount(const FCHAR* path, uint08 opt);			/* Mount/Unmount a logical drive */

/******************************************************************************
 * Function: tlkapi_file_mount
 * Descript: Create a FAT volume.
 * Params: 
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_mkfs(const FCHAR* path, const void* opt, void* work, uint32 len);	/* Create a FAT volume */

/******************************************************************************
 * Function: tlkapi_file_fdisk
 * Descript: Divide a physical drive into some partitio.
 * Params: 
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_fdisk(uint08 pdrv, const uint32 ptbl[], void* work);		/* Divide a physical drive into some partitions */



#endif

#endif //TLKAPI_FILE_H

