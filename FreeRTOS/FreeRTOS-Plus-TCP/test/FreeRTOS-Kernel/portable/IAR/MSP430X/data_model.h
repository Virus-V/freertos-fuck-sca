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

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#if __DATA_MODEL__ == __DATA_MODEL_SMALL__
    #define pushm_x    pushm.w
    #define popm_x     popm.w
    #define push_x     push.w
    #define pop_x      pop.w
    #define mov_x      mov.w
    #define cmp_x      cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_MEDIUM__
    #define pushm_x    pushm.a
    #define popm_x     popm.a
    #define push_x     pushx.a
    #define pop_x      popx.a
    #define mov_x      mov.w
    #define cmp_x      cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_LARGE__
    #define pushm_x    pushm.a
    #define popm_x     popm.a
    #define push_x     pushx.a
    #define pop_x      popx.a
    #define mov_x      movx.a
    #define cmp_x      cmpx.a
#endif

#ifndef pushm_x
    #error The assembler options must define one of the following symbols: __DATA_MODEL_SMALL__, __DATA_MODEL_MEDIUM__, or __DATA_MODEL_LARGE__
#endif

#endif /* DATA_MODEL_H */
