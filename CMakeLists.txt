cmake_minimum_required(VERSION 4.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(short_url_server)
file(GLOB_RECURSE short_url_server_header include/*.hpp include/*.h)
file(GLOB_RECURSE short_url_server_cpp src/*.cpp src/*.cc src/*.c)

find_package(MPI )
find_package(OpenMP)
IF(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_SHARED_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ELSE()
    message("Missed OpenMP!")
ENDIF()

option (USE_OPENMP "Use omp to parallel data checking." ON)
if(USE_OPENMP)
    add_definitions(-DUSE_OPENMP)
ENDIF()

include_directories(include)
add_executable(short_url_server ${short_url_server_header} ${short_url_server_cpp})

target_link_libraries(short_url_server PocoJSON PocoNet PocoFoundation PocoUtil glog)


target_include_directories(short_url_server PUBLIC include)
