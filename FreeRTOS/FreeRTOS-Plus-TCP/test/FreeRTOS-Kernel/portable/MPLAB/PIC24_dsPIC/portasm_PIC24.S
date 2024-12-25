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

#if defined( __PIC24E__ ) || defined ( __PIC24F__ ) || defined( __PIC24FK__ ) || defined( __PIC24H__ )

        .global _vPortYield
        .extern _vTaskSwitchContext
        .extern uxCriticalNesting

_vPortYield:

        PUSH    SR                      /* Save the SR used by the task.... */
        PUSH    W0                      /* ....then disable interrupts. */
        MOV     #32, W0
        MOV     W0, SR
        PUSH    W1                      /* Save registers to the stack. */
        PUSH.D  W2
        PUSH.D  W4
        PUSH.D  W6
        PUSH.D  W8
        PUSH.D  W10
        PUSH.D  W12
        PUSH    W14
        PUSH    RCOUNT
        PUSH    TBLPAG

        PUSH    CORCON
        #ifdef __HAS_EDS__
            PUSH    DSRPAG
            PUSH    DSWPAG
        #else
            PUSH    PSVPAG
        #endif /* __HAS_EDS__ */
        MOV     _uxCriticalNesting, W0      /* Save the critical nesting counter for the task. */
        PUSH    W0
        MOV     _pxCurrentTCB, W0           /* Save the new top of stack into the TCB. */
        MOV     W15, [W0]

        call    _vTaskSwitchContext

        MOV     _pxCurrentTCB, W0           /* Restore the stack pointer for the task. */
        MOV     [W0], W15
        POP     W0                          /* Restore the critical nesting counter for the task. */
        MOV     W0, _uxCriticalNesting
        #ifdef __HAS_EDS__
            POP     DSWPAG
            POP     DSRPAG
        #else
            POP     PSVPAG
        #endif /* __HAS_EDS__ */
        POP     CORCON
        POP     TBLPAG
        POP     RCOUNT                      /* Restore the registers from the stack. */
        POP     W14
        POP.D   W12
        POP.D   W10
        POP.D   W8
        POP.D   W6
        POP.D   W4
        POP.D   W2
        POP.D   W0
        POP     SR

        return

        .end

#endif /* defined( __PIC24E__ ) || defined ( __PIC24F__ ) || defined( __PIC24FK__ ) || defined( __PIC24H__ ) */
