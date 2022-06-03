cmake_minimum_required(VERSION 3.20)
project(KillProcesses)

set(CMAKE_CXX_STANDARD 23)

find_library(PSAPI Psapi)

add_executable(KillProcesses main.cpp)
target_link_libraries(KillProcesses -lpsapi)