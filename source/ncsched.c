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

#include "ncsched.h"
#include "ncconfig.h"
#include "ncport.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/

struct nc_task
{
    struct nc_task *    next;
    struct nc_task *    prev;
    void             (* fn)(void *);
    void *              stack;
    uint_fast8_t        priority;
    uint_fast8_t        ref;
};

struct nc_bitmap
{
#if (CONFIG_NUM_OF_PRIO_LEVELS > 16)
    uint32_t            level;
#elif (CONFIG_NUM_OF_PRIO_LEVELS > 8)
    uint16_t            level;
#else
    uint8_t             level;
#endif
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/


/**@brief       Set a bit corresponding to the the given priority
 */
static inline void bitmap_set(uint_fast8_t priority);



/**@brief       Clear a bit corresponding to the the given priority
 */
static inline void bitmap_clear(uint_fast8_t priority);



/**@brief       Get the highest set bit priority level
 */
static inline uint_fast8_t bitmap_highest(void);



/**@brief       Is the bitmap empty?
 * @return
 * @retval      true  - no bit is set
 * @retval      false - at least one bit is set
 */
static inline bool bitmap_is_empty(void);

/*=======================================================  LOCAL VARIABLES  ==*/

static struct nc_task   g_tasks[CONFIG_NUM_OF_NC_TASKS];

static struct nc_task * g_ready[CONFIG_NUM_OF_PRIO_LEVELS];

static struct nc_task * g_current;

static struct nc_bitmap g_bitmap;

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static inline void bitmap_set(uint_fast8_t priority)
{
#if (CONFIG_NUM_OF_PRIO_LEVELS > 16)
    g_bitmap.level |= nc_power_32(priority);
#elif (CONFIG_NUM_OF_PRIO_LEVELS > 8)
    g_bitmap.level |= nc_power_16(priority);
#else
    g_bitmap.level |= nc_exp2_8(priority);
#endif
}



static inline void bitmap_clear(uint_fast8_t priority)
{
#if (CONFIG_NUM_OF_PRIO_LEVELS > 16)
    g_bitmap.level &= (uint32_t)~nc_power_32(priority);
#elif (CONFIG_NUM_OF_PRIO_LEVELS > 8)
    g_bitmap.level &= (uint16_t)~nc_power_16(priority);
#else
    g_bitmap.level &= (uint8_t)~nc_exp2_8(priority);
#endif
}



static inline uint_fast8_t bitmap_highest(void)
{
#if (CONFIG_NUM_OF_PRIO_LEVELS > 16)
    return (nc_log2_32(g_bitmap.level));
#elif (CONFIG_NUM_OF_PRIO_LEVELS > 8)
    return (nc_log2_16(g_bitmap.level));
#else
    return (nc_log2_8(g_bitmap.level));
#endif
}



static inline bool bitmap_is_empty(void)
{
    if (g_bitmap.level == 0u) {
        return (true);
    } else {
        return (false);
    }
}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


nc_task * nc_task_create(nc_task_fn * fn, void * stack, uint8_t priority)
{
    uint8_t                     itr;
    nc_isr_lock                 isr_context;
    nc_task *                   new_task;

    new_task    = NULL;
    isr_context = nc_isr_save_lock();

    for (itr = 0; itr < CONFIG_NUM_OF_NC_TASKS; itr++) {   /* Find empty slot */
        if (g_tasks[itr].next == NULL) {
            new_task           = &g_tasks[itr];        /* Initialize new task */
            new_task->next     = new_task;       /* Init linked list pointers */
            new_task->prev     = new_task;
            new_task->fn       = fn;
            new_task->stack    = stack;
            new_task->priority = priority;
            new_task->ref      = 0u;

            break;
        }
    }
    nc_isr_unlock(isr_context);

    return (new_task);
}



void nc_task_destroy(nc_task * task)
{
    nc_isr_lock                 isr_context;

    isr_context = nc_isr_save_lock();

    if (task->ref != 0u) {                           /* Is this task running? */

        if (task->next == task) {           /* Is this the last task in list? */
            uint_fast8_t        priority;

            priority = task->priority;
            g_ready[priority] = NULL;            /* Mark this level as unused */
            bitmap_clear(priority);
        } else {
            task->next->prev = task->prev;
            task->prev->next = task->next;
        }
    }
    task->next = NULL;                               /* Mark the task as free */
    nc_isr_unlock(isr_context);
}



void nc_task_ready(nc_task * task)
{
    nc_isr_lock                 isr_context;

    isr_context = nc_isr_save_lock();
    task->ref++;

    if (task->ref == 1u) { /* Is this the first time we are marking it ready? */
                                             /* Then insert it in ready queue */
        uint_fast8_t            priority;

        priority  = task->priority;

        if (g_ready[priority] == NULL) {   /* Is this the first task in list? */
            g_ready[priority] = task;              /* Mark this level as used */
            bitmap_set(priority);
        } else {
            nc_task *           sentinel = g_ready[priority];

            task->next           = sentinel;
            task->prev           = sentinel->prev;
            sentinel->prev->next = task;
            sentinel->prev       = task;
        }
    }
    nc_isr_unlock(isr_context);
}



nc_task * nc_task_get_current(void)
{
    return (g_current);
}



nc_task_state nc_task_get_state(nc_task * task)
{
    if (task == nc_task_get_current()) {
        return (NC_STATE_RUNNING);
    } else if (task->ref != 0u) {
        return (NC_STATE_READY);
    } else {
        return (NC_STATE_IDLE);
    }
}



void nc_task_done(void)
{
    struct nc_task *            current;
    nc_isr_lock                 isr_context;

    isr_context = nc_isr_save_lock();
    current     = nc_task_get_current();
    current->ref--;

    if (current->ref == 0u) {             /* Is this the last task reference? */

        if (current->next == current) {     /* Is this the last task in list? */
            uint_fast8_t        priority;

            priority = current->priority;
            g_ready[priority] = NULL;
            bitmap_clear(priority);
        } else {
            current->next->prev = current->prev;
            current->prev->next = current->next;
        }
    }
    nc_isr_unlock(isr_context);
}



void nc_schedule(void)
{
    nc_isr_lock                 isr_context;

    isr_context = nc_isr_save_lock();

    while (!bitmap_is_empty()) {               /* While there are ready tasks */
        struct nc_task *        new_task;
        uint_fast8_t            priority;

        priority = bitmap_highest();        /* Get the highest priority level */
        new_task = g_ready[priority];           /* Fetch task from this level */
        g_ready[priority] = new_task->next;    /* Round-robin for other tasks */
        g_current         = new_task;
        nc_isr_unlock(isr_context);
        new_task->fn(new_task->stack);                    /* Execute the task */
        isr_context = nc_isr_save_lock();
    }
    g_current = NULL;
    nc_isr_unlock(isr_context);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of ncsched.c
 ******************************************************************************/
