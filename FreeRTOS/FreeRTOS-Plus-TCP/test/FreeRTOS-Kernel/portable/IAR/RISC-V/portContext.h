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

#ifndef PORTCONTEXT_H
#define PORTCONTEXT_H

#if __riscv_xlen == 64
    #define portWORD_SIZE    8
    #define store_x          sd
    #define load_x           ld
#elif __riscv_xlen == 32
    #define store_x          sw
    #define load_x           lw
    #define portWORD_SIZE    4
#else
    #error Assembler did not define __riscv_xlen
#endif

#include "freertos_risc_v_chip_specific_extensions.h"

/* Only the standard core registers are stored by default.  Any additional
 * registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
 * portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
 * specific version of freertos_risc_v_chip_specific_extensions.h.  See the
 * notes at the top of portASM.S file. */
#ifdef __riscv_32e
    #define portCONTEXT_SIZE               ( 15 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    13
    #define portMSTATUS_OFFSET             14
#else
    #define portCONTEXT_SIZE               ( 31 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    29
    #define portMSTATUS_OFFSET             30
#endif

EXTERN pxCurrentTCB
EXTERN xISRStackTop
EXTERN xCriticalNesting
EXTERN pxCriticalNesting

/*-----------------------------------------------------------*/

portcontextSAVE_CONTEXT_INTERNAL MACRO
addi sp, sp, -portCONTEXT_SIZE
store_x x1, 1 * portWORD_SIZE( sp )
store_x x5, 2 * portWORD_SIZE( sp )
store_x x6, 3 * portWORD_SIZE( sp )
store_x x7, 4 * portWORD_SIZE( sp )
store_x x8, 5 * portWORD_SIZE( sp )
store_x x9, 6 * portWORD_SIZE( sp )
store_x x10, 7 * portWORD_SIZE( sp )
store_x x11, 8 * portWORD_SIZE( sp )
store_x x12, 9 * portWORD_SIZE( sp )
store_x x13, 10 * portWORD_SIZE( sp )
store_x x14, 11 * portWORD_SIZE( sp )
store_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    store_x x16, 13 * portWORD_SIZE( sp )
    store_x x17, 14 * portWORD_SIZE( sp )
    store_x x18, 15 * portWORD_SIZE( sp )
    store_x x19, 16 * portWORD_SIZE( sp )
    store_x x20, 17 * portWORD_SIZE( sp )
    store_x x21, 18 * portWORD_SIZE( sp )
    store_x x22, 19 * portWORD_SIZE( sp )
    store_x x23, 20 * portWORD_SIZE( sp )
    store_x x24, 21 * portWORD_SIZE( sp )
    store_x x25, 22 * portWORD_SIZE( sp )
    store_x x26, 23 * portWORD_SIZE( sp )
    store_x x27, 24 * portWORD_SIZE( sp )
    store_x x28, 25 * portWORD_SIZE( sp )
    store_x x29, 26 * portWORD_SIZE( sp )
    store_x x30, 27 * portWORD_SIZE( sp )
    store_x x31, 28 * portWORD_SIZE( sp )
#endif /* ifndef __riscv_32e */

load_x t0, xCriticalNesting                                   /* Load the value of xCriticalNesting into t0. */
store_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Store the critical nesting value to the stack. */

csrr t0, mstatus                                              /* Required for MPIE bit. */
store_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )

portasmSAVE_ADDITIONAL_REGISTERS /* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

load_x t0, pxCurrentTCB          /* Load pxCurrentTCB. */
store_x sp, 0 ( t0 )             /* Write sp to first TCB member. */

ENDM
/*-----------------------------------------------------------*/

portcontextSAVE_EXCEPTION_CONTEXT MACRO
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
addi a1, a1, 4          /* Synchronous so update exception return address to the instruction after the instruction that generated the exception. */
store_x a1, 0 ( sp )    /* Save updated exception return address. */
load_x sp, xISRStackTop /* Switch to ISR stack. */
ENDM
/*-----------------------------------------------------------*/

portcontextSAVE_INTERRUPT_CONTEXT MACRO
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
store_x a1, 0 ( sp )    /* Asynchronous interrupt so save unmodified exception return address. */
load_x sp, xISRStackTop /* Switch to ISR stack. */
ENDM
/*-----------------------------------------------------------*/

portcontextRESTORE_CONTEXT MACRO
load_x t1, pxCurrentTCB /* Load pxCurrentTCB. */
load_x sp, 0 ( t1 )     /* Read sp from first TCB member. */

/* Load mepc with the address of the instruction in the task to run next. */
load_x t0, 0 ( sp )
csrw mepc, t0

/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */
portasmRESTORE_ADDITIONAL_REGISTERS

/* Load mstatus with the interrupt enable bits used by the task. */
load_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )
csrw mstatus, t0                                             /* Required for MPIE bit. */

load_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Obtain xCriticalNesting value for this task from task's stack. */
load_x t1, pxCriticalNesting                                 /* Load the address of xCriticalNesting into t1. */
store_x t0, 0 ( t1 )                                         /* Restore the critical nesting value for this task. */

load_x x1, 1 * portWORD_SIZE( sp )
load_x x5, 2 * portWORD_SIZE( sp )
load_x x6, 3 * portWORD_SIZE( sp )
load_x x7, 4 * portWORD_SIZE( sp )
load_x x8, 5 * portWORD_SIZE( sp )
load_x x9, 6 * portWORD_SIZE( sp )
load_x x10, 7 * portWORD_SIZE( sp )
load_x x11, 8 * portWORD_SIZE( sp )
load_x x12, 9 * portWORD_SIZE( sp )
load_x x13, 10 * portWORD_SIZE( sp )
load_x x14, 11 * portWORD_SIZE( sp )
load_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    load_x x16, 13 * portWORD_SIZE( sp )
    load_x x17, 14 * portWORD_SIZE( sp )
    load_x x18, 15 * portWORD_SIZE( sp )
    load_x x19, 16 * portWORD_SIZE( sp )
    load_x x20, 17 * portWORD_SIZE( sp )
    load_x x21, 18 * portWORD_SIZE( sp )
    load_x x22, 19 * portWORD_SIZE( sp )
    load_x x23, 20 * portWORD_SIZE( sp )
    load_x x24, 21 * portWORD_SIZE( sp )
    load_x x25, 22 * portWORD_SIZE( sp )
    load_x x26, 23 * portWORD_SIZE( sp )
    load_x x27, 24 * portWORD_SIZE( sp )
    load_x x28, 25 * portWORD_SIZE( sp )
    load_x x29, 26 * portWORD_SIZE( sp )
    load_x x30, 27 * portWORD_SIZE( sp )
    load_x x31, 28 * portWORD_SIZE( sp )
#endif /* ifndef __riscv_32e */
addi sp, sp, portCONTEXT_SIZE

mret
ENDM
/*-----------------------------------------------------------*/

#endif /* PORTCONTEXT_H */
