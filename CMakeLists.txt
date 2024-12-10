cmake_minimum_required(VERSION 3.28)

project(bminor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    main.cpp
    src/scanner/scanner.cpp
    src/utils/logger.cpp
    src/parser/parser.cpp
    src/AST/ASTPrinterJson.cpp
    src/Semantics/SemanticAnalyzer.cpp
    src/Semantics/Symbol.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
