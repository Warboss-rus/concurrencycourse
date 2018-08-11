cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(concurrencycourse)

include(cmake/functions.cmake)

add_subdirectory(src)

add_subdirectory(benchmark)
