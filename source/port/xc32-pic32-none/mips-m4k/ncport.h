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
#include <xc.h>

/*===============================================================  MACRO's  ==*/

#define CONFIG_ISR_PRIO_LEVEL           6

#define NCPU_DATA_WIDTH                 32

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

typedef unsigned char nc_isr_lock;

typedef unsigned char nc_cpu_reg;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

static inline __attribute__((nomips16)) void nc_isr_lock_save(
    nc_isr_lock *               lock)
{
#if (CONFIG_ISR_PRIO_LEVEL == 0)
    nc_isr_lock                 status;

    __asm __volatile__(
        " di %0                                             \n"
        : "=r"(status));
    *lock = status;
#else
    nc_isr_lock                 status;

    status  = _CP0_GET_STATUS();
    *lock   = status;
    status &= ~_CP0_STATUS_IPL_MASK;
    _CP0_SET_STATUS(status);
#endif
}


static inline void __attribute__((nomips16)) nc_isr_unlock(
    nc_isr_lock *               lock)
{
#if (CONFIG_INTR_MAX_ISR_PRIO == 0)

    if (*lock == 0u) {
        __asm __volatile__(
            " di \n");
    } else {
        __asm __volatile__(
            " ei \n");
    }
#else
    nc_isr_lock                 status;

    status  = _CP0_GET_STATUS();
    status &= ~_CP0_STATUS_IPL_MASK;
    status |= *lock & _CP0_STATUS_IPL_MASK;
    _CP0_SET_STATUS(status);
#endif
}



static inline nc_cpu_reg nc_exp2(uint_fast8_t value)
{
    extern const nc_cpu_reg g_exp2_lookup[8];

    return (g_exp2_lookup[value]);
}



static inline uint_fast8_t nc_log2(nc_cpu_reg value)
{
    extern const uint_fast8_t g_log2_lookup[256];

    return (g_log2_lookup[value]);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//**@} *//**@} *//***********************************************
 * END of ncport.h
 ******************************************************************************/
#endif /* NCPORT_H */
