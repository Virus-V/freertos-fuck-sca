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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/*------------------------------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the given hardware
 * and compiler.
 *
 * These settings should not be altered.
 *------------------------------------------------------------------------------
 */

/**
 * Architecture specifics.
 */
#define portARCH_NAME                    "Cortex-M33"
#define portHAS_ARMV8M_MAIN_EXTENSION    1
#define portDONT_DISCARD                 __root
/*-----------------------------------------------------------*/

#if ( configTOTAL_MPU_REGIONS == 16 )
    #error 16 MPU regions are not yet supported for this port.
#endif
/*-----------------------------------------------------------*/

/* ARMv8-M common port configurations. */
#include "portmacrocommon.h"
/*-----------------------------------------------------------*/

#ifndef configENABLE_MVE
    #define configENABLE_MVE    0
#elif( configENABLE_MVE != 0 )
    #error configENABLE_MVE must be left undefined, or defined to 0 for the Cortex-M33.
#endif
/*-----------------------------------------------------------*/

/**
 * @brief Critical section management.
 */
#define portDISABLE_INTERRUPTS()    ulSetInterruptMask()
#define portENABLE_INTERRUPTS()     vClearInterruptMask( 0 )
/*-----------------------------------------------------------*/

/* Suppress warnings that are generated by the IAR tools, but cannot be fixed in
 * the source code because to do so would cause other compilers to generate
 * warnings. */
#pragma diag_suppress=Be006
#pragma diag_suppress=Pa082
/*-----------------------------------------------------------*/

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
