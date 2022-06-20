/********************************************************************************************************
 * @file     udisk.c
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

#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdev/tlkdev_stdio.h"

#include "tlkstk/inner/tlkstk_inner.h"
#include "fat.h"
#include <string.h>




#if (TLK_DEV_XT2602E_ENABLE)

extern int tlkdev_xt26g02e_pageReadWrite(unsigned int row_address_old, unsigned int row_address, unsigned short col_address, unsigned char * buffer, unsigned short data_len);

//*****************************************************************
// file system
//******************************************************************

/**********************************************************************************************************************
*										Macro define													*
*********************************************************************************************************************/

#define	USB_EDP_UDISK_IN				1
#define	USB_EDP_UDISK_OUT				6

#define	FILE_SYSTEM_MAX_SIZE	(0x0f000000)


/**********************************************************************************************************************
*										Global variable													*
*********************************************************************************************************************/
volatile unsigned int	g_nand_flash_old_adr_begin;
volatile unsigned int	g_nand_flash_old_adr_end;
volatile unsigned int	g_nand_flash_save128k_adr_begin;
volatile unsigned int	g_nand_flash_wptr;	// wprt in the g_nand_buffer

__attribute__((aligned(4))) unsigned char g_nand_buffer0[2048+4];
__attribute__((aligned(4))) unsigned char g_nand_buffer1[2048+4];
//udisk fifo struct for nand flash
struct {
	unsigned char * g_udisk_fifo[2];//A total of two fifo
	unsigned short g_udisk_fifo_num[2];//data bytes number of fifo
	unsigned short g_udisk_fifo_status[2];//0 means ok for in push,1 means ok for pop.
	unsigned short g_udisk_fifo_in_select;//0 means select fifo0,1 select fifo1,2 select dbr
	unsigned short g_udisk_fifo_out_select;//0 means select fifo0,1 select fifo1,2 select dbr
	unsigned char *g_dbr;//dbr
	unsigned short g_dbr_num;
	unsigned short g_udisk_dbr_status;
} g_udisk_fifo_t;


volatile unsigned char disk_cmd_buf[64];

volatile unsigned char g_write_flag = 0;
volatile unsigned char g_edp6_irq_flag = 0;
volatile unsigned char g_edp1_irq_flag = 0;
volatile unsigned int g_cbw_flag=0;
volatile unsigned char g_last_packet = 0;
volatile unsigned char g_debug_ignor = 1;

volatile unsigned int g_flash_ptr_r;
volatile unsigned int g_flash_adr_r;
volatile unsigned int g_flash_ptr_w;
volatile unsigned int g_flash_adr_w;
volatile unsigned int g_flash_len_w;
volatile unsigned char g_flash_w_reserved_flag = 0;

volatile unsigned char g_mass_storage_busy = 0;

unsigned char CSW[16];

const unsigned char DiskInf[36]=
{
	0x00, //Disk device
	0x00, //The most significant D7 is RMB.  RMB=0, which means the device cannot be removed.  If RMB=1, it is a removable device.
	0x00, //Various version numbers 0
	0x01, //Data response format
	0x1F, //Additional data length, 31 bytes
	0x00, //reserved
	0x00, //reserved
	0x00, //reserved
	'T','E','L','I','N','K','-','-', //Vendor identification, as the string "TELINK--"
	//Product identification, which is the string "TDiskdemo"
	'T','D','i','s','k','d','e','m','o','n',0,0,0,0x00,0x00,0x00,
	0x31,0x2E,0x30,0x31 //Product version number, 1.01
};

const unsigned char MaximumCapacity[12]=
{
	0x00, 0x00, 0x00, //reserved
	0x08,	//Capacity list length
	0x00, 0x07, 0x80, 0x00,	//Number of blocks (maximum support 8GB)
	0x03, //The descriptor code is 3, indicating the maximum formatted capacity supported
	0x00, 0x02, 0x00 //Each block size is 512 bytes
};

const unsigned char DiskCapacity[8]=
{
	0x00,0x07,0x80,0x00, //Maximum logical block address that can be accessed
	0x00,0x00,0x02,0x00	//Block length
	//So the capacity of the disk is
	//(0x3FFFF+1)*0x200 = 0x8000000 = 128*1024*1024 = 128MB.
};

const unsigned char SenseData[18]=
{
	0x70, //Error code, fixed as 0x70
	0x00, //reserved
	0x05, //Sense Key 0x05,Indicates an invalid command opcode(ILLEGAL REQUEST)
	0x00, 0x00, 0x00, 0x00, //Information as 0
	0x0A, //The length of additional data is 10 bytes
	0x00, 0x00, 0x00, 0x00, //reserved
	0x20, //Additional Sense Code(ASC) 0x20,Indicates an invalid command opcode(INVALID COMMAND OPERATION CODE)
	0x00, //Additional Sense Code Qualifier(ASCQ)Îª0
	0x00, 0x00, 0x00, 0x00 //reserved
};
void usb_mass_storage_init(void)
{
	// initial the usb end point
	reg_usb_ep3_buf_addr = 128;		//
	reg_usb_ep5_buf_addr = 192;		// 192 max
	reg_usb_ep1_buf_addr = 0;		// 32
	reg_usb_ep6_buf_addr = 64;
	reg_usb_ep_max_size = (192 >> 3);
	reg_usb_ep_irq_mask = BIT(6)| BIT(1);			//audio in/out interrupt enable
	reg_usb_iso_mode = 0;						// disable ed6,ed7,iso mode

	usbhw_reset_ep_ptr(6);
	reg_usb_ep8_fifo_mode = 0;	// no fifo mode
	reg_usb_ep_ctrl(6) = BIT(0);

	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC | FLD_CTRL_EP_AUTO_INTF);
	reg_usb_edp_en = 0xff;//todo by zhangchong
}

/**********************************************************************************************************************
*										Udisk fifo Function													*
*********************************************************************************************************************/
typedef enum{
	UDISK_FIFO_READ_RESET = 0,
	UDISK_FIFO_WRITE_RESET = 3,
}udisk_fifo_reset_type_e;
/**
 * @brief      This function serves reset udisk fifo.
 * @param[in]  udisk_fifo_reset_type - 0 reset for read function, 3 reset for write function
 * @return     none.
 */
void udisk_fifo_reset(udisk_fifo_reset_type_e udisk_fifo_reset_type)
{
	g_udisk_fifo_t.g_udisk_fifo_num[0] \
	= g_udisk_fifo_t.g_udisk_fifo_num[1] \
	= g_udisk_fifo_t.g_udisk_fifo_status[0] \
	= g_udisk_fifo_t.g_udisk_fifo_status[1] \
	= g_udisk_fifo_t.g_udisk_fifo_in_select \
	= g_udisk_fifo_t.g_udisk_fifo_out_select \
	= 0;
	g_udisk_fifo_t.g_udisk_fifo[0] = (g_nand_buffer0 + udisk_fifo_reset_type);
	g_udisk_fifo_t.g_udisk_fifo[1] = (g_nand_buffer1 + udisk_fifo_reset_type);
	g_udisk_fifo_t.g_dbr = (unsigned char *)dbr;
	g_udisk_fifo_t.g_dbr_num = 0;
}


/**
 * @brief      This function serves push data to udisk fifo from endpoint 6.
 * @param[in]  push_num - the data number
 * @return     none.
 */
_attribute_ram_code_sec_ void udisk_fifo_push(unsigned short push_num)
{
	usbhw_reset_ep_ptr(USB_EDP_UDISK_OUT);
	if((g_udisk_fifo_t.g_udisk_fifo_in_select==0)&&(g_udisk_fifo_t.g_udisk_fifo_status[0]==0))//if fifo 0 
	{
		for(unsigned char i=0;i<push_num;i++)
			g_udisk_fifo_t.g_udisk_fifo[0][(g_udisk_fifo_t.g_udisk_fifo_num[0]++)] = reg_usb_ep_dat(USB_EDP_UDISK_OUT);

		if (g_udisk_fifo_t.g_udisk_fifo_num[0]==2048)
		{
			g_udisk_fifo_t.g_udisk_fifo_in_select=1;
			g_udisk_fifo_t.g_udisk_fifo_status[0]=1;
		}
	}
	else if((g_udisk_fifo_t.g_udisk_fifo_in_select==1)&&(g_udisk_fifo_t.g_udisk_fifo_status[1]==0))//if fifo 1
	{
		for(unsigned char i=0;i<push_num;i++)
			g_udisk_fifo_t.g_udisk_fifo[1][(g_udisk_fifo_t.g_udisk_fifo_num[1]++)] = reg_usb_ep_dat(USB_EDP_UDISK_OUT);

		if (g_udisk_fifo_t.g_udisk_fifo_num[1]==2048)
		{
			g_udisk_fifo_t.g_udisk_fifo_in_select=0;
			g_udisk_fifo_t.g_udisk_fifo_status[1]=1;
		}
	}
	usbhw_data_ep_ack(USB_EDP_UDISK_OUT);
}

/**
 * @brief      This function serves pop data to endpoint 1 from udisk fifo.
 * @param[in]  pop_num - the data number
 * @return     none.
 */
_attribute_ram_code_sec_ void udisk_fifo_pop(unsigned short pop_num)
{
	usbhw_reset_ep_ptr(USB_EDP_UDISK_IN);
	if(g_udisk_fifo_t.g_udisk_fifo_out_select==2)//if read dbr
	{
		for(unsigned char i=0;i<pop_num;i++)
			reg_usb_ep_dat(USB_EDP_UDISK_IN) = g_udisk_fifo_t.g_dbr[(g_udisk_fifo_t.g_dbr_num++)];

		if (g_udisk_fifo_t.g_dbr_num==512)
			g_udisk_fifo_t.g_udisk_fifo_out_select=0;
	}
	else if((g_udisk_fifo_t.g_udisk_fifo_out_select==0)&&(g_udisk_fifo_t.g_udisk_fifo_status[0]==1))//if fifo 0
	{
		for(unsigned char i=0;i<pop_num;i++)
			reg_usb_ep_dat(USB_EDP_UDISK_IN) = g_udisk_fifo_t.g_udisk_fifo[0][(g_udisk_fifo_t.g_udisk_fifo_num[0]++)];

		if (g_udisk_fifo_t.g_udisk_fifo_num[0]==2048)
		{
			g_udisk_fifo_t.g_udisk_fifo_out_select=1;
			g_udisk_fifo_t.g_udisk_fifo_status[0]=0;
		}
	}
	else if((g_udisk_fifo_t.g_udisk_fifo_out_select==1)&&(g_udisk_fifo_t.g_udisk_fifo_status[1]==1))//if fifo 1
	{
		for(unsigned char i=0;i<pop_num;i++)
			reg_usb_ep_dat(USB_EDP_UDISK_IN) = g_udisk_fifo_t.g_udisk_fifo[1][(g_udisk_fifo_t.g_udisk_fifo_num[1]++)];

		if (g_udisk_fifo_t.g_udisk_fifo_num[1]==2048)
		{
			g_udisk_fifo_t.g_udisk_fifo_out_select=0;
			g_udisk_fifo_t.g_udisk_fifo_status[1]=0;
		}
	}
	usbhw_data_ep_ack(USB_EDP_UDISK_IN);
}

/**
 * @brief      This function serves start write operation for FAT16 file system.
 * @param[in]  * adr -  the address need to be write.
 * @param[in]  len - the data length need to be write.
 * @return     none.
 */
void nand_flash_fat_write_start(unsigned long adr, int len) // Here we need to save the original data, ADR is the starting position of the data, len is the length of the data that needs to be saved from the block, which should be taken as an integer multiple of 2K
{
	unsigned int  ra = adr>>11;
	unsigned int  ca = adr & 0x7ff;

	unsigned int  ra_align = ra & 0xffffffc0;
	unsigned int  i,temp_ra;

	g_nand_flash_old_adr_begin = adr & 0xfffe0000;
	g_nand_flash_old_adr_end = (g_nand_flash_old_adr_begin + len) & 0xfffe0000; //256k;

	g_nand_flash_save128k_adr_begin = 0x0f800000+ ((ra_align&0x40)?0x20000:0);				// temp save address.
	temp_ra = g_nand_flash_save128k_adr_begin>>11;

	tlkdev_xt2602e_erase(temp_ra); // erase temp block.
	for(i=0;i<64;i++)
	{
		tlkdev_xt26g02e_pageReadWrite(ra_align+i,temp_ra+i,0,NULL, 0); // copy page to new address.
	}
	tlkdev_xt2602e_erase(ra_align);	// erase current block.
	for(i=0;i<64;i++)
	{
		if(ra_align+i < ra)
		{
			tlkdev_xt26g02e_pageReadWrite(temp_ra+i,ra_align+i,0,NULL, 0);
		}
		else
		{
			tlkdev_xt2602e_read(g_nand_flash_save128k_adr_begin + (i*0x800) ,g_udisk_fifo_t.g_udisk_fifo[0], 2048);
			g_udisk_fifo_t.g_udisk_fifo_num[0] = ca;

			g_nand_flash_wptr = adr&0xfffff800;
			break;
		}
	}
}

/**
 * @brief      This function serves do write operation for FAT16 file system.
 * @param[in]  * adr -  the address need to be write.
 * @param[in]  len - the data length need to be write.
 * @return     none.
 */
unsigned char nand_flash_fat_write_buffer(unsigned long adr, unsigned char * buff,int len) //Here, new data is written each time
{
	unsigned int  ra = adr>>11;
	unsigned int  adr_2048align = adr & 0xfffff800;
	unsigned int  i,temp_ra;

	if(adr != g_nand_flash_wptr) // The data required to be written is continuous.
	{
		return -1;
	}
	else
	{
		if(adr+len > adr_2048align + 2048) // error, can't cross page, according to fat, USB packet characteristics.
		{
			return -2;
		}
		else
		{
			if(adr+len ==adr_2048align + 2048)
			{
				tlkdev_xt2602e_write(adr_2048align,buff,2048);	// if buffer is full, write to nand flash!
			}
			g_nand_flash_wptr = g_nand_flash_wptr + len;

			if(g_nand_flash_wptr == (g_nand_flash_old_adr_begin + 0x20000)) // if cross block
			{
				g_nand_flash_save128k_adr_begin = 0x0f800000 + ((g_nand_flash_wptr&0x20000)?0x20000:0);

				temp_ra = g_nand_flash_save128k_adr_begin>>11;

				g_nand_flash_old_adr_begin = g_nand_flash_old_adr_begin + 0x20000; // new block begin
				g_nand_flash_old_adr_end = g_nand_flash_old_adr_begin + 0x20000; // new block end
				ra = g_nand_flash_old_adr_begin>>11;
				int ra_align = ra & 0xffffffc0;

				tlkdev_xt2602e_erase(temp_ra); // erase temp block. // save new block
				for(i=0;i<64;i++)
				{
					tlkdev_xt26g02e_pageReadWrite(ra_align+i,temp_ra+i,0,NULL, 0); // copy page to new address.
					
				}
				tlkdev_xt2602e_erase(ra_align);	// erase new blocks
			}
		}
	}
	return 0;
}

/**
 * @brief      This function serves end write operation for FAT16 file system.
 * @return     none.
 */
void nand_flash_fat_write_end(void)	// Here, we need to restore the original data
{
	unsigned int  ca = g_nand_flash_wptr & 0x7ff;
	unsigned int  adr_2048align	= g_nand_flash_wptr & 0xfffff800;
	unsigned int  ra,ra_save;
	unsigned int  last;
	unsigned int  save_adr;
	unsigned int  i;
	if(ca != 0)
	{
		last = 2048 - ca;
		save_adr = g_nand_flash_save128k_adr_begin + (g_nand_flash_wptr - g_nand_flash_old_adr_begin);

		if(g_udisk_fifo_t.g_udisk_fifo_out_select==1)
		{
			tlkdev_xt2602e_read(save_adr, &(g_udisk_fifo_t.g_udisk_fifo[1][ca]), last);
			tlkdev_xt2602e_write(adr_2048align,g_udisk_fifo_t.g_udisk_fifo[1],2048); // fill the buffer and write.
		}
		else if(g_udisk_fifo_t.g_udisk_fifo_out_select==0)
		{
			tlkdev_xt2602e_read(save_adr, &(g_udisk_fifo_t.g_udisk_fifo[0][ca]), last);
			tlkdev_xt2602e_write(adr_2048align,g_udisk_fifo_t.g_udisk_fifo[0],2048); // fill the buffer and write.
		}

		g_nand_flash_wptr = g_nand_flash_wptr + last;
	}
	ra =g_nand_flash_wptr>>11;
	save_adr = g_nand_flash_save128k_adr_begin + (g_nand_flash_wptr - g_nand_flash_old_adr_begin);
	ra_save = save_adr >> 11;

	i = 0;
	while(g_nand_flash_wptr<g_nand_flash_old_adr_end)
	{
		tlkdev_xt26g02e_pageReadWrite(ra_save+i,ra+i,0,NULL, 0); // copy page to old address.
		g_nand_flash_wptr = g_nand_flash_wptr + 2048;			// increase one page length.
		i++;
		if(i>64) // can't bigger than 64
			break;
	}
}
/**
 * @brief      This function serves to remap address get from usb to fat16 in nand flash.
 * @param[in]  adr - the address get from usb
 * @return     the remap address in nand flash.
 */
unsigned int fat_remap(unsigned int adr)
{
	unsigned int  adr1 = (adr - 512);
	unsigned int  adr_y = adr1 & 0x7ff;
	unsigned int  adr_x = adr1 >> 11;
	unsigned int  adr_new = 0x0f000000 + adr_y + (adr_x * 0x20000);
	return adr_new;
}

/**
 * @brief      This function serves to initialization nand spi flash.
 * @param[in]  reset - para = 1, nandflash will be initial.
 * @return     none.
 */
void nand_flash_fat_init(int reset)
{
	unsigned int i;
	// initial fat1

	#if (TLK_DEV_XTSD04G_ENABLE)
	tlkdev_xtsd04g_init();
	#endif
	#if (TLK_DEV_XT2602E_ENABLE)
	tlkdev_xt2602e_init();
	#endif
	
	udisk_fifo_reset(UDISK_FIFO_READ_RESET);
	tlkdev_xt2602e_read(0x0f880000, g_udisk_fifo_t.g_udisk_fifo[0], 16);
	
	if((g_udisk_fifo_t.g_udisk_fifo[0][3] != 'M') || (reset == 1) )
	{
		udisk_fifo_reset(UDISK_FIFO_WRITE_RESET);
		tlkdev_xt2602e_erase(0x0f880000>>11);
		memset(g_udisk_fifo_t.g_udisk_fifo[0],0,2048);
		memcpy(g_udisk_fifo_t.g_udisk_fifo[0],dbr,512);
		tlkdev_xt2602e_write(0x0f880000,g_udisk_fifo_t.g_udisk_fifo[0],512); // dbr

		memset(g_udisk_fifo_t.g_udisk_fifo[0],0,2048);
		for(i=0x0f000000;i< 0x0f400000 ;i = i + 0x20000)
		{
			tlkdev_xt2602e_erase(i>>11);
			tlkdev_xt2602e_write(i,g_udisk_fifo_t.g_udisk_fifo[0],2048);
		}
		tlkdev_xt2602e_erase(0x0f200000>>11);
		tlkdev_xt2602e_erase(0x0f000000>>11);
		memcpy(g_udisk_fifo_t.g_udisk_fifo[0],fat,64); // remap FAT to the last blocks
		tlkdev_xt2602e_write(0x0f000000,g_udisk_fifo_t.g_udisk_fifo[0],2048);	// 0~0x800	reserved page!	dbr1 FAT1
		tlkdev_xt2602e_write(0x0f200000,g_udisk_fifo_t.g_udisk_fifo[0],2048);	// 0~0x800	reserved page!	dbr1 FAT1

		for(i=0;i<0x0f000000;i=i+0x20000)
		{
			tlkdev_xt2602e_erase(i>>11); // erase 1 block = 128 page = 128 * 2k
		}

		memset(g_udisk_fifo_t.g_udisk_fifo[0],0,2048);
		memcpy(g_udisk_fifo_t.g_udisk_fifo[0],root_dir,64);
		tlkdev_xt2602e_write(0x10000,g_udisk_fifo_t.g_udisk_fifo[0],2048);	// 0x10200	root

		memset(g_udisk_fifo_t.g_udisk_fifo[0],0,2048);
		for(i=0x10800;i<0x18000;i=i+0x800)
		{
			tlkdev_xt2602e_write(i,g_udisk_fifo_t.g_udisk_fifo[0],2048);	// 0x10800~0x18000
		}

		memset(g_udisk_fifo_t.g_udisk_fifo[0],0xff,2048);
		memcpy(&g_udisk_fifo_t.g_udisk_fifo[0][0x00],test_file_data,64);	// 0x18200
		tlkdev_xt2602e_write(0x18000,g_udisk_fifo_t.g_udisk_fifo[0],2048);	// 0x18000
	}
}

/**
 * @brief      This function serves to get Udisk data.
 * @param[in]  reset - para = 1, nandflash will be initial.
 * @return     none.
 */
unsigned int get_disk_data(unsigned int byte_addr)
{
	unsigned int adr_new;
	tlkdev_xt2602e_resetUnlock();

	if((byte_addr<512))//return dbr
	{
		g_udisk_fifo_t.g_udisk_fifo_out_select = 2;
		g_udisk_fifo_t.g_dbr_num = (byte_addr&0x000001ff);
		return 512;
	}

	else if(byte_addr>=512 && byte_addr < 0x10200) // FAT page
	{
		adr_new = fat_remap(byte_addr);
	}
	else if(byte_addr>=0x10200)
	{
		adr_new = byte_addr - 512;
	}
	if((g_udisk_fifo_t.g_udisk_fifo_in_select==0)&&(g_udisk_fifo_t.g_udisk_fifo_status[0]==0))
	{
		tlkdev_xt2602e_read(adr_new, &g_udisk_fifo_t.g_udisk_fifo[0][adr_new&0x000007ff], 2048);
		g_udisk_fifo_t.g_udisk_fifo_num[0] = adr_new&0x000007ff;
		g_udisk_fifo_t.g_udisk_fifo_in_select=1;
		g_udisk_fifo_t.g_udisk_fifo_status[0]=1;
		return byte_addr+(2048-(adr_new&0x000007ff));
	}
	else if((g_udisk_fifo_t.g_udisk_fifo_in_select==1)&&(g_udisk_fifo_t.g_udisk_fifo_status[1]==0))
	{
		tlkdev_xt2602e_read(adr_new, &g_udisk_fifo_t.g_udisk_fifo[1][adr_new&0x000007ff], 2048);
		g_udisk_fifo_t.g_udisk_fifo_num[1] = adr_new&0x000007ff;
		g_udisk_fifo_t.g_udisk_fifo_in_select=0;
		g_udisk_fifo_t.g_udisk_fifo_status[1]=1;
		return byte_addr+(2048-(adr_new&0x000007ff));
	}
	return byte_addr;
}

/**
 * @brief      This function serves to set csw for Udisk.
 * @param[in]  residue - The remaining bytes.
 * @param[in]  status - The status of the command execution.
 * @return     none.
 */
void set_csw(int residue, unsigned char status)
{
 //Set the CSW signature, in fact, you don¡¯t need to set it every time,
 //Just start the initialization setting once, and set it here every time
	CSW[0]='U';
	CSW[1]='S';
	CSW[2]='B';
	CSW[3]='S';

 //Copy dCBWTag to dCSWTag of CSW
	CSW[4]=disk_cmd_buf[4];
	CSW[5]=disk_cmd_buf[5];
	CSW[6]=disk_cmd_buf[6];
	CSW[7]=disk_cmd_buf[7];

 //Remaining bytes
	CSW[8]=residue&0xFF;
	CSW[9]=(residue>>8)&0xFF;
	CSW[10]=(residue>>16)&0xFF;
	CSW[11]=(residue>>24)&0xFF;

 //The status of the command execution, 0 means success, 1 means failure.
	CSW[12]=status;
}

/**
 * @brief      This function serves to send string data back to PC by bulk transfer.
 * @param[in]  ptr - the data string.
 * @param[in]  len - the data length
 * @return     none.
 */
_attribute_ram_code_sec_ void	mass_storage_bulk_send(unsigned char * ptr, unsigned int len)
{
	unsigned int  i;
	usbhw_reset_ep_ptr(USB_EDP_UDISK_IN); // send status back to PC
	for(i=0;i<len;i++)
	{
		reg_usb_ep_dat(USB_EDP_UDISK_IN) = ptr[i];
	}
	usbhw_data_ep_ack(USB_EDP_UDISK_IN); // set the ack
}

/**
 * @brief      This is mass storage task.
 * @return     none.
 */
void mass_storage_task(void)
{
	unsigned char cmd;
	unsigned short n;

	if(g_mass_storage_busy == 0)
	{
		if(g_edp6_irq_flag==1) // cmd endpoint
		{
			if(g_write_flag==0) // not write command
			{
				cmd = disk_cmd_buf[15];
				if(cmd==0x12) // the inquire command
				{
					mass_storage_bulk_send((unsigned char *)DiskInf,36);
					g_cbw_flag = 1;
					set_csw(0,0);	//status
				}
				else if(cmd==0x03) // read format capacities
				{
					mass_storage_bulk_send((unsigned char *)SenseData,18);
					g_cbw_flag = 1;
					set_csw(0,0);	//status
				}
				else if(cmd==0x23) // read format capacities
				{
					mass_storage_bulk_send((unsigned char *)MaximumCapacity,12);
					g_cbw_flag = 1;
					set_csw(0,0);	//status
				}
				else if(cmd==0x25) // read capacities
				{
					mass_storage_bulk_send((unsigned char *)DiskCapacity,8);
					g_cbw_flag = 1;
					set_csw(0,0);	//status
				}
				else if(cmd==0x28) // read
				{

					unsigned long temp;
					temp = disk_cmd_buf[17];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[18];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[19];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[20];
					temp = temp << 9;	// temp = temp * 512

					g_flash_ptr_r = temp;

					temp = disk_cmd_buf[11];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[10];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[9];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[8];
					g_last_packet = temp & 0x3f;
					g_cbw_flag = temp>>6;
					g_cbw_flag = g_cbw_flag + 1;

					g_flash_adr_r = temp + g_flash_ptr_r;

					udisk_fifo_reset(UDISK_FIFO_READ_RESET);
					g_flash_ptr_r = get_disk_data(g_flash_ptr_r);
					if(temp<64)
						n = temp;
					else
						n = 64;

					udisk_fifo_pop(n);
					set_csw(0,0);	//status
				} // cmd = 0x28
				else if(cmd==0x00 )
				{
					g_debug_ignor = 1;
					set_csw(0,0);	//status

					mass_storage_bulk_send(CSW,13);
					g_cbw_flag = 0;
				}
				else if(cmd==0x2a ) //write command
				{
					unsigned long temp;
					udisk_fifo_reset(UDISK_FIFO_WRITE_RESET);
					// set write initial address
					temp = disk_cmd_buf[17];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[18];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[19];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[20];
					temp = temp << 9;	// temp = temp * 512
										//g_flash_ptr_r = 0x10000 + temp;
					if((temp>=512) && (temp<FILE_SYSTEM_MAX_SIZE))	//
					{
						g_flash_adr_w = temp;
						if(temp>=512 && temp < 0x10200) // FAT page
						{
							g_flash_adr_w = fat_remap(temp);
							nand_flash_fat_write_start(g_flash_adr_w,0x400); // save data!
						}
						else
						{
							g_flash_adr_w = temp - 512;
							nand_flash_fat_write_start(g_flash_adr_w,0x20000); // save data!
						}
						g_flash_w_reserved_flag = 0;
					}
					else
					{
						g_flash_adr_w = 0x0f880000 + temp;
						g_flash_w_reserved_flag = 1;
					}

					g_flash_ptr_w = g_flash_adr_w&0xfffff800;
					// get the length
					temp = disk_cmd_buf[11];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[10];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[9];
					temp = temp << 8;
					temp = temp | disk_cmd_buf[8];
					g_flash_len_w = temp;

					g_write_flag = 1; // need to in write status.
					set_csw(0,0);	//status
					g_cbw_flag = 0;

				}// cmd = 0x2a
				else if(cmd== 0x1e) //
				{
					set_csw(0,0);	//status
					mass_storage_bulk_send(CSW,13);
					g_cbw_flag = 0;
				}
				else //defaault not know command
				{
					if(disk_cmd_buf[12]&0x80) //input
					{
						unsigned char buf[4];
						buf[0] = 0;
						mass_storage_bulk_send(buf,1);
						g_cbw_flag = 1;
						set_csw(0,1);	//status
					}
					else
					{
						set_csw(0,1);	//status
						mass_storage_bulk_send(CSW,13);
						g_cbw_flag = 0;
					}
				}
				g_edp6_irq_flag=0;
				usbhw_data_ep_ack(USB_EDP_UDISK_OUT); // set the ack
			}
			else // write command
			{
				if(g_flash_w_reserved_flag==0) // if valid address!!
				{
					if((g_flash_ptr_w<g_flash_adr_w + g_flash_len_w)&&(g_flash_ptr_w+2048>g_flash_adr_w + g_flash_len_w))
					{

						if((g_udisk_fifo_t.g_udisk_fifo_out_select==0)&&g_flash_ptr_w+g_udisk_fifo_t.g_udisk_fifo_num[0]>=g_flash_adr_w + g_flash_len_w)
						{
							nand_flash_fat_write_buffer(g_flash_ptr_w,g_udisk_fifo_t.g_udisk_fifo[0],g_udisk_fifo_t.g_udisk_fifo_num[0]);
							g_flash_ptr_w += g_udisk_fifo_t.g_udisk_fifo_num[0];
						}
						if((g_udisk_fifo_t.g_udisk_fifo_out_select==1)&&g_flash_ptr_w+g_udisk_fifo_t.g_udisk_fifo_num[1]>=g_flash_adr_w + g_flash_len_w)
						{
							nand_flash_fat_write_buffer(g_flash_ptr_w,g_udisk_fifo_t.g_udisk_fifo[1],g_udisk_fifo_t.g_udisk_fifo_num[1]);
							g_flash_ptr_w += g_udisk_fifo_t.g_udisk_fifo_num[1];
						}
					}
					else if((g_udisk_fifo_t.g_udisk_fifo_out_select==0)&&(g_udisk_fifo_t.g_udisk_fifo_status[0]==1))
					{
						nand_flash_fat_write_buffer(g_flash_ptr_w,g_udisk_fifo_t.g_udisk_fifo[0],g_udisk_fifo_t.g_udisk_fifo_num[0]);
						g_flash_ptr_w += g_udisk_fifo_t.g_udisk_fifo_num[0];
						g_udisk_fifo_t.g_udisk_fifo_out_select=1;
						g_udisk_fifo_t.g_udisk_fifo_status[0]=g_udisk_fifo_t.g_udisk_fifo_num[0]=0;
					}
					else if((g_udisk_fifo_t.g_udisk_fifo_out_select==1)&&(g_udisk_fifo_t.g_udisk_fifo_status[1]==1))
					{
						nand_flash_fat_write_buffer(g_flash_ptr_w,g_udisk_fifo_t.g_udisk_fifo[1],g_udisk_fifo_t.g_udisk_fifo_num[1]);
						g_flash_ptr_w += g_udisk_fifo_t.g_udisk_fifo_num[1];
						g_udisk_fifo_t.g_udisk_fifo_out_select=0;
						g_udisk_fifo_t.g_udisk_fifo_status[1]=g_udisk_fifo_t.g_udisk_fifo_num[1]=0;
					}
				}
				else//dbr write
				{
					if(g_flash_ptr_w+g_udisk_fifo_t.g_udisk_fifo_num[0]>=g_flash_adr_w + g_flash_len_w)
					{
						g_flash_ptr_w += g_udisk_fifo_t.g_udisk_fifo_num[0];
						g_udisk_fifo_t.g_udisk_fifo_num[0]=0;
					}
				}
				if(g_flash_ptr_w>= (g_flash_adr_w + g_flash_len_w))
				{
					nand_flash_fat_write_end();
					g_mass_storage_busy = 1;
					set_csw(0,0);
					mass_storage_bulk_send(CSW,13);
				}
			}
			usbhw_set_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
		}
	}	// end of if(g_mass_storage_busy==0)
	if(g_edp1_irq_flag == 1)
	{
		if(g_cbw_flag==0)
		{
			if((g_write_flag)&&(g_mass_storage_busy == 1))
			{
				g_flash_w_reserved_flag = 0;
				g_edp6_irq_flag=0;
				g_mass_storage_busy = 0;
				g_write_flag = 0; // write stage complete!!.
			}
		}
		else if(g_cbw_flag > 1)
		{
			if((g_flash_ptr_r < g_flash_adr_r)&&((g_udisk_fifo_t.g_udisk_fifo_status[0]==0)||(g_udisk_fifo_t.g_udisk_fifo_status[1]==0)))
			{
				g_flash_ptr_r = get_disk_data(g_flash_ptr_r);
			}
		}
		g_edp1_irq_flag = 0;
		usbhw_set_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
	}
}

/**
 * @brief      This is mass storage interrupt handler function.
 * @return     none.
 */
_attribute_ram_code_sec_ void mass_storage_irq_handler(void)
{
	unsigned int irq;
	unsigned char i;
	volatile unsigned short disk_cmd_buf_num;
	irq = usbhw_get_eps_irq();	// data irq

	if((irq & FLD_USB_EDP6_IRQ)) // cmd endpoint
	{
		disk_cmd_buf_num = reg_usb_ep_ptr(USB_EDP_UDISK_OUT);
		if(disk_cmd_buf_num > 64) disk_cmd_buf_num = 64;
		if(g_write_flag)//if in write status
		{
			if((g_udisk_fifo_t.g_udisk_fifo_status[0]==0)||(g_udisk_fifo_t.g_udisk_fifo_status[1]==0))//if fifo is free send ack back.
			{
				udisk_fifo_push(disk_cmd_buf_num);
				usbhw_clr_eps_irq(FLD_USB_EDP6_IRQ);
			}
			else
				usbhw_clr_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
		}
		else//if in cmd status
		{
			usbhw_reset_ep_ptr(USB_EDP_UDISK_OUT);
			if(g_edp6_irq_flag==0)
			{
				for(i=0;i<disk_cmd_buf_num;i++)
				{
					disk_cmd_buf[i] = reg_usb_ep_dat(USB_EDP_UDISK_OUT);	// get the data
				}
			}
			usbhw_clr_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
			usbhw_clr_eps_irq(FLD_USB_EDP6_IRQ);
		}
		g_edp6_irq_flag=1;
	}
	if((irq & FLD_USB_EDP1_IRQ)) // end endpoint
	{
		if(g_cbw_flag!=0)
		{
			if((g_cbw_flag==1)||((g_cbw_flag == 2)&&(g_last_packet==0)))
			{
				mass_storage_bulk_send(CSW,13);
				g_cbw_flag = 0;
				usbhw_clr_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
				usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
			}
			else if((g_cbw_flag>2)&&((g_udisk_fifo_t.g_udisk_fifo_out_select==2)||(g_udisk_fifo_t.g_udisk_fifo_status[0]==1)||(g_udisk_fifo_t.g_udisk_fifo_status[1]==1)))
			{
				udisk_fifo_pop(64);
				g_cbw_flag --;
				usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
			}
			else if((g_cbw_flag == 2)&&(g_last_packet!=0)&&((g_udisk_fifo_t.g_udisk_fifo_out_select==2)||(g_udisk_fifo_t.g_udisk_fifo_status[0]==1)||(g_udisk_fifo_t.g_udisk_fifo_status[1]==1)))
			{
				udisk_fifo_pop(g_last_packet);
				g_cbw_flag--;
				usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
			}
			else
				usbhw_clr_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
		}
		else
		{
			usbhw_clr_eps_irq_mask(FLD_USB_EDP6_IRQ|FLD_USB_EDP1_IRQ);
			usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
		}
		g_edp1_irq_flag = 1;
	}
}
#endif
