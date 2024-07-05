cmake_minimum_required(VERSION 3.17)
project(JLinkage)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1)
    message(FATAL_ERROR "GCC version needs to be at least 9.1")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++17")

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/include/bm
${PROJECT_SOURCE_DIR}/include/kdtree++
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED
src/JLinkage.cpp
src/RandomSampler.cpp
src/Utilities.cpp
src/VPCluster.cpp
src/VPSample.cpp
src/updator.cpp
)

# Build examples
target_link_libraries(${PROJECT_NAME})
add_executable(main
./main.cpp)
target_link_libraries(main ${PROJECT_NAME})

set(JLINKAGE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                          ${PROJECT_SOURCE_DIR}/include/bm
                          ${PROJECT_SOURCE_DIR}/include/kdtree++
                          CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

