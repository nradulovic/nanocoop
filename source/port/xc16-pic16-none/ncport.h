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
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

typedef unsigned char nc_isr_lock;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


static inline nc_isr_lock nc_isr_save_lock(void)
{
    return (0);
}



static inline void nc_isr_unlock(nc_isr_lock lock)
{
    (void)lock;
}



static inline uint8_t nc_exp2_8(uint_fast8_t value)
{
    extern const uint8_t g_exp2_lookup_8[8];

    return (g_exp2_lookup_8[value]);
}



static inline uint16_t nc_exp2_16(uint_fast8_t value)
{
    extern const uint16_t g_exp2_lookup_16[16];

    return (g_exp2_lookup_16[value]);
}



static inline uint32_t nc_exp2_32(uint_fast8_t value)
{
    extern const uint32_t g_exp2_lookup_32[32];

    return (g_exp2_lookup_32[value]);
}



uint_fast8_t nc_log2_8(uint_fast8_t value);



uint_fast8_t nc_log2_16(uint_fast16_t value);



uint_fast8_t nc_log2_32(uint_fast32_t value);


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//**@} *//**@} *//***********************************************
 * END of ncport.h
 ******************************************************************************/
#endif /* NCPORT_H */
