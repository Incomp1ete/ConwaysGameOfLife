add_executable(GameOfLife)

target_include_directories(GameOfLife PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(SDL2 REQUIRED)
find_library(MATH_LIB m REQUIRED)

message(STATUS "Math Library: ${MATH_LIB}")

target_sources(GameOfLife
    PRIVATE
        GameOfLife.c
)

target_link_libraries(GameOfLife
    PRIVATE
    SDL2::SDL2
    ${MATH_LIB}
)