#include "taskmanager.h"
#include "task.h"
#include <chrono>

std::unique_ptr<TaskManager> g_task_manager;

std::unique_ptr<TaskManager> &TaskManager::instance()
{
    if (!g_task_manager)
    {
        g_task_manager.reset(new TaskManager());
    }
    return g_task_manager;
}

bool TaskManager::interruptTask(const std::string &uuid)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto it = _mapTask.find(uuid);
    if (it != _mapTask.end())
    {
        std::shared_ptr<Task> task = it->second;
        if (task)
        {
            task->cancel();
            auto infoIt = _mapProgressInfos.find(uuid);
            if (infoIt != _mapProgressInfos.end())
            {
                infoIt->second.status = Task::Interrupt;
                infoIt->second.endTime = std::chrono::steady_clock::now().time_since_epoch().count();
                infoIt->second.progressText = "Interrupted";
            }
            _mapTask.erase(it);
            return true;
        }
    }
    return false;
}

bool TaskManager::interruptTaskList(const std::vector<std::string> &uuids)
{
    bool ret = true;
    for (const auto &uuid: uuids)
    {
        ret &= interruptTask(uuid);
    }
    return ret;
}

bool TaskManager::exists(const std::string &uuid) const
{
    return _mapProgressInfos.find(uuid) != _mapProgressInfos.end();
}

std::tuple<bool, std::string> TaskManager::createTask(const std::string &id, const std::string &body_params)
{
    std::lock_guard<std::mutex> lock(_mtx);
    std::shared_ptr<Task> task = TaskFactory::createTask(id, body_params);
    if (!task)
    {
        return {false, std::string()};
    }
    task->onBeforeTaskStart.add(this, &TaskManager::onBeforeTaskStart);
    task->onBeforeTaskEnd.add(this, &TaskManager::onBeforeTaskEnd);
    task->onProgressUpdate.add(this, &TaskManager::onProgressUpdate);
    if (task)
    {
        _mapTask.insert({task->id(), task});
        TaskInfo info;
        info.id = task->id();
        info.status = Task::Pending;
        info.createTime = std::chrono::steady_clock::now().time_since_epoch().count();
        info.startTime = 0;
        info.endTime = 0;
        info.progressText = "wait to start...";
        info.progressValue = 0;
        info.result = false;
        _mapProgressInfos.insert({task->id(), info});

        if (task->getMode() == Task::Async)
        {
            _threadPool.enqueue([this, task]() {
                bool ret = task->execute();
                auto it = _mapTask.find(task->id());
                if (it != _mapTask.end())
                {
                    TaskInfo &info = _mapProgressInfos[task->id()];
                    info.result = ret;
                    info.endTime = std::chrono::steady_clock::now().time_since_epoch().count();
                    info.progressText = ret ? "Completed" : "Failed";
                    if (ret)
                    {
                        info.progressValue = 100;
                    }
                    _mapTask.erase(it);
                }
            });
        }
        else
        {
            bool ret = task->execute();
            auto it = _mapTask.find(task->id());
            if (it != _mapTask.end())
            {
                TaskInfo &info = _mapProgressInfos[task->id()];
                info.result = ret;
                info.endTime = std::chrono::steady_clock::now().time_since_epoch().count();
                info.progressText = ret ? "Completed" : "Failed";
                if (ret)
                {
                    info.progressValue = 100;
                }
                _mapTask.erase(it);
            }
        }
    }
    return {true, task->id()};
}

std::optional<TaskManager::TaskInfo> TaskManager::getTaskInfo(const std::string &uuid) const
{
    auto it = _mapProgressInfos.find(uuid);
    if (it != _mapProgressInfos.end())
    {
        return it->second;
    }
    return std::nullopt;
}

std::vector<TaskManager::TaskInfo> TaskManager::getTaskInfos(const std::vector<std::string> &uuids) const
{
    std::vector<TaskInfo> infos;
    for (auto &&[id, info]: _mapProgressInfos)
    {
        if (std::find(uuids.begin(), uuids.end(), id) != uuids.end())
        {
            infos.push_back(info);
        }
    }
    return infos;
}

void TaskManager::onBeforeTaskStart(Task *task)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto it = _mapTask.find(task->id());
    if (it != _mapTask.end())
    {
        TaskInfo &info = _mapProgressInfos[task->id()];
        info.status = Task::Running;
        info.startTime = std::chrono::steady_clock::now().time_since_epoch().count();
    }
}

void TaskManager::onBeforeTaskEnd(Task *task, const boost::json::object &object)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto it = _mapTask.find(task->id());
    if (it != _mapTask.end())
    {
        TaskInfo &info = _mapProgressInfos[task->id()];
        info.status = Task::Finished;
        info.endTime = std::chrono::steady_clock::now().time_since_epoch().count();
        info.object = object;
    }
}

void TaskManager::onProgressUpdate(Task *task, int progressValue, int progressMax, const std::string &progressText)
{
    (void) progressMax;
    std::lock_guard<std::mutex> lock(_mtx);
    auto it = _mapTask.find(task->id());
    if (it != _mapTask.end())
    {
        TaskInfo &info = _mapProgressInfos[task->id()];
        if (info.status == Task::Interrupt)
        {
            // If the task has been interrupted, do not update progress
            return;
        }
        info.status = Task::Running;
        info.progressValue = progressValue;
        info.progressText = progressText;
    }
}