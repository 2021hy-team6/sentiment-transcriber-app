cmake_minimum_required(VERSION 3.11)

project(ts LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/)
set(CMAKE_BUILD_TYPE Release)

set(BUILD_CTL OFF CACHE BOOL "Build drogon_ctl")
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(BUILD_ORM OFF CACHE BOOL "Build orm")
set(BUILD_BROTLI OFF CACHE BOOL "Build Brotli")

add_subdirectory(dependencies/drogon)
add_subdirectory(dependencies/fmt)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
find_package(AWSSDK COMPONENTS transcribestreaming)
find_package(PythonLibs REQUIRED)
find_package(rapidJSON REQUIRED MODULE)
find_package(Threads REQUIRED)
find_library(spacy spacy REQUIRED)
find_library(tensorflow tensorflow REQUIRED)

set(SOURCES
  controllers/websocket.h controllers/websocket.cpp

  include/aws-transcribe.h src/aws-transcribe.cpp 
  include/sentiment-inference.h src/sentiment-inference.cpp 
  include/server.h src/server.cpp  

  src/audio-capture.cpp
  src/main.cpp
)
file(COPY client DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include ${PYTHON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE drogon portaudio fmt::fmt rapidJSON::rapidJSON
                      Threads::Threads spacy tensorflow ${AWSSDK_LINK_LIBRARIES} ${PYTHON_LIBRARIES})
