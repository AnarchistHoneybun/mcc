cmake_minimum_required(VERSION 3.29)
project(mcc)

set(CMAKE_CXX_STANDARD 20)

add_executable(mcc main.cpp
        compiler_driver.h
        compiler_driver.cpp
        lexer.h
        lexer.cpp
        ast.h
        parser.h
        parser.cpp
        assembly_ast.h
        codegen.h
        codegen.cpp)
