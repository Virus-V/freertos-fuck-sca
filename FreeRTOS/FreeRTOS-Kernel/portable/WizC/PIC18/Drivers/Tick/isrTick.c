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

/*
Changes from V3.0.0
    + ISRcode pulled inline to reduce stack-usage.

    + Added functionality to only call vTaskSwitchContext() once
      when handling multiple interruptsources in a single interruptcall.

    + Filename changed to a .c extension to allow stepping through code
      using F7.

Changes from V3.0.1
*/

/*
 * ISR for the tick.
 * This increments the tick count and, if using the preemptive scheduler,
 * performs a context switch.  This must be identical to the manual
 * context switch in how it stores the context of a task.
 */

#ifndef _FREERTOS_DRIVERS_TICK_ISRTICK_C
#define _FREERTOS_DRIVERS_TICK_ISRTICK_C

{
    /*
     * Was the interrupt the SystemClock?
     */
    if( bCCP1IF && bCCP1IE )
    {
        /*
         * Reset the interrupt flag
         */
        bCCP1IF = 0;

        /*
         * Maintain the tick count.
         */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /*
             * Ask for a switch to the highest priority task
             * that is ready to run.
             */
            uxSwitchRequested = pdTRUE;
        }
    }
}

#pragma wizcpp uselib     "$__PATHNAME__/Tick.c"

#endif  /* _FREERTOS_DRIVERS_TICK_ISRTICK_C */
