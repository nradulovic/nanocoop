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
    DIVISION_ROUNDUP(CONFIG_NUM_OF_PRIO_LEVELS, NCPU_DATA_WIDTH)

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
#if     (CONFIG_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    nc_cpu_reg          group;
#endif
    nc_cpu_reg          level[BITMAP_GROUPS];
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

static struct nc_task   g_tasks[CONFIG_NUM_OF_NC_TASKS];

static struct nc_task * g_ready[CONFIG_NUM_OF_PRIO_LEVELS];

static struct nc_task * g_current;

static struct nc_bitmap g_bitmap;

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/


static inline void bitmap_set(
    struct nc_bitmap *          bitmap,
    uint_fast8_t                priority)
{
#if   (CONFIG_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
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
#if   (CONFIG_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
    uint_fast8_t                group;
    uint_fast8_t                index;

    index = priority &
        ((uint_fast8_t)~0u >> (sizeof(priority) * 8u - LOG2_8(NCPU_DATA_WIDTH)));
    group = priority >> LOG2_8(NCPU_DATA_WIDTH);
    bitmap->level[group] &= (nc_cpu_reg)~nc_exp2(index);

    if (bitmap->level[group] == 0u) {                                           /* If this is the last bit cleared in */
        bitmap->group &= (nc_cpu_reg)~nc_exp2(group);                           /* this level group then clear group  */
    }                                                                           /* bit indicator, too.                */
#else
    bitmap->level[0]  &= (nc_cpu_reg)~nc_exp2(priority);
#endif
}



static inline uint_fast8_t bitmap_get_highest(
    const struct nc_bitmap *    bitmap)
{
#if   (CONFIG_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
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
#if   (CONFIG_NUM_OF_PRIO_LEVELS > NCPU_DATA_WIDTH)
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


nc_task * nc_task_create(nc_task_fn * fn, void * stack, uint8_t priority)
{
    uint8_t                     itr;
    nc_isr_lock                 isr_context;
    nc_task *                   new_task;

    new_task    = NULL;
    nc_isr_lock_save(&isr_context);

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
    nc_isr_unlock(&isr_context);

    return (new_task);
}



void nc_task_destroy(nc_task * task)
{
    nc_isr_lock                 isr_context;

    nc_isr_lock_save(&isr_context);

    if (task->ref != 0u) {                           /* Is this task running? */

        if (task->next == task) {           /* Is this the last task in list? */
            uint_fast8_t        priority;

            priority = task->priority;
            g_ready[priority] = NULL;            /* Mark this level as unused */
            bitmap_clear(&g_bitmap, priority);
        } else {
            task->next->prev = task->prev;
            task->prev->next = task->next;
        }
    }
    task->next = NULL;                               /* Mark the task as free */
    nc_isr_unlock(&isr_context);
}



void nc_task_ready(nc_task * task)
{
    nc_isr_lock                 isr_context;

    nc_isr_lock_save(&isr_context);
    task->ref++;

    if (task->ref == 1u) { /* Is this the first time we are marking it ready? */
                                             /* Then insert it in ready queue */
        uint_fast8_t            priority;

        priority  = task->priority;

        if (g_ready[priority] == NULL) {   /* Is this the first task in list? */
            g_ready[priority] = task;              /* Mark this level as used */
            bitmap_set(&g_bitmap, priority);
        } else {
            nc_task *           sentinel = g_ready[priority];

            task->next           = sentinel;
            task->prev           = sentinel->prev;
            sentinel->prev->next = task;
            sentinel->prev       = task;
        }
    }
    nc_isr_unlock(&isr_context);
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

    nc_isr_lock_save(&isr_context);
    current     = nc_task_get_current();
    current->ref--;

    if (current->ref == 0u) {             /* Is this the last task reference? */

        if (current->next == current) {     /* Is this the last task in list? */
            uint_fast8_t        priority;

            priority = current->priority;
            g_ready[priority] = NULL;
            bitmap_clear(&g_bitmap, priority);
        } else {
            current->next->prev = current->prev;
            current->prev->next = current->next;
        }
    }
    nc_isr_unlock(&isr_context);
}



void nc_schedule(void)
{
    nc_isr_lock                 isr_context;

    nc_isr_lock_save(&isr_context);

    while (!bitmap_is_empty(&g_bitmap)) {      /* While there are ready tasks */
        struct nc_task *        new_task;
        uint_fast8_t            priority;

        priority = bitmap_get_highest(&g_bitmap);    /* Get the highest level */
        new_task          = g_ready[priority];          /* Fetch the new task */
        g_current         = new_task;
        g_ready[priority] = new_task->next;    /* Round-robin for other tasks */
        nc_isr_unlock(&isr_context);
        new_task->fn(new_task->stack);                    /* Execute the task */
        nc_isr_lock_save(&isr_context);
    }
    g_current = NULL;
    nc_isr_unlock(&isr_context);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/

#if (CONFIG_NUM_OF_PRIO_LEVELS > (NCPU_DATA_WIDTH * NCPU_DATA_WIDTH))
# error "nanocoop: CONFIG_NUM_OF_PRIO_LEVELS is out of range, the number of priority levels is beyond hardware capability."
#endif

/** @endcond *//** @} *//******************************************************
 * END of ncsched.c
 ******************************************************************************/
