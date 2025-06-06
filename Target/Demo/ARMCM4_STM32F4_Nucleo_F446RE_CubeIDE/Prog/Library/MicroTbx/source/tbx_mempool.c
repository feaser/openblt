/************************************************************************************//**
* \file         tbx_mempool.c
* \brief        Pool based dynamic heap memory manager source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019 by Feaser     www.feaser.com     All rights reserved
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
* Type definitions
****************************************************************************************/
/** \brief Layout of a block node, which forms the building block of a linked list
 *         consisting of data blocks.
 */
typedef struct t_block_node
{
  /** \brief Pointer to the block. */
  void                * blockPtr;
  /** \brief Pointer to the next node in the list or NULL if it is the list end. */
  struct t_block_node * nextNodePtr;
} tBlockNode;

/** \brief Linked list consisting of data block nodes. */
typedef tBlockNode (* tBlockList);

/** \brief Layout of a single memory pool. */
typedef struct
{
  /** \brief The number of bytes that fit in one block. */
  size_t       blockSize;
  /** \brief Pointer to the linked list with free data block nodes. */
  tBlockList * freeBlockListPtr;
  /** \brief Pointer to the linked list with used data block nodes. */
  tBlockList * usedBlockListPtr;
} tPool;

/** \brief Layout of a memory pool node, which forms the building block of a linked list
 *         consisting of memory pools.
 */
typedef struct t_pool_node
{
  /** \brief Pointer to the memory pool. */
  tPool              * poolPtr;
  /** \brief Pointer to the next node in the list or NULL if it is the list end. */
  struct t_pool_node * nextNodePtr;
} tPoolNode;

/** \brief Linked list consisting of memory pool nodes. */
typedef tPoolNode (* tPoolList);


/****************************************************************************************
* Function prototypes
****************************************************************************************/
/* Pool list management functions */
static tPoolNode  * TbxMemPoolListFind         (size_t             blockSize);

static tPoolNode  * TbxMemPoolListFindBestFit  (size_t             blockSize);

static void         TbxMemPoolListInsert       (tPoolNode        * nodePtr);

/* Block management functions. */
static void       * TbxMemPoolBlockCreate      (size_t             size);

static void       * TbxMemPoolBlockGetDataPtr  (void             * memPtr);

static size_t       TbxMemPoolBlockGetBlockSize(void       const * memPtr);

static void       * TbxMemPoolBlockGetMemPtr   (void             * dataPtr);

/* Block list management functions. */
static tBlockList * TbxMemPoolBlockListCreate  (void);

static void         TbxMemPoolBlockListInsert  (tBlockList       * listPtr,
                                                tBlockNode       * nodePtr);
                                              
static tBlockNode * TbxMemPoolBlockListExtract (tBlockList       * listPtr);

static uint8_t      TbxMemPoolBlockListIsEmpty (tBlockList const * listPtr);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Linked list with memory pools. */
static tPoolList tbxPoolList = NULL;


/************************************************************************************//**
** \brief     Creates a new memory pool with the specified number of blocks, where each
**            block has the size as specified by the second function parameter. The
**            required memory is statically preallocated on the heap. This function is
**            typically called one or more times during software program initialization,
**            before the infinite program loop is entered. Once one or more memory pools
**            were created, the functions TbxMemPoolAllocate() and TbxMemPoolRelease()
**            can be called to perform dynamic memory allocation. A well designed memory
**            pool approach makes dynamic memory allocation possible on RAM constrained
**            microcontrollers, without the need to worry about memory fragmentation.
**            Note that deleting a previously created memory pool is not supported on
**            purpose to prevent memory fragmentation.
**            When this function is called to create a memory pool with a size that was
**            already created before, the already existing memory pool is extended
**            instead of creating a new memory pool. This is an important feature because
**            it makes it possible to automatically expand the memory pool if more
**            blocks are needed.
** \param     numBlocks The number of blocks to statically preallocate on the heap for
**            this memory pool.
** \param     blockSize The size of each block in bytes.
** \return    TBX_OK if successful, TBX_ERROR otherwise for example when there is no
**            more space available on the heap to statically preallocated the blocks.
**
****************************************************************************************/
uint8_t TbxMemPoolCreate(size_t numBlocks, 
                         size_t blockSize)
{
  uint8_t      result = TBX_ERROR;
  void       * blockPtr;
  tPool      * poolPtr;

  /* Verify parameters. */
  TBX_ASSERT(numBlocks > 0U);
  TBX_ASSERT(blockSize > 0U);

  /* Only continue if the parameters are valid. */
  if ( (numBlocks > 0U) && (blockSize > 0U) )
  {
    /* Set the result value to okay. */
    result = TBX_OK;
    /* Obtain mutual exclusive access to the memory pool list. */
    TbxCriticalSectionEnter();
    /* Attempt to locate a memory pool node in the list that is configured for the same
     * block size.
     */
    tPoolNode * poolNodePtr = TbxMemPoolListFind(blockSize);
    /* Create a new memory pool node and its associated empty memory pool if a memory
     * pool node for this block size does not yet exist.
     */
    if (poolNodePtr == NULL)
    {
      /* Create a new memory pool node. */
      poolNodePtr = TbxHeapAllocate(sizeof(tPoolNode));
      /* Verify that the memory pool node could be created. */
      if (poolNodePtr == NULL)
      {
        /* Flag the error. */
        result = TBX_ERROR;
      }
      /* Continue with creating the memory pool object. */
      else
      {
        /* Create a new memory pool object. */
        poolPtr = TbxHeapAllocate(sizeof(tPool));
        /* Verify that the memory pool object could be created. */
        if (poolPtr == NULL)
        {
          /* Flag the error. */
          result = TBX_ERROR;
        }
        /* Continue with initializing both the memory pool and its node. */
        else
        {
          /* Initialize the memory pool node. */
          poolNodePtr->nextNodePtr = NULL;
          poolNodePtr->poolPtr = poolPtr;
          /* Store the data size of the blocks managed by the memory pool. */
          poolPtr->blockSize = blockSize;
          /* Create the block list object that will hold blocks that are still free. */
          poolPtr->freeBlockListPtr = TbxMemPoolBlockListCreate();
          /* Create the block list object that will hold blocks that are in use. */
          poolPtr->usedBlockListPtr = TbxMemPoolBlockListCreate();
          /* Verify that the block list objects could be created. */
          if ( (poolPtr->freeBlockListPtr == NULL) ||
               (poolPtr->usedBlockListPtr == NULL) )
          {
            /* Flag the error. */
            result = TBX_ERROR;
          }
          /* The (empty) memory pool and its node were created. Time to insert it into
           * the list.
           */
          else
          {
            TbxMemPoolListInsert(poolNodePtr);
          }
        }
      }
    }
    /* Only continue if all is okay so far. */
    if (result == TBX_OK)
    {
      /* The pool node pointer it now valid. It either points to a node that holds a
       * newly created and empty memory pool or to a node that holds an already existing
       * memory pool that can be extended. Create the blocks one by one and add them as
       * nodes to the free block list.
       */
      for (size_t blockNodeIdx = 0U; blockNodeIdx < numBlocks; blockNodeIdx++)
      {
        /* Allocate memory for the block node. */
        tBlockNode * blockNodePtr = TbxHeapAllocate(sizeof(tBlockNode));
        /* Verify that the node could be allocated. */
        if (blockNodePtr == NULL)
        {
          /* Flag the error. */
          result = TBX_ERROR;
        }
        /* The node was created. */
        else
        {
          /* Allocate memory for the actual data block itself. */
          blockPtr = TbxMemPoolBlockCreate(blockSize);
          /* Verify that the the data block could be allocated. */
          if (blockPtr == NULL)
          {
            /* Flag the error. */
            result = TBX_ERROR;
          }
          /* The data block was created. */
          else
          {
            /* Initialize the block node. */
            blockNodePtr->blockPtr = blockPtr;
            blockNodePtr->nextNodePtr = NULL;
            /* Sanity check. The pool node pointer should not be NULL here. */
            TBX_ASSERT(poolNodePtr != NULL);
            /* Flag error in case the sanity check failed. */
            if (poolNodePtr == NULL)
            {
              /* Flag the error. */
              result = TBX_ERROR;
            }
            else
            {
              /* Insert the block node into the free block list. */
              TbxMemPoolBlockListInsert(poolNodePtr->poolPtr->freeBlockListPtr,
                                        blockNodePtr);
            }
          }
        }
      }
    }
    /* Release mutual exclusive access to the memory pool list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolCreate ***/


/************************************************************************************//**
** \brief     Attempts to allocate the desired number of bytes in a previously created
**            memory pool. A block will be allocated in a memory pool that best fits the
**            requested byte size. For example, if the following memory pools were
**            created during program initialization:
**              TbxMemPoolCreate(10, 8);
**              TbxMemPoolCreate(10, 16);
**              TbxMemPoolCreate(10, 32);
**            And the following function call is made:
**              uint8_t * myMem = TbxMemPoolAllocate(9);
**            Then the memory will be allocated from the memory pool with block size 16,
**            so the second memory pool that was created. If there are no more blocks
**            available in that memory pool, then NULL is returned. So note that this
**            function does NOT move on to the memory pool with one block size
**            larger. Such a feature would be easy to realize but this design decision
**            was made on purpose.
** \param     size The number of bytes to allocate in a memory pool.
** \return    Pointer to the start of the newly allocated memory if successful, NULL
**            otherwise.
**
****************************************************************************************/
void * TbxMemPoolAllocate(size_t size)
{
  void            * result = NULL;
  void            * blockDataPtr;
  tPoolNode const * poolNodePtr;

  /* Verify parameter. */
  TBX_ASSERT(size > 0U);

  /* Only continue if the parameter is valid. */
  if (size > 0U)
  {
    /* Obtain mutual exclusive access to the memory pool list. */
    TbxCriticalSectionEnter();
    /* Try to find the best fitting memory pool that has a block available. */
    poolNodePtr = TbxMemPoolListFindBestFit(size);
    /* Only continue with the allocation of a memory pool candidate was found. */
    if (poolNodePtr != NULL)
    {
      /* Get the pointer to the actual memory pool. */
      tPool const * poolPtr = poolNodePtr->poolPtr;
      /* Sanity check. The memory pool should not be NULL here. */
      TBX_ASSERT(poolPtr != NULL);
      /* Only continue if the sanity check passed. */
      if (poolPtr != NULL)
      {
        /* Attempt to extract a block node from the linked list with free block nodes. */
        tBlockNode * blockNodePtr = TbxMemPoolBlockListExtract(poolPtr->freeBlockListPtr);
        /* Only continue if a free block node could be extracted. */
        if (blockNodePtr != NULL)
        {
          /* Read and store the pointer that points to the block's data. */
          blockDataPtr = TbxMemPoolBlockGetDataPtr(blockNodePtr->blockPtr);
          /* Now the node itself needs to be inserted into the linked list with used
           * block nodes. This way this node can be reused when freeing this or another
           * block.
           */
          TbxMemPoolBlockListInsert(poolPtr->usedBlockListPtr, blockNodePtr);
          /* Perform a sanity check. The block's data pointer should not be NULL here. */
          TBX_ASSERT(blockDataPtr != NULL);
          /* Set the result value. */
          result = blockDataPtr;
        }
      }
    }
    /* Release mutual exclusive access to the memory pool list. */
    TbxCriticalSectionExit();
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolAllocate ***/


/************************************************************************************//**
** \brief     An alternative version of TbxMemPoolAllocate(), which automatically creates
**            a new memory pool with one block, if one with the exact same blockSize was
**            not yet created. If one with the exact same blockSize was already created,
**            but it's full, then the memory pool is automatically expanded to have one
**            more block.
** \details   This offers a convenient way of working with memory pools compared to
**            TbxMemPoolAllocate(). In addition, it guarantees that it always works on
**            a memory pool where the blockSize  == "size". It bypasses the best fitting
**            algorithm used by TbxMemPoolAllocate(), which looks for an existing memory
**            pool with a blockSize >= "size".
**
**            For example, to allocate two blocks of 32 bytes using memory pools, all you
**            need to do is:
**              uint8_t * myMem[2];
**
**              myMem[0] = TbxMemPoolAllocateAuto(32);
**              myMem[1] = TbxMemPoolAllocateAuto(32);
**            
**            Note that there was no need to first create this memory pool with a call
**            to TbxMemPoolCreate(). During the first call to TbxMemPoolAllocateAuto(),
**            the memory pool with a block size of 32 bytes was automatically created,
**            including adding 1 block of 32 bytes to it, which was immediately
**            allocated. During the second call to TbxMemPoolAllocateAuto(), another
**            block of 32 bytes was automatically added to the existing memory pool and
**            the newly added block was immediately allocated.
** \param     size The number of bytes to allocate using a memory pool.
** \return    Pointer to the start of the newly allocated memory if successful, NULL
**            otherwise.
**
****************************************************************************************/
void * TbxMemPoolAllocateAuto(size_t size)
{
  void            * result      = NULL;
  tPoolNode const * poolNodePtr = NULL;
  tPoolNode const * currentPoolNodePtr;

  /* Verify parameter. */
  TBX_ASSERT(size > 0U);

  /* Only continue if the parameter is valid. */
  if (size > 0U)
  {
    /* Obtain mutual exclusive access to the memory pool list. */
    TbxCriticalSectionEnter();
    /* Get pointer to the pool node at the head of the linked list. */
    currentPoolNodePtr = tbxPoolList;
    /* Loop through all nodes until one of the exact same size is found. */
    while (currentPoolNodePtr != NULL)
    {
      /* Does this memory pool hold blocks of the exact same size? */
      if (currentPoolNodePtr->poolPtr->blockSize == size)
      {
        /* Existing memory pool of the exact same size was found. Set the poolNodePtr to
         * point to this memory pool.
         */
        poolNodePtr = currentPoolNodePtr;
        /* Memory pool found, so no need to continue searching. */
        break;
      }
      /* Continue with the next pool node in the list. */
      currentPoolNodePtr = currentPoolNodePtr->nextNodePtr;
    }
    /* Release mutual exclusive access to the memory pool list. */
    TbxCriticalSectionExit();

    /* No memory pool with the exact same size found? */
    if (poolNodePtr == NULL)
    {
      /* Automatically create a memory pool with the blockSize set to the size to
       * allocate. 
       */
      if (TbxMemPoolCreate(1U, size) == TBX_OK)
      {
        /* Allocate a block from the newly created memory pool. Should always work, since
         * we just created a memory pool with one block.
         */
        result = TbxMemPoolAllocate(size);      
      }
    }
    /* Memory pool with the exact same size found. */
    else
    {
      /* Allocate a block from the memory pool. */
      result = TbxMemPoolAllocate(size);
      /* No more blocks available in the memory pool? */
      if (result == NULL)
      {
        /* Automatically increase the memory pool by adding one more block to it. */
        if (TbxMemPoolCreate(1U, size) == TBX_OK)
        {
          /* Allocate a block from the memory pool. Should always work, since we just
           * expanded it with one block.
           */
          result = TbxMemPoolAllocate(size);      
       }
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolAllocateAuto ***/


/************************************************************************************//**
** \brief     Releases the previously allocated block of memory. Once the memory is
**            released, it can be allocated again afterwards with function
**            TbxMemPoolAllocate(). Note that this function automatically finds the
**            memory pool that the memory block belongs to.
** \param     memPtr Pointer to the start of the memory block. Basically, the pointer
**            that was returned by function TbxMemPoolAllocate(), when the memory was
**            initially allocated.
**
****************************************************************************************/
void TbxMemPoolRelease(void * memPtr)
{
  void            * blockPtr;
  tPoolNode const * poolNodePtr;

  /* Verify parameter. */
  TBX_ASSERT(memPtr != NULL);

  /* Only continue if the parameter is valid. */
  if (memPtr != NULL)
  {
    /* Obtain mutual exclusive access to the memory pool list. */
    TbxCriticalSectionEnter();
    /* First convert the block's data pointer to the block's base memory pointer. */
    blockPtr = TbxMemPoolBlockGetMemPtr(memPtr);
    /* Only continue if the block pointer is valid. */
    if (blockPtr != NULL)
    {
      /* Get the block's data size. */
      size_t blockSize = TbxMemPoolBlockGetBlockSize(blockPtr);
      /* Attempt to locate the memory pool node that holds the memory pool with this
       * block size.
       */
      poolNodePtr = TbxMemPoolListFind(blockSize);
      /* Sanity check. The memory pool node that the to be released memory originally
       * belonged to should have been found.
       */
      TBX_ASSERT(poolNodePtr != NULL);
      /* Only continue if the sanity check passed. */
      if (poolNodePtr != NULL)
      {
        /* Get the pointer to the actual memory pool. */
        tPool const * poolPtr = poolNodePtr->poolPtr;
        /* Sanity check. The memory pool should not be NULL here. */
        TBX_ASSERT(poolPtr != NULL);
        /* Only continue if the sanity check passed. */
        if (poolPtr != NULL)
        {
          /* Attempt to extract a block node from the linked list with used block
           * nodes.
           */
          tBlockNode * blockNodePtr = TbxMemPoolBlockListExtract(poolPtr->usedBlockListPtr);
          /* Sanity check. A node should be available, otherwise more blocks were
           * released than actually allocated, which shouldn't happen.
           */
          TBX_ASSERT(blockNodePtr != NULL);
          /* Only continue if a block node could be extracted. */
          if (blockNodePtr != NULL)
          {
            /* Initialize the block. */
            blockNodePtr->blockPtr = blockPtr;
            /* Now the node itself needs to be inserted into the linked list with free
             * block nodes. This way this node can be allocated again in the future.
             */
            TbxMemPoolBlockListInsert(poolPtr->freeBlockListPtr, blockNodePtr);
          }
        }
      }
    }
    /* Release mutual exclusive access to the memory pool list. */
    TbxCriticalSectionExit();
  }
} /*** end of TbxMemPoolRelease ***/


/****************************************************************************************
*   P O O L   L I S T   M A N A G E M E N T   F U N C T I O N S
****************************************************************************************/

/************************************************************************************//**
** \brief     Searches through the linked list with memory pools to find a pool that was
**            created to hold blocks that are of the exact size as specified by the
**            parameter.
** \param     blockSize Size of the blocks managed by the memory pool.
** \return    Pointer to the found memory pool node if successful, NULL otherwise.
**
****************************************************************************************/
static tPoolNode * TbxMemPoolListFind(size_t blockSize)
{
  tPoolNode * result = NULL;
  tPoolNode * poolNodePtr;

  /* Verify parameter. */
  TBX_ASSERT(blockSize > 0U);

  /* Only continue if the parameter is valid. */
  if (blockSize > 0U)
  {
    /* Get pointer to the pool node at the head of the linked list. */
    poolNodePtr = tbxPoolList;
    /* Loop through all nodes until a match is found. */
    while (poolNodePtr != NULL)
    {
      /* Does this node hold a memory pool created for the same block size as we are
       * trying to find?
       */
      if (poolNodePtr->poolPtr->blockSize == blockSize)
      {
        /* Update the result because a match was found. */
        result = poolNodePtr;
        /* No need to continue looping since a match was found. */
        break;
      }
      /* Continue with the next pool node in the list. */
      poolNodePtr = poolNodePtr->nextNodePtr;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolListFind ***/


/************************************************************************************//**
** \brief     Searches through the linked list with memory pools to find a pool that was
**            created to hold blocks that are of equal size or slightly greater. If the
**            found memory pool has no more free blocks available the search
**            is NOT continued for a memory pool of the next size up. Although this
**            sounds like a nice feature to have, this was not implemented on purpose.
**            The reason for this is that it is now possible to expand an existing memory
**            pool when it is full. Assume a situation where all blocks in the memory
**            pool are already allocated. The next call to TbxMemPoolAllocate() therefore
**            fails. You can now call TbxMemPoolCreate() again for the same block size
**            and the original memory pool is expanded automatically. Note that this
**            function relies on the fact that the memory pools in the list are sorted
**            by ascending block size.
** \param     blockSize Size of the block to fit.
** \return    Pointer to the found memory pool node if successful, NULL otherwise.
**
****************************************************************************************/
static tPoolNode * TbxMemPoolListFindBestFit(size_t blockSize)
{
  tPoolNode * result = NULL;
  tPoolNode * poolNodePtr;

  /* Verify parameter. */
  TBX_ASSERT(blockSize > 0U);

  /* Only continue if the parameter is valid. */
  if (blockSize > 0U)
  {
    /* Get pointer to the pool node at the head of the linked list. */
    poolNodePtr = tbxPoolList;
    /* Loop through all nodes until a fit is found. */
    while (poolNodePtr != NULL)
    {
      /* Does this memory pool hold blocks that would fit the specified block size? */
      if (poolNodePtr->poolPtr->blockSize >= blockSize)
      {
        /* A fit is found. Now check if this memory pool has free blocks available. */
        if (TbxMemPoolBlockListIsEmpty(poolNodePtr->poolPtr->freeBlockListPtr) \
            == TBX_FALSE)
        {
          /* Found a match so update the result value. */
          result = poolNodePtr;
        }
        /* Best fit is found, so no need to continue searching even if the memory pool
         * does not have any free blocks available.
         */
        break;
      }
      /* Continue with the next pool node in the list. */
      poolNodePtr = poolNodePtr->nextNodePtr;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolListFindBestFit ***/


/************************************************************************************//**
** \brief     Inserts the specified memory pool node into the linked list with memory
**            pool nodes. It automatically sorts the nodes by ascending block size. Note
**            that this function only works properly is there is not already a memory
**            pool in the list configured for the same block size as the new one that
**            this function should insert.
** \param     nodePtr Pointer to the memory pool node to insert.
**
****************************************************************************************/
static void TbxMemPoolListInsert(tPoolNode * nodePtr)
{
  tPoolNode * currentNodePtr;
  tPoolNode * prevNodePtr = NULL;
  uint8_t     nodeInserted = TBX_FALSE;

  /* Verify parameter. */
  TBX_ASSERT(nodePtr != NULL);

  /* Only continue if the parameter is valid. */
  if (nodePtr != NULL)
  {
    /* Sanity check. The pointer to the memory pool should not be NULL here. */
    TBX_ASSERT(nodePtr->poolPtr != NULL);
    /* Is the list with memory pools empty? */
    if (tbxPoolList == NULL)
    {
      /* Add the node at the start of the list. */
      nodePtr->nextNodePtr = NULL;
      tbxPoolList = nodePtr;
    }
    /* The list with memory pools is not empty. */
    else
    {
      /* Get pointer to the pool node at the head of the linked list. */
      currentNodePtr = tbxPoolList;
      /* Loop through the nodes to find the location where the new node should be
       * inserted, such that the nodes always remain sorted by ascending block size.
       */
      while (currentNodePtr != NULL)
      {
        /* Sanity check. The pointer to the memory pool should not be NULL here. */
        TBX_ASSERT(currentNodePtr->poolPtr != NULL);
        /* Only continue if the sanity check passed. */
        if (currentNodePtr->poolPtr != NULL)
        {
          /* This function should not be used to insert a memory pool node that has a
           * block size that equals an already existing node. Verify this.
          */
          TBX_ASSERT(currentNodePtr->poolPtr->blockSize != nodePtr->poolPtr->blockSize);
          /* Is the block size of this node's memory pool larger than the new one? */
          if (currentNodePtr->poolPtr->blockSize > nodePtr->poolPtr->blockSize)
          {
            /* The new node should be inserted before this one. If the current node is
             * the head of the list, the new node should become the new head.
            */
            if (currentNodePtr == tbxPoolList)
            {
              /* Sanity check. In this case the previous node should still be NULL. */
              TBX_ASSERT(prevNodePtr == NULL);
              /* Add the node at the start of the list, right before the current node. */
              nodePtr->nextNodePtr = currentNodePtr;
              tbxPoolList = nodePtr;
            }
            /* The current node is not the head of the list, so the new node should be
             * inserted between previous node and the current node.
            */
            else
            {
              /* Sanity check. In this case the previous node should not be NULL. */
              TBX_ASSERT(prevNodePtr != NULL);
              /* Only continue if the sanity check passed. */
              if (prevNodePtr != NULL)
              {
                /* Insert the node between the previous and current nodes. */
                nodePtr->nextNodePtr = currentNodePtr;
                prevNodePtr->nextNodePtr = nodePtr;
              }
            }
            /* Set flag to indicate that the new node was successfully inserted. */
            nodeInserted = TBX_TRUE;
          }
        }
        /* Did we reach the end of the list and the new node was not yet inserted? */
        if ( (currentNodePtr->nextNodePtr == NULL) && (nodeInserted == TBX_FALSE) )
        {
          /* There are no memory pools present in the list that have a block size larger
           * than the new node. This means that the new node should be inserted at the
           * tail of the list.
           */
          nodePtr->nextNodePtr = NULL;
          currentNodePtr->nextNodePtr = nodePtr;
          /* Set flag to indicate that the new node was successfully inserted. */
          nodeInserted = TBX_TRUE;
        }
        /* Check if the new node was already inserted. */
        if (nodeInserted == TBX_TRUE)
        {
          /* All done so no need to continue looping. */
          break;
        }
        /* Update loop variables. */
        prevNodePtr = currentNodePtr;
        currentNodePtr = currentNodePtr->nextNodePtr;
      }
    }
  }
} /*** end of TbxMemPoolListInsert ***/


/****************************************************************************************
*   B L O C K   M A N A G E M E N T   F U N C T I O N S
****************************************************************************************/

/************************************************************************************//**
** \brief     Creates and initializes a new block including its memory allocation on the
**            heap. A block consists of the actual memory to hold the block data and is
**            preceded by an element of size_t, where the size of the block is written
**            to:
**            memPtr  -> -----------
**                      | blockSize |
**            dataPtr ->|------------------------------------------------
**                      | data byte 0 | data byte 1 | data byte 2 | etc. |
**                       ------------------------------------------------
** \param     size The data size of the block in bytes.
** \return    Pointer to the memory of the created block if successful, NULL otherwise.
**
****************************************************************************************/
static void * TbxMemPoolBlockCreate(size_t size)
{
  void   * result = NULL;
  void   * blockMemPtr;

  /* Verify parameter. */
  TBX_ASSERT(size > 0U);

  /* Only continue if the parameter is valid. */
  if (size > 0U)
  {
    /* Allocate memory for the block data and an extra size_t value at the start. */
    blockMemPtr = TbxHeapAllocate(sizeof(size_t) + size);
    /* Only continue if the memory allocation was successful. */
    if (blockMemPtr != NULL)
    {
      /* Set the result value. */
      result = blockMemPtr;
      /* Create a pointer to an array of size_t elements. */
      size_t * blockSizeArray = blockMemPtr;
      /* Write to the first element, which should hold the block size. */
      blockSizeArray[0U] = size;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockCreate ***/


/************************************************************************************//**
** \brief     Converts the block memory pointer, which points to the start of the block's
**            allocated memory, to the pointer where the actual block data starts.
** \param     memPtr Pointer to the start of the block's allocated memory.
** \return    Pointer to where the block's data starts if successful, NULL otherwise.
**
****************************************************************************************/
static void * TbxMemPoolBlockGetDataPtr(void * memPtr)
{
  void   * result = NULL;
  size_t * blockSizeArray;
  void   * dataPtr;

  /* Verify parameter. */
  TBX_ASSERT(memPtr != NULL);

  /* Only continue if the parameter is valid. */
  if (memPtr != NULL)
  {
    /* Create a pointer to an array of size_t elements. */
    blockSizeArray = memPtr;
    /* The block data starts after the block size value. */
    dataPtr = &blockSizeArray[1U];
    /* Set the result value. */
    result = dataPtr;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockGetDataPtr ***/


/************************************************************************************//**
** \brief     Extract the size of the block data, given the pointer to the block's
**            allocated memory.
** \param     memPtr Pointer to the start of the block's allocated memory.
** \return    Size in bytes of the block's data if successful, 0 otherwise.
**
****************************************************************************************/
static size_t TbxMemPoolBlockGetBlockSize(void const * memPtr)
{
  size_t         result = 0U;
  size_t const * blockSizeArray;

  /* Verify parameter. */
  TBX_ASSERT(memPtr != NULL);

  /* Only continue if the parameter is valid. */
  if (memPtr != NULL)
  {
    /* Create a pointer to an array of size_t elements. */
    blockSizeArray = memPtr;
    /* The block size value is located at the start of the block, */
    size_t blockSize = blockSizeArray[0U];
    /* Set the result value. */
    result = blockSize;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockGetBlockSize ***/


/************************************************************************************//**
** \brief     Convert the pointer that points to where the block's data starts, to a
**            pointer that points to where the block's allocated memory starts.
** \param     dataPtr Pointer to the start of the block's data.
** \return    Pointer to the memory of the block if successful, NULL otherwise.
**
****************************************************************************************/
static void * TbxMemPoolBlockGetMemPtr(void * dataPtr)
{
  void   * result = NULL;
  void   * blockMemPtr;
  size_t * blockSizeArray;

  /* Verify parameter. */
  TBX_ASSERT(dataPtr != NULL);

  /* Only continue if the parameter is valid. */
  if (dataPtr != NULL)
  {
    /* Create a pointer to an array of size_t elements. */
    blockSizeArray = dataPtr;
    /* Get pointer to the start of the block, which is one size_t element before the
     * pointer to the block's data.
     */
    blockMemPtr = &blockSizeArray[-1];
    /* Set the result value. */
    result = blockMemPtr;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockGetMemPtr ***/


/****************************************************************************************
*   B L O C K   L I S T   M A N A G E M E N T   F U N C T I O N S
****************************************************************************************/

/************************************************************************************//**
** \brief     Creates a new and empty linked list that can be used for building a list
**            with block nodes. Memory needed for the linked list object itself is
**            allocated on the heap.
** \return    Pointer to the newly created blocks list if successful, NULL otherwise.
**
****************************************************************************************/
static tBlockList * TbxMemPoolBlockListCreate(void)
{
  tBlockList * result;

  /* Attempt to create the linked list object on the heap. */
  result = TbxHeapAllocate(sizeof(tBlockList));
  /* Only continue with the linked list initialization when the allocation was
   * successful.
   */
  if (result != NULL)
  {
    /* Initialize the linked list to be empty. */
    *result = NULL;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockListCreate ***/


/************************************************************************************//**
** \brief     Inserts the specified block node into the linked list with block nodes.
** \param     listPtr Pointer to the linked list with blocks.
** \param     nodePtr Pointer to the block node to insert.
**
****************************************************************************************/
static void TbxMemPoolBlockListInsert(tBlockList * listPtr, 
                                      tBlockNode * nodePtr)
{
  /* Verify parameters. */
  TBX_ASSERT(listPtr != NULL);
  TBX_ASSERT(nodePtr != NULL);

  /* Only continue if the parameters are valid. */
  if ( (listPtr != NULL) && (nodePtr != NULL) )
  {
    /* Is the list currently empty? */
    if (*listPtr == NULL)
    {
      /* There is no next node. */
      nodePtr->nextNodePtr = NULL;
    }
    /* Linked list is not empty. */
    else
    {
      /* The new node will be the current head of the list. */
      nodePtr->nextNodePtr = *listPtr;
    }
    /* Insert the new node at the start of the list. */
    *listPtr = nodePtr;
  }
} /*** end of TbxMemPoolBlockListInsert ***/


/************************************************************************************//**
** \brief     Extracts a block node from the linked list with block nodes.
** \param     listPtr Pointer to the linked list with blocks.
** \return    Pointer to the block node that was extracted or NULL if the linked list
**            contained no more nodes.
**
****************************************************************************************/
static tBlockNode * TbxMemPoolBlockListExtract(tBlockList * listPtr)
{
  tBlockNode * result = NULL;

  /* Verify parameter. */
  TBX_ASSERT(listPtr != NULL);

  /* Only continue if the parameter is valid. */
  if (listPtr != NULL)
  {
    /* Only extract a node if the list currently not empty. */
    if (*listPtr != NULL)
    {
      /* Get the first node. */
      result = *listPtr;
      /* Is there currently only one node in the list? */
      if ((*listPtr)->nextNodePtr == NULL)
      {
        /* Set the list to be empty. */
        *listPtr = NULL;
      }
      /* There are currently at least two nodes in the list. */
      else
      {
        /* Make the second node the first one. */
        *listPtr = result->nextNodePtr;
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockListExtract ***/


/************************************************************************************//**
** \brief     Checks if the linked list with block nodes is empty. So when it does not
**            contain any nodes.
** \param     listPtr Pointer to the linked list with blocks.
** \return    TBX_TRUE if the block list is empty, TBX_FALSE otherwise.
**
****************************************************************************************/
static uint8_t TbxMemPoolBlockListIsEmpty(tBlockList const * listPtr)
{
  uint8_t result = TBX_FALSE;

  /* Verify parameter. */
  TBX_ASSERT(listPtr != NULL);

  /* Only continue if the parameter is valid. */
  if (listPtr != NULL)
  {
    /* Is the list empty? */
    if (*listPtr == NULL)
    {
      /* Update the result value. */
      result = TBX_TRUE;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMemPoolBlockListIsEmpty ***/


/*********************************** end of tbx_mempool.c ******************************/
