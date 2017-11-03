cmake_minimum_required(VERSION 3.8)
project(imgopt LANGUAGES C)

include_directories(
        "${PROJECT_SOURCE_DIR}/../../Ipopt/include/coin"
)
set(
        SOURCE_FILES
        src/imgopt.c
        src/signal_network.c
)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_library(imgopt SHARED ${SOURCE_FILES})
