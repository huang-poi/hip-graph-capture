cmake_minimum_required(VERSION 3.20)
project(hip-graph-capture VERSION 0.1.0 LANGUAGES CXX HIP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_HIP_STANDARD 17)

find_package(hip REQUIRED)
find_package(Threads REQUIRED)

add_library(hip_graph_capture SHARED
    src/capture.cpp
    src/graph.cpp
    src/serializer.cpp
    src/analyzer.cpp
    src/replay.cpp
)
target_include_directories(hip_graph_capture PUBLIC include)
target_link_libraries(hip_graph_capture PUBLIC hip::device hip::host Threads::Threads)

add_executable(hip-gcapture tools/hip-gcapture/main.cpp)
target_link_libraries(hip-gcapture PRIVATE hip_graph_capture)

enable_testing()
add_subdirectory(tests)
