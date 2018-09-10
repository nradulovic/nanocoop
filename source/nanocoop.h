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
 *//**********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Scheduler header
 * @defgroup    scheduler Scheduler
 * @brief       Scheduler
 ********************************************************************//** @{ */

#ifndef NANOCOOP_H
#define NANOCOOP_H

/*========================================================  INCLUDE FILES  ==*/

#include <stdint.h>

/*==============================================================  MACRO's  ==*/

/**@brief       Nanocoop version number
 * @details     Patch version number is defined by  7 -  0 bits.
 *              Minor version number is defined by 15 -  8 bits.
 *              Major version number is defined by 23 - 16 bits.
 */
#define NC_VERSION                      0x010201

/*-----------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================  DATA TYPES  ==*/

/**@brief       Thread execution state
 */
enum nc_thread_state
{
    NC_STATE_UNINITIALIZED,                 /**<@brief Not initialized       */
    NC_STATE_IDLE,                          /**<@brief Not executing         */
    NC_STATE_READY,                         /**<@brief Ready for execution   */
    NC_STATE_BLOCKED,                       /**<@brief Blocked from execution*/
    NC_STATE_RUNNING                        /**<@brief Executing             */
};

/**@brief       Thread execution state type
 */
typedef enum nc_thread_state nc_thread_state;

/**@brief       Task function type
 * @details     Each thread is executing the function with the following
 *              prototype: `void function(void * stack);`
 */
typedef void (nc_thread_fn)(void *);

/**@brief       Task opaque type
 */
typedef struct nc_thread nc_thread;

/*=====================================================  GLOBAL VARIABLES  ==*/
/*==================================================  FUNCTION PROTOTYPES  ==*/


/**@brief       Create a new thread
 * @param       fn
 *              Pointer to thread function
 * @param       stack
 *              Thread stack pointer
 * @param       priority
 *              Thread priority. The higher the number higher the imporatance
 *              of the thread. The priority may be in the following range:
 *              `0 <= priority < CONFIG_NC_NUM_OF_PRIO_LEVELS`. Several threads
 *              can have same priority. In this case a round robin scheduling
 *              takes place.
 * @return      Opaque pointer to thread structure. Use this pointer to
 *              identify the thread.
 * @retval      NULL - no memory for thread allocation
 */
nc_thread *     nc_thread_create(
    nc_thread_fn *              fn,
    void *                      stack,
    uint_fast8_t                priority);



/**@brief       Destroy a thread
 * @param       thread
 *              Thread identification opaque pointer.
 */
void            nc_thread_destroy(
    nc_thread *                 thread);



/**@brief       Make a thread ready for execution
 * @param       thread
 *              Thread identification opaque pointer.
 * @details     After this call the thread will enter RUNNING state.
 */
void            nc_thread_ready(
    nc_thread *                 thread);



/**@brief       Make a thread blocked
 * @param       thread
 *              Thread identification opaque pointer.
 */
void            nc_thread_block(
    nc_thread *                 thread);



/**@brief       When a thread has finished its execution it needs to call this
 *              function in order to transit to IDLE state.
 */
void            nc_thread_done(void);



/**@brief       Get the currently executing thread id pointer
 */
nc_thread *     nc_thread_get_current(void);



/**@brief       Get the current state of a thread
 * @param       thread
 *              Task identification opaque pointer.
 * @return      Current state of a thread
 * @retval      NC_STATE_IDLE - thread is not executing
 * @retval      NC_STATE_RUNNING - thread is executing
 *
 */
nc_thread_state nc_thread_get_state(
    const nc_thread *           thread);



/**@brief       Do the scheduling and execute the tasks.
 * @details     This function must be continiosly invoked.
 */
void            nc_schedule(void);

/*-------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*===============================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//********************************************
 * END of nanocoop.h
 *****************************************************************************/
#endif /* NANOCOOP_H */
