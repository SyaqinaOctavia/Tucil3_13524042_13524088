cmake_minimum_required(VERSION 3.15)
project(IceSliderPuzzle CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Letakkan executable di bin/ sesuai spek pengumpulan
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/loadSave.cpp
    src/heuristikCost.cpp
    src/searchAlgorithm.cpp
    src/gui.cpp
)

add_executable(main ${SOURCES})
target_include_directories(main PRIVATE src)

# Raylib (untuk GUI). Cari otomatis via find_package — bekerja kalau raylib
# diinstal via pacman MSYS2 (mingw-w64-x86_64-raylib), apt (libraylib-dev),
# vcpkg (pakai -DCMAKE_TOOLCHAIN_FILE=...), dll.
find_package(raylib REQUIRED)
target_link_libraries(main PRIVATE raylib)

# Warning flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(main PRIVATE /W4)
endif()
