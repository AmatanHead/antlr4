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
        src/antlr4/antlr4.c
        src/antlr4/antlr4.h
        src/antlr4/common.h
        src/antlr4/common.c
        src/antlr4/input.c
        src/antlr4/input.h
        src/antlr4/lc_mode.c
        src/antlr4/lc_mode.h
        src/antlr4/lexer.c
        src/antlr4/lexer.h
        src/antlr4/lexing_error.c
        src/antlr4/lexing_error.h
        src/antlr4/lexing_result.c
        src/antlr4/lexing_result.h
        src/antlr4/token.c
        src/antlr4/token.h

        src/antlr4/atn/fwd.h
        src/antlr4/atn/atn.c
        src/antlr4/atn/atn.h
        src/antlr4/atn/atn_config.c
        src/antlr4/atn/atn_config.h
        src/antlr4/atn/state.c
        src/antlr4/atn/state.h
        src/antlr4/atn/transition.c
        src/antlr4/atn/transition.h
        src/antlr4/atn/lexer_action.c
        src/antlr4/atn/lexer_action.h
        src/antlr4/atn/lexer_simulator.c
        src/antlr4/atn/lexer_simulator.h

        src/antlr4/dfa/dfa.c
        src/antlr4/dfa/dfa.h
        src/antlr4/dfa/state.c
        src/antlr4/dfa/state.h

        src/antlr4/util/align.c
        src/antlr4/util/align.h
        src/antlr4/util/bitset.c
        src/antlr4/util/bitset.h
        src/antlr4/util/intset.c
        src/antlr4/util/intset.h
        src/antlr4/util/one_of.c
        src/antlr4/util/one_of.h
        src/antlr4/util/pool.c
        src/antlr4/util/pool.h
        src/antlr4/util/utf.c
        src/antlr4/util/utf.h
        src/antlr4/util/uuid.c
        src/antlr4/util/uuid.h
)
TARGET_INCLUDE_DIRECTORIES(cantlr4 PRIVATE src)
ADD_SANITIZERS(cantlr4)

FIND_PACKAGE(GTest)
IF(GTEST_FOUND)
    MESSAGE(STATUS "Google test framework found")
    ADD_EXECUTABLE(test
            test/atn/atn.cpp

            test/lexer/lexer.c


            test/util/align.cpp
            test/util/bitset.cpp
            test/util/hashtable.cpp
            test/util/intset.cpp
            test/util/list.cpp
            test/util/pool.cpp
            test/util/utf.cpp
            test/util/uuid.cpp
            test/util/counted.h
    )
    TARGET_LINK_LIBRARIES(test GTest::GTest GTest::Main)
    TARGET_LINK_LIBRARIES(test cantlr4)
    TARGET_INCLUDE_DIRECTORIES(test PRIVATE src)
    ADD_SANITIZERS(test)
ELSE()
    MESSAGE(STATUS "Google test framework not found, tests are not available")
ENDIF()
