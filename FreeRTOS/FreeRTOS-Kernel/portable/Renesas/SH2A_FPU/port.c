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

/*-----------------------------------------------------------*/

/* The SR assigned to a newly created task.  The only important thing in this
 * value is for all interrupts to be enabled. */
#define portINITIAL_SR                 ( 0UL )

/* Dimensions the array into which the floating point context is saved.
 * Allocate enough space for FPR0 to FPR15, FPUL and FPSCR, each of which is 4
 * bytes big.  If this number is changed then the 72 in portasm.src also needs
 * changing. */
#define portFLOP_REGISTERS_TO_STORE    ( 18 )
#define portFLOP_STORAGE_SIZE          ( portFLOP_REGISTERS_TO_STORE * 4 )

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
    #error configSUPPORT_DYNAMIC_ALLOCATION must be 1 to use this port.
#endif

/*-----------------------------------------------------------*/

/*
 * The TRAPA handler used to force a context switch.
 */
void vPortYield( void );

/*
 * Function to start the first task executing - defined in portasm.src.
 */
extern void vPortStartFirstTask( void );

/*
 * Obtains the current GBR value - defined in portasm.src.
 */
extern uint32_t ulPortGetGBR( void );

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* Mark the end of the stack - used for debugging only and can be removed. */
    *pxTopOfStack = 0x11111111UL;
    pxTopOfStack--;
    *pxTopOfStack = 0x22222222UL;
    pxTopOfStack--;
    *pxTopOfStack = 0x33333333UL;
    pxTopOfStack--;

    /* SR. */
    *pxTopOfStack = portINITIAL_SR;
    pxTopOfStack--;

    /* PC. */
    *pxTopOfStack = ( uint32_t ) pxCode;
    pxTopOfStack--;

    /* PR. */
    *pxTopOfStack = 15;
    pxTopOfStack--;

    /* 14. */
    *pxTopOfStack = 14;
    pxTopOfStack--;

    /* R13. */
    *pxTopOfStack = 13;
    pxTopOfStack--;

    /* R12. */
    *pxTopOfStack = 12;
    pxTopOfStack--;

    /* R11. */
    *pxTopOfStack = 11;
    pxTopOfStack--;

    /* R10. */
    *pxTopOfStack = 10;
    pxTopOfStack--;

    /* R9. */
    *pxTopOfStack = 9;
    pxTopOfStack--;

    /* R8. */
    *pxTopOfStack = 8;
    pxTopOfStack--;

    /* R7. */
    *pxTopOfStack = 7;
    pxTopOfStack--;

    /* R6. */
    *pxTopOfStack = 6;
    pxTopOfStack--;

    /* R5. */
    *pxTopOfStack = 5;
    pxTopOfStack--;

    /* R4. */
    *pxTopOfStack = ( uint32_t ) pvParameters;
    pxTopOfStack--;

    /* R3. */
    *pxTopOfStack = 3;
    pxTopOfStack--;

    /* R2. */
    *pxTopOfStack = 2;
    pxTopOfStack--;

    /* R1. */
    *pxTopOfStack = 1;
    pxTopOfStack--;

    /* R0 */
    *pxTopOfStack = 0;
    pxTopOfStack--;

    /* MACL. */
    *pxTopOfStack = 16;
    pxTopOfStack--;

    /* MACH. */
    *pxTopOfStack = 17;
    pxTopOfStack--;

    /* GBR. */
    *pxTopOfStack = ulPortGetGBR();

    /* GBR = global base register.
    * VBR = vector base register.
    * TBR = jump table base register.
    * R15 is the stack pointer. */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    extern void vApplicationSetupTimerInterrupt( void );

    /* Call an application function to set up the timer that will generate the
     * tick interrupt.  This way the application can decide which peripheral to
     * use.  A demo application is provided to show a suitable example. */
    vApplicationSetupTimerInterrupt();

    /* Start the first task.  This will only restore the standard registers and
     * not the flop registers.  This does not really matter though because the only
     * flop register that is initialised to a particular value is fpscr, and it is
     * only initialised to the current value, which will still be the current value
     * when the first task starts executing. */
    trapa( portSTART_SCHEDULER_TRAP_NO );

    /* Should not get here. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented as there is nothing to return to. */
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
    int32_t lInterruptMask;

    /* Ensure the yield trap runs at the same priority as the other interrupts
     * that can cause a context switch. */
    lInterruptMask = get_imask();

    /* taskYIELD() can only be called from a task, not an interrupt, so the
     * current interrupt mask can only be 0 or portKERNEL_INTERRUPT_PRIORITY and
     * the mask can be set without risk of accidentally lowering the mask value. */
    set_imask( portKERNEL_INTERRUPT_PRIORITY );

    trapa( portYIELD_TRAP_NO );

    /* Restore the interrupt mask to whatever it was previously (when the
     * function was entered). */
    set_imask( ( int ) lInterruptMask );
}
/*-----------------------------------------------------------*/

BaseType_t xPortUsesFloatingPoint( TaskHandle_t xTask )
{
    uint32_t * pulFlopBuffer;
    BaseType_t xReturn;
    extern void * volatile pxCurrentTCB;

    /* This function tells the kernel that the task referenced by xTask is
     * going to use the floating point registers and therefore requires the
     * floating point registers saved as part of its context. */

    /* Passing NULL as xTask is used to indicate that the calling task is the
     * subject task - so pxCurrentTCB is the task handle. */
    if( xTask == NULL )
    {
        xTask = ( TaskHandle_t ) pxCurrentTCB;
    }

    /* Allocate a buffer large enough to hold all the flop registers. */
    pulFlopBuffer = ( uint32_t * ) pvPortMalloc( portFLOP_STORAGE_SIZE );

    if( pulFlopBuffer != NULL )
    {
        /* Start with the registers in a benign state. */
        memset( ( void * ) pulFlopBuffer, 0x00, portFLOP_STORAGE_SIZE );

        /* The first thing to get saved in the buffer is the FPSCR value -
         * initialise this to the current FPSCR value. */
        *pulFlopBuffer = get_fpscr();

        /* Use the task tag to point to the flop buffer.  Pass pointer to just
         * above the buffer because the flop save routine uses a pre-decrement. */
        vTaskSetApplicationTaskTag( xTask, ( void * ) ( pulFlopBuffer + portFLOP_REGISTERS_TO_STORE ) );
        xReturn = pdPASS;
    }
    else
    {
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/
