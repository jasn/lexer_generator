cmake_minimum_required (VERSION 2.6) 
project (lexer-generator)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules/")

add_subdirectory (src)

add_subdirectory (test)
