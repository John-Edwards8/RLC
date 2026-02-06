cmake_minimum_required(VERSION 3.21)
project(RLC LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include(FetchContent)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
)

FetchContent_MakeAvailable(SDL3)

file(GLOB_RECURSE APP_SOURCES
    src/*.cpp
)

add_executable(RLC
    ${APP_SOURCES}
)

target_include_directories(RLC
    PRIVATE headers
)

target_link_libraries(RLC
    PRIVATE
        SDL3::SDL3
)

if (WIN32)
    target_compile_definitions(RLC PRIVATE SDL_MAIN_HANDLED)
endif()