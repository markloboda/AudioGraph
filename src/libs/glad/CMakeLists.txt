cmake_minimum_required(VERSION 3.0)
project(glad)

add_library(glad STATIC include/glad/gl.h 
   src/gl.c)

target_include_directories(glad PUBLIC include/)