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
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code that tailors the port to a specific RISC-V chip:
 *
 * + FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S contains the code that
 *   is common to all currently supported RISC-V chips.  There is only one
 *   portASM.S file because the same file is built for all RISC-V target chips.
 *
 * + Header files called freertos_risc_v_chip_specific_extensions.h contain the
 *   code that tailors the FreeRTOS kernel's RISC-V port to a specific RISC-V
 *   chip.  There are multiple freertos_risc_v_chip_specific_extensions.h files
 *   as there are multiple RISC-V chip implementations.
 *
 * !!!NOTE!!!
 * TAKE CARE TO INCLUDE THE CORRECT freertos_risc_v_chip_specific_extensions.h
 * HEADER FILE FOR THE CHIP IN USE.  This is done using the assembler's (not the
 * compiler's!) include path.  For example, if the chip in use includes a core
 * local interrupter (CLINT) and does not include any chip specific register
 * extensions then add the path below to the assembler's include path:
 * FreeRTOS\Source\portable\GCC\RISC-V-RV32\chip_specific_extensions\RV32I_CLINT_no_extensions
 *
 */

/*
 * This freertos_risc_v_chip_specific_extensions.h is for use with Pulpino Ri5cy
 * devices, developed and tested using the Vega board RV32M1RM.
 */

#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_MTIME                  0

/* Constants to define the additional registers found on the Pulpino RI5KY. */
#define lpstart0                          0x7b0
#define lpend0                            0x7b1
#define lpcount0                          0x7b2
#define lpstart1                          0x7b4
#define lpend1                            0x7b5
#define lpcount1                          0x7b6

/* Six additional registers to save and restore, as per the #defines above. */
#define portasmADDITIONAL_CONTEXT_SIZE    6 /* Must be even number on 32-bit cores. */

/* Save additional registers found on the Pulpino. */
.macro portasmSAVE_ADDITIONAL_REGISTERS
addi sp, sp, -( portasmADDITIONAL_CONTEXT_SIZE * portWORD_SIZE ) /* Make room for the additional registers. */
csrr t0, lpstart0                                                /* Load additional registers into accessible temporary registers. */
csrr t1, lpend0
csrr t2, lpcount0
csrr t3, lpstart1
csrr t4, lpend1
csrr t5, lpcount1
sw t0, 1 * portWORD_SIZE( sp )
sw t1, 2 * portWORD_SIZE( sp )
sw t2, 3 * portWORD_SIZE( sp )
sw t3, 4 * portWORD_SIZE( sp )
sw t4, 5 * portWORD_SIZE( sp )
sw t5, 6 * portWORD_SIZE( sp )
   .endm

/* Restore the additional registers found on the Pulpino. */
   .macro portasmRESTORE_ADDITIONAL_REGISTERS
lw t0, 1 * portWORD_SIZE( sp ) /* Load additional registers into accessible temporary registers. */
lw t1, 2 * portWORD_SIZE( sp )
lw t2, 3 * portWORD_SIZE( sp )
lw t3, 4 * portWORD_SIZE( sp )
lw t4, 5 * portWORD_SIZE( sp )
lw t5, 6 * portWORD_SIZE( sp )
csrw lpstart0, t0
csrw lpend0, t1
csrw lpcount0, t2
csrw lpstart1, t3
csrw lpend1, t4
csrw lpcount1, t5
addi sp, sp, ( portasmADDITIONAL_CONTEXT_SIZE * portWORD_SIZE ) /* Remove space added for additional registers. */
   .endm

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
