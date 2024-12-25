/*
 * FreeRTOS-Cellular-Interface v1.4.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 */

/**
 * @file cellular_common_portable.h
 */

#ifndef __CELLULAR_COMMON_PORTABLE_H__
#define __CELLULAR_COMMON_PORTABLE_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include <stdint.h>
#include "cellular_common.h"

/*-----------------------------------------------------------*/

/**
 * @brief Cellular module init function.
 *
 * This function is called in Cellular_CommonInit to setup cellular module context.
 *
 * @param[in,out] pContext FreeRTOS Cellular Library context created in Cellular_Init.
 * @param[in] ppModuleContext Cellular module context can be obtained with _Cellular_GetModuleContext.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_ModuleInit( const CellularContext_t * pContext,
                                     void ** ppModuleContext );

/**
 * @brief Cellular module cleanup function.
 *
 * This function cleans up the module context.
 *
 * @param[in,out] pContext FreeRTOS Cellular Library context created in Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_ModuleCleanUp( const CellularContext_t * pContext );

/**
 * @brief Cellular module user equipment setup function.
 *
 * This function setups the user equipment of the cellular module.
 *
 * @param[in,out] pContext FreeRTOS Cellular Library context created in Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_ModuleEnableUE( CellularContext_t * pContext );

/**
 * @brief Cellular module URC enable function.
 *
 * This function enable the unsolicited result code of the cellular module.
 *
 * @param[in,out] pContext FreeRTOS Cellular Library context created in Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_ModuleEnableUrc( CellularContext_t * pContext );

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* End of __CELLULAR_COMMON_PORTABLE_H__. */
