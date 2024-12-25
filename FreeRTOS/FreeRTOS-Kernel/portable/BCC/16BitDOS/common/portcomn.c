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
Changes from V1.00:

    + pxPortInitialiseStack() now initialises the stack of new tasks to the
      same format used by the compiler.  This allows the compiler generated
      interrupt mechanism to be used for context switches.

Changes from V2.6.1

    + Move usPortCheckFreeStackSpace() to tasks.c.
*/


#include <dos.h>
#include <stdlib.h>
#include "FreeRTOS.h"

/*-----------------------------------------------------------*/

/* See header file for description. */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
StackType_t DS_Reg = 0;

    /* Place a few bytes of known values on the bottom of the stack.
    This is just useful for debugging. */

    *pxTopOfStack = 0x1111;
    pxTopOfStack--;
    *pxTopOfStack = 0x2222;
    pxTopOfStack--;
    *pxTopOfStack = 0x3333;
    pxTopOfStack--;
    *pxTopOfStack = 0x4444;
    pxTopOfStack--;
    *pxTopOfStack = 0x5555;
    pxTopOfStack--;


    /*lint -e950 -e611 -e923 Lint doesn't like this much - but nothing I can do about it. */

    /* We are going to start the scheduler using a return from interrupt
    instruction to load the program counter, so first there would be the
    function call with parameters preamble. */

    *pxTopOfStack = FP_SEG( pvParameters );
    pxTopOfStack--;
    *pxTopOfStack = FP_OFF( pvParameters );
    pxTopOfStack--;
    *pxTopOfStack = FP_SEG( pxCode );
    pxTopOfStack--;
    *pxTopOfStack = FP_OFF( pxCode );
    pxTopOfStack--;

    /* Next the status register and interrupt return address. */
    *pxTopOfStack = portINITIAL_SW;
    pxTopOfStack--;
    *pxTopOfStack = FP_SEG( pxCode );
    pxTopOfStack--;
    *pxTopOfStack = FP_OFF( pxCode );
    pxTopOfStack--;

    /* The remaining registers would be pushed on the stack by our context
    switch function.  These are loaded with values simply to make debugging
    easier. */
    *pxTopOfStack = ( StackType_t ) 0xAAAA; /* AX */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0xBBBB; /* BX */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0xCCCC; /* CX */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0xDDDD; /* DX */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0xEEEE; /* ES */
    pxTopOfStack--;

    /* We need the true data segment. */
    __asm{  MOV DS_Reg, DS };

    *pxTopOfStack = DS_Reg;                     /* DS */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x0123; /* SI */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0xDDDD; /* DI */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0xBBBB; /* BP */

    /*lint +e950 +e611 +e923 */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/
