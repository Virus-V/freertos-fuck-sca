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
#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      long
#define portLONG        long
#define portSHORT       int
#define portSTACK_TYPE  uint16_t
#define portBASE_TYPE   short

typedef portSTACK_TYPE StackType_t;
typedef short BaseType_t;
typedef unsigned short UBaseType_t;


#if( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
    typedef uint16_t TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS  == TICK_TYPE_WIDTH_32_BITS )
    typedef uint32_t             TickType_t;
    #define portMAX_DELAY    ( TickType_t ) 0xffffffffUL
#else
    #error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
#endif
/*-----------------------------------------------------------*/

/* Critical section handling. */
#define portENTER_CRITICAL()            __asm{ pushf }  \
                                        __asm{ cli   }  \

#define portEXIT_CRITICAL()             __asm{ popf }

#define portDISABLE_INTERRUPTS()        __asm{ cli }

#define portENABLE_INTERRUPTS()         __asm{ sti }
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portNOP()                       __asm{ nop }
#define portSTACK_GROWTH                ( -1 )
#define portSWITCH_INT_NUMBER           0x80
#define portYIELD()                     __asm{ int portSWITCH_INT_NUMBER }
#define portTICK_PERIOD_MS              ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT              2
#define portINITIAL_SW                  ( ( portSTACK_TYPE ) 0x0202 )   /* Start the tasks with interrupts enabled. */
/*-----------------------------------------------------------*/

/* Compiler specifics. */
#define portINPUT_BYTE( xAddr )             inp( xAddr )
#define portOUTPUT_BYTE( xAddr, ucValue )   outp( xAddr, ucValue )
#define portINPUT_WORD( xAddr )             inpw( xAddr )
#define portOUTPUT_WORD( xAddr, usValue )   outpw( xAddr, usValue )
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vTaskFunction, vParameters ) void vTaskFunction( void *pvParameters )
#define portTASK_FUNCTION( vTaskFunction, vParameters ) void vTaskFunction( void *pvParameters )

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */


#endif /* PORTMACRO_H */
