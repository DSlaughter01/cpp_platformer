cmake_minimum_required(VERSION 3.24)
project(Platformer)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

add_definitions(-DPROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.26.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.6.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

# START ADDITION
set(SDL2IMAGE_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)
# END ADDITION

FetchContent_MakeAvailable(SDL2_image)

file(GLOB SOURCES "src/*.cpp")
add_executable(Platformer ${SOURCES})
target_link_libraries(Platformer SDL2::SDL2main SDL2::SDL2-static SDL2_image::SDL2_image-static)