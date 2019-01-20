cmake_minimum_required(VERSION 3.13)
project(oted C)

set(CMAKE_C_STANDARD 99)

add_executable(oted main.c line.h line.c file.h file.c)