/*
 * This file is part of nanocoop.
 *
 * Copyright (C) 2014 Nenad Radulovic
 *
 * nanocoop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nanocoop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with nanocoop.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    http://github.com/nradulovic
 * e-mail  :    nenad.b.radulovic@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Port Implementation
 * @addtogroup  port
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "ncport.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

const uint8_t g_exp2_lookup_8[8] =
{
    (1u <<  0), (1u <<  1), (1u <<  2), (1u <<  3),
    (1u <<  4), (1u <<  5), (1u <<  6), (1u <<  7)
};

const uint16_t g_exp2_lookup_16[16] =
{
    (1u <<  0), (1u <<  1), (1u <<  2), (1u <<  3),
    (1u <<  4), (1u <<  5), (1u <<  6), (1u <<  7),
    (1u <<  8), (1u <<  9), (1u << 10), (1u << 11),
    (1u << 12), (1u << 13), (1u << 14), (1u << 15)
};

const uint32_t g_exp2_lookup_32[32] =
{
    (1u <<  0), (1u <<  1), (1u <<  2), (1u <<  3),
    (1u <<  4), (1u <<  5), (1u <<  6), (1u <<  7),
    (1u <<  8), (1u <<  9), (1u << 10), (1u << 11),
    (1u << 12), (1u << 13), (1u << 14), (1u << 15),
    (1u << 16), (1u << 17), (1u << 18), (1u << 19),
    (1u << 20), (1u << 21), (1u << 22), (1u << 23),
    (1u << 24), (1u << 25), (1u << 26), (1u << 27),
    (1u << 28), (1u << 29), (1u << 30), (1u << 31)
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


uint_fast8_t nc_log2_8(uint_fast8_t val)
{
    uint_fast8_t                tmp;
    uint_fast8_t                ret;

    ret = 0u;
    tmp = val >> 4;

    if (tmp != 0u) {
        val  = tmp;
        ret += 4u;
    }
    tmp = val >> 2;

    if (tmp != 0u) {
        val  = tmp;
        ret += 2u;
    }
    tmp = val >> 1;

    if (tmp != 0u) {
        ret += 1u;
    }

    return (ret);
}



uint_fast8_t nc_log2_16(uint_fast16_t val)
{
    uint_fast16_t               tmp;
    uint_fast8_t                ret;

    ret = 0u;
    tmp = val >> 8;

    if (tmp != 0u) {
        val  = tmp;
        ret += 8u;
    }
    tmp = val >> 4;

    if (tmp != 0u) {
        val  = tmp;
        ret += 4u;
    }
    tmp = val >> 2;

    if (tmp != 0u) {
        val  = tmp;
        ret += 2u;
    }
    tmp = val >> 1;

    if (tmp != 0u) {
        ret += 1u;
    }

    return (ret);
}



uint_fast8_t nc_log2_32(uint_fast32_t val)
{
    uint_fast32_t               tmp;
    uint_fast8_t                ret;

    ret = 0u;
    tmp = val >> 16;

    if (tmp != 0u) {
        val  = tmp;
        ret  = 16u;
    }
    tmp = val >> 8;

    if (tmp != 0u) {
        val  = tmp;
        ret += 8u;
    }
    tmp = val >> 4;

    if (tmp != 0u) {
        val  = tmp;
        ret += 4u;
    }
    tmp = val >> 2;

    if (tmp != 0u) {
        val  = tmp;
        ret += 2u;
    }
    tmp = val >> 1;

    if (tmp != 0u) {
        ret += 1u;
    }

    return (ret);
}



/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ncport.c
 ******************************************************************************/
