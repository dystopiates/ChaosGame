cmake_minimum_required(VERSION 3.21)
project(ChaosGenerator C)

set(CMAKE_C_STANDARD 23)

add_executable(ChaosGenerator main.c path_generator.c path_generator.h chaos_iterator.c chaos_iterator.h render.c render.h point.c point.h main_helper.h)

target_link_libraries(ChaosGenerator m)
