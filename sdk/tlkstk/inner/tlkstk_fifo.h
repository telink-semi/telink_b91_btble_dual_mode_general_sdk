/********************************************************************************************************
 * @file     fifo.h
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

#ifndef FIFO_H_
#define FIFO_H_

#include "types.h"
#include "tlkdrv/B91/compiler.h"

typedef	struct {
	u32		size;
	u16		num;
	u8		wptr;
	u8		rptr;
	u8*		p;
}	__attribute__ ((aligned (4))) my_fifo_t;

void my_fifo_init (my_fifo_t *f, int s, u8 n, u8 *p);

u8* my_fifo_wptr (my_fifo_t *f);

void my_fifo_next (my_fifo_t *f);

int my_fifo_push (my_fifo_t *f, u8 *p, int n);


static inline void my_fifo_pop (my_fifo_t *f)
{
	if (f->rptr != f->wptr)
	{
		f->rptr++;
	}
}

static inline void my_fifo_reset (my_fifo_t *f)
{
	f->rptr = f->wptr;
}

static inline u8 * my_fifo_get (my_fifo_t *f)
{
	if (f->rptr != f->wptr)
	{
		u8 *p = f->p + (f->rptr & (f->num-1)) * f->size;
		return p;
	}
	return 0;
}

#define		MYFIFO_INIT(name,size,n)		/*__attribute__ ((aligned (4)))*/ u8 name##_b[size * n]__attribute__((aligned(4)))/*={0}*/;my_fifo_t name = {size,n,0,0, name##_b}

#define		MYFIFO_INIT_IRAM(name,size,n)		/*__attribute__ ((aligned (4)))*/ _attribute_iram_data_ u8 name##_b[size * n]__attribute__((aligned(4)))/*={0}*/;\
							_attribute_data_retention_sec_ my_fifo_t name = {size,n,0,0, name##_b}

typedef	struct {
	u16		fifo_size;
	u8		fifo_num;
	u8		conn_num;
	u8		*p;
}	multi_conn_fifo_t;

#define		MULTI_CONN_FIFO_INIT(name,fifo_size,fifo_num,conn_num)		u8 name##_b[fifo_size*fifo_num*conn_num]={0}; multi_conn_fifo_t name = {fifo_size,fifo_num,conn_num, name##_b}


#endif /* FIFO_H_ */
