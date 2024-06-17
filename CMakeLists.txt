cmake_minimum_required(VERSION 3.0)
project(search-ads-filtering)

##################
# Compiler setting
##################
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# We require a C++17 compliant compiler
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################
# Build executable files
########################
add_executable(apiGateWay src/main.cpp src/server/epoll_server.cpp src/server/epoll_event.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(apiGateWay PRIVATE Threads::Threads)
