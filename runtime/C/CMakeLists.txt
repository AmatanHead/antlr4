CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(CAntlr4)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Sanitizers)

ADD_LIBRARY(CAntlr4
        antlr4/antlr4.h
        antlr4/antlr4.c
        antlr4/memory/pool.c
        antlr4/memory/pool.h
        antlr4/atn/atn.c
        antlr4/atn/atn.h
        antlr4/atn/atn_config.c
        antlr4/atn/atn_config.h
        antlr4/dfa/dfa.c
        antlr4/dfa/dfa.h)
ADD_SANITIZERS(CAntlr4)

ADD_EXECUTABLE(Test
        test/memory/pool.cpp)
TARGET_LINK_LIBRARIES(Test gtest gtest_main)
TARGET_LINK_LIBRARIES(Test CAntlr4)
TARGET_INCLUDE_DIRECTORIES(Test PRIVATE .)
ADD_SANITIZERS(Test)
