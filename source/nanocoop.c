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
 * along with nanocoop.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    http://github.com/nradulovic
 * e-mail  :    nenad.b.radulovic@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Scheduler Implementation
 * @addtogroup  scheduler
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>
#include <stdbool.h>

#include "nanocoop.h"
#include "nc_config.h"
#include "nc_port.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define LOG2_8(x)                                                               \
    ((x) <   2u ? 0u :                                                          \
     ((x) <   4u ? 1u :                                                         \
      ((x) <   8u ? 2u :                                                        \
       ((x) <  16u ? 3u :                                                       \
        ((x) <  32u ? 4u :                                                      \
         ((x) <  64u ? 5u :                                                     \
          ((x) < 128u ? 6u : 7u)))))))

#define DIVISION_ROUNDUP(numerator, denominator)                                \
    (((numerator) + (denominator) - 1u) / (denominator))

#define BITMAP_GROUPS                                                           \
    DIVISION_ROUNDUP(CONFIG_NC_NUM_OF_PRIO_LEVELS, NCPU_DATA_WIDTH)

/*======================================================  LOCAL DATA TYPES  ==*/

struct nc_thread
{
    struct nc_thread *          next;
    struct nc_thread *          prev;
    void                     (* fn)(void *);
    void *                      stack;
    uint_fast8_t                priority;
    nc_cpu_reg                  ref;
};

struct nc_bitmap
{
#if     (CONFIG_NC_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    nc_cpu_reg                  group;
#endif
    nc_cpu_reg                  level[BITMAP_GROUPS];
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/


/**@brief       Set a bit corresponding to the the given priority
 */
static inline void bitmap_set(
    struct nc_bitmap *          bitmap,
    uint_fast8_t                priority);



/**@brief       Clear a bit corresponding to the the given priority
 */
static inline void bitmap_clear(
    struct nc_bitmap *          bitmap,
    uint_fast8_t                priority);



/**@brief       Get the highest set bit priority level
 */
static inline uint_fast8_t bitmap_get_highest(
    const struct nc_bitmap *    bitmap);



/**@brief       Is the bitmap empty?
 * @return
 * @retval      true  - no bit is set
 * @retval      false - at least one bit is set
 */
static inline bool bitmap_is_empty(
    const struct nc_bitmap *    bitmap);

/*=======================================================  LOCAL VARIABLES  ==*/

/**@brief       Pool memory for task structures which are allocated through
 *              nc_thread_create() function.
 */
static struct nc_thread   g_threads[CONFIG_NC_NUM_OF_THREADS];

/**@brief       Pointer array to ready tasks in the system
 */
static struct nc_thread * g_ready[CONFIG_NC_NUM_OF_PRIO_LEVELS];

/**@brief       Pointer to the currently executed task
 */
static struct nc_thread * g_current;

/**@brief       Bitmap which allows to quickly select which ready task to run
 */
static struct nc_bitmap   g_bitmap;

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static inline void bitmap_set(
    struct nc_bitmap *          bitmap,
    uint_fast8_t                priority)
{
#if   (CONFIG_NC_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    uint_fast8_t                group;
    uint_fast8_t                index;

    index = priority &
        ((uint_fast8_t)~0u >> (sizeof(priority) * 8u - LOG2_8(NCPU_DATA_WIDTH)));
    group = priority >> LOG2_8(NCPU_DATA_WIDTH);
    bitmap->group        |= nc_exp2(group);
    bitmap->level[group] |= nc_exp2(index);
#else
    bitmap->level[0]     |= nc_exp2(priority);
#endif
}



static inline void bitmap_clear(
    struct nc_bitmap *          bitmap,
    uint_fast8_t                priority)
{
#if   (CONFIG_NC_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    uint_fast8_t                group;
    uint_fast8_t                index;

    index = priority &
        ((uint_fast8_t)~0u >> (sizeof(priority) * 8u - LOG2_8(NCPU_DATA_WIDTH)));
    group = priority >> LOG2_8(NCPU_DATA_WIDTH);
    bitmap->level[group] &= (nc_cpu_reg)~nc_exp2(index);

    if (bitmap->level[group] == 0u) {   /* If this is the last bit cleared in */
                                        /* this level group then clear group  */
                                        /* bit indicator, too.                */
        bitmap->group &= (nc_cpu_reg)~nc_exp2(group);
    }
#else
    bitmap->level[0]  &= (nc_cpu_reg)~nc_exp2(priority);
#endif
}



static inline uint_fast8_t bitmap_get_highest(
    const struct nc_bitmap *    bitmap)
{
#if   (CONFIG_NC_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    uint_fast8_t                group;
    uint_fast8_t                index;

    group = nc_log2(bitmap->group);
    index = nc_log2(bitmap->level[group]);

    return ((uint_fast8_t)((group << LOG2_8(NCPU_DATA_WIDTH)) | index));
#else
    uint_fast8_t                index;

    index = nc_log2(bitmap->level[0]);

    return (index);
#endif
}



static inline bool bitmap_is_empty(
    const struct nc_bitmap *    bitmap)
{
#if   (CONFIG_NC_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    if (bitmap->group == 0u) {
        return (true);
    } else {
        return (false);
    }
#else
    if (bitmap->level[0] == 0u) {
        return (true);
    } else {
        return (false);
    }
#endif
}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


nc_thread * nc_thread_create(
    nc_thread_fn *              fn,
    void *                      stack,
    uint_fast8_t                priority)
{
    uint16_t                    itr;
    nc_isr_lock                 isr_context;
    nc_thread *                 new_thread;

    new_thread = NULL;
    nc_isr_lock_save(&isr_context);

    for (itr = 0; itr < CONFIG_NC_NUM_OF_THREADS; itr++) {   /* Find empty slot */
        if (g_threads[itr].next == NULL) {
            new_thread           = &g_threads[itr];    /* Initialize new task */
            new_thread->next     = new_thread;   /* Init linked list pointers */
            new_thread->prev     = new_thread;
            new_thread->fn       = fn;
            new_thread->stack    = stack;
            new_thread->priority = priority;
            new_thread->ref      = 0u;

            break;
        }
    }
    nc_isr_unlock(&isr_context);

    return (new_thread);
}



void nc_thread_destroy(
    nc_thread *                 thread)

    nc_thread_block(thread);
    thread->next = NULL;                           /* Mark the thread as free */
}



void nc_thread_ready(
    nc_thread *                 thread)
{
    nc_isr_lock                 isr_context;

    nc_isr_lock_save(&isr_context);
    nc_sat_increment(&thread->ref);

    if (thread->ref == 1u) {       /* Is this the first time thread is ready? */
                                             /* Then insert it in ready queue */
        uint_fast8_t            priority;

        priority  = thread->priority;

        if (g_ready[priority] == NULL) { /* Is this the first thread in list? */
            g_ready[priority] = thread;            /* Mark this level as used */
            bitmap_set(&g_bitmap, priority);
        } else {
            nc_thread *         sentinel = g_ready[priority];

            thread->next         = sentinel;
            thread->prev         = sentinel->prev;
            sentinel->prev->next = thread;
            sentinel->prev       = thread;
        }
    }
    nc_isr_unlock(&isr_context);
}



void nc_thread_done(void)
{
    nc_thread_block(nc_thread_get_current());
}



void nc_thread_block(
    nc_thread *                 thread)
{
    nc_isr_lock                 isr_context;

    nc_isr_lock_save(&isr_context);
    nc_sat_increment(&thread->ref);

    if (thread->ref == 0u) {            /* Is this the last thread reference? */

        if (thread->next == thread) {     /* Is this the last thread in list? */
            uint_fast8_t        priority;

            priority          = thread->priority;
            g_ready[priority] = NULL;
            bitmap_clear(&g_bitmap, priority);
        } else {
            thread->next->prev = thread->prev;
            thread->prev->next = thread->next;
        }
    }
    nc_isr_unlock(&isr_context);
}



nc_thread * nc_thread_get_current(void)
{
    return (g_current);
}



nc_thread_state nc_thread_get_state(
    const nc_thread *           thread)
{
    if (thread == nc_thread_get_current()) {
        return (NC_STATE_RUNNING);
    } else if (thread->ref != 0u) {
        return (NC_STATE_READY);
    } else {
        return (NC_STATE_IDLE);
    }
}



void nc_schedule(void)
{
    nc_isr_lock                 isr_context;

    nc_isr_lock_save(&isr_context);

    while (!bitmap_is_empty(&g_bitmap)) {      /* While there are ready tasks */
        struct nc_thread *      new_thread;
        uint_fast8_t            priority;

        priority = bitmap_get_highest(&g_bitmap);    /* Get the highest level */
        new_thread        = g_ready[priority];          /* Fetch the new task */
        g_current         = new_thread;
        g_ready[priority] = new_thread->next;  /* Round-robin for other tasks */
        nc_isr_unlock(&isr_context);
        new_thread->fn(new_thread->stack);              /* Execute the thread */
        nc_isr_lock_save(&isr_context);
    }
    g_current = NULL;           /* We are exiting the loop, no task is active */
    nc_isr_unlock(&isr_context);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/

#if (CONFIG_NC_NUM_OF_PRIO_LEVELS > (NCPU_DATA_WIDTH * NCPU_DATA_WIDTH))
# error "nanocoop: CONFIG_NUM_OF_PRIO_LEVELS is out of range, the number of priority levels is beyond hardware capability."
#endif

/** @endcond *//** @} *//******************************************************
 * END of ncsched.c
 ******************************************************************************/
