cmake_minimum_required(VERSION 3.14)
project(pixpq)

option(BUILD_TESTS "Build tests program" OFF)

include_directories("./include/")

set(PIXPQ_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
set(PIXPQ_LIBRARY pixpq pthread pqxx PARENT_SCOPE)

add_library(pixpq src/pixpq.cpp)
target_link_libraries(pixpq pthread pqxx)

if (BUILD_TESTS)
  add_executable(tests tests/tests.cpp)
  target_link_libraries(tests pixpq pthread pqxx)
endif()