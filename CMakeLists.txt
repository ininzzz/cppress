cmake_minimum_required(VERSION 2.8)
PROJECT(cppress)
include_directories(${CMAKE_SOURCE_DIR}/include)
FIND_PACKAGE(Threads)
AUX_SOURCE_DIRECTORY(./src SRC_LIST)
ADD_EXECUTABLE(cppress main.cpp ${SRC_LIST})
TARGET_LINK_LIBRARIES(cppress ${CMAKE_THREAD_LIBS_INIT})

# cmake_minimum_required(VERSION 2.8)
# project(cppress)

# include_directories(${CMAKE_SOURCE_DIR}/include)
# link_directories(${CMAKE_SOURCE_DIR}/lib)

# set(LIB_SRC
#     src/Log.cpp
# )

# add_library(cppress SHARED ${LIB_SRC})

# add_executable(main main.cpp)
# add_dependencies(main cppress)
# target_link_libraries(main cppress)
# # target_link_libraries(cppress)

# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
# set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
