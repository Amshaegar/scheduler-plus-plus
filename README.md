# sCheduler++

*DISCLAIMER: this library is not ready yet for the production usage.*

Use this library if you want to build your program on tasks and you need not only put tasks in queue, but also schedule them. E.g. you are making an Unified Endpoint Management (UEM) agent and you want to perform some action in specific time: install software, update OS, make another maintanence actions.

## Basic information
1. C++ 20 support required.
2. Calculations run in UTC time.
3. Scheduler precision - seconds.
4. Use chrono time primitives - don't use int for time.
5. There can be only one scheduler in your program, so it's a singletone.
6. This library do not provide multithreading - you can use it only from one thread and tasks are executed in the same thread where Scheduler was made. But getting instance of the scheduler is threadsafe due to the implementation of the singletone.

## Behaviour
1. If execution time of the task is passed, then this task will be executed immidiately.
2. Tasks are not replaced - if you are adding a task and task with the same type is already scheduled, than it will be just added. If tasks have the same time of execution then there is not defined in which order they will execute.

## Possible future improvements
1. Persist tasks (e.g. in SQLite). Add tests: restoration of the tasks from DB and execution of them after restore if it's time is passed.
2. Provide multithreading use of Scheduler - make it possible to be used from different threads.
3. Provide multithread execution of the tasks.
4. Add execution policy - replace tasks if it exist or add it.
5. Determine order of execution tasks in case when youre tasks with the same type and time of execution. I assume the better option is to execute in order of adding tasks - FIFO model.
6. Add support of using of local time, not only UTC.
7. Separate the timer into a separate entity.
    1. Add possibility to tweak scheduler behaviour by using youre own timer.

# Architecture

## Class diagram
```mermaid
classDiagram
    class C_0015491163892027536027["ITimeProvider"]
    class C_0015491163892027536027 {
        <<abstract>>
        +ITimeProvider() void
        +~ITimeProvider() void
        +currentTime() std::chrono::time_point&lt;std::chrono::high_resolution_clock&gt;*
    }
    class C_0000331926717123680878["ITask"]
    class C_0000331926717123680878 {
        <<abstract>>
        +ITask() void
        +~ITask() void
        +execute() void*
        +executionTime() std::chrono::time_point&lt;std::chrono::high_resolution_clock&gt;*
        +name() [const] std::string*
    }
    class C_0003448121167937401076["Scheduler"]
    class C_0003448121167937401076 {
        +Scheduler(const Scheduler & other) void
        -Scheduler() void
        -Scheduler(std::unique_ptr&lt;ITimeProvider&gt; timeProvider) void
        +getInstance(std::unique_ptr&lt;ITimeProvider&gt; timeProvider) Scheduler *$
        +processQueue() void
        -processQueueWithTimeout(std::chrono::duration&lt;double,std::milli&gt; delay_ms) void
        +scheduleTask(std::unique_ptr&lt;ITask&gt; task) void
        -m_queue : std::priority_queue&lt;std::unique_ptr&lt;ITask&gt;,std::vector&lt;std::unique_ptr&lt;ITask&gt;&gt;,decltype&lpar;[]&lpar;const std::unique_ptr&lt;ITask&gt; &l,const std::unique_ptr&lt;ITask&gt; &r&rpar; &lbrace; return l-&gt;executionTime&lpar;&rpar; &gt; r-&gt;executionTime&lpar;&rpar;; &rbrace;&rpar;&gt;
        -m_timeProvider : std::unique_ptr&lt;ITimeProvider&gt;
    }
    class C_0015648913087087734534["SchedulerTest_SortedTasksQueue_Test"]
    class C_0015648913087087734534 {
    }
    C_0003448121167937401076 o-- C_0015491163892027536027 : -m_timeProvider
    C_0003448121167937401076 o-- C_0000331926717123680878 : -m_queue
    C_0003448121167937401076 <.. C_0015648913087087734534 : +[friend]

%% Generated with clang-uml, version 0.6.2
%% LLVM version Ubuntu clang version 18.1.3 (1ubuntu1)
```
## Sequence diagram
```mermaid
sequenceDiagram
    participant Client
    participant Scheduler
    participant Task
    participant TimeProvider

    Client ->> Scheduler : getInstance(...)
    activate Scheduler
    Scheduler ->> Client : *instance
    deactivate Scheduler

    Client ->> Scheduler : scheduleTask(...)
    Scheduler ->> Scheduler : Add task to queue

    loop
    Scheduler ->> Scheduler : processQueue()

    Scheduler ->> Task : executionTime()
    activate Task
    Task ->> Scheduler : Execution time
    deactivate Task

    Scheduler ->> TimeProvider : currentTime()
    activate TimeProvider
    TimeProvider ->> Scheduler : Current time
    deactivate TimeProvider

    Scheduler ->> Task : execute()
    activate Task
    Task ->> Scheduler : Return
    deactivate Task
    end

    Scheduler ->> Scheduler : processQueueWithTimeout(<next task start time>)

    %% Based on generated with clang-uml, version 0.6.2
```

# Example

```cpp
class ExampleTask : public scheduler::ITask {
public:
    ExampleTask(std::chrono::time_point<std::chrono::high_resolution_clock> time, const std::string& name)
        : m_executionTime(time)
        , m_name(name)
    {}
    virtual ~ExampleTask(){}

    void execute() override {
        std::cout << "Exec!" << std::endl;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> executionTime() override {
        return m_executionTime;
    }

    std::string name() const override {
        return m_name;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_executionTime;
    std::string m_name;
};

int main(int argc, char **argv) {
    scheduler::Scheduler* scheduler = scheduler::Scheduler::getInstance(std::make_unique<scheduler::DefaultTimeProvider>());
    scheduler->scheduleTask(std::make_unique<ExampleTask>(std::chrono::high_resolution_clock::now(),
                                                          "ExampleTask"));
    return 0;
}
```


# Reference
1. Base doc for the project is [cppreference](https://en.cppreference.com/index.html), especially [Date and time library](https://en.cppreference.com/w/cpp/chrono.html).
2. Thanks to the project [timercpp](https://github.com/99x/timercpp) and article where I've found it [A Simple Timer in C++](https://www.fluentcpp.com/2018/12/28/timer-cpp/).
3. [GoogleTest User’s Guide](https://google.github.io/googletest/).
4. [Mermaid](https://mermaid.js.org/).
5. [doxygen](https://www.doxygen.nl/index.html).
6. [Visualizing modern C++ code using Clang and UML](https://blog.bkryza.com/posts/generating-uml-diagrams-from-moderncpp-using-clang/).
