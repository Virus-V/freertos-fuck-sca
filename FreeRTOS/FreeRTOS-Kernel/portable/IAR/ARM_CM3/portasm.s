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

#include <FreeRTOSConfig.h>

    RSEG    CODE:CODE(2)
    thumb

    EXTERN pxCurrentTCB
    EXTERN vTaskSwitchContext

    PUBLIC xPortPendSVHandler
    PUBLIC vPortSVCHandler
    PUBLIC vPortStartFirstTask



/*-----------------------------------------------------------*/

xPortPendSVHandler:
    mrs r0, psp
    isb
    ldr r3, =pxCurrentTCB           /* Get the location of the current TCB. */
    ldr r2, [r3]

    stmdb r0!, {r4-r11}             /* Save the remaining registers. */
    str r0, [r2]                    /* Save the new top of stack into the first member of the TCB. */

    stmdb sp!, {r3, r14}
    mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri, r0
    dsb
    isb
    bl vTaskSwitchContext
    mov r0, #0
    msr basepri, r0
    ldmia sp!, {r3, r14}

    ldr r1, [r3]
    ldr r0, [r1]                    /* The first item in pxCurrentTCB is the task top of stack. */
    ldmia r0!, {r4-r11}             /* Pop the registers. */
    msr psp, r0
    isb
    bx r14


/*-----------------------------------------------------------*/

vPortSVCHandler:
    /* Get the location of the current TCB. */
    ldr r3, =pxCurrentTCB
    ldr r1, [r3]
    ldr r0, [r1]
    /* Pop the core registers. */
    ldmia r0!, {r4-r11}
    msr psp, r0
    isb
    mov r0, #0
    msr basepri, r0
    orr r14, r14, #13
    bx r14

/*-----------------------------------------------------------*/

vPortStartFirstTask
    /* Use the NVIC offset register to locate the stack. */
    ldr r0, =0xE000ED08
    ldr r0, [r0]
    ldr r0, [r0]
    /* Set the msp back to the start of the stack. */
    msr msp, r0
    /* Call SVC to start the first task, ensuring interrupts are enabled. */
    cpsie i
    cpsie f
    dsb
    isb
    svc 0

    END
