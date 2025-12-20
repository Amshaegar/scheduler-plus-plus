// SPDX-License-Identifier: MIT

#include "scheduler-plus-plus.hpp"

#include <thread>
#include <stdexcept>

namespace scheduler{


Scheduler::Scheduler(const std::string& dbPath, const std::map<std::string, std::function<ITask *()> > &tasksRegistry, std::unique_ptr<ITimeProvider> timeProvider)
    : m_dbName(dbPath)
    , m_tasksRegistry(tasksRegistry)
{
    m_timeProvider = std::move(timeProvider);

    initDb();
    createDbIfNeeded();
    loadTasksFromDb();
    processQueue();
}

Scheduler::~Scheduler()
{
    // TODO Check for success
    sqlite3_close(m_db);
}

void Scheduler::scheduleTask(std::unique_ptr<ITask> task)
{
    long taskId = saveTaskToDb(task->name(), task->executionTime());
    // TODO remove this connection between task and DB through id
    task->setId(taskId);
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
        removeTaskFromDb(m_queue.top()->id());
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

void Scheduler::initDb()
{
    if (sqlite3_open(m_dbName.c_str(), &m_db) != SQLITE_OK) {
        throw std::runtime_error("Unable to open scheduler DB!");
    }
}

void Scheduler::createDbIfNeeded()
{
    char* erm = nullptr;
    // NOTE statement IF NOT EXISTS works only starting SQLite 3.3 (didn't find it in official doc)
    int execStatus = sqlite3_exec(m_db,
                                  "CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "name TEXT, "
                                  "executionTime INTEGER)",
                                  0,
                                  0,
                                  &erm);
    if(execStatus != SQLITE_OK) {
        std::string errMessage(erm);
        sqlite3_free(erm);
        throw std::runtime_error("Unable to create scheduler DB!" + errMessage);
    }
}

void Scheduler::loadTasksFromDb()
{
    using TasksVector = std::vector<std::tuple<long, std::string, long>>;
    TasksVector tasks;
    auto callback = [](void *callbackArg, int colNum, char **colDataVec, char **colNames) -> int
    {
        (void)colNames;
        if (colNum != 3)
        {
            return 1;
        }
        long id = std::stol(colDataVec[0]);
        std::string taskName(colDataVec[1]);
        long time = std::stol(colDataVec[2]);
        TasksVector* tasksVector = static_cast<TasksVector*>(callbackArg);
        tasksVector->push_back(std::make_tuple(id, taskName, time));
        return 0;
    };

    char* erm = nullptr;
    std::string statement = "SELECT id, name, executionTime FROM tasks";
    int execStatus = sqlite3_exec(m_db,
                                  statement.c_str(),
                                  callback,
                                  &tasks,
                                  &erm);

    if(execStatus != SQLITE_OK) {
        std::string errMessage(erm);
        sqlite3_free(erm);
        throw std::runtime_error("Unable to save task in scheduler DB!" + errMessage);
    }
    for (const auto& task : tasks) {
        auto [id, taskName, time] = task;
        auto taskTime = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::seconds(time));
        if (m_tasksRegistry.contains(taskName)) {
            scheduler::ITask* task = (m_tasksRegistry)[taskName]();
            task->setName(taskName);
            task->setExecutionTime(taskTime);
            task->setId(id);
            auto taskUnique = std::unique_ptr<ITask>(task);
            m_queue.push(std::move(taskUnique));
        }
    }
}

long Scheduler::saveTaskToDb(const std::string &taskName,
                             std::chrono::time_point<std::chrono::high_resolution_clock> taskExecutionTime)
{
    long taskId;
    char* erm = nullptr;
    std::string statement;
    // TODO Try to store time in ISO 8601 string
    long time = std::chrono::duration_cast<std::chrono::seconds>(taskExecutionTime.time_since_epoch()).count();
    statement = "INSERT INTO tasks (name, executionTime) VALUES "
                "('" + taskName + "', '" + std::to_string(time) + "') "
                "RETURNING id";
    int execStatus = sqlite3_exec(m_db,
                                  statement.c_str(),
                                  [](void *callbackArg, int colNum, char **colDataVec, char **colNames) -> int
                                  {
                                      (void)colNames;
                                      if (colNum != 1)
                                      {
                                          return 1;
                                      }
                                      long *taskIdLong = static_cast<long*>(callbackArg);
                                      *taskIdLong = std::stol(colDataVec[0]);
                                      return 0;
                                  },
                                  &taskId,
                                  &erm);

    if(execStatus != SQLITE_OK) {
        std::string errMessage(erm);
        sqlite3_free(erm);
        throw std::runtime_error("Unable to save task in scheduler DB!" + errMessage);
    }
    return taskId;
}

void Scheduler::removeTaskFromDb(long id)
{
    char* erm = nullptr;
    std::string statement = "DELETE FROM tasks WHERE id = '" + std::to_string(id) + "'";
    int execStatus = sqlite3_exec(m_db,
                                  statement.c_str(),
                                  0,
                                  0,
                                  &erm);

    if(execStatus != SQLITE_OK) {
        std::string errMessage(erm);
        sqlite3_free(erm);
        throw std::runtime_error("Unable to delete task in scheduler DB!" + errMessage);
    }
}

} // namespace scheduler
