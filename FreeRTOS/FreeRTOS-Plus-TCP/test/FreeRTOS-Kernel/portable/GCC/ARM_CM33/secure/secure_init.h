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

#ifndef __SECURE_INIT_H__
#define __SECURE_INIT_H__

/**
 * @brief De-prioritizes the non-secure exceptions.
 *
 * This is needed to ensure that the non-secure PendSV runs at the lowest
 * priority. Context switch is done in the non-secure PendSV handler.
 *
 * @note This function must be called in the handler mode. It is no-op if called
 * in the thread mode.
 */
void SecureInit_DePrioritizeNSExceptions( void );

/**
 * @brief Sets up the Floating Point Unit (FPU) for Non-Secure access.
 *
 * Also sets FPCCR.TS=1 to ensure that the content of the Floating Point
 * Registers are not leaked to the non-secure side.
 *
 * @note This function must be called in the handler mode. It is no-op if called
 * in the thread mode.
 */
void SecureInit_EnableNSFPUAccess( void );

#endif /* __SECURE_INIT_H__ */
