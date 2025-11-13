# The Geometry Factory

A geometry processing library that doesn't get in your way.

No complex dependencies, no undocumented frameworks, no example-only code.
Just the essentials needed to experiment with geometry, nothing more.

## Vision
A simple REPL where scientists can test geometric ideas without fighting the tools.

## Current Status
- OpenGL 3.3 Core visualization
- CMake build system
- C17 implementation

## Build

### Dependencies
- C compiler (GCC or Clang)
- CMake 3.10 or higher
- OpenGL development libraries
- GLFW3

GLAD is included in the source tree. It was generated using the [GLAD web service](https://glad.dav1d.de/) with the following settings:
- Language: C/C++
- Specification: OpenGL
- API: gl Version 3.3
- Profile: Core

### On Ubuntu/Debian:
```bash
# Install dependencies
sudo apt install build-essential cmake libglfw3-dev libgl1-mesa-dev

# Build
mkdir build && cd build
cmake ..
make

# Run
./GeometryFactory
```

### On Other Systems
Just make sure you have a C compiler, CMake, and GLFW3 installed. The build process is the same.

![The Geometry Factory](images/ok.png)
