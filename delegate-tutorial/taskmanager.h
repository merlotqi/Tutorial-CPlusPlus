#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "task.h"
#include "threadpool.h"
#include <map>
#include <memory>
#include <mutex>
#include <optional>

class TaskManager
{
public:
    struct TaskInfo
    {
        std::string id;
        Task::Status status;
        int64_t createTime;
        int64_t startTime;
        int64_t endTime;
        std::string progressText;
        int progressValue;
        bool result;
        boost::json::object object;
    };

    static std::unique_ptr<TaskManager> &instance();

    bool interruptTask(const std::string &uuid);

    bool interruptTaskList(const std::vector<std::string> &uuids);

    bool exists(const std::string &uuid) const;

    std::tuple<bool, std::string> createTask(const std::string &name, const std::string &body_params = std::string());

    std::optional<TaskInfo> getTaskInfo(const std::string &uuid) const;

    std::vector<TaskInfo> getTaskInfos(const std::vector<std::string> &uuids) const;

private:
    void onBeforeTaskStart(Task *task);

    void onBeforeTaskEnd(Task *task, const boost::json::object &);

    void onProgressUpdate(Task *task, int progressValue, int progressMax, const std::string &progressText);

private:
    std::map<std::string, std::shared_ptr<Task>> _mapTask;
    std::map<std::string, TaskInfo> _mapProgressInfos;
    std::mutex _mtx;
    ThreadPool _threadPool;
};

#endif// TASK_MANAGER_H