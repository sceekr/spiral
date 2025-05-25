cmake_minimum_required(VERSION 3.16)
project(SPIRAL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geant4 REQUIRED)

add_subdirectory(external/gdstk)
add_subdirectory(external/CLI11)

file(GLOB_RECURSE SPIRAL_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(spiral main.cpp ${SPIRAL_SOURCES})

target_include_directories(spiral PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/gdstk/include
    ${PROJECT_SOURCE_DIR}/external/CLI11/include
    ${Geant4_INCLUDE_DIRS}
)

target_link_libraries(spiral PRIVATE
    gdstk
    ${Geant4_LIBRARIES}
)