cmake_minimum_required(VERSION 3.8)
project(imgopt LANGUAGES C CXX)

include_directories(
        "${PROJECT_SOURCE_DIR}/../../Ipopt/include/coin"
        ${PROJECT_SOURCE_DIR}/include
        /usr/local/include/plplot
)
set(
        SOURCE_FILES
#        src/imgopt.c
        include/imgopt/util.h
        include/imgopt/signal_network.h
        include/imgopt/thpool.h
        src/signal_network.c
        src/util.c
        src/node.cpp
        src/thpool.c
)

set(
        LIBRARIES
        gc
        m
        pthread
)

link_directories(/usr/local/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Ofast -std=c11 -pthread -fPIC")
#set(CMAKE_LINK_LIBRARY_FLAG )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_executable(imgopt ${SOURCE_FILES})
target_link_libraries(imgopt ${LIBRARIES})

add_executable(test_plplot src/script/test_plplot.c)
target_link_libraries(test_plplot plplotd)