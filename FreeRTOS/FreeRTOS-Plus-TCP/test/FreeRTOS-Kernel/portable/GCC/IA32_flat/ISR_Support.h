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

.extern ulTopOfSystemStack
   .extern ulInterruptNesting

/*-----------------------------------------------------------*/

   .macro portFREERTOS_INTERRUPT_ENTRY

/* Save general purpose registers. */
pusha

/* If ulInterruptNesting is zero the rest of the task context will need
 * saving and a stack switch might be required. */
movl ulInterruptNesting, % eax
test % eax, % eax
jne     2f

/* Interrupts are not nested, so save the rest of the task context. */
   .

   if configSUPPORT_FPU == 1

/* If the task has a buffer allocated to save the FPU context then
 * save the FPU context now. */
movl pucPortTaskFPUContextBuffer, % eax
test % eax, % eax
je      1f
fnsave( % eax ) /* Save FLOP context into ucTempFPUBuffer array. */
fwait

1 :
/* Save the address of the FPU context, if any. */
push pucPortTaskFPUContextBuffer

   .endif /* configSUPPORT_FPU */

/* Find the TCB. */
movl pxCurrentTCB, % eax

/* Stack location is first item in the TCB. */
     movl % esp, ( % eax )

/* Switch stacks. */
movl ulTopOfSystemStack, % esp
movl % esp, % ebp

2 :
/* Increment nesting count. */
add $1, ulInterruptNesting

   .endm
/*-----------------------------------------------------------*/

   .macro portINTERRUPT_EPILOGUE

cli
sub $1, ulInterruptNesting

/* If the nesting has unwound to zero. */
movl ulInterruptNesting, % eax
     test % eax, % eax
     jne     2f

/* If a yield was requested then select a new TCB now. */
movl ulPortYieldPending, % eax
     test % eax, % eax
     je      1f
movl $0, ulPortYieldPending
call vTaskSwitchContext

1 :
/* Stack location is first item in the TCB. */
movl pxCurrentTCB, % eax movl( % eax ), % esp

   .

   if configSUPPORT_FPU == 1

/* Restore address of task's FPU context buffer. */
pop pucPortTaskFPUContextBuffer

/* If the task has a buffer allocated in which its FPU context is saved,
 * then restore it now. */
movl pucPortTaskFPUContextBuffer, % eax
test % eax, % eax
je      1f
frstor( % eax )
1 :
.endif

2 :
popa

   .endm
/*-----------------------------------------------------------*/

   .macro portFREERTOS_INTERRUPT_EXIT

portINTERRUPT_EPILOGUE
/* EOI. */
movl $0x00, ( 0xFEE000B0 )
iret

   .endm
