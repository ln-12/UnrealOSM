cmake_minimum_required(VERSION 3.5)
project(OSMImporter)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(PROTOZERO_INCLUDE_DIR "/usr/local/include")

find_package(Protozero 1.6.1 REQUIRED)

if(NOT PROTOZERO_FOUND)
    message(WARNING "Protozero not found!\n")
endif()

include_directories(SYSTEM ${PROTOZERO_INCLUDE_DIR})

find_package(Osmium 2.15.3 REQUIRED COMPONENTS io gdal)

if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()

include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

set(FLATBUFFERS_SRC_DIR "${CMAKE_SOURCE_DIR}/flatbuffers")

# Add FlatBuffers directly to our build. This defines the `flatbuffers` target.
add_subdirectory(${FLATBUFFERS_SRC_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build
        EXCLUDE_FROM_ALL)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(OSMImporter OSMImporter.cpp AreaHandler.cpp  NodeWayHandler.cpp OSMHelper.cpp UTM.cpp SplineHandler.cpp)
set_pthread_on_target(OSMImporter)
target_link_libraries(OSMImporter ${OSMIUM_IO_LIBRARIES} flatbuffers)