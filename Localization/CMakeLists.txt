cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
set(DCMAKE_BUILD_TYPE Debug)

project(Line_localization)

find_package(Python3 COMPONENTS Development NumPy)
include_directories(${Python3_INCLUDE_DIRS})

find_package(COLMAP REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS})
link_directories(${COLMAP_LINK_DIRS})

find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)

## CUDA
FIND_PACKAGE(CUDA)
IF(CUDA_FOUND)
  set (EXTRA_INC_DIRS
      ${CUDA_INCLUDE_DIRS}
      ${CUDA_SDK_INCLUDE_DIR}
  )
  cuda_include_directories(${EXTRA_INC_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
  set (EXTRA_LIBRARIES
      ${CUDA_LIBS}
      ${CUDA_cublas_LIBRARY}
  )
ENDIF(CUDA_FOUND)

include_directories(${EXTRA_INC_DIRS})

link_libraries(tiff)

FILE(GLOB SRC_FILES "src/*.cpp")

add_executable(Line_localization ${SRC_FILES})

target_link_libraries(Line_localization 
                        ${Python3_LIBRARIES}
                        ${COLMAP_LIBRARIES}
                        ${Boost_LIBRARIES}
                        ${OpenCV_LIBS} 
                        ${CERES_LIBRARIES} 
                        ${EIGEN3_LIBS} 
                        ${EXTRA_LIBRARIES})