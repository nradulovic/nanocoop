
# Introduction 

This is a cooperative kernel for embedded systems. It is intended for small to
medium sized micro-controllers.

## System specification
1. Static design, no heap memory required. All data structures are allocated
    during the compile time.
2. Static configuration, no execution overhead
3. Can be easily integrated into an existing system since it does not requere
    any custom linker script or start-up file.

## Scheduler specification
1. Unlimited number of threads
2. Up to 32 priority levels for a thread
3. Round-robing scheduling of threads with same priority
4. O(1) constant time complexity, scheduling time does not increase if new 
    threads are added

# Usage
## Configuration
All configuration is static. Configuration options are available in 
`source/ncconfig.h` header file. 

Configuration option `CONFIG_NC_NUM_OF_THREADS` is used to specify the maximum 
amount of concurrent threads in the system. Note that threads can be deleted.
So if the system is not executing all threads at the same time then this number
can be set to the maximum number of active threads in order to save RAM memory.

Configuration option `CONFIG_NC_NUM_OF_PRIO_LEVELS` is used to specify the number 
of thread priority levels. It is preferred that this configuration option is held 
below or equal to 8 on low end 8-bit micro-controllers. Higher number of levels 
may impact the execution performance on low end 8-bit micro-controllers.

## Threads
A thread is a function with the following prototype: 

        void function(void * stack)
    
Each thread must return after some defined time. By running till completition the
thread leaves the CPU time for other threads to execute. Ideally, threads are 
organized as finite state machines which by design are always returning.

During the thread execution interrupts are allowed. 

The argument to thread function is always the stack pointer which was given during 
the thread creation process. This gives the ability to write parametrized thread 
functions.

### Creating
A new thread is created using `nc_thread_create()` function. The function searches
through free task pool to obtain a thread data structure.

1. First parameter is pointer to thread function.
2. Second parameter is pointer to thread stack space. This parameter is optional 
and it is needed when writing parametrized thread functions.
3. Third parameter is thread priority. The higher the number the higher the 
importance of the thread. The maximum priority level is defined by 
`CONFIG_NC_NUM_OF_PRIO_LEVELS` configuration option.

After the thread is created the it is put in `NC_STATE_IDLE` state. To put the 
thread in ready/running state use `nc_thread_ready()` function.

### Running
The tasks are invoked by scheduler. Scheduler function `nc_schedule()` must be
periodically called. The scheduler will evaluate all threads that are ready and
schedule them for execution. When there are no threads ready for execution the 
scheduler function will return.

Threads can be created and destroyed during the scheduler execution.

### Destroying
A thread is destroyed by using `nc_thread_destroy()` function. If the thread is 
ready for execution or is currently executing then it will be removed from ready 
queue. When the thread is destroyed its data structure is returned to free thread 
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


