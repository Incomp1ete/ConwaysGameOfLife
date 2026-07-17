
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

target_sources(GameOfLife
    PRIVATE
    Client.c
    GameOfLifeUI.c
    GameOfLifeSimulation.c
    PerformanceMeasuring.c
    nuklear_impl.c
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
    target_link_libraries(GameOfLife
    PRIVATE
    m
    )
endif()