cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

project(lopm VERSION 0.1.0)

set(VERSION ${PROJECT_VERSION})

add_subdirectory(src)