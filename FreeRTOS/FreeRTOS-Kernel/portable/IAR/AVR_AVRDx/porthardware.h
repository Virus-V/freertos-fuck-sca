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
#ifndef PORTHARDWARE_H
#define PORTHARDWARE_H

#ifndef __IAR_SYSTEMS_ASM__
    #include <ioavr.h>
#endif
#include "FreeRTOSConfig.h"

/*-----------------------------------------------------------*/

#if ( configUSE_TIMER_INSTANCE == 0 )

    #define TICK_INT_vect    TCB0_INT_vect
    #define INT_FLAGS        TCB0_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB0.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB0.INTCTRL = TCB_CAPT_bm;                          \
        TCB0.CTRLA = TCB_ENABLE_bm;                          \
    }

#elif ( configUSE_TIMER_INSTANCE == 1 )

    #define TICK_INT_vect    TCB1_INT_vect
    #define INT_FLAGS        TCB1_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB1.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB1.INTCTRL = TCB_CAPT_bm;                          \
        TCB1.CTRLA = TCB_ENABLE_bm;                          \
    }

#elif ( configUSE_TIMER_INSTANCE == 2 )

    #define TICK_INT_vect    TCB2_INT_vect
    #define INT_FLAGS        TCB2_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB2.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB2.INTCTRL = TCB_CAPT_bm;                          \
        TCB2.CTRLA = TCB_ENABLE_bm;                          \
    }

#elif ( configUSE_TIMER_INSTANCE == 3 )

    #define TICK_INT_vect    TCB3_INT_vect
    #define INT_FLAGS        TCB3_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB3.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB3.INTCTRL = TCB_CAPT_bm;                          \
        TCB3.CTRLA = TCB_ENABLE_bm;                          \
    }

#elif ( configUSE_TIMER_INSTANCE == 4 )

    #define TICK_INT_vect    TCB4_INT_vect
    #define INT_FLAGS        TCB4_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB4.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB4.INTCTRL = TCB_CAPT_bm;                          \
        TCB4.CTRLA = TCB_ENABLE_bm;                          \
    }

#elif ( configUSE_TIMER_INSTANCE == 5 )

    #define TICK_INT_vect    RTC_CNT_vect
    #define INT_FLAGS        RTC_INTFLAGS
    #define INT_MASK         RTC_OVF_bm

/* Hertz to period for RTC setup */
    #define RTC_PERIOD_HZ( x )    ( 32768 * ( ( 1.0 / x ) ) )
    #define TICK_init()                                        \
    {                                                          \
        while( RTC.STATUS > 0 ) {; }                           \
        RTC.CTRLA = RTC_PRESCALER_DIV1_gc | 1 << RTC_RTCEN_bp; \
        RTC.PER = RTC_PERIOD_HZ( configTICK_RATE_HZ );         \
        RTC.INTCTRL |= 1 << RTC_OVF_bp;                        \
    }

#else /* if ( configUSE_TIMER_INSTANCE == 0 ) */
    #undef TICK_INT_vect
    #undef INT_FLAGS
    #undef INT_MASK
    #undef TICK_init()
    #error Invalid timer setting.
#endif /* if ( configUSE_TIMER_INSTANCE == 0 ) */

/*-----------------------------------------------------------*/

#endif /* PORTHARDWARE_H */
