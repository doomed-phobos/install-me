cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

project(lopm VERSION 1.1.0)

set(VERSION ${PROJECT_VERSION})

add_subdirectory(src)