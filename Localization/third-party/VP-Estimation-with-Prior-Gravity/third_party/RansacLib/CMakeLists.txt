cmake_minimum_required (VERSION 3.0)

project(RansacLib VERSION 1.0.0)

if (EXISTS "${CMAKE_SOURCE_DIR}/cmake")
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
endif ()

set(RANSACLIB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
                           ${PROJECT_SOURCE_DIR}/RansacLib
                           CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)


