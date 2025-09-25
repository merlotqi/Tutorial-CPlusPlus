#ifndef TASK_H
#define TASK_H

#include "delegate.h"
#include <boost/json.hpp>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Task
{
public:
    enum Status
    {
        Pending,  // Task is created but not started yet
        Running,  // Task is currently running
        Finished, // Task has finished, regardless of success or failure
        Interrupt,// Task was interrupted, usually by user action
    };

    enum Mode
    {
        Sync,// Task runs synchronously
        Async// Task runs asynchronously
    };

    explicit Task(const std::string &parameters);
    virtual ~Task();
    virtual bool execute();
    virtual bool cancel();
    virtual std::string name() const;

    std::string id() const;
    Mode getMode() const;

    void success(bool bSuccess);
    bool isSuccess() const;

    Delegate<void(Task *)> onBeforeTaskStart;
    Delegate<void(Task *, int, int, const std::string &)> onProgressUpdate;
    Delegate<void(Task *, const boost::json::object &)> onBeforeTaskEnd;

protected:
    Mode m_mode;
    std::string m_id;
    bool m_success;
    bool m_canceled;
};


class TaskFactory
{
public:
    using Creator = std::function<std::shared_ptr<Task>(const std::string &)>;

    static bool registerClass(const std::string &name, Creator creator);
    static std::shared_ptr<Task> createTask(const std::string &name, const std::string &body_params = std::string());
    static std::vector<std::string> getRegisteredClasses();
    virtual ~TaskFactory() = default;

private:
    std::map<std::string, Creator> creators_;

    static TaskFactory &Instance();
};

#define REGISTER_CLASS(CLASS, NAME)                                                                 \
    namespace {                                                                                     \
    bool CLASS##_registered = []() {                                                                \
        return TaskFactory::registerClass(                                                          \
                NAME, [](const QString &params) { return QSharedPointer<CLASS>::create(params); }); \
    }();                                                                                            \
    }


#endif// TASK_H