/*
   (c) Copyright 2001-2009  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   Fast memcpy code was taken from xine (see below).

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "routine.h"
#include <inttypes.h>

static void *
direct_memcpy_8( void * to, const void * from, size_t len )
{
     register uint8_t       *d = (uint8_t*)to;
     register const uint8_t *s = (const uint8_t*)from;
     size_t             n;

     if (len >= 128) {
          unsigned long delta;

          /* Align destination to 8-byte boundary */
          delta = (unsigned long)d & 7;
          if (delta) {
               len -= 8 - delta;

               if ((unsigned long)d & 1) {
                    *d++ = *s++;
               }
               if ((unsigned long)d & 2) {
                    *((uint16_t*)d) = *((const uint16_t*)s);
                    d += 2; s += 2;
               }
               if ((unsigned long)d & 4) {
                    *((uint32_t*)d) = *((const uint32_t*)s);
                    d += 4; s += 4;
               }
          }

          n    = len >> 6;
          len &= 63;

          for (; n; n--) {
               ((uint64_t*)d)[0] = ((const uint64_t*)s)[0];
               ((uint64_t*)d)[1] = ((const uint64_t*)s)[1];
               ((uint64_t*)d)[2] = ((const uint64_t*)s)[2];
               ((uint64_t*)d)[3] = ((const uint64_t*)s)[3];
               ((uint64_t*)d)[4] = ((const uint64_t*)s)[4];
               ((uint64_t*)d)[5] = ((const uint64_t*)s)[5];
               ((uint64_t*)d)[6] = ((const uint64_t*)s)[6];
               ((uint64_t*)d)[7] = ((const uint64_t*)s)[7];
               d += 64; s += 64;
          }
     }
     /*
      * Now do the tail of the block
      */
     if (len) {
          n = len >> 3;

          for (; n; n--) {
               *((uint64_t*)d) = *((const uint64_t*)s);
               d += 8; s += 8;
          }
          if (len & 4) {
               *((uint32_t*)d) = *((const uint32_t*)s);
               d += 4; s += 4;
          }
          if (len & 2)  {
               *((uint16_t*)d) = *((const uint16_t*)s);
               d += 2; s += 2;
          }
          if (len & 1)
               *d = *s;
     }

     return to;
}

ROUTINE_REGISTER_MEMCPY(direct_memcpy_8,
     "DirectFB.org memcpy written in C with 8 byte writes")
