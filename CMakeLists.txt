cmake_minimum_required(VERSION 3.20)

project(Compiler)
set(TARGET ${PROJECT_NAME} )
set(SOURCE_PATH  "src/")
set (CMAKE_CXX_STANDARD 20)

file( GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp" )

add_executable(${TARGET} ${CPPS})