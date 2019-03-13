CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(cantlr4)

SET(DEFAULT_FLAGS "\
    -Wall\
    -Wextra\
    -Wshadow\
    -Wconversion\
    -Wswitch-default\
    -Wswitch-enum\
    -Wunreachable-code\
    -Wwrite-strings\
    -Wundef\
    -Wno-unused-function\
    -Werror\
")

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEFAULT_FLAGS}")
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFAULT_FLAGS}")

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Sanitizers)

ADD_LIBRARY(cantlr4
        antlr4/antlr4.c
        antlr4/antlr4.h
        antlr4/common.h
        antlr4/common.c
        antlr4/input.c
        antlr4/input.h
        antlr4/lc_mode.c
        antlr4/lc_mode.h
        antlr4/lexer.c
        antlr4/lexer.h
        antlr4/lexing_error.c
        antlr4/lexing_error.h
        antlr4/lexing_result.c
        antlr4/lexing_result.h
        antlr4/token.c
        antlr4/token.h

        antlr4/atn/fwd.h
        antlr4/atn/atn.c
        antlr4/atn/atn.h
        antlr4/atn/atn_config.c
        antlr4/atn/atn_config.h
        antlr4/atn/state.c
        antlr4/atn/state.h
        antlr4/atn/transition.c
        antlr4/atn/transition.h
        antlr4/atn/lexer_action.c
        antlr4/atn/lexer_action.h
        antlr4/atn/lexer_simulator.c
        antlr4/atn/lexer_simulator.h

        antlr4/dfa/dfa.c
        antlr4/dfa/dfa.h
        antlr4/dfa/state.c
        antlr4/dfa/state.h

        antlr4/memory/align.c
        antlr4/memory/align.h
        antlr4/memory/pool.c
        antlr4/memory/pool.h

        antlr4/util/bitset.c
        antlr4/util/bitset.h
        antlr4/util/intset.c
        antlr4/util/intset.h
        antlr4/util/one_of.c
        antlr4/util/one_of.h
        antlr4/util/utf.c
        antlr4/util/utf.h
        antlr4/util/uuid.c
        antlr4/util/uuid.h
)
ADD_SANITIZERS(cantlr4)

ADD_EXECUTABLE(test
        test/atn/atn.cpp

        test/lexer/lexer.c

        test/memory/align.cpp
        test/memory/pool.cpp

        test/util/bitset.cpp
        test/util/intset.cpp
        test/util/list.cpp
        test/util/utf.cpp
        test/util/uuid.cpp
        test/util/hashtable.cpp
        test/util/counted.h
)
TARGET_LINK_LIBRARIES(test gtest gtest_main)
TARGET_LINK_LIBRARIES(test cantlr4)
TARGET_INCLUDE_DIRECTORIES(test PRIVATE .)
ADD_SANITIZERS(test)
