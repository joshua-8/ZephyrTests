# SPDX-License-Identifier: Apache-2.0
#TODO 
set(BOARD nrf5340dk/nrf5340/cpuapp)

cmake_minimum_required(VERSION 3.20.0)

find_package(zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(nRF5340_multithreaded)

target_sources(app PRIVATE src/main.cpp)
