cmake_minimum_required( VERSION 2.8.2 )

project( iomap )

include_directories("${CMAKE_SOURCE_DIR}/src")
file( GLOB C_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c" )
add_definitions("-Wall")

add_executable( ${PROJECT_NAME} ${C_SOURCES} )
