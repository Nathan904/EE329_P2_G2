cmake_minimum_required(VERSION 3.20)

project("EE329_P2_G2" C CXX ASM)

include(cmake/st-project.cmake)

add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})