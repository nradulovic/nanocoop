/*
 * This file is part of nanocoop
 *
 * Copyright (C) 2014 - Nenad Radulovic
 *
 * nanocoop is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * nanocoop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with eSolid.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    http://github.com/nradulovic
 * e-mail  :    nenad.b.radulovic@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Port header
 *********************************************************************//** @{ */

#ifndef NC_PORT_H
#define NC_PORT_H

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*===============================================================  MACRO's  ==*/

#define CONFIG_ISR_PRIO_LEVEL           0

#define CONFIG_ISR_PRIO_BITS            4

#define NCPU_DATA_WIDTH                 32

#define NCPU_DATA_REG_MAX               UINT32_MAX

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

typedef unsigned int            nc_isr_lock;

typedef unsigned int            nc_cpu_reg;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


static inline void nc_isr_lock_save(
    nc_isr_lock *               lock)
{
#if (CONFIG_ISR_PRIO_LEVEL != 0)
    uint32_t                    new;

    new = (((CONFIG_ISR_PRIO_LEVEL) << (8u - CONFIG_ISR_PRIO_BITS)) & 0xfful);

    __asm __volatile__ (
        "@  nc_isr_lock_save                                \n"
        "   mrs     %0, basepri                             \n"
        "   msr     basepri, %1                             \n"
        : "=&r"(*lock)
        : "r"(new));
#else
    uint32_t                    new;

    new = 1u;

    __asm __volatile__ (
        "@  nc_isr_lock_save                                \n"
        "   mrs    %0, primask                              \n"
        "   msr    primask, %1                              \n"
        : "=&r"(*lock)
        : "r"(new));
#endif
}



static inline void nc_isr_unlock(
    nc_isr_lock *               lock)
{
#if (CONFIG_ISR_PRIO_LEVEL != 0)
    __asm __volatile__ (
        "@  nc_isr_unlock                                   \n"
        "   msr    basepri, %0                              \n"
        :
        : "r"(*lock));
#else
    __asm __volatile__ (
        "@  nc_isr_unlock                                   \n"
        "   msr    primask, %0                              \n"
        :
        : "r"(*lock));
#endif
}



static inline nc_cpu_reg nc_exp2(
    uint_fast8_t                value)
{
    return (0x1u << value);
}



static inline uint_fast8_t nc_log2(
    nc_cpu_reg                  value)
{
    uint_fast8_t                clz;

    __asm__ __volatile__ (
        "@  nc_log                                          \n"
        "   clz    %0, %1                                   \n"
        : "=r"(clz)
        : "r"(value));

    return (31u - clz);
}



static inline void nc_sat_increment(
    nc_cpu_reg *                value)
{
    if (*value != NCPU_DATA_REG_MAX) {
        (*value)++;
    }
}



static inline void nc_sat_decrement(
    nc_cpu_reg *                value)
{
    if (*value != 0u) {
        (*value)--;
    }
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//**@} *//**@} *//***********************************************
 * END of nc_port.h
 ******************************************************************************/
#endif /* NC_PORT_H */
