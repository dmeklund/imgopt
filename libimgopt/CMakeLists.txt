cmake_minimum_required(VERSION 3.8)
project(imgopt LANGUAGES C)

include_directories(
        "${PROJECT_SOURCE_DIR}/../../Ipopt/include/coin"
        ${PROJECT_SOURCE_DIR}/include

)
set(
        SOURCE_FILES
#        src/imgopt.c
        include/imgopt/util.h
        include/imgopt/signal_network.h
        include/imgopt/thpool.h
        src/signal_network.c
        src/util.c
        src/thpool.c
)

set(
        LIBRARIES
        gc
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Ofast")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_executable(imgopt ${SOURCE_FILES})
target_link_libraries(imgopt ${LIBRARIES})
