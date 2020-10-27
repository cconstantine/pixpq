cmake_minimum_required(VERSION 3.14)
project(pixpq)

option(BUILD_TESTS "Build tests program" OFF)

find_package(PostgreSQL REQUIRED )

include_directories("./include/")

add_library(pixpq src/pixpq.cpp)
target_link_libraries(pixpq pthread PostgreSQL::PostgreSQL)

if (BUILD_TESTS)
  add_executable(tests tests/tests.cpp)
  target_link_libraries(tests pixpq)
endif()