cmake_minimum_required(VERSION 3.10)
project(tutotial-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

add_subdirectory(ftxui-tutorial)
add_subdirectory(gtk-tutorial)
add_subdirectory(opengl-tutorial)
add_subdirectory(asio_server)
add_subdirectory(delegate-tutorial)