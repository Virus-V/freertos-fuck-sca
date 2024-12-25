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

/*
Changes from V3.0.0

Changes from V3.0.1

Changes from V4.0.1
    Uselib pragma added for Croutine.c
*/

/*
 * The installation script will automatically prepend this file to the default FreeRTOS.h.
 */

#ifndef WIZC_FREERTOS_H
#define WIZC_FREERTOS_H

#pragma noheap
#pragma wizcpp expandnl   on
#pragma wizcpp searchpath "$__PATHNAME__/libFreeRTOS/Include/"
#pragma wizcpp uselib     "$__PATHNAME__/libFreeRTOS/Modules/Croutine.c"
#pragma wizcpp uselib     "$__PATHNAME__/libFreeRTOS/Modules/Tasks.c"
#pragma wizcpp uselib     "$__PATHNAME__/libFreeRTOS/Modules/Queue.c"
#pragma wizcpp uselib     "$__PATHNAME__/libFreeRTOS/Modules/List.c"
#pragma wizcpp uselib     "$__PATHNAME__/libFreeRTOS/Modules/Port.c"

#endif  /* WIZC_FREERTOS_H */
