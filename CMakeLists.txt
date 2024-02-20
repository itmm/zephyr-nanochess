# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

if(NOT ("${OS}" STREQUAL "LINUX"))
    MESSAGE("search for zephyr")
    find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
endif()

project(nanochess)

if("${OS}" STREQUAL "LINUX")
    MESSAGE("plain linux")
    add_executable(app)
    target_compile_options(app PRIVATE -DCONFIG_ARCH_POSIX)
endif()

target_sources(app PRIVATE src/main.cpp src/chess-interface.cpp src/nano-chess.cpp)
