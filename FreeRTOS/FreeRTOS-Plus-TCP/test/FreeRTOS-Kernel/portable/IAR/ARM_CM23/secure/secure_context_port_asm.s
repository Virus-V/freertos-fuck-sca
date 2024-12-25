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

    SECTION .text:CODE:NOROOT(2)
    THUMB

/* Including FreeRTOSConfig.h here will cause build errors if the header file
contains code not understood by the assembler - for example the 'extern' keyword.
To avoid errors place any such code inside a #ifdef __ICCARM__/#endif block so
the code is included in C files but excluded by the preprocessor in assembly
files (__ICCARM__ is defined by the IAR C compiler but not by the IAR assembler. */
#include "FreeRTOSConfig.h"

    PUBLIC SecureContext_LoadContextAsm
    PUBLIC SecureContext_SaveContextAsm

#if ( configENABLE_FPU == 1 )
    #error Cortex-M23 does not have a Floating Point Unit (FPU) and therefore configENABLE_FPU must be set to 0.
#endif
/*-----------------------------------------------------------*/

SecureContext_LoadContextAsm:
    /* pxSecureContext value is in r0. */
    mrs r1, ipsr                    /* r1 = IPSR. */
    cbz r1, load_ctx_therad_mode    /* Do nothing if the processor is running in the Thread Mode. */
    ldmia r0!, {r1, r2}             /* r1 = pxSecureContext->pucCurrentStackPointer, r2 = pxSecureContext->pucStackLimit. */

#if ( configENABLE_MPU == 1 )
    ldmia r1!, {r3}                 /* Read CONTROL register value from task's stack. r3 = CONTROL. */
    msr control, r3                 /* CONTROL = r3. */
#endif /* configENABLE_MPU */

    msr psplim, r2                  /* PSPLIM = r2. */
    msr psp, r1                     /* PSP = r1. */

    load_ctx_therad_mode:
        bx lr
/*-----------------------------------------------------------*/

SecureContext_SaveContextAsm:
    /* pxSecureContext value is in r0. */
    mrs r1, ipsr                    /* r1 = IPSR. */
    cbz r1, save_ctx_therad_mode    /* Do nothing if the processor is running in the Thread Mode. */
    mrs r1, psp                     /* r1 = PSP. */

#if ( configENABLE_MPU == 1 )
    mrs r2, control                 /* r2 = CONTROL. */
    subs r1, r1, #4                 /* Make space for the CONTROL value on the stack. */
    str r1, [r0]                    /* Save the top of stack in context. pxSecureContext->pucCurrentStackPointer = r1. */
    stmia r1!, {r2}                 /* Store CONTROL value on the stack. */
#else /* configENABLE_MPU */
    str r1, [r0]                    /* Save the top of stack in context. pxSecureContext->pucCurrentStackPointer = r1. */
#endif /* configENABLE_MPU */

    movs r1, #0                     /* r1 = securecontextNO_STACK. */
    msr psplim, r1                  /* PSPLIM = securecontextNO_STACK. */
    msr psp, r1                     /* PSP = securecontextNO_STACK i.e. No stack for thread mode until next task's context is loaded. */

    save_ctx_therad_mode:
        bx lr
/*-----------------------------------------------------------*/

    END
