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
    \file   itempool.c

    \brief	see itempool.h

    \details see itempool.h
	
*/

#include "itempool.h"

/*
 tmp = (int*)ITEMPOOL_ALLOC(pool);
 00442E    403F 0005          mov.w   #0x5,R15          !!!! pass param
 004432    432E               mov.w   #0x2,R14          !!!! pass param
 004434    403D 1C00          mov.w   #0x1C00,R13       !!!! pass param
 004438    403C 1C06          mov.w   #0x1C06,R12       !!!! pass param
 00443C    13B0 4482          calla   #itempool_alloc
 004440    4C0A               mov.w   R12,R10
 */
void* itempool_alloc(uint8_t* items, uint8_t* status, size_t itemsize, size_t poolsize)
{
    size_t tmp;
	
    for (tmp=0; tmp < poolsize; tmp++)
    {
        if (status[tmp] == ITEMPOOL_ITEM_FREE)
        {
            status[tmp] =  ITEMPOOL_ITEM_USED;
            return items;
        }
        items+=itemsize;
    }

    return NULL;
}


void* itempool_calloc(uint8_t* items, uint8_t* status, size_t itemsize, size_t poolsize)
{
    items = itempool_alloc(items,status,itemsize,poolsize);

    if (items)
    {
        memset(items,0x00,itemsize);
    }

    return items;
}

