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

.extern     vTaskSwitchContext

.set noat

# Exported to start the first task.
.globl restore_sp_from_pxCurrentTCB

# Entry point for exceptions.
.section .exceptions.entry.user, "xa"

# Save the entire context of a task.
save_context:
    addi    sp, sp, -116        # Create space on the stack.
    stw     ra, 0(sp)
                                # Leave a gap for muldiv 0
    stw     at, 8(sp)
    stw     r2, 12(sp)
    stw     r3, 16(sp)
    stw     r4, 20(sp)
    stw     r5, 24(sp)
    stw     r6, 28(sp)
    stw     r7, 32(sp)
    stw     r8, 36(sp)
    stw     r9, 40(sp)
    stw     r10, 44(sp)
    stw     r11, 48(sp)
    stw     r12, 52(sp)
    stw     r13, 56(sp)
    stw     r14, 60(sp)
    stw     r15, 64(sp)
    rdctl   r5, estatus         # Save the eStatus
    stw     r5, 68(sp)
    addi    r15, ea, -4         # Instruction that caused exception
    stw     r15, 72(sp)         # Save as EA
    stw     r16, 76(sp)         # Save the remaining registers
    stw     r17, 80(sp)
    stw     r18, 84(sp)
    stw     r19, 88(sp)
    stw     r20, 92(sp)
    stw     r21, 96(sp)
    stw     r22, 100(sp)
    stw     r23, 104(sp)
    stw     gp, 108(sp)
    stw     fp, 112(sp)

save_sp_to_pxCurrentTCB:
    movia   et, pxCurrentTCB    # Load the address of the pxCurrentTCB pointer
    ldw     et, (et)            # Load the value of the pxCurrentTCB pointer
    stw     sp, (et)            # Store the stack pointer into the top of the TCB

    br      irq_test_user       # skip the section .exceptions.entry

    .section .exceptions.irqtest, "xa"
irq_test_user:

    .section .exceptions.exit.user, "xa"
restore_sp_from_pxCurrentTCB:
    movia   et, pxCurrentTCB        # Load the address of the pxCurrentTCB pointer
    ldw     et, (et)                # Load the value of the pxCurrentTCB pointer
    ldw     sp, (et)                # Load the stack pointer with the top value of the TCB

restore_context:
    ldw     ra, 0(sp)       # Restore the registers.
                            # Leave a gap for muldiv 0.
    ldw     at, 8(sp)
    ldw     r2, 12(sp)
    ldw     r3, 16(sp)
    ldw     r4, 20(sp)
    ldw     r5, 24(sp)
    ldw     r6, 28(sp)
    ldw     r7, 32(sp)
    ldw     r8, 36(sp)
    ldw     r9, 40(sp)
    ldw     r10, 44(sp)
    ldw     r11, 48(sp)
    ldw     r12, 52(sp)
    ldw     r13, 56(sp)
    ldw     r14, 60(sp)
    ldw     r15, 64(sp)
    ldw     et, 68(sp)      # Load the eStatus
    wrctl   estatus, et     # Write the eStatus
    ldw     ea, 72(sp)      # Load the Program Counter
    ldw     r16, 76(sp)
    ldw     r17, 80(sp)
    ldw     r18, 84(sp)
    ldw     r19, 88(sp)
    ldw     r20, 92(sp)
    ldw     r21, 96(sp)
    ldw     r22, 100(sp)
    ldw     r23, 104(sp)
    ldw     gp, 108(sp)
    ldw     fp, 112(sp)
    addi    sp, sp, 116     # Release stack space

    eret                    # Return to address ea, loading eStatus into Status.

    .section .exceptions.soft, "xa"
soft_exceptions:
    movhi   r3, 0x003b              /* upper half of trap opcode */
    ori     r3, r3, 0x683a          /* lower half of trap opcode */
    beq     r2, r3, call_scheduler
    br      exceptions_unknown_user         # its something else

call_scheduler:
    stw     ea, 72(sp)                      # EA is PC+4 so will skip over instruction causing exception
    movia   r15, vTaskSwitchContext         # Pick the next context - use long call version in place of "call"
    callr   r15
    br      restore_sp_from_pxCurrentTCB    # Switch in the task context and restore.

    .section .exceptions.unknown.user
exceptions_unknown_user:
