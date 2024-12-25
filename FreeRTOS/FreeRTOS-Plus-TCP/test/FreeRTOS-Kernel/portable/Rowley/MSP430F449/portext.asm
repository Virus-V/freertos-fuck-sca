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

#include "FreeRTOSConfig.h"
#include "portasm.h"


.CODE

/*
 * The RTOS tick ISR.
 *
 * If the cooperative scheduler is in use this simply increments the tick
 * count.
 *
 * If the preemptive scheduler is in use a context switch can also occur.
 */
_vTickISR:
        portSAVE_CONTEXT

        call    #_xTaskIncrementTick
        cmp.w   #0x00, r15
                jeq     _SkipContextSwitch
        call    #_vTaskSwitchContext
_SkipContextSwitch:
        portRESTORE_CONTEXT
/*-----------------------------------------------------------*/


/*
 * Manual context switch called by the portYIELD() macro.
 */
_vPortYield::

        /* Mimic an interrupt by pushing the SR. */
        push    SR

        /* Now the SR is stacked we can disable interrupts. */
        dint

        /* Save the context of the current task. */
        portSAVE_CONTEXT

        /* Switch to the highest priority task that is ready to run. */
        call    #_vTaskSwitchContext

        /* Restore the context of the new task. */
        portRESTORE_CONTEXT
/*-----------------------------------------------------------*/


/*
 * Start off the scheduler by initialising the RTOS tick timer, then restoring
 * the context of the first task.
 */
_xPortStartScheduler::

        /* Setup the hardware to generate the tick.  Interrupts are disabled
        when this function is called. */
        call    #_prvSetupTimerInterrupt

        /* Restore the context of the first task that is going to run. */
        portRESTORE_CONTEXT
/*-----------------------------------------------------------*/


        /* Place the tick ISR in the correct vector. */
        .VECTORS

        .KEEP

        ORG     TIMERA0_VECTOR
        DW      _vTickISR



        END

