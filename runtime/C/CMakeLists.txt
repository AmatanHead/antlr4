CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(cantlr4)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Sanitizers)

ADD_LIBRARY(cantlr4
        antlr4/antlr4.c
        antlr4/antlr4.h
        antlr4/common.h
        antlr4/common.c

        antlr4/atn/atn.c
        antlr4/atn/atn.h
        antlr4/atn/atn_config.c
        antlr4/atn/atn_config.h

        antlr4/dfa/dfa.c
        antlr4/dfa/dfa.h

        antlr4/memory/align.c
        antlr4/memory/align.h
        antlr4/memory/pool.c
        antlr4/memory/pool.h

        antlr4/util/bitset.c
        antlr4/util/bitset.h
        antlr4/util/one_of.c
        antlr4/util/one_of.h
)
ADD_SANITIZERS(cantlr4)

ADD_EXECUTABLE(test
        test/memory/align.cpp
        test/memory/pool.cpp
        test/util/bitset.cpp)
TARGET_LINK_LIBRARIES(test gtest gtest_main)
TARGET_LINK_LIBRARIES(test cantlr4)
TARGET_INCLUDE_DIRECTORIES(test PRIVATE .)
ADD_SANITIZERS(test)
