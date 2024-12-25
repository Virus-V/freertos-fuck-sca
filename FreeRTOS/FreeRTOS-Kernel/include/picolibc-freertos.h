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

#ifndef INC_PICOLIBC_FREERTOS_H
#define INC_PICOLIBC_FREERTOS_H

/* Use picolibc TLS support to allocate space for __thread variables,
 * initialize them at thread creation and set the TLS context at
 * thread switch time.
 *
 * See the picolibc TLS docs:
 * https://github.com/picolibc/picolibc/blob/main/doc/tls.md
 * for additional information. */

#include <picotls.h>

#define configUSE_C_RUNTIME_TLS_SUPPORT    1

#define configTLS_BLOCK_TYPE               void *

#define picolibcTLS_SIZE                   ( ( portPOINTER_SIZE_TYPE ) _tls_size() )
#define picolibcSTACK_ALIGNMENT_MASK       ( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK )

#if __PICOLIBC_MAJOR__ > 1 || __PICOLIBC_MINOR__ >= 8

/* Picolibc 1.8 and newer have explicit alignment values provided
 * by the _tls_align() inline */
    #define picolibcTLS_ALIGNMENT_MASK    ( ( portPOINTER_SIZE_TYPE ) ( _tls_align() - 1 ) )
#else

/* For older Picolibc versions, use the general port alignment value */
    #define picolibcTLS_ALIGNMENT_MASK    ( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK )
#endif

/* Allocate thread local storage block off the end of the
 * stack. The picolibcTLS_SIZE macro returns the size (in
 * bytes) of the total TLS area used by the application.
 * Calculate the top of stack address. */
#if ( portSTACK_GROWTH < 0 )

    #define configINIT_TLS_BLOCK( xTLSBlock, pxTopOfStack )                                  \
    do {                                                                                     \
        xTLSBlock = ( void * ) ( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) -              \
                                   picolibcTLS_SIZE ) &                                      \
                                 ~picolibcTLS_ALIGNMENT_MASK );                              \
        pxTopOfStack = ( StackType_t * ) ( ( ( ( portPOINTER_SIZE_TYPE ) xTLSBlock ) - 1 ) & \
                                           ~picolibcSTACK_ALIGNMENT_MASK );                  \
        _init_tls( xTLSBlock );                                                              \
    } while( 0 )
#else /* portSTACK_GROWTH */
    #define configINIT_TLS_BLOCK( xTLSBlock, pxTopOfStack )                                          \
    do {                                                                                             \
        xTLSBlock = ( void * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack +                          \
                                   picolibcTLS_ALIGNMENT_MASK ) & ~picolibcTLS_ALIGNMENT_MASK );     \
        pxTopOfStack = ( StackType_t * ) ( ( ( ( ( portPOINTER_SIZE_TYPE ) xTLSBlock ) +             \
                                               picolibcTLS_SIZE ) + picolibcSTACK_ALIGNMENT_MASK ) & \
                                           ~picolibcSTACK_ALIGNMENT_MASK );                          \
        _init_tls( xTLSBlock );                                                                      \
    } while( 0 )
#endif /* portSTACK_GROWTH */

#define configSET_TLS_BLOCK( xTLSBlock )    _set_tls( xTLSBlock )

#define configDEINIT_TLS_BLOCK( xTLSBlock )

#endif /* INC_PICOLIBC_FREERTOS_H */
