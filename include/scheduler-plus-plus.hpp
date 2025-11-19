 // SPDX-License-Identifier: MIT

#include <chrono>
#include <memory>

namespace scheduler{

// Time provider
class ITimeProvider {
public:
    ITimeProvider(){}
    virtual ~ITimeProvider(){};

    virtual std::chrono::time_point<std::chrono::high_resolution_clock> currentTime() = 0;
};

// TODO move to separate file
class DefaultTimeProvider : public ITimeProvider {
public:
    DefaultTimeProvider(){}
    virtual ~DefaultTimeProvider(){}

    virtual std::chrono::time_point<std::chrono::high_resolution_clock> currentTime() override {
        return std::chrono::high_resolution_clock::now();
    };
};

// Timer
class ITimer {
public:
    ITimer(){}
    virtual ~ITimer(){}

    virtual void shoot() = 0;
    virtual void set() = 0;
};

class DefaultTimer : public ITimer {
    DefaultTimer(){}
    virtual ~DefaultTimer(){}

    void shoot() override {

    }

    void set() override {

    }
};

class ITask {
public:
    ITask(){}
    virtual ~ITask(){}

    virtual void execute() = 0;
    virtual std::chrono::time_point<std::chrono::high_resolution_clock> executionTime() = 0;
};

class Scheduler {
public:
    static Scheduler* getInstance(std::unique_ptr<ITimeProvider> timeProvider = std::make_unique<DefaultTimeProvider>());

    void scheduleTask(std::unique_ptr<ITask> task);
    void processTasksQueue();
private:
    Scheduler(){}

    Scheduler(std::unique_ptr<ITimeProvider> timeProvider) {
        m_timeProvider = std::move(timeProvider);
    }

    std::unique_ptr<ITimeProvider> m_timeProvider;
};

/*static*/ Scheduler *Scheduler::getInstance(std::unique_ptr<ITimeProvider> timeProvider)
{
    static Scheduler* instance = new Scheduler(std::move(timeProvider));
    return instance;
}

void Scheduler::scheduleTask(std::unique_ptr<ITask> task)
{
    auto now = m_timeProvider->currentTime();
    if (task->executionTime() <= now) {
        task->execute();
    }
}

/* TODO delete
#include <iostream>
void check() {
   try
   {
       const std::chrono::zoned_time cur_time
       {
           std::chrono::current_zone(), // may throw
           std::chrono::system_clock::now()
       };
       std::cout << cur_time << '\n';
   }
   catch(const std::runtime_error& ex)
   {
       std::cerr << ex.what() << '\n';
   }
}
*/

} // namespace scheduler
