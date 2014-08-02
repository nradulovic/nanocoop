/*
 * This file is part of nanocoop
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
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
 * web site:    http://blueskynet.dyndns-server.com
 * e-mail  :    blueskyniss@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Parametrized blinky example
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <stdbool.h>
#include <stdio.h>

#include "nanocoop.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define FAST_BLINKY_PERIOD              100
#define SLOW_BLINKY_PERIOD              1000
#define GREEN_LED                       0
#define RED_LED                         1

/*======================================================  LOCAL DATA TYPES  ==*/

struct blinky_stack
{
    uint32_t counter;
    uint32_t reload;
    uint8_t  led;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void blinky_fn      (void *);
static void toggle_green_fn(void *);
static void toggle_red_fn  (void *);

/*=======================================================  LOCAL VARIABLES  ==*/

/* Each thread is referenced by using nc_thread structure.
 */
static nc_thread * g_fast_blinky;
static nc_thread * g_slow_blinky;
static nc_thread * g_toggle_green;
static nc_thread * g_toggle_red;

static struct blinky_stack g_fast_blinky_stack =
{
     FAST_BLINKY_PERIOD,
     FAST_BLINKY_PERIOD,
     GREEN_LED
};

static struct blinky_stack g_slow_blinky_stack =
{
     SLOW_BLINKY_PERIOD,
     SLOW_BLINKY_PERIOD,
     RED_LED
};

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void blinky_fn(void * stack_)
{
    /* Cast the void pointer to something more appropriate.
     */
    struct blinky_stack * stack = stack_;

    stack->counter--;

    if (stack->counter == 0) {
        stack->counter = stack->reload;

        switch (stack->led) {
            case GREEN_LED : {
                nc_thread_ready(g_toggle_green);
                break;
            }
            case RED_LED : {
                nc_thread_ready(g_toggle_red);
                break;
            }
        }
    }
}

static void toggle_green_fn(void * stack)
{
    (void)stack;
    printf("\n Toggle GREEN led\n");

    /* This is one shot thread. When it's done doing it's job it will make
     * himself idle.
     */
    nc_thread_done();
}

static void toggle_red_fn(void * stack)
{
    (void)stack;
    printf("\n Toggle RED led\n");
    nc_thread_done();
}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

int main(void)
{
    /* Create four threads.
     *
     * Threads g_fast_blinky and g_slow_blinky are implementing counters for
     * software timers. When the timer finishes each thread will make either
     * g_toggle_green or g_toggle_red to do the actual job.
     */
    g_fast_blinky  = nc_thread_create(blinky_fn,       &g_fast_blinky_stack, 1);
    g_slow_blinky  = nc_thread_create(blinky_fn,       &g_slow_blinky_stack, 1);
    g_toggle_green = nc_thread_create(toggle_green_fn, NULL,                 7);
    g_toggle_red   = nc_thread_create(toggle_red_fn,   NULL,                 7);

    /* When threads are created they are in NC_STATE_IDLE state. In order to run
     * them nc_thread_ready() must be called with pointer to thread structure.
     */
    nc_thread_ready(g_fast_blinky);
    nc_thread_ready(g_slow_blinky);

    /* Execute all running threads. Function nc_schedule() must be periodically
     * called to execute all ready threads.
     */
    while (true) {
        nc_schedule();
    }

    return (0);
}

/* Start-up code exit function declaration
 */
void _exit(int);

/* Start-up code exit function implementation
 */
void _exit(int code)
{
    (void)code;

    while (true);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of main.c
 ******************************************************************************/
