#include "task.h"

bool TaskFactory::registerClass(const std::string &name, Creator creator)
{
    auto &&factory = Instance();
    return factory.creators_.emplace(name, creator).second;
}

std::shared_ptr<Task> TaskFactory::createTask(const std::string &name, const std::string &body_params)
{
    auto &&factory = Instance();
    std::shared_ptr<Task> task;
    auto it = factory.creators_.find(name);
    if (it == factory.creators_.end())
        return nullptr;

    task = it->second(body_params);
    if (!task)
    {
        return nullptr;
    }
    return task;
}

std::vector<std::string> TaskFactory::getRegisteredClasses()
{
    auto &factory = Instance();
    std::vector<std::string> names;
    for (const auto &pair: factory.creators_)
    {
        names.push_back(pair.first);
    }
    return names;
}

TaskFactory &TaskFactory::Instance()
{
    static TaskFactory instance;
    return instance;
}