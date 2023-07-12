/********************************************************************************************************
 * @file	btc_list.h
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
/**
 ****************************************************************************************
 *
 * @file co_list.h
 *
 * @brief Common list structures definitions
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef BTC_LIST_H
#define BTC_LIST_H


#include <stdint.h>         // standard definition
#include <stdbool.h>        // boolean definition


/*
 * DEFINES
 ****************************************************************************************
 */

/// structure of a list element header
/*@TRACE*/
struct co_list_hdr
{
    /// Pointer to next co_list_hdr
    struct co_list_hdr *next;
} __attribute__ ((aligned (4)));
/// structure of a list
struct co_list
{
    /// pointer to first element of the list
    struct co_list_hdr *first;
    /// pointer to the last element
    struct co_list_hdr *last;
} __attribute__ ((aligned (4))) ;




/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize a list to defaults values.
 *
 * @param list           Pointer to the list structure.
 ****************************************************************************************
 */
void co_list_init(struct co_list *list);

/**
 ****************************************************************************************
 * @brief Construct a list of free elements representing a pool
 *
 * @param list           Pointer to the list structure
 * @param pool           Pointer to the pool to be initialized
 * @param elmt_size      Size of one element of the pool
 * @param elmt_cnt       Nb of elements available in the pool
 *
 ****************************************************************************************
 */
void co_list_pool_init(struct co_list *list,
                       void *pool,
                       uint32_t elmt_size,
                       uint32_t elmt_cnt);

/**
 ****************************************************************************************
 * @brief Add an element as last on the list.
 *
 * @param list           Pointer to the list structure
 * @param list_hdr       Pointer to the header to add at the end of the list
 *
 ****************************************************************************************
 */
void co_list_push_back(struct co_list *list, struct co_list_hdr *list_hdr);


/**
 ****************************************************************************************
 * @brief Extract the first element of the list.
 * @param list           Pointer to the list structure
 * @return The pointer to the element extracted, and NULL if the list is empty.
 ****************************************************************************************
 */
struct co_list_hdr *co_list_pop_front(struct co_list *list);

/**
 ****************************************************************************************
 * @brief Extract an element when the previous element is known
 *
 * Note: the element to remove shall follow immediately the reference within the list
 *
 * @param list           Pointer to the list structure
 * @param elt_ref_hdr    Pointer to the referenced element (NULL if element to extract is the first in the list)
 * @param elt_to_rem_hdr Pointer to the element to be extracted
 ****************************************************************************************
 */
void co_list_extract_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_rem_hdr);


/**
 ****************************************************************************************
 * @brief Test if the list is empty.
 * @param list           Pointer to the list structure.
 * @return true if the list is empty, false else otherwise.
 ****************************************************************************************
 */
static __attribute__((__always_inline__)) inline
bool co_list_is_empty(const struct co_list *const list)
{
    bool listempty;
    listempty = (list->first == NULL);
    return (listempty);
}

/**
 ****************************************************************************************
 * @brief Pick the first element from the list without removing it.
 *
 * @param list           Pointer to the list structure.
 *
 * @return First element address. Returns NULL pointer if the list is empty.
 ****************************************************************************************
 */
static __attribute__((__always_inline__)) inline
struct co_list_hdr *co_list_pick(const struct co_list *const list)
{
    return(list->first);
}


/**
 ****************************************************************************************
 * @brief Return following element of a list element.
 *
 * @param list_hdr     Pointer to the list element.
 *
 * @return The pointer to the next element.
 ****************************************************************************************
 */
static __attribute__((__always_inline__)) inline
struct co_list_hdr *co_list_next(const struct co_list_hdr *const list_hdr)
{
    return(list_hdr->next);
}


/// @} CO_LIST
#endif // BTC_LIST_H

