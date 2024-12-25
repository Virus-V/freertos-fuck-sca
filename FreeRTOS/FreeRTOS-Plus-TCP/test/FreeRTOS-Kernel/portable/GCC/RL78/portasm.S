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
#include "ISR_Support.h"

    .global    _vPortYield
    .global    _vPortStartFirstTask
    .global    _vPortTickISR

    .extern    _vTaskSwitchContext
    .extern    _xTaskIncrementTick

    .text
    .align 2

/* FreeRTOS yield handler.  This is installed as the BRK software interrupt
handler. */
_vPortYield:
    /* Save the context of the current task. */
    portSAVE_CONTEXT
    /* Call the scheduler to select the next task. */
    call      !!_vTaskSwitchContext
    /* Restore the context of the next task to run. */
    portRESTORE_CONTEXT
    retb


/* Starts the scheduler by restoring the context of the task that will execute
first. */
    .align 2
_vPortStartFirstTask:
    /* Restore the context of whichever task will execute first. */
    portRESTORE_CONTEXT
    /* An interrupt stack frame is used so the task is started using RETI. */
    reti

/* FreeRTOS tick handler.  This is installed as the interval timer interrupt
handler. */
    .align 2
_vPortTickISR:

    /* Save the context of the currently executing task. */
    portSAVE_CONTEXT
    /* Call the RTOS tick function. */
    call      !!_xTaskIncrementTick
#if configUSE_PREEMPTION == 1
    /* Select the next task to run. */
    call      !!_vTaskSwitchContext
#endif
    /* Retore the context of whichever task will run next. */
    portRESTORE_CONTEXT
    reti

    .end
