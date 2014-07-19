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
 * @brief       Scheduler header
 * @defgroup    scheduler Scheduler
 * @brief       Scheduler
 *********************************************************************//** @{ */

#ifndef NCSCHED_H
#define NCSCHED_H

/*=========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/**@brief       Task execution state
 */
enum nc_task_state
{
    NC_STATE_IDLE,                          /**<@brief Not executing          */
    NC_STATE_READY,                         /**<@brief Ready for execution    */
    NC_STATE_RUNNING                        /**<@brief Executing              */
};

/**@brief       Task execution state type
 */
typedef enum nc_task_state nc_task_state;

/**@brief       Task function type
 * @details     Each task is executing the function with the following prototype:
 *              `void function(void * stack);`
 */
typedef void (nc_task_fn)(void *);

/**@brief       Task opaque type
 */
typedef struct nc_task nc_task;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


/**@brief       Create a new task
 * @param       fn
 *              Pointer to task function
 * @param       stack
 *              Task stack pointer
 * @param       priority
 *              Task priority. The higher the number higher the imporatance of
 *              the task. The priority may be in the following range:
 *              `0 <= priority < CONFIG_NUM_OF_PRIO_LEVELS`. Several tasks can
 *              have same priority. In this case a round robin scheduling takes
 *              place.
 * @return      Opaque pointer to task structure. Use this pointer to identify
 *              the task.
 * @retval      NULL - no memory for task allocation
 */
nc_task *       nc_task_create(nc_task_fn * fn, void * stack, uint8_t priority);



/**@brief       Destroy a task
 * @param       task
 *              Task identification opaque pointer.
 */
void            nc_task_destroy(nc_task * task);



/**@brief       Make a task ready for execution
 * @param       task
 *              Task identification opaque pointer.
 * @details     After this call the task will enter RUNNING state.
 */
void            nc_task_ready(nc_task * task);



/**@brief       When a task has finished its execution it needs to call this
 *              function in order to transit to IDLE state.
 */
void            nc_task_done(void);



/**@brief       Get the currently executing task id pointer
 */
nc_task *       nc_task_get_current(void);



/**@brief       Get the current state of a task
 * @param       task
 *              Task identification opaque pointer.
 * @return      Current state of a task
 * @retval      NC_STATE_IDLE - task is not executing
 * @retval      NC_STATE_RUNNING - task is executing
 *
 */
nc_task_state   nc_task_get_state(nc_task * task);



/**@brief       Do the scheduling and execute the tasks.
 * @details     This function must be pe
 */
void            nc_schedule(void);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of ncsched.h
 ******************************************************************************/
#endif /* NCSCHED_H */
