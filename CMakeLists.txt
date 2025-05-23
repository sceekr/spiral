cmake_minimum_required(VERSION 3.16)
project(SPIRAL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ======== System-installed gdstk ========
# find_library(GDSTK_LIBRARY NAMES gdstk REQUIRED)
# find_path(GDSTK_INCLUDE_DIR gdstk.h)

# ======== Local gdstk sources ========
add_subdirectory(external/gdstk)

file(GLOB_RECURSE SPIRAL_SOURCES
    src/*.cc
    src/*.cpp
)

include_directories(
    ${Geant4_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/gdstk/include
)

add_executable(spiral ${SPIRAL_SOURCES})

# -----------------------
# Link libraries
# -----------------------
target_link_libraries(spiral
    ${Geant4_LIBRARIES}
    gdstk
)

# -----------------------
# Optional: install and post-build data setup
# -----------------------
install(TARGETS spiral DESTINATION bin)

# Optionally copy Geant4 datasets or GDML files post-build
# (add_custom_command/post_build logic if needed)
