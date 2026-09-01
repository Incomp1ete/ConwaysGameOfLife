include(CheckIncludeFile)
include(FetchContent)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.32.8
)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)

add_executable(GameOfLife)

if(MSVC)
    target_compile_options(GameOfLife
    PRIVATE
    /W4
    /WX
    )
    set_source_files_properties(nuklear_impl.c
    PROPERTIES
    COMPILE_OPTIONS
    "/wd4701" # Disabling flow analysis warning for nuklear_sdl_render, because it can't be disabled using pragma
    )
else()
    target_compile_options(GameOfLife
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    )
endif()

target_sources(GameOfLife
    PRIVATE
    Client.c
    GameOfLifeUI.c
    GameOfLifeSimulation.c
    nuklear_impl.c
    Threading.c
    Clock.c
)

target_include_directories(GameOfLife PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(GameOfLife
    PRIVATE
    /experimental:c11atomics
    )
endif()

if(WIN32)
    target_link_libraries(GameOfLife
    PRIVATE
    SDL2::SDL2main
    )
endif()

target_link_libraries(GameOfLife
    PRIVATE
    SDL2::SDL2
)

if(UNIX AND NOT WIN32)
    check_include_file(unistd.h HAVE_UNISTD_H)
    if(NOT HAVE_UNISTD_H)
        message(FATAL_ERROR "unsupported platform")
    endif()
    target_compile_definitions(GameOfLife
    PRIVATE    
    _POSIX_C_SOURCE=200809L)
    target_link_libraries(GameOfLife
    PRIVATE
    m
    )
elseif(NOT UNIX AND NOT WIN32)
    message(FATAL_ERROR "unsupported platform")
endif()
