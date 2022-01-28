cmake_minimum_required(VERSION 2.8)
PROJECT(cppress)
include_directories(${CMAKE_SOURCE_DIR}/include)
FIND_PACKAGE(Threads)
AUX_SOURCE_DIRECTORY(./src SRC_LIST)
ADD_EXECUTABLE(cppress main.cpp ${SRC_LIST})
TARGET_LINK_LIBRARIES(cppress ${CMAKE_THREAD_LIBS_INIT})


