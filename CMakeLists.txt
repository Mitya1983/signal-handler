cmake_minimum_required(VERSION 3.5)

project(SignalHandler LANGUAGES CXX DESCRIPTION "Simple UNIX signal handler")
include(CustomFunctions.cmake)

option(GENERATE_DEB_PACKAGE "" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_project_properties()

set(LIB_VERSION 0.1)

if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    message(STATUS "CMAKE_INSTALL_PREFIX is not set. $ENV{HOME} directory will be used")
    set(INSTALL_PREFIX "$ENV{HOME}")
else ()
    set(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif ("${CMAKE_INSTALL_PREFIX}" STREQUAL "")

set(GENERAL_INSTALL_DIR ${INSTALL_PREFIX}/tristan_libs)
set(PROJECT_INSTALL_DIR ${GENERAL_INSTALL_DIR}/${PROJECT_NAME})
set(LIBRARY_INSTALL_DIR ${PROJECT_INSTALL_DIR}/${CMAKE_BUILD_TYPE})
message(STATUS "Library install directory is set to ${LIBRARY_INSTALL_DIR}")
set(INCLUDE_INSTALL_DIR ${LIBRARY_INSTALL_DIR}/inc)
message(STATUS "Include install directory is set to ${INCLUDE_INSTALL_DIR}")
set(CMAKE_CONFIG_INSTALL_DIR ${GENERAL_INSTALL_DIR}/cmake/${CMAKE_BUILD_TYPE})
message(STATUS "CMake config files install directory is set to ${CMAKE_CONFIG_INSTALL_DIR}")

set(UNINSTALLER_NAME uninstall_${CMAKE_BUILD_TYPE}.sh)

add_library(${PROJECT_NAME} STATIC signal_handler.hpp signal_handler.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC TRISTAN_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        VERSION ${LIB_VERSION}
        OUTPUT_NAME ${PROJECT_NAME}
)

if (GENERATE_DEB_PACKAGE)
    add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND cpack .
    )
endif (GENERATE_DEB_PACKAGE)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/PackageConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
        PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${LIB_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        DESTINATION ${LIBRARY_INSTALL_DIR})

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        DESTINATION ${CMAKE_CONFIG_INSTALL_DIR})

install(FILES
        signal_handler.hpp
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        DESTINATION ${INCLUDE_INSTALL_DIR})

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "SignalHandler")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "tristan.it@outlook.com")
include(CPack)
