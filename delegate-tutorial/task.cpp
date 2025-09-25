#include "task.h"
#include <boost/uuid.hpp>

Task::Task(const std::string &parameters) : m_canceled(false), m_success(false)
{
    m_id = boost::uuids::to_string(boost::uuids::random_generator()());
    m_mode = Mode::Async;
}

Task::~Task() = default;

bool Task::execute()
{
    m_canceled = false;
    return true;
}

bool Task::cancel()
{
    m_canceled = true;
    return true;
}

std::string Task::name() const
{
    return "";
}

std::string Task::id() const
{
    return m_id;
}

Task::Mode Task::getMode() const
{
    return m_mode;
}

void Task::success(bool bSuccess)
{
    m_success = bSuccess;
}

bool Task::isSuccess() const
{
    return m_success;
}