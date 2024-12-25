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

#ifndef configENABLE_MVE
    #error configENABLE_MVE must be defined in FreeRTOSConfig.h. Set configENABLE_MVE to 1 to enable the MVE or 0 to disable the MVE.
#endif /* configENABLE_MVE */
/*-----------------------------------------------------------*/

/**
 * Architecture specifics.
 */
#define portARCH_NAME                    "Cortex-M85"
#define portHAS_ARMV8M_MAIN_EXTENSION    1
#define portDONT_DISCARD                 __attribute__( ( used ) )
/*-----------------------------------------------------------*/

/* ARMv8-M common port configurations. */
#include "portmacrocommon.h"
/*-----------------------------------------------------------*/

/**
 * @brief Critical section management.
 */
#define portDISABLE_INTERRUPTS()    ulSetInterruptMask()
#define portENABLE_INTERRUPTS()     vClearInterruptMask( 0 )
/*-----------------------------------------------------------*/

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
