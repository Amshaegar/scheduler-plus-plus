// SPDX-License-Identifier: MIT

#include "scheduler-plus-plus.hpp"

#include <thread>

namespace scheduler{

Scheduler::Scheduler() {}

Scheduler::Scheduler(std::unique_ptr<ITimeProvider> timeProvider) {
    m_timeProvider = std::move(timeProvider);
}

/*static*/ Scheduler* Scheduler::getInstance(std::unique_ptr<ITimeProvider> timeProvider)
{
    static Scheduler* instance = new Scheduler(std::move(timeProvider));
    return instance;
}

void Scheduler::scheduleTask(std::unique_ptr<ITask> task)
{
    m_queue.push(std::move(task));
    processQueue();
}

void Scheduler::processQueue()
{
    while (!m_queue.empty()) {
        if (m_queue.top()->executionTime() > m_timeProvider->currentTime()) {
            break;
        }
        m_queue.top()->execute();
        m_queue.pop();
    }

    if (m_queue.empty()) {
        return;
    }

    processQueueWithTimeout(m_queue.top()->executionTime() - m_timeProvider->currentTime());
}

void Scheduler::processQueueWithTimeout(std::chrono::duration<double, std::milli> delay_ms)
{
    std::thread t([this, delay_ms]() {
        std::this_thread::sleep_for(delay_ms);
        processQueue();
    });
    t.detach();
}

} // namespace scheduler
