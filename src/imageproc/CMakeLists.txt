cmake_minimum_required(VERSION 3.5)
project(imageproc VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAG "${CMAKE_CXX_FLAGS} -std=c++11 ")

set(LIB_TARGET_NAME ${PROJECT_NAME})
include_directories("..")
set(${LIB_TARGET_NAME}_SOURCES src/imageproc.cpp)

add_library(${LIB_TARGET_NAME} SHARED
        ${${LIB_TARGET_NAME}_SOURCES}
        )

set(${LIB_TARGET_NAME}_PRIVATE_LIBS
        ${LIB_TARGET_NAME}
        )

set(${LIB_TARGET_NAME}_PUBLIC_LIBS
        ${Boost_LIBRARIES}
        ${OpenCV_LIBS}
        )

set(${LIB_TART_NAME}_LIBS
        ${${LIB_TARGET_NAME}_PUBLIC_LIBS}
        ${${LIB_TARGET_NAME}_PRIVATE_LIBS}
        )
        
target_link_libraries(${LIB_TARGET_NAME}
        ${${LIB_TARGET_NAME}_PUBLIC_LIBS}
        )

add_executable(imagetransform src/imagetransform.cpp)
target_link_libraries(imagetransform
        ${${LIB_TART_NAME}_LIBS}
        )

install(TARGETS imagetransform DESTINATION bin/)

install(TARGETS ${LIB_TARGET_NAME}
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/)

install(DIRECTORY src/
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h*"
)        