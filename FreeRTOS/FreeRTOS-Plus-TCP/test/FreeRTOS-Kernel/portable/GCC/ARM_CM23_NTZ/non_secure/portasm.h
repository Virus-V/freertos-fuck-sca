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

#ifndef __PORT_ASM_H__
#define __PORT_ASM_H__

/* Scheduler includes. */
#include "FreeRTOS.h"

/* MPU wrappers includes. */
#include "mpu_wrappers.h"

/**
 * @brief Restore the context of the first task so that the first task starts
 * executing.
 */
void vRestoreContextOfFirstTask( void ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief Checks whether or not the processor is privileged.
 *
 * @return 1 if the processor is already privileged, 0 otherwise.
 */
BaseType_t xIsPrivileged( void ) __attribute__( ( naked ) );

/**
 * @brief Raises the privilege level by clearing the bit 0 of the CONTROL
 * register.
 *
 * @note This is a privileged function and should only be called from the kenrel
 * code.
 *
 * Bit 0 of the CONTROL register defines the privilege level of Thread Mode.
 *  Bit[0] = 0 --> The processor is running privileged
 *  Bit[0] = 1 --> The processor is running unprivileged.
 */
void vRaisePrivilege( void ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief Lowers the privilege level by setting the bit 0 of the CONTROL
 * register.
 *
 * Bit 0 of the CONTROL register defines the privilege level of Thread Mode.
 *  Bit[0] = 0 --> The processor is running privileged
 *  Bit[0] = 1 --> The processor is running unprivileged.
 */
void vResetPrivilege( void ) __attribute__( ( naked ) );

/**
 * @brief Starts the first task.
 */
void vStartFirstTask( void ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief Disables interrupts.
 */
uint32_t ulSetInterruptMask( void ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief Enables interrupts.
 */
void vClearInterruptMask( uint32_t ulMask ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief PendSV Exception handler.
 */
void PendSV_Handler( void ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief SVC Handler.
 */
void SVC_Handler( void ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

/**
 * @brief Allocate a Secure context for the calling task.
 *
 * @param[in] ulSecureStackSize The size of the stack to be allocated on the
 * secure side for the calling task.
 */
void vPortAllocateSecureContext( uint32_t ulSecureStackSize ) __attribute__( ( naked ) );

/**
 * @brief Free the task's secure context.
 *
 * @param[in] pulTCB Pointer to the Task Control Block (TCB) of the task.
 */
void vPortFreeSecureContext( uint32_t * pulTCB ) __attribute__( ( naked ) ) PRIVILEGED_FUNCTION;

#endif /* __PORT_ASM_H__ */
