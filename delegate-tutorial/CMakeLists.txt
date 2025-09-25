find_package(Boost REQUIRED COMPONENTS json)

add_library(task STATIC 
    task.h
    task.cpp
    task_factory.cpp
    taskmanager.h
    taskmanager.cpp
)

target_link_libraries(task Boost::json)

add_executable(lambda_delegate lambda_delegate.cpp)

set_target_properties(task PROPERTIES FOLDER "delegate-tutorial")
set_target_properties(lambda_delegate PROPERTIES FOLDER "delegate-tutorial")