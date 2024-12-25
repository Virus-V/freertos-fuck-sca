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

/* Secure context includes. */
#include "secure_context.h"

/* Secure port macros. */
#include "secure_port_macros.h"

#if ( configENABLE_FPU == 1 )
    #error Cortex-M23 does not have a Floating Point Unit (FPU) and therefore configENABLE_FPU must be set to 0.
#endif

void SecureContext_LoadContextAsm( SecureContext_t * pxSecureContext ) __attribute__( ( naked ) );
void SecureContext_SaveContextAsm( SecureContext_t * pxSecureContext ) __attribute__( ( naked ) );

void SecureContext_LoadContextAsm( SecureContext_t * pxSecureContext )
{
    /* pxSecureContext value is in r0. */
    __asm volatile
    (
        " .syntax unified                   \n"
        "                                   \n"
        " mrs r1, ipsr                      \n" /* r1 = IPSR. */
        " cbz r1, load_ctx_therad_mode      \n" /* Do nothing if the processor is running in the Thread Mode. */
        " ldmia r0!, {r1, r2}               \n" /* r1 = pxSecureContext->pucCurrentStackPointer, r2 = pxSecureContext->pucStackLimit. */
        "                                   \n"
        #if ( configENABLE_MPU == 1 )
            " ldmia r1!, {r3}               \n" /* Read CONTROL register value from task's stack. r3 = CONTROL. */
            " msr control, r3               \n" /* CONTROL = r3. */
        #endif /* configENABLE_MPU */
        "                                   \n"
        " msr psplim, r2                    \n" /* PSPLIM = r2. */
        " msr psp, r1                       \n" /* PSP = r1. */
        "                                   \n"
        " load_ctx_therad_mode:             \n"
        "    bx lr                          \n"
        "                                   \n"
        ::: "r0", "r1", "r2"
    );
}
/*-----------------------------------------------------------*/

void SecureContext_SaveContextAsm( SecureContext_t * pxSecureContext )
{
    /* pxSecureContext value is in r0. */
    __asm volatile
    (
        " .syntax unified                   \n"
        "                                   \n"
        " mrs r1, ipsr                      \n" /* r1 = IPSR. */
        " cbz r1, save_ctx_therad_mode      \n" /* Do nothing if the processor is running in the Thread Mode. */
        " mrs r1, psp                       \n" /* r1 = PSP. */
        "                                   \n"
        #if ( configENABLE_MPU == 1 )
            " mrs r2, control               \n" /* r2 = CONTROL. */
            " subs r1, r1, #4               \n" /* Make space for the CONTROL value on the stack. */
            " str r1, [r0]                  \n" /* Save the top of stack in context. pxSecureContext->pucCurrentStackPointer = r1. */
            " stmia r1!, {r2}               \n" /* Store CONTROL value on the stack. */
        #else /* configENABLE_MPU */
            " str r1, [r0]                  \n" /* Save the top of stack in context. pxSecureContext->pucCurrentStackPointer = r1. */
        #endif /* configENABLE_MPU */
        "                                   \n"
        " movs r1, %0                       \n" /* r1 = securecontextNO_STACK. */
        " msr psplim, r1                    \n" /* PSPLIM = securecontextNO_STACK. */
        " msr psp, r1                       \n" /* PSP = securecontextNO_STACK i.e. No stack for thread mode until next task's context is loaded. */
        "                                   \n"
        " save_ctx_therad_mode:             \n"
        "   bx lr                           \n"
        "                                   \n"
        ::"i" ( securecontextNO_STACK ) : "r1", "memory"
    );
}
/*-----------------------------------------------------------*/
