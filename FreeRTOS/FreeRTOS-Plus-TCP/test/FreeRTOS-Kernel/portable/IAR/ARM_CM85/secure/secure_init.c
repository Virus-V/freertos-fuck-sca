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

/* Standard includes. */
#include <stdint.h>

/* Secure init includes. */
#include "secure_init.h"

/* Secure port macros. */
#include "secure_port_macros.h"

/**
 * @brief Constants required to manipulate the SCB.
 */
#define secureinitSCB_AIRCR                 ( ( volatile uint32_t * ) 0xe000ed0c )  /* Application Interrupt and Reset Control Register. */
#define secureinitSCB_AIRCR_VECTKEY_POS     ( 16UL )
#define secureinitSCB_AIRCR_VECTKEY_MASK    ( 0xFFFFUL << secureinitSCB_AIRCR_VECTKEY_POS )
#define secureinitSCB_AIRCR_PRIS_POS        ( 14UL )
#define secureinitSCB_AIRCR_PRIS_MASK       ( 1UL << secureinitSCB_AIRCR_PRIS_POS )

/**
 * @brief Constants required to manipulate the FPU.
 */
#define secureinitFPCCR                     ( ( volatile uint32_t * ) 0xe000ef34 )  /* Floating Point Context Control Register. */
#define secureinitFPCCR_LSPENS_POS          ( 29UL )
#define secureinitFPCCR_LSPENS_MASK         ( 1UL << secureinitFPCCR_LSPENS_POS )
#define secureinitFPCCR_TS_POS              ( 26UL )
#define secureinitFPCCR_TS_MASK             ( 1UL << secureinitFPCCR_TS_POS )

#define secureinitNSACR                     ( ( volatile uint32_t * ) 0xe000ed8c )  /* Non-secure Access Control Register. */
#define secureinitNSACR_CP10_POS            ( 10UL )
#define secureinitNSACR_CP10_MASK           ( 1UL << secureinitNSACR_CP10_POS )
#define secureinitNSACR_CP11_POS            ( 11UL )
#define secureinitNSACR_CP11_MASK           ( 1UL << secureinitNSACR_CP11_POS )
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureInit_DePrioritizeNSExceptions( void )
{
    uint32_t ulIPSR;

    /* Read the Interrupt Program Status Register (IPSR) value. */
    secureportREAD_IPSR( ulIPSR );

    /* Do nothing if the processor is running in the Thread Mode. IPSR is zero
     * when the processor is running in the Thread Mode. */
    if( ulIPSR != 0 )
    {
        *( secureinitSCB_AIRCR ) = ( *( secureinitSCB_AIRCR ) & ~( secureinitSCB_AIRCR_VECTKEY_MASK | secureinitSCB_AIRCR_PRIS_MASK ) ) |
                                   ( ( 0x05FAUL << secureinitSCB_AIRCR_VECTKEY_POS ) & secureinitSCB_AIRCR_VECTKEY_MASK ) |
                                   ( ( 0x1UL << secureinitSCB_AIRCR_PRIS_POS ) & secureinitSCB_AIRCR_PRIS_MASK );
    }
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureInit_EnableNSFPUAccess( void )
{
    uint32_t ulIPSR;

    /* Read the Interrupt Program Status Register (IPSR) value. */
    secureportREAD_IPSR( ulIPSR );

    /* Do nothing if the processor is running in the Thread Mode. IPSR is zero
     * when the processor is running in the Thread Mode. */
    if( ulIPSR != 0 )
    {
        /* CP10 = 1 ==> Non-secure access to the Floating Point Unit is
         * permitted. CP11 should be programmed to the same value as CP10. */
        *( secureinitNSACR ) |= ( secureinitNSACR_CP10_MASK | secureinitNSACR_CP11_MASK );

        /* LSPENS = 0 ==> LSPEN is writable from non-secure state. This ensures
         * that we can enable/disable lazy stacking in port.c file. */
        *( secureinitFPCCR ) &= ~( secureinitFPCCR_LSPENS_MASK );

        /* TS = 1 ==> Treat FP registers as secure i.e. callee saved FP
         * registers (S16-S31) are also pushed to stack on exception entry and
         * restored on exception return. */
        *( secureinitFPCCR ) |= ( secureinitFPCCR_TS_MASK );
    }
}
/*-----------------------------------------------------------*/
