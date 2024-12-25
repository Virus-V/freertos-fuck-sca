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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include <machine/cpu.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR          char
#define portFLOAT         float
#define portDOUBLE        double
#define portLONG          long
#define portSHORT         short
#define portSTACK_TYPE    uint32_t
#define portBASE_TYPE     long

typedef portSTACK_TYPE   StackType_t;
typedef long             BaseType_t;
typedef unsigned long    UBaseType_t;

#if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
    typedef uint16_t     TickType_t;
    #define portMAX_DELAY    ( TickType_t ) 0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
    typedef uint32_t     TickType_t;
    #define portMAX_DELAY    ( TickType_t ) ( 0xFFFFFFFFUL )
#else
    #error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH               ( -1 )
#define portTICK_PERIOD_MS             ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT             4
#define portNOP()    __asm__ volatile ( "mov r0, r0" )
#define portCRITICAL_NESTING_IN_TCB    1
#define portIRQ_TRAP_YIELD             31
/*-----------------------------------------------------------*/

/* Task utilities. */

extern void vPortYield( void );

/*---------------------------------------------------------------------------*/

#define portYIELD()    asm __volatile__ ( " trap #%0 " : : "i" ( portIRQ_TRAP_YIELD ) : "memory" )
/*---------------------------------------------------------------------------*/

extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL()    vTaskEnterCritical()
#define portEXIT_CRITICAL()     vTaskExitCritical()
/*---------------------------------------------------------------------------*/

/* Critical section management. */
#define portDISABLE_INTERRUPTS()    cpu_int_disable()
#define portENABLE_INTERRUPTS()     cpu_int_enable()

/*---------------------------------------------------------------------------*/

#define portYIELD_FROM_ISR( xHigherPriorityTaskWoken )    do { if( xHigherPriorityTaskWoken != pdFALSE ) vTaskSwitchContext( ); } while( 0 )

/*---------------------------------------------------------------------------*/

#define portSAVE_CONTEXT()                                                                                      \
    asm __volatile__                                                                                            \
    (                                                                                                           \
        "sub    r1, #68                 \n" /* Make space on the stack for the context. */                      \
        "std    r2, [r1] +  0           \n"                                                                     \
        "stq    r4, [r1] +  8           \n"                                                                     \
        "stq    r8, [r1] +  24          \n"                                                                     \
        "stq    r12, [r1] + 40          \n"                                                                     \
        "mov    r6, rtt                 \n"                                                                     \
        "mov    r7, psr                 \n"                                                                     \
        "std    r6, [r1] +  56          \n"                                                                     \
        "movhi  r2, #16384              \n"         /* Set the pointer to the IC. */                            \
        "ldub   r3, [r2] + 2            \n"         /* Load the current interrupt mask. */                      \
        "st     r3, [r1]+ 64            \n"         /* Store the interrupt mask on the stack. */                \
        "ld     r2, [r0]+short(pxCurrentTCB)    \n" /* Load the pointer to the TCB. */                          \
        "st     r1, [r2]                \n"         /* Save the stack pointer into the TCB. */                  \
        "mov    r14, r1                 \n"         /* Compiler expects r14 to be set to the function stack. */ \
    );
/*---------------------------------------------------------------------------*/

#define portRESTORE_CONTEXT()                                                                                 \
    asm __volatile__ (                                                                                        \
        "ld     r2, [r0]+short(pxCurrentTCB)    \n" /* Load the TCB to find the stack pointer and context. */ \
        "ld     r1, [r2]                \n"                                                                   \
        "movhi  r2, #16384              \n"         /* Set the pointer to the IC. */                          \
        "ld     r3, [r1] + 64           \n"         /* Load the previous interrupt mask. */                   \
        "stb    r3, [r2] + 2            \n"         /* Set the current interrupt mask to be the previous. */  \
        "ldd    r6, [r1] + 56           \n"         /* Restore context. */                                    \
        "mov    rtt, r6                 \n"                                                                   \
        "mov    psr, r7                 \n"                                                                   \
        "ldd    r2, [r1] + 0            \n"                                                                   \
        "ldq    r4, [r1] +  8           \n"                                                                   \
        "ldq    r8, [r1] +  24          \n"                                                                   \
        "ldq    r12, [r1] + 40          \n"                                                                   \
        "add    r1, #68                 \n"                                                                   \
        "rti                            \n"                                                                   \
        );

/*---------------------------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )
/*---------------------------------------------------------------------------*/

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
