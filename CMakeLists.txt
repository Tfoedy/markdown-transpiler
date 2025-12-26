cmake_minimum_required(VERSION 3.15)
project(markdowntranspiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	src/main.cpp
	
	src/lexer.cpp
	src/block_parser.cpp
	src/inline_parser.cpp
	src/html_renderer.cpp
	src/transpiler.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
