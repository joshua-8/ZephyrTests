# SPDX-License-Identifier: Apache-2.0

set(BOARD esp32_devkitc_wroom/esp32/procpu)

# Add the overlay file
set(DTC_OVERLAY_FILE esp32.overlay)

cmake_minimum_required(VERSION 3.20.0)

find_package(zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(esp32_motor_ctrl)

target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/hbridge.cpp)
target_sources(app PRIVATE src/BSED.cpp)
target_sources(app PRIVATE src/controlLoop.cpp)
