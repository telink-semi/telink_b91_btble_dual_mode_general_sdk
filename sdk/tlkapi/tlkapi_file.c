/********************************************************************************************************
 * @file     tlkapi_file.c
 *
 * @brief    This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlklib/fs/filesystem.h"
#include "tlkapi/tlkapi_file.h"



#if (TLK_CFG_FS_ENABLE)


extern unsigned int plic_enter_critical_sec2(unsigned char preempt_en ,unsigned char threshold);
extern void plic_exit_critical_sec2(unsigned char preempt_en ,unsigned int r);


FATFS gTlkFileFatFs;


/******************************************************************************
 * Function: tlkapi_file_init
 * Descript: Initial the file system.
 * Params: None.
 * Return: Return the TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkapi_file_init(void)
{
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_file_strlen
 * Descript: Get the string length.
 * Params: None.
 * Return: Return the length of String.
 * Others: None.
*******************************************************************************/
int tlkapi_file_strlen(FCHAR *pPath)
{
	uint08 length = 0;
	FCHAR *ptr = pPath;
	if(pPath == NULL) return 0;
	while((*ptr++) != '\0'){
		#if (TLK_FS_FAT_ENABLE)
		length += 2;
		#else
		length += 1;
		#endif
		if(length >= 0xFD) break;
	}	
	return length;
}

/******************************************************************************
 * Function: tlkapi_file_size
 * Descript: Get the File size.
 * Params: None.
 * Return: Return the size of file.
 * Others: None.
*******************************************************************************/
int tlkapi_file_size(FIL *pFile)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_size(pFile);
	#elif (TLK_FS_PFF_ENABLE)
		ret = gTlkFileFatFs.fsize;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_open
 * Descript: open the File.
 * Params: 
 *      @path[IN]--The file path.
 *      @mode[IN]--The file mode.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_open(FIL *pFile, const FCHAR* path, uint08 mode)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_open(pFile, path, mode); //int
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_open(path);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_close
 * Descript: close the File.
 * Params: 
 *      @pFile[IN]--The file.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_close(FIL *pFile)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_close(pFile);
	#elif (TLK_FS_PFF_ENABLE)
		ret = TLK_ENONE;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_seek
 * Descript: locate the position of File.
 * Params: 
 *      @pFile[IN]--The file.
 *      @ofs[IN]--The offset need to seek.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_seek(FIL *pFile, uint32 ofs)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_lseek(pFile, ofs);
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_lseek(ofs);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_read
 * Descript: read data from FIle.
 * Params: 
 *      @pFile[IN]--The file.
 *      @buff[IN]--The buff stored the read data.
 *      @btr[IN]--Num of bytes to read.
 *      @bt[IN]--Num of bytes to read.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_read(FIL *pFile, void* buff, uint32 btr, uint32* br)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_read(pFile, buff, btr, br);
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_read(buff, btr, br);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_write
 * Descript: write data to FIle.
 * Params: 
 *      @pFile[IN]--The file.
 *      @buff[IN]--The buff stored the write data.
 *      @btr[IN]--Num of bytes to write.
 *      @bt[IN]--Num of bytes actully to write.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_write(FIL *pFile, const void* buff, uint32 btw, uint32* bw)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_write(pFile, buff, btw, bw);
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_write(buff, btw, bw);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_opendir
 * Descript: Open the dir.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @path[IN]--The path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_opendir(DIR *pDir, const FCHAR* path)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_opendir(pDir, path);
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_opendir(pDir, path);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_closedir
 * Descript: Close the dir.
 * Params: 
 *      @pDir[IN]--The Dir.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_closedir(DIR *pDir)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_closedir(pDir);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_readdir
 * Descript: read the dir item.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @fno[IN]--The file info.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_readdir(DIR *pDir, FILINFO* fno)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_readdir(pDir, (FILINFO*)fno);
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_readdir(pDir, (FILINFO*)fno);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

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
int tlkapi_file_findfirst(DIR *pDir, FILINFO* fno, const FCHAR* path, const FCHAR* pattern)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_findfirst(pDir, fno, path, pattern);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_findnext
 * Descript: Find the next file.
 * Params: 
 *      @pDir[IN]--The Dir.
 *      @fno[IN]--The file info.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_findnext(DIR *pDir, FILINFO* fno)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_findnext(pDir, fno);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_mkdir
 * Descript: Create a sub directory.
 * Params: 
 *       @path[IN]--The file path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_mkdir(const FCHAR* path)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_mkdir(path);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_unlink
 * Descript: Delete an existing file or directory .
 * Params: 
 *      @path[IN]--The file path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_unlink(const FCHAR* path)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_unlink(path);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_rename
 * Descript: Rename/Move a file or directory.
 * Params: 
 *      @path_old[IN]--The old file path.
 *      @path_new[IN]--The new file path.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_rename(const FCHAR* path_old, const FCHAR* path_new)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_rename(path_old, path_new);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_mount
 * Descript: Mount/Unmount a logical drive.
 * Params: 
 *      @path[IN]--The file path.
 *      @opt[IN]--The cmd of mount/unmount.
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_mount(const FCHAR* path, uint08 opt)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_mount(&gTlkFileFatFs, path, opt);
	#elif (TLK_FS_PFF_ENABLE)
		ret = pf_mount(&gTlkFileFatFs);
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_mkfs
 * Descript: Create a FAT volume.
 * Params: 
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_mkfs(const FCHAR* path, const void* opt, void* work, uint32 len)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_mkfs(path, opt, work, len);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_fdisk
 * Descript: Divide a physical drive into some partitio.
 * Params: 
 * Return: Return the result.
 * Others: None.
*******************************************************************************/
int tlkapi_file_fdisk(uint08 pdrv, const uint32 ptbl[], void* work)
{
	int ret;
	unsigned int r=plic_enter_critical_sec2(1,1);
	#if (TLK_FS_FAT_ENABLE)
		ret = f_fdisk(pdrv, (const LBA_t*)ptbl, work);
	#elif (TLK_FS_PFF_ENABLE)
		ret = 0xFF;
	#endif
	plic_exit_critical_sec2(1,r);
	return ret;
}



#endif

