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

EXTERN pxCurrentTCB
EXTERN ulCriticalNesting

;
Context save and restore macro definitions
;

portSAVE_CONTEXT MACRO

;
Push R0 as we are going to use the register.
   STMDB SP !, {
    R0
}

Set R0 to point to the task stack pointer.
   STMDB SP, {
    SP
}
^
NOP
SUB SP, SP, # 4
LDMIA SP !, {
    R0
}

;
Push the return address onto the stack.
          STMDB R0 !, {
           LR
       }

       Now we have saved LR we can use it instead of R0.
          MOV LR, R0

;

Pop R0 so we can save it onto the system mode stack.
   LDMIA SP !, {
    R0
}

Push all the system mode registers onto the task stack.
   STMDB LR, {
    R0 - LR
}
^
NOP
SUB LR, LR, # 60

;
Push the SPSR onto the task stack.
   MRS R0, SPSR
STMDB LR !, {
    R0
}

LDR R0, = ulCriticalNesting
          LDR R0, [ R0 ]
STMDB LR !, {
    R0
}

Store the new top of stack

for the task.
   LDR R1, = pxCurrentTCB
             LDR R0, [ R1 ]
STR LR, [ R0 ]

ENDM


portRESTORE_CONTEXT MACRO

;
Set the LR to the task stack.
   LDR R1, = pxCurrentTCB
             LDR R0, [ R1 ]
LDR LR, [ R0 ]

;
The critical nesting depth is the first item on the stack.
   ;
Load it into the ulCriticalNesting variable.
   LDR R0, = ulCriticalNesting
             LDMFD LR !, {
    R1
}

STR R1, [ R0 ]

;
Get the SPSR from the stack.
   LDMFD LR !, {
    R0
}
MSR SPSR_cxsf, R0

;
Restore all system mode registers

for the task.
   LDMFD LR, {
    R0 - R14
}

^
NOP

;
Restore the return address.
          LDR LR, [ LR, # + 60 ]

;

And return -correcting the offset in the LR to obtain the
;

correct address.
   SUBS PC, LR, # 4

ENDM
