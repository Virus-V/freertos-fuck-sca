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

/*-----------------------------------------------------------
* Implementation of functions defined in portable.h for the SH2A port.
*----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include "string.h"

/* Hardware specifics. */
#include <iorx62n.h>

/*-----------------------------------------------------------*/

/* Tasks should start with interrupts enabled and in Supervisor mode, therefore
 * PSW is set with U and I set, and PM and IPL clear. */
#define portINITIAL_PSW     ( ( StackType_t ) 0x00030000 )
#define portINITIAL_FPSW    ( ( StackType_t ) 0x00000100 )

/*-----------------------------------------------------------*/

/*
 * Function to start the first task executing - written in asm code as direct
 * access to registers is required.
 */
extern void prvStartFirstTask( void );

/*
 * The tick ISR handler.  The peripheral used is configured by the application
 * via a hook/callback function.
 */
__interrupt void vTickISR( void );

/*-----------------------------------------------------------*/

extern void * pxCurrentTCB;

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* R0 is not included as it is the stack pointer. */

    *pxTopOfStack = 0x00;
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_PSW;
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxCode;

    /* When debugging it can be useful if every register is set to a known
     * value.  Otherwise code space can be saved by just setting the registers
     * that need to be set. */
    #ifdef USE_FULL_REGISTER_INITIALISATION
    {
        pxTopOfStack--;
        *pxTopOfStack = 0xffffffff; /* r15. */
        pxTopOfStack--;
        *pxTopOfStack = 0xeeeeeeee;
        pxTopOfStack--;
        *pxTopOfStack = 0xdddddddd;
        pxTopOfStack--;
        *pxTopOfStack = 0xcccccccc;
        pxTopOfStack--;
        *pxTopOfStack = 0xbbbbbbbb;
        pxTopOfStack--;
        *pxTopOfStack = 0xaaaaaaaa;
        pxTopOfStack--;
        *pxTopOfStack = 0x99999999;
        pxTopOfStack--;
        *pxTopOfStack = 0x88888888;
        pxTopOfStack--;
        *pxTopOfStack = 0x77777777;
        pxTopOfStack--;
        *pxTopOfStack = 0x66666666;
        pxTopOfStack--;
        *pxTopOfStack = 0x55555555;
        pxTopOfStack--;
        *pxTopOfStack = 0x44444444;
        pxTopOfStack--;
        *pxTopOfStack = 0x33333333;
        pxTopOfStack--;
        *pxTopOfStack = 0x22222222;
        pxTopOfStack--;
    }
    #else /* ifdef USE_FULL_REGISTER_INITIALISATION */
    {
        pxTopOfStack -= 15;
    }
    #endif /* ifdef USE_FULL_REGISTER_INITIALISATION */

    *pxTopOfStack = ( StackType_t ) pvParameters; /* R1 */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_FPSW;
    pxTopOfStack--;
    *pxTopOfStack = 0x12345678; /* Accumulator. */
    pxTopOfStack--;
    *pxTopOfStack = 0x87654321; /* Accumulator. */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    extern void vApplicationSetupTimerInterrupt( void );

    /* Use pxCurrentTCB just so it does not get optimised away. */
    if( pxCurrentTCB != NULL )
    {
        /* Call an application function to set up the timer that will generate the
         * tick interrupt.  This way the application can decide which peripheral to
         * use.  A demo application is provided to show a suitable example. */
        vApplicationSetupTimerInterrupt();

        /* Enable the software interrupt. */
        _IEN( _ICU_SWINT ) = 1;

        /* Ensure the software interrupt is clear. */
        _IR( _ICU_SWINT ) = 0;

        /* Ensure the software interrupt is set to the kernel priority. */
        _IPR( _ICU_SWINT ) = configKERNEL_INTERRUPT_PRIORITY;

        /* Start the first task. */
        prvStartFirstTask();
    }

    /* Should not get here. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

#pragma vector = configTICK_VECTOR
__interrupt void vTickISR( void )
{
    /* Re-enable interrupts. */
    __enable_interrupt();

    /* Increment the tick, and perform any processing the new tick value
     * necessitates. */
    __set_interrupt_level( configMAX_SYSCALL_INTERRUPT_PRIORITY );
    {
        if( xTaskIncrementTick() != pdFALSE )
        {
            taskYIELD();
        }
    }
    __set_interrupt_level( configKERNEL_INTERRUPT_PRIORITY );
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to.
     * Artificially force an assert. */
    configASSERT( pxCurrentTCB == NULL );
}
/*-----------------------------------------------------------*/
