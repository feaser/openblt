/************************************************************************************//**
* \file         tbx_list.c
* \brief        Linked lists source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX global header             */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static tTbxListNode * TbxListFindListNode(tTbxList const * list, 
                                          void     const * item);


/************************************************************************************//**
** \brief     Creates a new and empty linked list and returns its pointer. Make sure to
**            store the pointer because it serves as a handle to the linked list, which
**            is needed when calling the other API functions in this module.
** \return    The pointer to the created linked list or NULL in case or error.
**
****************************************************************************************/
tTbxList * TbxListCreate(void)
{
  tTbxList         * result = NULL;
  static uint8_t     memPoolsCreated = TBX_FALSE;
  uint8_t            errorDetected = TBX_FALSE;
  tTbxList         * newListPtr;

  /* Check if this is the first time that this function is called. */
  if (memPoolsCreated == TBX_FALSE)
  {
    /* Invert flag because this part only needs to run one time. */
    memPoolsCreated = TBX_TRUE;
    /* This module allows the dynamic creation and deletion of a linked list and its
     * nodes. For both these times (tTbxList and tTbxListNode) a memory pool needs to be
     * created. An initial size of 1 is sufficient, because the plan is to expand each
     * memory pool whenever more blocks need to be allocated from it.
     */
    if (TbxMemPoolCreate(1, sizeof(tTbxList)) == TBX_ERROR)
    {
      /* Flag the error. */
      errorDetected = TBX_TRUE;
    }
    if (TbxMemPoolCreate(1, sizeof(tTbxListNode)) == TBX_ERROR)
    {
      /* Flag the error. */
      errorDetected = TBX_TRUE;
    }
  }

  /* Only continue if no error was detected so far. */
  if (errorDetected == TBX_FALSE)
  {
    /* Attempt to allocate a block for the new list. */
    newListPtr = TbxMemPoolAllocate(sizeof(tTbxList));
    /* In case the allocation failed, the memory pool could be exhausted. Try to add
     * another block to the memory pool. This works as long as there is enough heap
     * configured.
     */
    if (newListPtr == NULL)
    {
      /* Try to add another block to the memory pool. */
      if (TbxMemPoolCreate(1, sizeof(tTbxList)) == TBX_OK)
      {
        /* Second attempt of the block allocation. */
        newListPtr = TbxMemPoolAllocate(sizeof(tTbxList));
      }
    }

    /* Only continue if the allocation was successful. */
    if (newListPtr != NULL)
    {
      /* By default the created list is empty. */
      newListPtr->firstNodePtr = NULL;
      newListPtr->lastNodePtr = NULL;
      newListPtr->nodeCount = 0U;
      /* The list was successfully created so update the result to give the pointer to
       * the newly created list back to the caller. This pointer serves as the handle to
       * the list and is needed when calling API function of this module.
       */
      result = newListPtr;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListCreate ***/


/************************************************************************************//**
** \brief     Deletes a previously created linked list. Afterwards, the pointer to the
**            linked list is no longer valid and should not be used anymore.
** \param     list Pointer to a previously created linked list to operate on.
**
****************************************************************************************/
void TbxListDelete(tTbxList * list)
{
  /* Verify parameters. */
  TBX_ASSERT(list != NULL);

  /* Only continue if the parameter is valid. */
  if (list != NULL)
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Clear the list. */
    TbxListClear(list);
    /* Release memory of the list. */
    TbxMemPoolRelease(list);
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }
} /*** end of TbxListDelete ***/


/* cppcheck-suppress [misra-c2012-8.7,unmatchedSuppression] 
 * MISRA exception: External linkage for API functions.
 */
/************************************************************************************//**
** \brief     Removes all items from the linked list. Keep in mind that it is the
**            caller's responsibility to release the memory of the items that were placed
**            in the list, before calling this function.
** \param     list Pointer to a previously created linked list to operate on.
**
****************************************************************************************/
void TbxListClear(tTbxList * list)
{
  tTbxListNode * currentListNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);

  /* Only continue if the parameter is valid. */
  if (list != NULL)
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Get the pointer to the node at the head of the internal linked list. */
    currentListNodePtr = list->firstNodePtr;
    /* Loop through the nodes to find the location of the list that is to be deleted. */
    while (currentListNodePtr != NULL)
    {
      /* Make a copy of the current node pointer, which will later be used to release its
       * allocated memory back to the memory pool.
       */
      tTbxListNode * tempListNodePtr = currentListNodePtr;
      /* Update the node pointer to continue with the next node in the following loop
       * iteration.
       */
      currentListNodePtr = currentListNodePtr->nextNodePtr;
      /* The node is now no longer needed and its memory can be released. */
      TbxMemPoolRelease(tempListNodePtr);
    }
    /* Set the list to empty now that the memory to each of its nodes was released. */
    list->firstNodePtr = NULL;
    list->lastNodePtr = NULL;
    list->nodeCount = 0U;
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }
} /*** end of TbxListClear ***/


/************************************************************************************//**
** \brief     Obtains the number of items that are currently stored in the list.
** \param     list Pointer to a previously created linked list to operate on.
** \return    Total number of items currently stored in the list.
**
****************************************************************************************/
size_t TbxListGetSize(tTbxList const * list)
{
  size_t result = 0;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);

  /* Only continue if the parameter is valid. */
  if (list != NULL)
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Store the current number of items in the list in the result variable. */
    result = list->nodeCount;
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListGetSize ***/


/************************************************************************************//**
** \brief     Inserts an item into the list. The item will be added at the start of the
**            list.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item Pointer to the item to insert.
** \return    TBX_OK if the item could be inserted, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxListInsertItemFront(tTbxList * list, 
                               void     * item)
{
  uint8_t        result = TBX_ERROR;
  tTbxListNode * newListNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item != NULL) )
  {
    /* Attempt to allocate a block for a node in the list. */
    newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
    /* In case the allocation failed, the memory pool could be exhausted. Try to add
     * another block to the memory pool. This works as long as there is enough heap
     * configured.
     */
    if (newListNodePtr == NULL)
    {
      /* Try to add another block to the memory pool. */
      if (TbxMemPoolCreate(1, sizeof(tTbxListNode)) == TBX_OK)
      {
        /* Second attempt of the block allocation. */
        newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
      }
    }
    /* Only continue if the allocation was successful. */
    if (newListNodePtr != NULL)
    {
      /* Initialize the node for the list. */
      newListNodePtr->itemPtr = item;
      newListNodePtr->prevNodePtr = NULL;
      newListNodePtr->nextNodePtr = NULL;
      /* Obtain mutual exclusive access to the list. */
      TbxCriticalSectionEnter();
      /* Check if the list is not empty. */
      if (list->firstNodePtr != NULL)
      {
        /* Sanity check. An non-empty list should have at least one node. */
        TBX_ASSERT(list->nodeCount > 0U);
        /* The new node that is to be inserted will be added at the start and the current
         * start of the list should be moved down.
         */
        newListNodePtr->nextNodePtr = list->firstNodePtr;
        newListNodePtr->nextNodePtr->prevNodePtr = newListNodePtr;
      }
      /* The list is currently empty. */
      else
      {
        /* The to be added node will be the only node, so it is not only the first node
         * but also the last node.
         */
        list->lastNodePtr = newListNodePtr;
      }
      /* Insert the new node at the start of the list. */
      list->firstNodePtr = newListNodePtr;
      /* Increment the node counter. */
      list->nodeCount++;
      /* Release mutual exclusive access for the list. */
      TbxCriticalSectionExit();
      /* Update the result for success. */
      result = TBX_OK;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListInsertItemFront ***/


/************************************************************************************//**
** \brief     Inserts an item into the list. The item will be added at the end of the
**            list.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item Pointer to the item to insert.
** \return    TBX_OK if the item could be inserted, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxListInsertItemBack(tTbxList * list, 
                              void     * item)
{
  uint8_t        result = TBX_ERROR;
  tTbxListNode * newListNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item != NULL) )
  {
    /* Attempt to allocate a block for a node in the list. */
    newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
    /* In case the allocation failed, the memory pool could be exhausted. Try to add
     * another block to the memory pool. This works as long as there is enough heap
     * configured.
     */
    if (newListNodePtr == NULL)
    {
      /* Try to add another block to the memory pool. */
      if (TbxMemPoolCreate(1, sizeof(tTbxListNode)) == TBX_OK)
      {
        /* Second attempt of the block allocation. */
        newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
      }
    }
    /* Only continue if the allocation was successful. */
    if (newListNodePtr != NULL)
    {
      /* Initialize the node for the list. */
      newListNodePtr->itemPtr = item;
      newListNodePtr->prevNodePtr = NULL;
      newListNodePtr->nextNodePtr = NULL;
      /* Obtain mutual exclusive access to the list. */
      TbxCriticalSectionEnter();
      /* Check if the list is not empty. */
      if (list->firstNodePtr != NULL)
      {
        /* Sanity check. An non-empty list should have at least one node. */
        TBX_ASSERT(list->nodeCount > 0U);
        /* The new node that is to be inserted will be added at the end and the current
         * end of the list should be moved up.
         */
        newListNodePtr->prevNodePtr = list->lastNodePtr;
        newListNodePtr->prevNodePtr->nextNodePtr = newListNodePtr;
      }
      /* The list is currently empty. */
      else
      {
        /* The to be added node will be the only node, so it is not only the last node
         * but also the first node.
         */
        list->firstNodePtr = newListNodePtr;
      }
      /* Insert the new node at the end of the list. */
      list->lastNodePtr = newListNodePtr;
      /* Increment the node counter. */
      list->nodeCount++;
      /* Release mutual exclusive access for the list. */
      TbxCriticalSectionExit();
      /* Update the result for success. */
      result = TBX_OK;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListInsertItemBack ***/


/************************************************************************************//**
** \brief     Inserts an item into the list. The item will be added before the reference
**            item.
** \attention Only use this API function if the list does NOT contain items with a 
**            duplicate value, meaning items that point to the exact same memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item Pointer to the item to insert.
** \param     itemRef Reference item before which the new item should be inserted.
** \return    TBX_OK if the item could be inserted, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxListInsertItemBefore(tTbxList       * list, 
                                void           * item, 
                                void     const * itemRef)
{
  uint8_t        result = TBX_ERROR;
  tTbxListNode * newListNodePtr;
  tTbxListNode * refListNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item != NULL);
  TBX_ASSERT(itemRef != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item != NULL) && (itemRef != NULL) )
  {
    /* Try to get pointer to the reference node. */
    refListNodePtr = TbxListFindListNode(list, itemRef);
    /* Only continue if the refeernce node exists. */
    if (refListNodePtr != NULL)
    {
      /* Attempt to allocate a block for a node in the list. */
      newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
      /* In case the allocation failed, the memory pool could be exhausted. Try to add
       * another block to the memory pool. This works as long as there is enough heap
       * configured.
       */
      if (newListNodePtr == NULL)
      {
        /* Try to add another block to the memory pool. */
        if (TbxMemPoolCreate(1, sizeof(tTbxListNode)) == TBX_OK)
        {
          /* Second attempt of the block allocation. */
          newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
        }
      }
      /* Only continue if the allocation was successful. */
      if (newListNodePtr != NULL)
      {
        /* Initialize the node for the list. */
        newListNodePtr->itemPtr = item;
        newListNodePtr->prevNodePtr = NULL;
        newListNodePtr->nextNodePtr = NULL;
        /* Obtain mutual exclusive access to the list. */
        TbxCriticalSectionEnter();
        /* Is the reference item the first (or only) one in the list? */
        if (refListNodePtr == list->firstNodePtr)
        {
          /* Add the item at the start of the list. */
          newListNodePtr->nextNodePtr = list->firstNodePtr;
          newListNodePtr->nextNodePtr->prevNodePtr = newListNodePtr;
          list->firstNodePtr = newListNodePtr;
        }
        /* The reference item is not the first one. This also means that there are at
         * least two items in the list. Sqeeze the new item in before the reference item.
         */
        else
        {
          /* Sanity check. The reference node must have a previous node. */
          TBX_ASSERT(refListNodePtr->prevNodePtr != NULL);
          newListNodePtr->prevNodePtr = refListNodePtr->prevNodePtr;
          newListNodePtr->nextNodePtr = refListNodePtr;
          newListNodePtr->prevNodePtr->nextNodePtr = newListNodePtr;
          newListNodePtr->nextNodePtr->prevNodePtr = newListNodePtr;
        }
        /* Increment the node counter. */
        list->nodeCount++;
        /* Release mutual exclusive access for the list. */
        TbxCriticalSectionExit();
        /* Update the result for success. */
        result = TBX_OK;
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListInsertItemBefore ***/


/************************************************************************************//**
** \brief     Inserts an item into the list. The item will be added after the reference
**            item.
** \attention Only use this API function if the list does NOT contain items with a 
**            duplicate value, meaning items that point to the exact same memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item Pointer to the item to insert.
** \param     itemRef Reference item after which the new item should be inserted.
** \return    TBX_OK if the item could be inserted, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxListInsertItemAfter(tTbxList       * list, 
                               void           * item, 
                               void     const * itemRef)
{
  uint8_t        result = TBX_ERROR;
  tTbxListNode * newListNodePtr;
  tTbxListNode * refListNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item != NULL);
  TBX_ASSERT(itemRef != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item != NULL) && (itemRef != NULL) )
  {
    /* Try to get pointer to the reference node. */
    refListNodePtr = TbxListFindListNode(list, itemRef);
    /* Only continue if the refeernce node exists. */
    if (refListNodePtr != NULL)
    {
      /* Attempt to allocate a block for a node in the list. */
      newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
      /* In case the allocation failed, the memory pool could be exhausted. Try to add
       * another block to the memory pool. This works as long as there is enough heap
       * configured.
       */
      if (newListNodePtr == NULL)
      {
        /* Try to add another block to the memory pool. */
        if (TbxMemPoolCreate(1, sizeof(tTbxListNode)) == TBX_OK)
        {
          /* Second attempt of the block allocation. */
          newListNodePtr = TbxMemPoolAllocate(sizeof(tTbxListNode));
        }
      }
      /* Only continue if the allocation was successful. */
      if (newListNodePtr != NULL)
      {
        /* Initialize the node for the list. */
        newListNodePtr->itemPtr = item;
        newListNodePtr->prevNodePtr = NULL;
        newListNodePtr->nextNodePtr = NULL;
        /* Obtain mutual exclusive access to the list. */
        TbxCriticalSectionEnter();


        /* Is the reference item the last (or only) one in the list? */
        if (refListNodePtr == list->lastNodePtr)
        {
          /* Add the item at the end of the list. */
          newListNodePtr->prevNodePtr = list->lastNodePtr;
          newListNodePtr->prevNodePtr->nextNodePtr = newListNodePtr;
          list->lastNodePtr = newListNodePtr;
        }
        /* The reference item is not the last one. This also means that there are at
         * least two items in the list. Sqeeze the new item in after the reference item.
         */
        else
        {
          /* Sanity check. The reference node must have a next node. */
          TBX_ASSERT(refListNodePtr->nextNodePtr != NULL);
          /* Only continue if the sanity check passed. */
          if (refListNodePtr->nextNodePtr != NULL)
          {
            newListNodePtr->prevNodePtr = refListNodePtr;
            newListNodePtr->nextNodePtr = refListNodePtr->nextNodePtr;
            newListNodePtr->prevNodePtr->nextNodePtr = newListNodePtr;
            newListNodePtr->nextNodePtr->prevNodePtr = newListNodePtr;
          }
        }
        /* Increment the node counter. */
        list->nodeCount++;
        /* Release mutual exclusive access for the list. */
        TbxCriticalSectionExit();
        /* Update the result for success. */
        result = TBX_OK;
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListInsertItemAfter ***/


/************************************************************************************//**
** \brief     Removes an item from the list, if present. Keep in mind that it is the
**            caller's responsibility to release the memory of the item that is being
**            removed from the list, before calling this function.
** \attention Only use this API function if the list does NOT contain items with a 
**            duplicate value, meaning items that point to the exact same memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item Pointer to the item to remove.
**
****************************************************************************************/
void TbxListRemoveItem(tTbxList       * list, 
                       void     const * item)
{
  tTbxListNode * listNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item != NULL) )
  {
    /* Try to find the node that this item belongs to. */
    listNodePtr = TbxListFindListNode(list, item);
    /* Only continue with removal if the item actually belongs to the list. */
    if (listNodePtr != NULL)
    {
      /* Obtain mutual exclusive access to the list. */
      TbxCriticalSectionEnter();
      /* Remove the node from the list. First check if it is the only node in the
       * list.
       */
      if ( (listNodePtr->prevNodePtr == NULL) && (listNodePtr->nextNodePtr == NULL) )
      {
        /* Sanity check. This should also be the start of the list. */
        TBX_ASSERT(listNodePtr == list->firstNodePtr);
        /* Sanity check. This should also be the end of the list. */
        TBX_ASSERT(listNodePtr == list->lastNodePtr);
        /* Sanity check. The list should only have one node. */
        TBX_ASSERT(list->nodeCount == 1U);
        /* Set the list to empty. */
        list->firstNodePtr = NULL;
        list->lastNodePtr = NULL;
      }
      /* Check if it is at the start of the list. */
      else if (listNodePtr->prevNodePtr == NULL)
      {
        /* Sanity check. This should be the start of the list. */
        TBX_ASSERT(listNodePtr == list->firstNodePtr);
        /* Sanity check. There should be a next node. */
        TBX_ASSERT(listNodePtr->nextNodePtr != NULL);
        /* Sanity check. The list should have at least two nodes. */
        TBX_ASSERT(list->nodeCount > 1U);
        /* Make the next node the new start of the list. */
        list->firstNodePtr = listNodePtr->nextNodePtr;
        list->firstNodePtr->prevNodePtr = NULL;
      }
      /* Check if it is at the end of the list. */
      else if (listNodePtr->nextNodePtr == NULL)
      {
        /* Sanity check. This should be the end of the list. */
        TBX_ASSERT(listNodePtr == list->lastNodePtr);
        /* Sanity check. There should be a previous node. */
        TBX_ASSERT(listNodePtr->prevNodePtr != NULL);
        /* Sanity check. The list should have at least two nodes. */
        TBX_ASSERT(list->nodeCount > 1U);
        /* Make the previous node the end of the list. */
        list->lastNodePtr = listNodePtr->prevNodePtr;
        list->lastNodePtr->nextNodePtr = NULL;
      }
      /* If it is not the only node in the list, not at the start and not at the end,
       * then the list must have at least three nodes and the current node is somewhere
       * in the middle.
       */
      else
      {
        /* Sanity check. There should be a previous node. */
        TBX_ASSERT(listNodePtr->prevNodePtr != NULL);
        /* Sanity check. There should be a next node. */
        TBX_ASSERT(listNodePtr->nextNodePtr != NULL);
        /* Sanity check. The list should have at least three nodes. */
        TBX_ASSERT(list->nodeCount > 2U);
        /* Remove ourselves from the list. */
        listNodePtr->prevNodePtr->nextNodePtr = listNodePtr->nextNodePtr;
        listNodePtr->nextNodePtr->prevNodePtr = listNodePtr->prevNodePtr;
      }
      /* Decrement the node counter. */
      list->nodeCount--;
      /* Release mutual exclusive access of the list. */
      TbxCriticalSectionExit();
      /* Give the node back to the memory pool. */
      TbxMemPoolRelease(listNodePtr);
    }
  }
} /*** end of TbxListRemoveItem ***/


/************************************************************************************//**
** \brief     Obtains the item that is located at the start of the list. Note that the
**            item is just read, not removed.
** \param     list Pointer to a previously created linked list to operate on.
** \return    The item at the start of the list or NULL if the list is empty.
**
****************************************************************************************/
void * TbxListGetFirstItem(tTbxList const * list)
{
  void * result = NULL;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);

  /* Only continue if the parameters are valid. */
  if (list != NULL)
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Get the first item in the list if there is one. */
    if (list->firstNodePtr != NULL)
    {
      result = list->firstNodePtr->itemPtr;
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListGetFirstItem ***/


/************************************************************************************//**
** \brief     Obtains the item that is located at the end of the list. Note that the
**            item is just read, not removed.
** \param     list Pointer to a previously created linked list to operate on.
** \return    The item at the end of the list or NULL if the list is empty.
**
****************************************************************************************/
void * TbxListGetLastItem(tTbxList const * list)
{
  void * result = NULL;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);

  /* Only continue if the parameters are valid. */
  if (list != NULL)
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Get the last item in the list if there is one. */
    if (list->lastNodePtr != NULL)
    {
      result = list->lastNodePtr->itemPtr;
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListGetLastItem ***/


/************************************************************************************//**
** \brief     Obtains the item that is located one position before in the list,
**            relative to the item given in the parameter. Note that the item is just
**            read, not removed.
** \attention Only use this API function if the list does NOT contain items with a 
**            duplicate value, meaning items that point to the exact same memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     itemRef The item that is the next one in the list relative to the one
**            this function should return.
** \return    The item one position before in the list or NULL if there are no
**            more items in the list before the item given in the parameter.
**
****************************************************************************************/
void * TbxListGetPreviousItem(tTbxList const * list, 
                              void     const * itemRef)
{
  void               * result = NULL;
  tTbxListNode const * listNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(itemRef != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (itemRef != NULL) )
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Obtain the node of the item specified in the parameter. */
    listNodePtr = TbxListFindListNode(list, itemRef);
    /* Only continue if the node could be found. */
    if (listNodePtr != NULL)
    {
      /* Get the pointer to the previous node. */
      listNodePtr = listNodePtr->prevNodePtr;
      /* Only continue if there is actually a node here. */
      if (listNodePtr != NULL)
      {
        /* Set the result to the item of the next node. */
        result = listNodePtr->itemPtr;
      }
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListGetPreviousItem ***/


/************************************************************************************//**
** \brief     Obtains the item that is located one position further down in the list,
**            relative to the item given in the parameter. Note that the item is just
**            read, not removed.
** \attention Only use this API function if the list does NOT contain items with a 
**            duplicate value, meaning items that point to the exact same memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     itemRef The item that is the previous one in the list relative to the one
**            this function should return.
** \return    The item one position further down in the list or NULL if there are no
**            more items in the list after the item given in the parameter.
**
****************************************************************************************/
void * TbxListGetNextItem(tTbxList const * list, 
                          void     const * itemRef)
{
  void               * result = NULL;
  tTbxListNode const * listNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(itemRef != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (itemRef != NULL) )
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Obtain the node of the item specified in the parameter. */
    listNodePtr = TbxListFindListNode(list, itemRef);
    /* Only continue if the node could be found. */
    if (listNodePtr != NULL)
    {
      /* Get the pointer to the next node. */
      listNodePtr = listNodePtr->nextNodePtr;
      /* Only continue if there is actually a node here. */
      if (listNodePtr != NULL)
      {
        /* Set the result to the item of the next node. */
        result = listNodePtr->itemPtr;
      }
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListGetNextItem ***/


/************************************************************************************//**
** \brief     Swaps the specified list items around.
** \attention Only use this API function if the list does NOT contain items with a 
**            duplicate value, meaning items that point to the exact same memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item1 The first item for the swap operation.
** \param     item2 The second item for the swap operation.
**
****************************************************************************************/
void TbxListSwapItems(tTbxList const * list, 
                      void           * item1, 
                      void           * item2)
{
  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item1 != NULL);
  TBX_ASSERT(item2 != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item1 != NULL) && (item2 != NULL) )
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Obtain the node pointers of the items that need to be swapped. */
    tTbxListNode * listNode1Ptr = TbxListFindListNode(list, item1);
    tTbxListNode * listNode2Ptr = TbxListFindListNode(list, item2);

    /* Only continue if the nodes actually exist in the list. */
    if ( (listNode1Ptr != NULL) && (listNode2Ptr != NULL) )
    {
      /* Perform the swap operation. */
      listNode1Ptr->itemPtr = item2;
      listNode2Ptr->itemPtr = item1;
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }
} /*** end of TbxListSwapItems ***/


/************************************************************************************//**
** \brief     Sorts the items in the list. While sorting, it calls the specified callback
**            function which should do the actual comparison of the items.
** \param     list Pointer to a previously created linked list to operate on.
** \param     compareItemsFcn Callback function that does the item comparison.
**
****************************************************************************************/
void TbxListSortItems(tTbxList             const * list, 
                      tTbxListCompareItems         compareItemsFcn)
{
  tTbxListNode * currentNode;
  tTbxListNode * idxNode;
  void         * tempItemPtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(compareItemsFcn != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (compareItemsFcn != NULL) )
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* To accomplish the sort, we maintain two pointers: current and index. Initially,
     * current points to head node and index will point to node next to current.
     * Traverse through the list till current's next node points to NULL, by comparing
     * current's data with index's data. If the current's data is greater than the
     * index's data, then swap data between them. Continue this process till the entire
     * list is sorted in ascending order. It is a basic bubble sort algorithm.
     */
    currentNode = list->firstNodePtr;
    /* Traverse through the entire list. Note that sorting is only done if there are
     * at least two nodes in the list.
     */
    while (currentNode->nextNodePtr != NULL)
    {
      /* Point index to the node next to current. */
      idxNode = currentNode->nextNodePtr;
      /* Keep looping while the index node is valid. */
      while (idxNode != NULL)
      {
        /* If current's data is greater than index's data, swap the data of current and
         * index.
         */
        if (compareItemsFcn(currentNode->itemPtr, idxNode->itemPtr) == TBX_TRUE)
        {
          /* Perform the swap. */
          tempItemPtr = currentNode->itemPtr;
          currentNode->itemPtr = idxNode->itemPtr;
          idxNode->itemPtr = tempItemPtr;
        }
        /* Move the index node one forward and continue there the next loop. */
        idxNode = idxNode->nextNodePtr;
      }
      /* Move the current node one forward and continue there the next loop. */
      currentNode = currentNode->nextNodePtr;
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }
} /*** end of TbxListSortItems ***/


/************************************************************************************//**
** \brief     Helper function to get the node that a specific item in the list belongs
**            to.
** \attention Note that this function does not work as intended, if the list contains
**            items with a duplicate value, meaning items that point to the exact same
**            memory address.
** \param     list Pointer to a previously created linked list to operate on.
** \param     item Pointer to the item of whoms owning node should be found.
** \return    Pointer to the node if successful, NULL otherwise.
**
****************************************************************************************/
static tTbxListNode * TbxListFindListNode(tTbxList const * list, 
                                          void     const * item)
{
  tTbxListNode * result = NULL;
  tTbxListNode * currentListNodePtr;

  /* Verify parameters. */
  TBX_ASSERT(list != NULL);
  TBX_ASSERT(item != NULL);

  /* Only continue if the parameters are valid. */
  if ( (list != NULL) && (item != NULL) )
  {
    /* Obtain mutual exclusive access to the list. */
    TbxCriticalSectionEnter();
    /* Get the pointer to the node at the head of the internal linked list. */
    currentListNodePtr = list->firstNodePtr;
    /* Loop through the nodes to find the node that the item belongs to. */
    while (currentListNodePtr != NULL)
    {
      /* Is this the node we are looking for? */
      if (currentListNodePtr->itemPtr == item)
      {
        /* Found the node. Update the result and stop looping. */
        result = currentListNodePtr;
        break;
      }
      /* Update the node pointer to continue with the next node in the following loop
       * iteration.
       */
      currentListNodePtr = currentListNodePtr->nextNodePtr;
    }
    /* Release mutual exclusive access of the list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxListFindListNode ***/


/*********************************** end of tbx_list.c *********************************/
