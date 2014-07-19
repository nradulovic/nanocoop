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
 * @brief       Configuration
 *********************************************************************//** @{ */

#ifndef NCPORT_H
#define NCPORT_H

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*===============================================================  MACRO's  ==*/

#define CONFIG_ISR_PRIO_LEVEL           0
#define CONFIG_ISR_PRIO_BITS            4

#define nc_exp2_8(value)                (uint8_t)nc_exp2_32(value)
#define nc_exp2_16(value)               (uint16_t)nc_exp2_32(value)
#define nc_exp2_32(value)               (0x1u << (value))

#define nc_log2_8(value)                nc_log2(value)
#define nc_log2_16(value)               nc_log2(value)
#define nc_log2_32(value)               nc_log2(value)

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

typedef uint32_t nc_isr_lock;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

static inline nc_isr_lock nc_isr_save_lock(void)
{
#if (CONFIG_ISR_PRIO_LEVEL != 0)
    uint32_t                    new;
    uint32_t                    old;

    new = (((CONFIG_ISR_PRIO_LEVEL) << (8u - CONFIG_ISR_PRIO_BITS)) & 0xfful);

    __asm __volatile__ (
        "@  nc_isr_save_lock                                \n"
        "   mrs     %0, basepri                             \n"
        "   msr     basepri, %1                             \n"
        : "=&r"(old)
        : "r"(new));

    return (old);
#else
    uint32_t                    new;
    uint32_t                    old;

    new = 1u;

    __asm __volatile__ (
        "@  nintr_replace_mask                              \n"
        "   mrs     %0, primask                             \n"
        "   msr    primask, %1                              \n"
        : "=&r"(old)
        : "r"(new));

    return (old);
#endif
}



static inline void nc_isr_unlock(nc_isr_lock lock)
{
#if (CONFIG_ISR_PRIO_LEVEL != 0)
    __asm __volatile__ (
        "@  nc_isr_unlock                                   \n"
        "   msr    basepri, %0                              \n"
        :
        : "r"(lock));
#else
    __asm __volatile__ (
        "@  nc_isr_unlock                                   \n"
        "   msr    primask, %0                              \n"
        :
        : "r"(lock));
#endif
}



static inline uint_fast8_t nc_log2(uint_fast32_t value)
{
    uint_fast8_t                clz;

    __asm__ __volatile__ (
        "@  nc_log                                          \n"
        "   clz    %0, %1                                   \n"
        : "=r"(clz)
        : "r"(value));

    return (31u - clz);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//**@} *//**@} *//***********************************************
 * END of ncport.h
 ******************************************************************************/
#endif /* NCPORT_H */
