/************************************************************************************//**
* \file         tbx_list.h
* \brief        Linked lists header file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2020 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/
#ifndef TBX_LIST_H
#define TBX_LIST_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of a linked list node, which forms the building block of a linked list
 *         internally. Note that its elements should be considered private and only be
 *         accessed internally by this linked list module.
 */
typedef struct t_tbx_list_node
{
  /** \brief Pointer to the actual item stored in this node. */
  void                   * itemPtr;
  /** \brief Pointer to the previous node in the list or NULL if it is the list start. */
  struct t_tbx_list_node * prevNodePtr;
  /** \brief Pointer to the next node in the list or NULL if it is the list end. */
  struct t_tbx_list_node * nextNodePtr;
} tTbxListNode;

/** \brief Layout of a linked list. Its pointer serves as the handle to the linked list
 *         which is obtained after creation of the list and which is needed in the other
 *         functions of this module. Note that its elements should be considered private
 *         and only be accessed internally by this linked list module.
 */
typedef struct
{
  /** \brief Total number of nodes that are currently present in the linked list. */
  size_t         nodeCount;
  /** \brief Pointer to the first node of the linked list, also known as the head. */
  tTbxListNode * firstNodePtr;
  /** \brief Pointer to the last node of the linked list, also known as the tail. */
  tTbxListNode * lastNodePtr;
} tTbxList;

/** \brief Callback function to compare items. It is called during list sorting. The
 *         return value of the callback function has the following meaning: TBX_TRUE if
 *         item1's data is greater than item2's data, TBX_FALSE otherwise.
 */
typedef uint8_t (* tTbxListCompareItems)(void const * item1, 
                                         void const * item2);


/****************************************************************************************
* Function prototypes
****************************************************************************************/
tTbxList * TbxListCreate          (void);

void       TbxListDelete          (tTbxList                   * list);

void       TbxListClear           (tTbxList                   * list);

size_t     TbxListGetSize         (tTbxList             const * list);

uint8_t    TbxListInsertItemFront(tTbxList                    * list,
                                  void                        * item);

uint8_t    TbxListInsertItemBack  (tTbxList                   * list,
                                   void                       * item);

uint8_t    TbxListInsertItemBefore(tTbxList                   * list,
                                   void                       * item,
                                   void                 const * itemRef);

uint8_t    TbxListInsertItemAfter (tTbxList                   * list,
                                   void                       * item,
                                   void                 const * itemRef);

void       TbxListRemoveItem      (tTbxList                   * list,
                                   void                 const * item);

void     * TbxListGetFirstItem    (tTbxList             const * list);

void     * TbxListGetLastItem     (tTbxList             const * list);

void     * TbxListGetPreviousItem (tTbxList             const * list,
                                   void                 const * itemRef);

void     * TbxListGetNextItem     (tTbxList             const * list, 
                                   void                 const * itemRef);

void       TbxListSwapItems       (tTbxList             const * list,
                                   void                       * item1,
                                   void                       * item2);

void       TbxListSortItems       (tTbxList             const * list, 
                                   tTbxListCompareItems         compareItemsFcn);


#ifdef __cplusplus
}
#endif

#endif /* TBX_LIST_H */
/*********************************** end of tbx_list.h *********************************/
