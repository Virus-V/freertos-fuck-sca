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

/* Kernel includes. */
#include "FreeRTOS.h"

/* Machine includes */
#include <tc1782.h>
#include <machine/intrinsics.h>
#include <machine/cint.h>
/*---------------------------------------------------------------------------*/

/*
 * This reference is required by the Save/Restore Context Macros.
 */
extern volatile uint32_t * pxCurrentTCB;
/*-----------------------------------------------------------*/

/*
 * This file contains base definitions for all of the possible traps in the system.
 * It is suggested to provide implementations for all of the traps but for
 * the time being they simply trigger a DEBUG instruction so that it is easy
 * to see what caused a particular trap.
 *
 * Trap Class 6, the SYSCALL, is used exclusively by the operating system.
 */

/* The Trap Classes. */
#define portMMU_TRAP                                                      0
#define portIPT_TRAP                                                      1
#define portIE_TRAP                                                       2
#define portCM_TRAP                                                       3
#define portSBP_TRAP                                                      4
#define portASSERT_TRAP                                                   5
#define portNMI_TRAP                                                      7

/* MMU Trap Identifications. */
#define portTIN_MMU_VIRTUAL_ADDRESS_FILL                                  0
#define portTIN_MMU_VIRTUAL_ADDRESS_PROTECTION                            1

/* Internal Protection Trap Identifications. */
#define portTIN_IPT_PRIVILEGED_INSTRUCTION                                1
#define portTIN_IPT_MEMORY_PROTECTION_READ                                2
#define portTIN_IPT_MEMORY_PROTECTION_WRITE                               3
#define portTIN_IPT_MEMORY_PROTECTION_EXECUTION                           4
#define portTIN_IPT_MEMORY_PROTECTION_PERIPHERAL_ACCESS                   5
#define portTIN_IPT_MEMORY_PROTECTION_NULL_ADDRESS                        6
#define portTIN_IPT_MEMORY_PROTECTION_GLOBAL_REGISTER_WRITE_PROTECTION    7

/* Instruction Error Trap Identifications. */
#define portTIN_IE_ILLEGAL_OPCODE                                         1
#define portTIN_IE_UNIMPLEMENTED_OPCODE                                   2
#define portTIN_IE_INVALID_OPERAND                                        3
#define portTIN_IE_DATA_ADDRESS_ALIGNMENT                                 4
#define portTIN_IE_INVALID_LOCAL_MEMORY_ADDRESS                           5

/* Context Management Trap Identifications. */
#define portTIN_CM_FREE_CONTEXT_LIST_DEPLETION                            1
#define portTIN_CM_CALL_DEPTH_OVERFLOW                                    2
#define portTIN_CM_CALL_DEPTH_UNDEFLOW                                    3
#define portTIN_CM_FREE_CONTEXT_LIST_UNDERFLOW                            4
#define portTIN_CM_CALL_STACK_UNDERFLOW                                   5
#define portTIN_CM_CONTEXT_TYPE                                           6
#define portTIN_CM_NESTING_ERROR                                          7

/* System Bus and Peripherals Trap Identifications. */
#define portTIN_SBP_PROGRAM_FETCH_SYNCHRONOUS_ERROR                       1
#define portTIN_SBP_DATA_ACCESS_SYNCHRONOUS_ERROR                         2
#define portTIN_SBP_DATA_ACCESS_ASYNCHRONOUS_ERROR                        3
#define portTIN_SBP_COPROCESSOR_TRAP_ASYNCHRONOUS_ERROR                   4
#define portTIN_SBP_PROGRAM_MEMORY_INTEGRITY_ERROR                        5
#define portTIN_SBP_DATA_MEMORY_INTEGRITY_ERROR                           6

/* Assertion Trap Identifications. */
#define portTIN_ASSERT_ARITHMETIC_OVERFLOW                                1
#define portTIN_ASSERT_STICKY_ARITHMETIC_OVERFLOW                         2

/* Non-maskable Interrupt Trap Identifications. */
#define portTIN_NMI_NON_MASKABLE_INTERRUPT                                0
/*---------------------------------------------------------------------------*/

void vMMUTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
void vInternalProtectionTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
void vInstructionErrorTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
void vContextManagementTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
void vSystemBusAndPeripheralsTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
void vAssertionTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
void vNonMaskableInterruptTrap( int iTrapIdentification ) __attribute__( ( longcall, weak ) );
/*---------------------------------------------------------------------------*/

void vTrapInstallHandlers( void )
{
    if( 0 == _install_trap_handler( portMMU_TRAP, vMMUTrap ) )
    {
        _debug();
    }

    if( 0 == _install_trap_handler( portIPT_TRAP, vInternalProtectionTrap ) )
    {
        _debug();
    }

    if( 0 == _install_trap_handler( portIE_TRAP, vInstructionErrorTrap ) )
    {
        _debug();
    }

    if( 0 == _install_trap_handler( portCM_TRAP, vContextManagementTrap ) )
    {
        _debug();
    }

    if( 0 == _install_trap_handler( portSBP_TRAP, vSystemBusAndPeripheralsTrap ) )
    {
        _debug();
    }

    if( 0 == _install_trap_handler( portASSERT_TRAP, vAssertionTrap ) )
    {
        _debug();
    }

    if( 0 == _install_trap_handler( portNMI_TRAP, vNonMaskableInterruptTrap ) )
    {
        _debug();
    }
}
/*-----------------------------------------------------------*/

void vMMUTrap( int iTrapIdentification )
{
    switch( iTrapIdentification )
    {
        case portTIN_MMU_VIRTUAL_ADDRESS_FILL:
        case portTIN_MMU_VIRTUAL_ADDRESS_PROTECTION:
        default:
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/

void vInternalProtectionTrap( int iTrapIdentification )
{
    /* Deliberate fall through to default. */
    switch( iTrapIdentification )
    {
        case portTIN_IPT_PRIVILEGED_INSTRUCTION:
        /* Instruction is not allowed at current execution level, eg DISABLE at User-0. */

        case portTIN_IPT_MEMORY_PROTECTION_READ:
        /* Load word using invalid address. */

        case portTIN_IPT_MEMORY_PROTECTION_WRITE:
        /* Store Word using invalid address. */

        case portTIN_IPT_MEMORY_PROTECTION_EXECUTION:
        /* PC jumped to an address outside of the valid range. */

        case portTIN_IPT_MEMORY_PROTECTION_PERIPHERAL_ACCESS:
        /* Access to a peripheral denied at current execution level. */

        case portTIN_IPT_MEMORY_PROTECTION_NULL_ADDRESS:
        /* NULL Pointer. */

        case portTIN_IPT_MEMORY_PROTECTION_GLOBAL_REGISTER_WRITE_PROTECTION:
        /* Tried to modify a global address pointer register. */

        default:

            pxCurrentTCB[ 0 ] = __MFCR( $PCXI );
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/

void vInstructionErrorTrap( int iTrapIdentification )
{
    /* Deliberate fall through to default. */
    switch( iTrapIdentification )
    {
        case portTIN_IE_ILLEGAL_OPCODE:
        case portTIN_IE_UNIMPLEMENTED_OPCODE:
        case portTIN_IE_INVALID_OPERAND:
        case portTIN_IE_DATA_ADDRESS_ALIGNMENT:
        case portTIN_IE_INVALID_LOCAL_MEMORY_ADDRESS:
        default:
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/

void vContextManagementTrap( int iTrapIdentification )
{
    /* Deliberate fall through to default. */
    switch( iTrapIdentification )
    {
        case portTIN_CM_FREE_CONTEXT_LIST_DEPLETION:
        case portTIN_CM_CALL_DEPTH_OVERFLOW:
        case portTIN_CM_CALL_DEPTH_UNDEFLOW:
        case portTIN_CM_FREE_CONTEXT_LIST_UNDERFLOW:
        case portTIN_CM_CALL_STACK_UNDERFLOW:
        case portTIN_CM_CONTEXT_TYPE:
        case portTIN_CM_NESTING_ERROR:
        default:
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/

void vSystemBusAndPeripheralsTrap( int iTrapIdentification )
{
    /* Deliberate fall through to default. */
    switch( iTrapIdentification )
    {
        case portTIN_SBP_PROGRAM_FETCH_SYNCHRONOUS_ERROR:
        case portTIN_SBP_DATA_ACCESS_SYNCHRONOUS_ERROR:
        case portTIN_SBP_DATA_ACCESS_ASYNCHRONOUS_ERROR:
        case portTIN_SBP_COPROCESSOR_TRAP_ASYNCHRONOUS_ERROR:
        case portTIN_SBP_PROGRAM_MEMORY_INTEGRITY_ERROR:
        case portTIN_SBP_DATA_MEMORY_INTEGRITY_ERROR:
        default:
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/

void vAssertionTrap( int iTrapIdentification )
{
    /* Deliberate fall through to default. */
    switch( iTrapIdentification )
    {
        case portTIN_ASSERT_ARITHMETIC_OVERFLOW:
        case portTIN_ASSERT_STICKY_ARITHMETIC_OVERFLOW:
        default:
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/

void vNonMaskableInterruptTrap( int iTrapIdentification )
{
    /* Deliberate fall through to default. */
    switch( iTrapIdentification )
    {
        case portTIN_NMI_NON_MASKABLE_INTERRUPT:
        default:
            _debug();
            break;
    }
}
/*---------------------------------------------------------------------------*/
