/*
 * FreeRTOS Kernel V11.1.0
 * Copyright (C) 2024 Fuck SCA.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* THIS SAMPLE CODE IS PROVIDED AS IS AND IS SUBJECT TO ALTERATIONS. FUJITSU */
/* MICROELECTRONICS ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR */
/* ELIGIBILITY FOR ANY PURPOSES.                                             */
/*                 (C) Fujitsu Microelectronics Europe GmbH                  */
/*---------------------------------------------------------------------------
  __STD_LIB_sbrk.C
  - Used by heap_3.c for memory allocation and deletion.

/*---------------------------------------------------------------------------*/

#include "FreeRTOSConfig.h"
#include <stdlib.h>

    static  long         brk_siz  =  0;
    typedef int          _heep_t;
    #define ROUNDUP(s)   (((s)+sizeof(_heep_t)-1)&~(sizeof(_heep_t)-1))
    static  _heep_t      _heep[ROUNDUP(configTOTAL_HEAP_SIZE)/sizeof(_heep_t)];
    #define              _heep_size      ROUNDUP(configTOTAL_HEAP_SIZE)

    extern  char  *sbrk(int  size)
    {
       if  (brk_siz  +  size  >  _heep_size  ||  brk_siz  +  size  <  0)

          return((char*)-1);
       brk_siz  +=  size;
       return(  (char*)_heep  +  brk_siz  -  size);
    }
