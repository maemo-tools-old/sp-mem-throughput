/* This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2011-2012 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
Test functions for the following test cases:
1. Read 524288 x 4B words from sequential addresses (see ReadSequential())
2. Read 32768 x 4B words from sequential addresses 128 times (see ReadSequential())
3. Read 4096 x 4B words from sequential addresses 1024 times (see ReadSequential())
4. Read 4B words with 32B offset over 2MiB address range (see ReadPerCacheline())
5. Read 4B words with 4096B offset over 2MiB address range (see ReadPerPage())
6. Write 524288 x 4B words to sequential addresses (see WriteSequential())
7. Write 4B words with 32B offset over 2MiB address range (see WritePerCacheline())
8. Write 4B words with 4096B offset over 2MiB address range (see WritePerPage())
9. Memory copy 2MiB (see MemCopyTest())
10. Memory copy 128KiB 128 times (see MemCopyTest())
11. Memory copy 16KiB 4096 times (see MemCopyTest())
12. Memory fill 2MiB (see MemFillTest())
13. Stream copy benchmark with 2MiB arrays (see StreamCopy())
14. Stream scale benchmark with 2MiB arrays (see StreamScale())
15. Stream sum benchmark with 2MiB arrays (see StreamSum())
16. Stream triad benchmark with 2MiB arrays (see StreamTriad())
17. Memory copy from 8KiB buffer to 16MiB buffer (see MemCopyTest())
18. Memory copy from 16MiB buffer to 8KiB buffer (see MemCopyTest())
19. Memory copy from 8MiB buffer to 8MiB buffer (see MemCopyTest())
20. Memory copy from 4KiB buffer to 4KiB buffer 100000 times (see MemCopyTest())
21. Memory fill 8KiB buffer 100000 times (see MemFillTest())
22. Memory fill 16MiB buffer (see MemFillTest())
23. Memory allocation of 10B objects 500000 times (see MemAllocTest())
24. Memory allocation of 100B objects 200000 times (see MemAllocTest())
25. Memory allocation of 1000B objects 20000 times (see MemAllocTest())

Note1. The source code is manually extracted from Symbian devices test sw source code by mikko.k.koivisto@nokia.com

Note2. The source code is modified by mikko.k.koivisto@nokia.com

Note3. The source code does not work in isolation.
The test functions require test harness that call functions with parameters mentioned in test case descriptions above.
Proper parameters are also mentioned in comments of each function.

Note4. The source code does not contain any reference to timers.
Test duration should be measured in test environment specific manner.

Note5. Tests should be compiled with the best possible speed optimisation flags.
*/

#include <string.h> // for memcpy() and memset()

#define CACHE_LINESIZE      ((int)(32 / sizeof(int))) // cache line size (int)
#define PAGE_SIZE           ((int)(4096 / sizeof(int)))  // page size (int)

/*
Paramenters for test case 6:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void WriteSequential( int* aMemory, int aSize, int aIterations, int c )
    {
    while (aIterations--)
        {
        int loopcount = aSize;
        int* pos = aMemory;
        while (loopcount > 0)
            {
                *pos++ = c;
                *pos++ = c;
                *pos++ = c;
                *pos++ = c;
                *pos++ = c;
                *pos++ = c;
                *pos++ = c;
                *pos++ = c;

                loopcount -= sizeof(int) * 8;
            }
        }
    }

/*
Paramenters for test case 7:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void WritePerCacheline( int* aMemory, int aSize, int aIterations, int c )
    {
    while (aIterations--)
        {
        int offset = 0;
        while (offset < CACHE_LINESIZE)
            {
            int loopcount = aSize;
            int* pos = aMemory + offset;
            while (loopcount > 0)
                {
                pos[0] = c;
                pos[CACHE_LINESIZE] = c;
                pos[2 * CACHE_LINESIZE] = c;
                pos[3 * CACHE_LINESIZE] = c;
                pos[4 * CACHE_LINESIZE] = c;
                pos[5 * CACHE_LINESIZE] = c;
                pos[6 * CACHE_LINESIZE] = c;
                pos[7 * CACHE_LINESIZE] = c;

                loopcount -= sizeof(int) * 8 * CACHE_LINESIZE;
                pos += 8 * CACHE_LINESIZE;
                }
            offset++;
            }

        }
    }

/*
Paramenters for test case 8:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void WritePerPage( int* aMemory, int aSize, int aIterations, int c )
    {
    while (aIterations--)
        {
        int offset2 = 0;
        while (offset2 < CACHE_LINESIZE)
            {
            int offset = 0;
            while (offset < PAGE_SIZE)
                {
                int loopcount = aSize;
                int* pos = aMemory + offset + offset2;
                while (loopcount > 0)
                    {
                    pos[0] = c;
                    pos[PAGE_SIZE] = c;
                    pos[2 * PAGE_SIZE] = c;
                    pos[3 * PAGE_SIZE] = c;

                    loopcount -= sizeof(int) * 4 * PAGE_SIZE;
                    pos += 4 * PAGE_SIZE;
                    }
                offset += CACHE_LINESIZE;

                }
            offset2++;
            }
        }
    }


/*
Paramenters for test case 1:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1

Paramenters for test case 2:
aMemory = pointer to big enough memory buffer
aSize = 128 * 1024
aIterations = 128

Paramenters for test case 3:
aMemory = pointer to big enough memory buffer
aSize = 16 * 1024
aIterations = 1024
*/
int ReadSequential(const int* aMemory, int aSize, int aIterations)
    {
    int d1,d2,d3,d4,d5,d6,d7,d8,sum;
    sum = 0;
    d5 = 0;
    d6 = 0;
    d7 = 0;
    d8 = 0;
    while (aIterations--)
        {
        const int* pos = aMemory;
        int loopcount = aSize;

        // The whole memory block is read on after one
        // Read data size is 4 bytes (int size)
        // 0011 1100 1100 0011 1100 0011 1100 1100 1100 0000 1110 0011
        //
        // *pos---------------------*pos++-------------------
        while (loopcount > 0)
            {
            d1 = *pos++;
            d2 = *pos++;
            d3 = *pos++;
            d4 = *pos++;
            sum += d5 + d6 + d7 + d8;
            d5 = *pos++;
            d6 = *pos++;
            d7 = *pos++;
            d8 = *pos++;
            sum += d1 + d2 + d3 + d4;

            loopcount -= sizeof(int) * 8;
            }
        }
    // Sum must be returned. Otherwise compiler optimizes previous code so that nothing is done
    return sum;
    }

/*
Paramenters for test case 4:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
int ReadPerCacheline( const int* aMemory, int aSize, int aIterations )
    {
    int d1,d2,d3,d4,d5,d6,d7,d8,sum;
    sum = 0;
    d5 = 0;
    d6 = 0;
    d7 = 0;
    d8 = 0;
    while (aIterations--)
        {
        int offset = 0;
        // Every address is read from the address space
        // n[0],n[8],n[16]...
        // n[1],n[9],n[17]...
        //...
        //n[7],n[15],n[23]...

        while (offset < CACHE_LINESIZE)
            {
            int loopcount = aSize;
            const int* pos = aMemory + offset;

            // Offset is 32 Bytes
            // CACHE_LINESIZE is 8
            // pos[8] - pos[0]  is 32 Bytes because
            // sizeof(int) * 8 is 32
            while (loopcount > 0)
                {
                d1 = pos[0];
                d2 = pos[CACHE_LINESIZE];
                d3 = pos[2 * CACHE_LINESIZE];
                d4 = pos[3 * CACHE_LINESIZE];
                sum += d5 + d6 + d7 + d8;
                d5 = pos[4 * CACHE_LINESIZE];
                d6 = pos[5 * CACHE_LINESIZE];
                d7 = pos[6 * CACHE_LINESIZE];
                d8 = pos[7 * CACHE_LINESIZE];
                sum += d1 + d2 + d3 + d4;

                loopcount -= sizeof(int) * 8 * CACHE_LINESIZE;
                pos += 8 * CACHE_LINESIZE;
                }
            offset++;
            }

        }

    return sum;
    }

/*
Paramenters for test case 5:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
int ReadPerPage( const int* aMemory, int aSize, int aIterations )
    {
    int d1,d2,d3,d4,sum;
    sum = 0;
    while (aIterations--)
        {
        int offset2 = 0;
        while (offset2 < CACHE_LINESIZE)
            {
            int offset = 0;
            while (offset < PAGE_SIZE)
                {

                int loopcount = aSize;
                const int* pos = aMemory + offset + offset2;
                while (loopcount > 0)
                    {
                    d1 = pos[0];
                    d2 = pos[PAGE_SIZE];
                    d3 = pos[2 * PAGE_SIZE];
                    d4 = pos[3 * PAGE_SIZE];
                    sum += d1 + d2 + d3 + d4;

                    loopcount -= sizeof(int) * 4 * PAGE_SIZE;
                    pos += 4 * PAGE_SIZE;
                    }
                offset += CACHE_LINESIZE;
                }
            offset2++;
            }
        }
    return sum;

    }

/*
Paramenters for test case 13:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void StreamCopy( int* aMemory, int aSize, int aIterations )
    {
    int intsize = aSize / sizeof(int);
    int* a = aMemory;
    int* c = aMemory + intsize + intsize;
    int max = aSize / sizeof(int);
    while (aIterations--)
        {
        for (int j=0; j<max; j++)
            {
            c[j] = a[j];
            }
        }
    }

/*
Paramenters for test case 14:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void StreamScale( int* aMemory, int aSize, int aIterations )
    {
    int intsize = aSize / sizeof(int);
    int* b = aMemory + intsize;
    int* c = aMemory + intsize + intsize;
    int scalar = 3;
    int max = aSize / sizeof(int);
    while (aIterations--)
        {
        for (int j=0; j<max; j++)
            {
            b[j] = scalar*c[j];
            }
        }

    }

/*
Paramenters for test case 15:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void StreamSum( int* aMemory, int aSize, int aIterations )
    {
    int intsize = aSize / sizeof(int);
    int* a = aMemory;
    int* b = aMemory + intsize;
    int* c = aMemory + intsize + intsize;
    int max = aSize / sizeof(int);
    while (aIterations--)
        {
        for (int j=0; j<max; j++)
            {
            c[j] = a[j]+b[j];
            }
        }

    }

/*
Paramenters for test case 16:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1
*/
void StreamTriad( int* aMemory, int aSize, int aIterations )
    {
    int intsize = aSize / sizeof(int);
    int* a = aMemory;
    int* b = aMemory + intsize;
    int* c = aMemory + intsize + intsize;
    int scalar = 3;
    int max = aSize / sizeof(int);

    while (aIterations--)
        {
        for (int j=0; j<max; j++)
            {
            a[j] = b[j]+scalar*c[j];
            }
        }

    }

/*
Paramenters for test case 9:
aMemory = pointer to big enough memory buffer
aTargetSize = aSourceSize = 2048 * 1024
aIterations = 1

Paramenters for test case 10:
aMemory = pointer to big enough memory buffer
aTargetSize = aSourceSize = 128 * 1024
aIterations = 128

Paramenters for test case 11:
aMemory = pointer to big enough memory buffer
aTargetSize = aSourceSize = 16 * 1024
aIterations = 4096

Paramenters for test case 17:
aMemory = pointer to big enough memory buffer
aTargetSize = 16 * 1024 * 1024
aSourceSize = 8 * 1024
aIterations = 1

Paramenters for test case 18:
aMemory = pointer to big enough memory buffer
aTargetSize = 8 * 1024
aSourceSize = 16 * 1024 * 1024
aIterations = 1

Paramenters for test case 19:
aMemory = pointer to big enough memory buffer
aTargetSize = 8 * 1024 * 1024
aSourceSize = 8 * 1024 * 1024
aIterations = 1

Paramenters for test case 20:
aMemory = pointer to big enough memory buffer
aTargetSize = 4 * 1024
aSourceSize = 4 * 1024
aIterations = 100000
*/
void MemCopyTest( int* aMemory, int aTargetSize, int aSourceSize, int aIterations )
    {
    void* target = (void*) aMemory + aSourceSize;
    void* source = (void*) aMemory;

    // Resolve block size (Smaller of source and target)
    int blocksize = aTargetSize < aSourceSize ? aTargetSize : aSourceSize;
    int repetitions = (aTargetSize > aSourceSize ? aTargetSize : aSourceSize) / blocksize;

    while( aIterations-- )
        {
        while ( repetitions--)
            {
            memcpy(target, source, blocksize);
            if (aSourceSize > aTargetSize)
                {
                source += blocksize;
                }
            else if (aSourceSize < aTargetSize)
                {
                target += blocksize;
                }
            }
        }

    }

/*
Paramenters for test case 12:
aMemory = pointer to big enough memory buffer
aSize = 2048 * 1024
aIterations = 1

Paramenters for test case 21:
aMemory = pointer to big enough memory buffer
aSize = 8 * 1024
aIterations = 100000

Paramenters for test case 22:
aMemory = pointer to big enough memory buffer
aSize = 16 * 1024 * 1024
aIterations = 1
*/
void MemFillTest( int* aMemory, int aSize, int aIterations )
    {
    void* target = (void*) aMemory;

    while( aIterations-- )
        {
        memset( target, aSize, 42 );
        }

    }

/*
Paramenters for test case 23:
aMemory = pointer to big enough memory buffer
aSize = 10
aIterations = 500000

Paramenters for test case 24:
aMemory = pointer to big enough memory buffer
aSize = 100
aIterations = 200000

Paramenters for test case 25:
aMemory = pointer to big enough memory buffer
aSize = 1000
aIterations = 20000
*/

void MemAllocTest( int** aMemory, int aSize, int aIterations)
    {

    while ( aIterations-- )
        {
        *aMemory = malloc(aSize);
        aMemory++;
        }

    }

#include "routine.h"

void *
sym_write_sequential(void *ptr, int c, size_t n)
{
	c |= (c << 24) | (c << 16) | (c << 8);
	WriteSequential(ptr, n, 1, c);
	return ptr;
}
ROUTINE_REGISTER_MEMSET(sym_write_sequential,
	"Write memory sequentially 32 bytes per iteration with 4 byte aligned writes")

void *
sym_write_per_cacheline(void *ptr, int c, size_t n)
{
	c |= (c << 24) | (c << 16) | (c << 8);
	WritePerCacheline(ptr, n, 1, c);
	return ptr;
}
ROUTINE_REGISTER_MEMSET(sym_write_per_cacheline,
	"Striped 32 bytes per iteration striped to cachelines write routine with 4 byte aligned writes")

void *
sym_write_per_page(void *ptr, int c, size_t n)
{
	c |= (c << 24) | (c << 16) | (c << 8);
	WritePerPage(ptr, n, 1, c);
	return ptr;
}
ROUTINE_REGISTER_MEMSET(sym_write_per_page,
	"Striped 16 bytes per page per iteration write routine with 4 byte aligned writes")

void
sym_read_sequential(const void *ptr, size_t n)
{
	ReadSequential(ptr, n, 1);
}
ROUTINE_REGISTER_MEMREAD(sym_read_sequential,
	"Sequential read through input with 4 byte aligned reads")

void
sym_read_per_cacheline(const void *ptr, size_t n)
{
	ReadPerCacheline(ptr, n, 1);
}
ROUTINE_REGISTER_MEMREAD(sym_read_per_cacheline,
	"Read through input from two cachelines with 4 byte aligned reads")

void
sym_read_per_page(const void *ptr, size_t n)
{
	ReadPerPage(ptr, n, 1);
}
ROUTINE_REGISTER_MEMREAD(sym_read_per_page,
	"Read 16 bytes per page per iteration with 4 byte aligned reads")
