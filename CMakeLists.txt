cmake_minimum_required(VERSION 3.6)
project(VJDetector)
set(CMAKE_CXX_STANDARD 11)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
set(SOURCE_FILES main.cpp Classifiers.h Base.h Search.h Pictures.h Image.h JPG.h PNG.h)
add_executable(VJDetector ${SOURCE_FILES})
target_link_libraries(VJDetector
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_REGEX_LIBRARY} ${OpenCV_LIBS}
        )