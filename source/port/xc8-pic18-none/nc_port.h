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

#ifndef NC_PORT_H
#define NC_PORT_H

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>
#include <xc.h>

/*===============================================================  MACRO's  ==*/

#define NCPU_DATA_WIDTH                 8

#define NCPU_DATA_REG_MAX               UINT8_MAX

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

typedef uint8_t                 nc_isr_lock;

typedef uint8_t                 nc_cpu_reg;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


static inline void nc_isr_lock_save(
    nc_isr_lock *               lock)
{
    *lock = INTCONbits.GIE;
    INTCONbits.GIE = 0;
}



static inline void nc_isr_unlock(
    nc_isr_lock *               lock)
{
    INTCONbits.GIE = *lock;
}



static inline nc_cpu_reg nc_exp2(
    uint_fast8_t                value)
{
    extern const nc_cpu_reg     g_exp2_lookup[8];

    return (g_exp2_lookup[value]);
}



static inline uint_fast8_t nc_log2(
    nc_cpu_reg                  value)
{
    extern const uint_fast8_t   g_log2_lookup[256];

    return (g_log2_lookup[value]);
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
