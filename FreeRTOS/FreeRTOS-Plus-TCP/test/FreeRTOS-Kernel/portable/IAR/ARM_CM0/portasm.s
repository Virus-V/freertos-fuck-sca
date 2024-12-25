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

    EXTERN vPortYieldFromISR
    EXTERN pxCurrentTCB
    EXTERN vTaskSwitchContext

    PUBLIC vSetMSP
    PUBLIC xPortPendSVHandler
    PUBLIC vPortSVCHandler
    PUBLIC vPortStartFirstTask
    PUBLIC ulSetInterruptMaskFromISR
    PUBLIC vClearInterruptMaskFromISR

/*-----------------------------------------------------------*/

vSetMSP
    msr msp, r0
    bx lr

/*-----------------------------------------------------------*/

xPortPendSVHandler:
    mrs r0, psp

    ldr r3, =pxCurrentTCB   /* Get the location of the current TCB. */
    ldr r2, [r3]

    subs r0, r0, #32        /* Make space for the remaining low registers. */
    str r0, [r2]            /* Save the new top of stack. */
    stmia r0!, {r4-r7}      /* Store the low registers that are not saved automatically. */
    mov r4, r8              /* Store the high registers. */
    mov r5, r9
    mov r6, r10
    mov r7, r11
    stmia r0!, {r4-r7}

    push {r3, r14}
    cpsid i
    bl vTaskSwitchContext
    cpsie i
    pop {r2, r3}            /* lr goes in r3. r2 now holds tcb pointer. */

    ldr r1, [r2]
    ldr r0, [r1]            /* The first item in pxCurrentTCB is the task top of stack. */
    adds r0, r0, #16        /* Move to the high registers. */
    ldmia r0!, {r4-r7}      /* Pop the high registers. */
    mov r8, r4
    mov r9, r5
    mov r10, r6
    mov r11, r7

    msr psp, r0             /* Remember the new top of stack for the task. */

    subs r0, r0, #32        /* Go back for the low registers that are not automatically restored. */
    ldmia r0!, {r4-r7}      /* Pop low registers.  */

    bx r3

/*-----------------------------------------------------------*/

vPortSVCHandler;
    /* This function is no longer used, but retained for backward
     * compatibility. */
    bx lr

/*-----------------------------------------------------------*/

vPortStartFirstTask
    /* Don't reset the MSP stack as is done on CM3/4 devices. The vector table
     * in some CM0 devices cannot be modified and thus may not hold the
     * application's initial MSP value. */

    ldr r3, =pxCurrentTCB   /* Obtain location of pxCurrentTCB. */
    ldr r1, [r3]
    ldr r0, [r1]            /* The first item in pxCurrentTCB is the task top of stack. */
    adds r0, #32            /* Discard everything up to r0. */
    msr psp, r0             /* This is now the new top of stack to use in the task. */
    movs r0, #2             /* Switch to the psp stack. */
    msr CONTROL, r0
    isb
    pop {r0-r5}             /* Pop the registers that are saved automatically. */
    mov lr, r5              /* lr is now in r5. */
    pop {r3}                /* The return address is now in r3. */
    pop {r2}                /* Pop and discard the XPSR. */
    cpsie i                 /* The first task has its context and interrupts can be enabled. */
    bx r3                   /* Jump to the user defined task code. */

/*-----------------------------------------------------------*/

ulSetInterruptMaskFromISR
    mrs r0, PRIMASK
    cpsid i
    bx lr

/*-----------------------------------------------------------*/

vClearInterruptMaskFromISR
    msr PRIMASK, r0
    bx lr

    END
