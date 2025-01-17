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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"


/*-----------------------------------------------------------
* Implementation of functions defined in portable.h for the H8S port.
*----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/* When the task starts interrupts should be enabled. */
#define portINITIAL_CCR                    ( ( StackType_t ) 0x00 )

/* Hardware specific constants used to generate the RTOS tick from the TPU. */
#define portCLEAR_ON_TGRA_COMPARE_MATCH    ( ( uint8_t ) 0x20 )
#define portCLOCK_DIV_64                   ( ( uint8_t ) 0x03 )
#define portCLOCK_DIV                      ( ( uint32_t ) 64 )
#define portTGRA_INTERRUPT_ENABLE          ( ( uint8_t ) 0x01 )
#define portTIMER_CHANNEL                  ( ( uint8_t ) 0x02 )
#define portMSTP13                         ( ( uint16_t ) 0x2000 )

/*
 * Setup TPU channel one for the RTOS tick at the requested frequency.
 */
static void prvSetupTimerInterrupt( void );

/*
 * The ISR used by portYIELD(). This is installed as a trap handler.
 */
void vPortYield( void ) __attribute__( ( saveall, interrupt_handler ) );

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    uint32_t ulValue;

    /* This requires an even address. */
    ulValue = ( uint32_t ) pxTopOfStack;

    if( ulValue & 1UL )
    {
        pxTopOfStack = pxTopOfStack - 1;
    }

    /* Place a few bytes of known values on the bottom of the stack.
     * This is just useful for debugging. */
    pxTopOfStack--;
    *pxTopOfStack = 0xaa;
    pxTopOfStack--;
    *pxTopOfStack = 0xbb;
    pxTopOfStack--;
    *pxTopOfStack = 0xcc;
    pxTopOfStack--;
    *pxTopOfStack = 0xdd;

    /* The initial stack mimics an interrupt stack.  First there is the program
     * counter (24 bits). */
    ulValue = ( uint32_t ) pxCode;

    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );
    pxTopOfStack--;
    ulValue >>= 8UL;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );
    pxTopOfStack--;
    ulValue >>= 8UL;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );

    /* Followed by the CCR. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_CCR;

    /* Next all the general purpose registers - with the parameters being passed
     * in ER0.  The parameter order must match that used by the compiler when the
     * "saveall" function attribute is used. */

    /* ER6 */
    pxTopOfStack--;
    *pxTopOfStack = 0x66;
    pxTopOfStack--;
    *pxTopOfStack = 0x66;
    pxTopOfStack--;
    *pxTopOfStack = 0x66;
    pxTopOfStack--;
    *pxTopOfStack = 0x66;

    /* ER0 */
    ulValue = ( uint32_t ) pvParameters;

    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );
    pxTopOfStack--;
    ulValue >>= 8UL;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );
    pxTopOfStack--;
    ulValue >>= 8UL;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );
    pxTopOfStack--;
    ulValue >>= 8UL;
    *pxTopOfStack = ( StackType_t ) ( ulValue & 0xff );

    /* ER1 */
    pxTopOfStack--;
    *pxTopOfStack = 0x11;
    pxTopOfStack--;
    *pxTopOfStack = 0x11;
    pxTopOfStack--;
    *pxTopOfStack = 0x11;
    pxTopOfStack--;
    *pxTopOfStack = 0x11;

    /* ER2 */
    pxTopOfStack--;
    *pxTopOfStack = 0x22;
    pxTopOfStack--;
    *pxTopOfStack = 0x22;
    pxTopOfStack--;
    *pxTopOfStack = 0x22;
    pxTopOfStack--;
    *pxTopOfStack = 0x22;

    /* ER3 */
    pxTopOfStack--;
    *pxTopOfStack = 0x33;
    pxTopOfStack--;
    *pxTopOfStack = 0x33;
    pxTopOfStack--;
    *pxTopOfStack = 0x33;
    pxTopOfStack--;
    *pxTopOfStack = 0x33;

    /* ER4 */
    pxTopOfStack--;
    *pxTopOfStack = 0x44;
    pxTopOfStack--;
    *pxTopOfStack = 0x44;
    pxTopOfStack--;
    *pxTopOfStack = 0x44;
    pxTopOfStack--;
    *pxTopOfStack = 0x44;

    /* ER5 */
    pxTopOfStack--;
    *pxTopOfStack = 0x55;
    pxTopOfStack--;
    *pxTopOfStack = 0x55;
    pxTopOfStack--;
    *pxTopOfStack = 0x55;
    pxTopOfStack--;
    *pxTopOfStack = 0x55;

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    extern void * pxCurrentTCB;

    /* Setup the hardware to generate the tick. */
    prvSetupTimerInterrupt();

    /* Restore the context of the first task that is going to run.  This
     * mirrors the function epilogue code generated by the compiler when the
     * "saveall" function attribute is used. */
    asm volatile (
        "MOV.L      @_pxCurrentTCB, ER6         \n\t"
        "MOV.L      @ER6, ER7                   \n\t"
        "LDM.L      @SP+, (ER4-ER5)             \n\t"
        "LDM.L      @SP+, (ER0-ER3)             \n\t"
        "MOV.L      @ER7+, ER6                  \n\t"
        "RTE                                    \n\t"
        );

    ( void ) pxCurrentTCB;

    /* Should not get here. */
    return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* It is unlikely that the h8 port will get stopped. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch.  This is a trap handler.  The "saveall" function
 * attribute is used so the context is saved by the compiler prologue.  All
 * we have to do is save the stack pointer.
 */
void vPortYield( void )
{
    portSAVE_STACK_POINTER();
    vTaskSwitchContext();
    portRESTORE_STACK_POINTER();
}
/*-----------------------------------------------------------*/

/*
 * The interrupt handler installed for the RTOS tick depends on whether the
 * preemptive or cooperative scheduler is being used.
 */
#if ( configUSE_PREEMPTION == 1 )

/*
 * The preemptive scheduler is used so the ISR calls vTaskSwitchContext().
 * The function prologue saves the context so all we have to do is save
 * the stack pointer.
 */
    void vTickISR( void ) __attribute__( ( saveall, interrupt_handler ) );
    void vTickISR( void )
    {
        portSAVE_STACK_POINTER();

        if( xTaskIncrementTick() != pdFALSE )
        {
            vTaskSwitchContext();
        }

        /* Clear the interrupt. */
        TSR1 &= ~0x01;

        portRESTORE_STACK_POINTER();
    }

#else /* if ( configUSE_PREEMPTION == 1 ) */

/*
 * The cooperative scheduler is being used so all we have to do is
 * periodically increment the tick.  This can just be a normal ISR and
 * the "saveall" attribute is not required.
 */
    void vTickISR( void ) __attribute__( ( interrupt_handler ) );
    void vTickISR( void )
    {
        xTaskIncrementTick();

        /* Clear the interrupt. */
        TSR1 &= ~0x01;
    }

#endif /* if ( configUSE_PREEMPTION == 1 ) */
/*-----------------------------------------------------------*/

/*
 * Setup timer 1 compare match to generate a tick interrupt.
 */
static void prvSetupTimerInterrupt( void )
{
    const uint32_t ulCompareMatch = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) / portCLOCK_DIV;

    /* Turn the module on. */
    MSTPCR &= ~portMSTP13;

    /* Configure timer 1. */
    TCR1 = portCLEAR_ON_TGRA_COMPARE_MATCH | portCLOCK_DIV_64;

    /* Configure the compare match value for a tick of configTICK_RATE_HZ. */
    TGR1A = ulCompareMatch;

    /* Start the timer and enable the interrupt - we can do this here as
     * interrupts are globally disabled when this function is called. */
    TIER1 |= portTGRA_INTERRUPT_ENABLE;
    TSTR |= portTIMER_CHANNEL;
}
/*-----------------------------------------------------------*/
