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

/* Standard includes. */
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Machine includes */
#include <machine/counter.h>
#include <machine/ic.h>
/*-----------------------------------------------------------*/

/* The initial PSR has the Previous Interrupt Enabled (PIEN) flag set. */
#define portINITIAL_PSR    ( 0x00020000 )

/*-----------------------------------------------------------*/

/*
 * Perform any hardware configuration necessary to generate the tick interrupt.
 */
static void prvSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* Make space on the stack for the context - this leaves a couple of spaces
     * empty.  */
    pxTopOfStack -= 20;

    /* Fill the registers with known values to assist debugging. */
    pxTopOfStack[ 16 ] = 0;
    pxTopOfStack[ 15 ] = portINITIAL_PSR;
    pxTopOfStack[ 14 ] = ( uint32_t ) pxCode;
    pxTopOfStack[ 13 ] = 0x00000000UL; /* R15. */
    pxTopOfStack[ 12 ] = 0x00000000UL; /* R14. */
    pxTopOfStack[ 11 ] = 0x0d0d0d0dUL;
    pxTopOfStack[ 10 ] = 0x0c0c0c0cUL;
    pxTopOfStack[ 9 ] = 0x0b0b0b0bUL;
    pxTopOfStack[ 8 ] = 0x0a0a0a0aUL;
    pxTopOfStack[ 7 ] = 0x09090909UL;
    pxTopOfStack[ 6 ] = 0x08080808UL;
    pxTopOfStack[ 5 ] = 0x07070707UL;
    pxTopOfStack[ 4 ] = 0x06060606UL;
    pxTopOfStack[ 3 ] = 0x05050505UL;
    pxTopOfStack[ 2 ] = 0x04040404UL;
    pxTopOfStack[ 1 ] = 0x03030303UL;
    pxTopOfStack[ 0 ] = ( uint32_t ) pvParameters;

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    /* Set-up the timer interrupt. */
    prvSetupTimerInterrupt();

    /* Integrated Interrupt Controller: Enable all interrupts. */
    ic->ien = 1;

    /* Restore callee saved registers. */
    portRESTORE_CONTEXT();

    /* Should not get here. */
    return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{
    /* Enable timer interrupts */
    counter1->reload = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1;
    counter1->value = counter1->reload;
    counter1->mask = 1;

    /* Set the IRQ Handler priority and enable it. */
    irq[ IRQ_COUNTER1 ].ien = 1;
}
/*-----------------------------------------------------------*/

/* Trap 31 handler. */
void interrupt31_handler( void ) __attribute__( ( naked ) );
void interrupt31_handler( void )
{
    portSAVE_CONTEXT();
    __asm volatile ( "call vTaskSwitchContext" );
    portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

static void prvProcessTick( void ) __attribute__( ( noinline ) );
static void prvProcessTick( void )
{
    if( xTaskIncrementTick() != pdFALSE )
    {
        vTaskSwitchContext();
    }

    /* Clear the Tick Interrupt. */
    counter1->expired = 0;
}
/*-----------------------------------------------------------*/

/* Timer 1 interrupt handler, used for tick interrupt. */
void interrupt7_handler( void ) __attribute__( ( naked ) );
void interrupt7_handler( void )
{
    portSAVE_CONTEXT();
    prvProcessTick();
    portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Nothing to do. Unlikely to want to end. */
}
/*-----------------------------------------------------------*/
