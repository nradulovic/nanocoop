
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

Edit the `CONFIG_NUM_OF_NC_TASKS` configuration option to specify the maximum 
amount of concurrent tasks in the system. Note that tasks can be deleted, too.
So if you are not executing all tasks at the same time you can set this number
to the maximum number of active tasks in order to save RAM memory.

To specify the number of priority levels edit the `CONFIG_NUM_OF_PRIO_LEVELS`
configuration option. It is preferred that this configuration option is set to
maximum value of 8 on low end 8-bit micro-controllers.

## TODO list

- Integrate a profiling system (memory/stack usage, cpu usage...)
- test, test, test...


