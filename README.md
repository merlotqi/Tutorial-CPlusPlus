# Tutorial-CPlusPlus

A collection of C++ tutorials and small demos with CMake build support. Each subfolder contains a focused tutorial or sample and usually its own CMakeLists.txt.

## Quick Links
- [CMakeLists.txt](Tutorial-CPlusPlus/CMakeLists.txt) — top-level project file that aggregates subprojects.
- [ftxui-tutorial](Tutorial-CPlusPlus/ftxui-tutorial) — terminal UI examples using FTXUI. See [ftxui-tutorial/CMakeLists.txt](Tutorial-CPlusPlus/ftxui-tutorial/CMakeLists.txt) and examples like [`canvas_animated.cpp`](Tutorial-CPlusPlus/ftxui-tutorial/component/canvas_animated.cpp).
- [opengl-tutorial](Tutorial-CPlusPlus/opengl-tutorial) — OpenGL/GLFW/GLEW examples. See [opengl-tutorial/CMakeLists.txt](Tutorial-CPlusPlus/opengl-tutorial/CMakeLists.txt) and the helper class [`Shader`](Tutorial-CPlusPlus/opengl-tutorial/shader.h).
- [delegate-tutorial](Tutorial-CPlusPlus/delegate-tutorial) — small delegate/task framework. Key files: [`delegate.h`](Tutorial-CPlusPlus/delegate-tutorial/delegate.h), [`task.h`](Tutorial-CPlusPlus/delegate-tutorial/task.h), [`taskmanager.h`](Tutorial-CPlusPlus/delegate-tutorial/taskmanager.h).
- [asio_server](Tutorial-CPlusPlus/asio_server) — Boost ASIO based server examples. See [asio_server/CMakeLists.txt](Tutorial-CPlusPlus/asio_server/CMakeLists.txt).
- [gtk-tutorial](Tutorial-CPlusPlus/gtk-tutorial) — GTK4 example (CMake).
- [ds-C](Tutorial-CPlusPlus/ds-C) — data-structure C examples with tests and CMake.
- [hash-algorithm](Tutorial-CPlusPlus/hash-algorithm) — hashing examples and CMakeLists.
- [queue](Tutorial-CPlusPlus/queue) — queue-related examples.

## Prerequisites
- CMake 3.10+
- C++17 compatible compiler
- Platform-specific dependencies:
  - OpenGL/GLFW/GLEW for [opengl-tutorial](Tutorial-CPlusPlus/opengl-tutorial)
  - Boost (system/json) for [asio_server](Tutorial-CPlusPlus/asio_server) and parts of [delegate-tutorial](Tutorial-CPlusPlus/delegate-tutorial)
  - ftxui library for [ftxui-tutorial](Tutorial-CPlusPlus/ftxui-tutorial)
  - GTK4 for [gtk-tutorial](Tutorial-CPlusPlus/gtk-tutorial)

## Build (example)
Build all tutorials from the Tutorial-CPlusPlus root:

```sh
mkdir build && cd build
cmake ..
cmake --build .