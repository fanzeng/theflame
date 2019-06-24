cmake_minimum_required(VERSION 3.5)
project(apriltageval VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")


set(LIB_TARGET_NAME ${PROJECT_NAME})

SET(${LIB_TARGET_NAME}_SOURCES
        src/tag_projector.cpp
        src/tag_detector.cpp)

add_library(${LIB_TARGET_NAME} SHARED
        ${${LIB_TARGET_NAME}_SOURCES}
        ${Boost_LIBRARIES}
        ${OpenCV_LIBRARIES}
        )

add_subdirectory("${PROJECT_SOURCE_DIR}/test/" "test/bin")
