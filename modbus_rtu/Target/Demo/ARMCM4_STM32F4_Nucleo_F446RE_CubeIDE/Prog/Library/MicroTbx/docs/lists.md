# Linked lists

This software component consists of an easy-to-use set of functions for managing and
sorting items in a linked list. It builds upon the [memory pool](mempools.md)
functionality, which makes is possible to dynamically create, delete and automatically
grow linked lists in a convenient way. Think of a linked list as an array on steroids.
You do not yet have to know its size upon creation and you can insert, remove and
sort its items with ease.

A linked list is a sequence of data items, where each data item contains a connection
to the adjacent items. In this software component the definition of a data item is
a pointer to the data. The only thing you as the programmer need to take care of,
is that the data pointer actually points to valid data before inserting it into
the linked list.

Linked lists are commonly used data structures in software programs and they are
perfect for scenarios where you need to store a sequence of data, but you do not
yet know the maximum number of items to store ahead of time. An example would be a
first-in-first-out (FIFO) buffer.

## Usage

Linked list can be created and deleted at any time in the software program with
functions [`TbxListCreate()`](apiref.md#tbxlistcreate) and [`TbxListDelete()`](apiref.md#tbxlistdelete). The [`TbxListCreate()`](apiref.md#tbxlistcreate) function
returns a pointer to the new list. This list pointer serves as a handle to the list.
You can pass this handle as a parameter to all the other functions in the software
component to identify the list that should be operated on.

Once the list is created, you can start adding items to the list with functions
[`TbxListInsertItemFront()`](apiref.md#tbxlistinsertitemfront), [`TbxListInsertItemBack()`](apiref.md#tbxlistinsertitemback), [`TbxListInsertItemBefore()`](apiref.md#tbxlistinsertitembefore),
and [`TbxListInsertItemAfter()`](apiref.md#tbxlistinsertitemafter). The function to call depends on where in the list
you would like to add the item.

For reading items and for iterating over items, the functions [`TbxListGetFirstItem()`](apiref.md#tbxlistgetfirstitem),
[`TbxListGetLastItem()`](apiref.md#tbxlistgetlastitem), [`TbxListGetPreviousItem()`](apiref.md#tbxlistgetpreviousitem), and [`TbxListGetNextItem()`](apiref.md#tbxlistgetnextitem) are
available.

At any given time, you can obtain the number of items that are stored in the list
with function [`TbxListGetSize()`](apiref.md#tbxlistgetsize). Call function [`TbxListRemoveItem()`](apiref.md#tbxlistremoveitem) to remove
a single item from the list, or call [`TbxListClear()`](apiref.md#tbxlistclear) to remove all items at once.

For editing the order of the items in the list, functions [`TbxListSwapItems()`](apiref.md#tbxlistswapitems) and
[`TbxListSortItems()`](apiref.md#tbxlistsortitems) are available. When calling [`TbxListSortItems()`](apiref.md#tbxlistsortitems) you can
specify your own function that will be called during the sort operation. In this
callback function you can implement your own application specific logic for
comparing two data items, therefore giving you full control and flexibility
over how the sorting works.

## Examples

This section contains a few examples to demonstrate how the linked list software
component works. To keep the examples simple, some error checking of function
return values was omitted. The examples assume that the following type for an
arbitrary message is defined:

```c
typedef struct
{
  uint32_t id;
  uint8_t  len;
  uint8_t  data[8];
} tMsg;
```

### Example 1 - FIFO buffer

The first example demonstrates how quickly a first-in-first-out (FIFO) buffer
can be created. Keep in mind that a linked list holds pointers to data items. It
is up to the developer to allocate and release the memory for the data item.
Luckily, this is easy with the help of a [memory pool](mempools.md).

```c
tTbxList * msgBuffer;

void MsgBufferInit(void)
{
  /* Create the linked list. */
  msgBuffer = TbxListCreate();
  /* Create message memory pool with an initial size of 1. */
  TbxMemPoolCreate(1, sizeof(tMsg));
}

void MsgBufferAdd(tMsg const * msg)
{
  tMsg * newMsg;

  /* Attempt to allocate memory to store the message. */
  newMsg = TbxMemPoolAllocate(sizeof(tMsg));
  /* Automatically increase the memory pool if it was too small. */
  if (newMsg == NULL)
  {
    TbxMemPoolCreate(1, sizeof(tMsg));
    newMsg = TbxMemPoolAllocate(sizeof(tMsg));
  }
  /* Copy the message. */
  *newMsg = *msg;
  /* Add the message at the end of the list. */
  TbxListInsertItemBack(msgBuffer, newMsg);
}

uint8_t MsgBufferGet(tMsg * msg)
{
  uint8_t   result = TBX_ERROR;
  tMsg    * oldMsg;
  uint8_t   idx;

  /* Get the oldest message from the list, if any. */
  oldMsg = TbxListGetFirstItem(msgBuffer);
  if (oldMsg != NULL)
  {
    /* Delete it from the list now that we read it. */
    TbxListRemoveItem(msgBuffer, oldMsg);
    /* Copy the message contents. */
    *msg = *oldMsg;
    /* Give the allocated memory for the message back to the pool. */
    TbxMemPoolRelease(oldMsg);
    /* Update the result. */
    result = TBX_OK;
  }
  return result;
}
```

You now not only have a FIFO buffer for storing messages, but its implementation
is such that it can automatically grow in size. Memory pools form the foundation
for this, meaning that you do not have to worry about memory fragmentation.

The following code demonstrates how messages can be stored and retrieved from this
FIFO buffer:

```c
/* Initialize the message FIFO buffer. */
MsgBufferInit();

/* Add a few messages. */
tMsg newMsg1 = { 0x200, 8, { 1, 2, 3, 4, 5, 6, 7, 8} };
tMsg newMsg2 = { 0x300, 8, { 1, 2, 3, 4, 5, 6, 7, 8} };
tMsg newMsg3 = { 0x100, 8, { 1, 2, 3, 4, 5, 6, 7, 8} };
MsgBufferAdd(&newMsg1);
MsgBufferAdd(&newMsg2);
MsgBufferAdd(&newMsg3);

/* Extract messages from the FIFO buffer. */
tMsg msg;
while (MsgBufferGet(&msg) == TBX_OK)
{
  printf("  Item ID: 0x%x\n", msg.id);
}
```

After running this code, the following is shown:

```
Item ID: 0x200
Item ID: 0x300
Item ID: 0x100
```

### Example 2 - Iterate list

The second example demonstrates how to iterate through contents of a linked list.
The following function prints information about the list contents:

```c
void MsgBufferPrint(void)
{
  tMsg   * msg;
  size_t   cnt = 0;

  printf("Number of items: %d\n", TbxListGetSize(msgBuffer));
  msg = TbxListGetFirstItem(msgBuffer);
  while (msg != NULL)
  {
    printf("  Item %d ID: 0x%x\n", ++cnt, msg->id);
    msg = TbxListGetNextItem(msgBuffer, msg);
  }
}
```

After running this example code:

```c
/* Initialize the message FIFO buffer. */
MsgBufferInit();

/* Add a few messages. */
tMsg newMsg1 = { 0x200, 8, { 1, 2, 3, 4, 5, 6, 7, 8} };
tMsg newMsg2 = { 0x300, 8, { 1, 2, 3, 4, 5, 6, 7, 8} };
tMsg newMsg3 = { 0x100, 8, { 1, 2, 3, 4, 5, 6, 7, 8} };
MsgBufferAdd(&newMsg1);
MsgBufferAdd(&newMsg2);
MsgBufferAdd(&newMsg3);

/* Print current FIFO buffer contents. */
MsgBufferPrint();
```

The following is shown:

```
Number of items: 3
  Item 1 ID: 0x200
  Item 2 ID: 0x300
  Item 3 ID: 0x100
```

### Example 3 - Sort list

The third example builds upon example 2, where there are three messages in the
list. Let's assume you want to sort the list contents based on the message
identifier. For this you could implement the following function for comparing
two messages:

```c
uint8_t CompareMessage(void const * item1, void const * item2)
{
  uint8_t         result = TBX_FALSE;
  tMsg    const * msg1 = item1;
  tMsg    const * msg2 = item2;

  if (msg1->id > msg2->id)
  {
    result = TBX_TRUE;
  }
  return result;
}
```

To perform the actual sort operation, you could run this code:

```c
/* Print FIFO buffer contents before sorting. */
printf("--- Before sorting ---\n");
MsgBufferPrint();

/* Sort the buffer based on the message identifier. */
TbxListSortItems(msgBuffer, CompareMessage);

/* Print FIFO buffer contents after sorting. */
printf("--- After sorting ---\n");
MsgBufferPrint();
```

The following is shown:

```
--- Before sorting ---
Number of items: 3
  Item 1 ID: 0x200
  Item 2 ID: 0x300
  Item 3 ID: 0x100
--- After sorting ---
Number of items: 3
  Item 1 ID: 0x100
  Item 2 ID: 0x200
  Item 3 ID: 0x300
```

Note that the items in the list are now sorted based on the message identifier
in ascending order.

## Configuration

The linked list software component itself does not have to be configured. However, when
creating a linked list and inserting items into it, the memory needed is dynamically
allocated with the help of a memory pool. Because a memory pool takes memory from the
heap, make sure the heap size is configured large enough with the help of macro
[`TBX_CONF_HEAP_SIZE`](apiref.md#configuration):

```c
/** \brief Configure the size of the heap in bytes. */
#define TBX_CONF_HEAP_SIZE                       (2048U)
```
