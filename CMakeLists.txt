cmake_minimum_required(VERSION 3.5)

project(SignalHandler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SignalHandler main.cpp signalhandler.hpp signalhandler.cpp)
