
# Introduction 

This is a cooperative kernel for embedded systems. It is intended for small to
medium sized micro-controllers.

## System specification
1. Static design, no heap memory required. All data structures are allocated
    during the compile time.
2. Static configuration, no execution overhead
3. Can be easily integrated into an existing system

## Scheduler specification
1. Unlimited number of tasks
2. Up to 32 priority levels
3. Round-robing scheduling of tasks with same priority
4. O(1) constant time complexity, scheduling time does not increase if new tasks 
    are added

# Usage
## Configuration
All configuration is static. Configuration options are available in 
`source/ncconfig.h` header file. 

Configuration option `CONFIG_NUM_OF_NC_TASKS` is used to specify the maximum 
amount of concurrent tasks in the system. Note that tasks can be deleted, too.
So if the system is not executing all tasks at the same time then this number
can be set to the maximum number of active tasks in order to save RAM memory.

Configuration option `CONFIG_NUM_OF_PRIO_LEVELS` is used to specify the number 
of task priority levels. It is preferred that this configuration option is held 
below or equal to 8 on low end 8-bit micro-controllers. Higher number of levels 
may impact the execution performance on low end 8-bit micro-controllers.

## Tasks
A task is a function with the following prototype: 

        void function(void * stack)
    
Each task must return after some defined time. By returning the task leaves the
CPU time for other tasks to execute. Ideally, tasks are organized as finite 
state machines which by design are always returning.

During the task execution interrupts are allowed. 

The argument to task function is always the stack pointer which was given during 
the task creation process. This gives the ability to write parametrized tasks 
functions.

### Creating
A new task is created using `nc_task_create()` function. The function searches
through free task pool to obtain a task data structure.

1. First parameter is pointer to task function.
2. Second parameter is pointer to task stack space. This parameter is optional 
and it is needed when writing parametrized task functions.
3. Third parameter is task priority. The higher the number the higher the 
importance of the task. The maximum priority level is defined by 
`CONFIG_NUM_OF_PRIO_LEVELS` configuration option.

After the task is created the task is in `NC_STATE_IDLE` state. To put the task
in ready/running state use `nc_task_ready()` function.

### Running
The tasks are invoked by scheduler. Scheduler function `nc_schedule()` must be
periodically called. The scheduler will evaluate all tasks that are ready and
schedule them for execution. When there are no tasks ready for execution the 
scheduler function will return.

Tasks can be created and destroyed during the scheduler execution.

### Destroying
A task is destroyed by using `nc_task_destroy()` function. If the task is ready
for execution or is currently executing then it will be removed from ready 
queue. When the task is destroyed its data structure is returned to free task 
pool.

## Building

## TODO list

- Integrate a profiling system (memory/stack usage, cpu usage...)
- test, test, test...

# Licence

nanocoop is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

nanocoop is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with nanocoop.  If not, see <http://www.gnu.org/licenses/>.



* web site:    http://github.com/nradulovic
* e-mail  :    nenad.b.radulovic@gmail.com


