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

#ifndef __SECURE_PORT_MACROS_H__
#define __SECURE_PORT_MACROS_H__

/**
 * @brief Byte alignment requirements.
 */
#define secureportBYTE_ALIGNMENT         8
#define secureportBYTE_ALIGNMENT_MASK    ( 0x0007 )

/**
 * @brief Macro to declare a function as non-secure callable.
 */
#if defined( __IAR_SYSTEMS_ICC__ )
    #define secureportNON_SECURE_CALLABLE    __cmse_nonsecure_entry __root
#else
    #define secureportNON_SECURE_CALLABLE    __attribute__( ( cmse_nonsecure_entry ) ) __attribute__( ( used ) )
#endif

/**
 * @brief Set the secure PRIMASK value.
 */
#define secureportSET_SECURE_PRIMASK( ulPrimaskValue ) \
    __asm volatile ( "msr primask, %0" : : "r" ( ulPrimaskValue ) : "memory" )

/**
 * @brief Set the non-secure PRIMASK value.
 */
#define secureportSET_NON_SECURE_PRIMASK( ulPrimaskValue ) \
    __asm volatile ( "msr primask_ns, %0" : : "r" ( ulPrimaskValue ) : "memory" )

/**
 * @brief Read the PSP value in the given variable.
 */
#define secureportREAD_PSP( pucOutCurrentStackPointer ) \
    __asm volatile ( "mrs %0, psp"  : "=r" ( pucOutCurrentStackPointer ) )

/**
 * @brief Set the PSP to the given value.
 */
#define secureportSET_PSP( pucCurrentStackPointer ) \
    __asm volatile ( "msr psp, %0" : : "r" ( pucCurrentStackPointer ) )

/**
 * @brief Read the PSPLIM value in the given variable.
 */
#define secureportREAD_PSPLIM( pucOutStackLimit ) \
    __asm volatile ( "mrs %0, psplim"  : "=r" ( pucOutStackLimit ) )

/**
 * @brief Set the PSPLIM to the given value.
 */
#define secureportSET_PSPLIM( pucStackLimit ) \
    __asm volatile ( "msr psplim, %0" : : "r" ( pucStackLimit ) )

/**
 * @brief Set the NonSecure MSP to the given value.
 */
#define secureportSET_MSP_NS( pucMainStackPointer ) \
    __asm volatile ( "msr msp_ns, %0" : : "r" ( pucMainStackPointer ) )

/**
 * @brief Set the CONTROL register to the given value.
 */
#define secureportSET_CONTROL( ulControl ) \
    __asm volatile ( "msr control, %0" : : "r" ( ulControl ) : "memory" )

/**
 * @brief Read the Interrupt Program Status Register (IPSR) value in the given
 * variable.
 */
#define secureportREAD_IPSR( ulIPSR ) \
    __asm volatile ( "mrs %0, ipsr"  : "=r" ( ulIPSR ) )

/**
 * @brief PRIMASK value to enable interrupts.
 */
#define secureportPRIMASK_ENABLE_INTERRUPTS_VAL     0

/**
 * @brief PRIMASK value to disable interrupts.
 */
#define secureportPRIMASK_DISABLE_INTERRUPTS_VAL    1

/**
 * @brief Disable secure interrupts.
 */
#define secureportDISABLE_SECURE_INTERRUPTS()        secureportSET_SECURE_PRIMASK( secureportPRIMASK_DISABLE_INTERRUPTS_VAL )

/**
 * @brief Disable non-secure interrupts.
 *
 * This effectively disables context switches.
 */
#define secureportDISABLE_NON_SECURE_INTERRUPTS()    secureportSET_NON_SECURE_PRIMASK( secureportPRIMASK_DISABLE_INTERRUPTS_VAL )

/**
 * @brief Enable non-secure interrupts.
 */
#define secureportENABLE_NON_SECURE_INTERRUPTS()     secureportSET_NON_SECURE_PRIMASK( secureportPRIMASK_ENABLE_INTERRUPTS_VAL )

/**
 * @brief Assert definition.
 */
#define secureportASSERT( x )                      \
    if( ( x ) == 0 )                               \
    {                                              \
        secureportDISABLE_SECURE_INTERRUPTS();     \
        secureportDISABLE_NON_SECURE_INTERRUPTS(); \
        for( ; ; ) {; }                            \
    }

#endif /* __SECURE_PORT_MACROS_H__ */
