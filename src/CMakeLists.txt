
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
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

target_sources(GameOfLife
    PRIVATE
    Client.c
    GameOfLifeUI.c
    GameOfLifeSimulation.c
    PerformanceMeasuring.c
    nuklear_impl.c
    Threading.c
)

target_include_directories(GameOfLife PRIVATE ${CMAKE_SOURCE_DIR}/include)

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
    target_compile_definitions(GameOfLife
    PRIVATE    
    _POSIX_C_SOURCE=200809L)
    target_link_libraries(GameOfLife
    PRIVATE
    m
    )
endif()
