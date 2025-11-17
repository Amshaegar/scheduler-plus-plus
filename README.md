# sCheduler++

Use this library if you want to build your program on tasks and you need not only put tasks in queue, but also schedule them. E.g. you are making an Unified Endpoint Management (UEM) agent and you want to perform some action in specific time: install software, update OS, make another maintanence actions.

DISCLAIMER: this library is not ready yet for the production usage.

TODO determine precision - second/miliseconds/etc

## Basic information
1. C++ 20 support required.
2. Calculations run in UTC time.
3. Use chrono time primitives - don't use int for time.
4. Tasks store in SQLite DB. You can provide path where to store this DB.
5. There can be only one scheduler in your program - it's singletone.
6. This library do not provide multithreading - you can use it only from one thread and tasks are executed in the same thread where Scheduler was made. But getting instance of the scheduler is threadsafe due to the specific implementation of the singletone.
7. Tasks are not replaced - if you are adding a task and task with the same type is already scheduled, than it will be just added. If tasks have the same time of execution then there is not defined in which order they will execute.

## Behaviour
1. If execution time of the task is passed, then this task will be executed immidiately.

## Possible future improvements
1. Provide registry interface to persist tasks in another place - not in SQLite DB.
2. Provide multithreading use of Scheduler - make it possible to be used from different threads.
3. Provide multithread execution of the tasks.
4. Add execution policy - replace tasks if it exist or add.
5. Determine order of execution tasks in case when you tasks with the same type and time of execution. Iassume the better option is to execute in order of adding tasks - FIFO model.
6. Add support of using of local time, not only UTC.
7. Add possibility to tweak scheduler behaviour by using you one implementation of time provider and timer.

# Architecture

## Class diagram

## Sequence diagram

# Example

```cpp


```


# TDD
0. V Scheduler is singletone.
1. V Execute tasks now.
2. -> Execute tasks in specific time.
3. Get tasks list. List must be ordered.
4. Execute list of tasks in different time.
---
5. Restore tasks from DB.
6. Execute all tasks after restore if it's time is passed.
