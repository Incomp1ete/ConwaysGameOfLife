cmake_minimum_required(VERSION 3.23)

project(GameOfLife
LANGUAGES C)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)