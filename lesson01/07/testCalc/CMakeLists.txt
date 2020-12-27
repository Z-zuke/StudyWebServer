cmake_minimum_required(VERSION 3.0)

project(app)

set(CMAKE_CXX_STANDARD 11)

add_executable(Demo main.c add.c sub.c mult.c div.c)
