
# Introduction 

This is a cooperative kernel for embedded systems. It is intended for small to
medium sized micro-controllers.

## System specification
1. Static design, no heap memory required

## Scheduler specification
1. Unlimited number of tasks
2. Up to 32 priority levels
3. Round-robing scheduling of tasks with same priority
4. O(1) constant time complexity, scheduling time does not increase if new tasks 
    are added

## TODO list

- Integrate a profiling system (memory/stack usage, cpu usage...)
- test, test, test...


