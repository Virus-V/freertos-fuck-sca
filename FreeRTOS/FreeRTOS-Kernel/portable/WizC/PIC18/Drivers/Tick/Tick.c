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
    + ISRcode is pulled inline and portTICKisr() is therefore
      deleted from this file.

    + Prescaler logic for Timer1 added to allow for a wider
      range of TickRates.

Changes from V3.0.1
*/

#include <FreeRTOS.h>
#include <task.h>

/* IO port constants. */
#define portBIT_SET     (1)
#define portBIT_CLEAR   (0)

/*
 * Hardware setup for the tick.
 * We use a compare match on timer1. Depending on MPU-frequency
 * and requested tickrate, a prescaled value with a matching
 * prescaler are determined.
 */
#define portTIMER_COMPARE_BASE          ((APROCFREQ/4)/configTICK_RATE_HZ)

#if portTIMER_COMPARE_BASE   < 0x10000
    #define portTIMER_COMPARE_VALUE     (portTIMER_COMPARE_BASE)
    #define portTIMER_COMPARE_PS1       (portBIT_CLEAR)
    #define portTIMER_COMPARE_PS0       (portBIT_CLEAR)
#elif portTIMER_COMPARE_BASE < 0x20000
    #define portTIMER_COMPARE_VALUE     (portTIMER_COMPARE_BASE / 2)
    #define portTIMER_COMPARE_PS1       (portBIT_CLEAR)
    #define portTIMER_COMPARE_PS0       (portBIT_SET)
#elif portTIMER_COMPARE_BASE < 0x40000
    #define portTIMER_COMPARE_VALUE     (portTIMER_COMPARE_BASE / 4)
    #define portTIMER_COMPARE_PS1       (portBIT_SET)
    #define portTIMER_COMPARE_PS0       (portBIT_CLEAR)
#elif portTIMER_COMPARE_BASE < 0x80000
    #define portTIMER_COMPARE_VALUE     (portTIMER_COMPARE_BASE / 8)
    #define portTIMER_COMPARE_PS1       (portBIT_SET)
    #define portTIMER_COMPARE_PS0       (portBIT_SET)
#else
    #error "TickRate out of range"
#endif

/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.
 */
void portSetupTick( void )
{
    /*
     * Interrupts are disabled when this function is called.
     */

    /*
     * Setup CCP1
     * Provide the tick interrupt using a compare match on timer1.
     */

    /*
     * Set the compare match value.
     */
    CCPR1H = ( uint8_t ) ( ( portTIMER_COMPARE_VALUE >> 8 ) & 0xff );
    CCPR1L = ( uint8_t )   ( portTIMER_COMPARE_VALUE & 0xff );

    /*
     * Set Compare Special Event Trigger Mode
     */
    bCCP1M3     = portBIT_SET;
    bCCP1M2     = portBIT_CLEAR;
    bCCP1M1     = portBIT_SET;
    bCCP1M0     = portBIT_SET;

    /*
     * Enable CCP1 interrupt
     */
    bCCP1IE     = portBIT_SET;

    /*
     * We are only going to use the global interrupt bit, so disable
     * interruptpriorities and enable peripheral interrupts.
     */
    bIPEN       = portBIT_CLEAR;
    bPEIE       = portBIT_SET;

    /*
     * Set up timer1
     * It will produce the system tick.
     */

    /*
     * Clear the time count
     */
    TMR1H = ( uint8_t ) 0x00;
    TMR1L = ( uint8_t ) 0x00;

    /*
     * Setup the timer
     */
    bRD16       = portBIT_SET;              // 16-bit
    bT1CKPS1    = portTIMER_COMPARE_PS1;    // prescaler
    bT1CKPS0    = portTIMER_COMPARE_PS0;    // prescaler
    bT1OSCEN    = portBIT_SET;              // Oscillator enable
    bT1SYNC     = portBIT_SET;              // No external clock sync
    bTMR1CS     = portBIT_CLEAR;            // Internal clock

    bTMR1ON     = portBIT_SET;              // Start timer1
}
