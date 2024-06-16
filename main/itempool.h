/*
   Copyright 2013-2023 Marco Bacchi <marco@bacchi.at>
   
   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:
   
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
*/


/*!
    \file   itempool.h

    \brief

    \details Memory pools, also known as fixed-size block allocation, enable dynamic memory allocation similar to malloc or C++'s operator new. However, these implementations are prone to fragmentation due to variable block sizes and are therefore not recommended for use in real-time systems where performance is critical. To address this issue, a more efficient approach is to preallocate a fixed number of memory blocks of the same size, which is known as a memory pool. During runtime, the application can allocate, access, and free blocks represented by handles, resulting in improved performance.
*/
#ifndef ITEMPOOL_H_
#define ITEMPOOL_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define ITEMPOOL_ITEM_FREE 0
#define ITEMPOOL_ITEM_USED 1

#define ITEMPOOL_TYPEDEF(name,type,size) \
    typedef struct { \
        uint8_t status[size]; \
        type items[size]; \
    }name##_itempool_t

#define ITEMPOOL_T(name) \
    name##_itempool_t

#define ITEMPOOL_INIT(itempool) \
        memset(ITEMPOOL_STATUS(itempool), \
               ITEMPOOL_ITEM_FREE, \
               ITEMPOOL_SIZE(itempool))

#define ITEMPOOL_SIZE(itempool) \
    (sizeof(ITEMPOOL_STATUS(itempool))/ \
     sizeof(*ITEMPOOL_STATUS(itempool)))

#define ITEMPOOL_ITEM_SIZE(itempool) \
    (sizeof(*ITEMPOOL_ITEMS(itempool)))

#define ITEMPOOL_STATUS(itempool) \
        ((itempool).status)

#define ITEMPOOL_ITEMS(itempool) \
        ((itempool).items)

#define ITEMPOOL_ALLOC(itempool) \
    itempool_alloc( \
        (uint8_t*)ITEMPOOL_ITEMS(itempool), \
        ITEMPOOL_STATUS(itempool), \
        ITEMPOOL_ITEM_SIZE(itempool), \
        ITEMPOOL_SIZE(itempool))

#define ITEMPOOL_CALLOC(itempool) \
    itempool_calloc( \
        (uint8_t*)ITEMPOOL_ITEMS(itempool), \
        ITEMPOOL_STATUS(itempool), \
        ITEMPOOL_ITEM_SIZE(itempool), \
        ITEMPOOL_SIZE(itempool))


/*
 ITEMPOOL_FREE(pool,tmp);
 00445C    4A0F               mov.w   R10,R15
 00445E    803F 1C06          sub.w   #0x1C06,R15
 004462    035F               rrum.w  #1,R15
 004464    43CF 1C00          clr.b   0x1C00(R15)
 */
#define ITEMPOOL_FREE(itempool,itemptr) \
        do{ITEMPOOL_STATUS(itempool) \
            [((uint8_t*)itemptr-(uint8_t*)ITEMPOOL_ITEMS(itempool))/ \
             ITEMPOOL_ITEM_SIZE(itempool)] \
             =ITEMPOOL_ITEM_FREE;}while(0)


void* itempool_alloc(uint8_t* items, uint8_t* status, size_t itemsize, size_t poolsize);
void* itempool_calloc(uint8_t* items, uint8_t* status, size_t itemsize, size_t poolsize);

#endif /* ITEMPOOL_H_ */
