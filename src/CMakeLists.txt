cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(BuildTest VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

if (BISON_FOUND)
  # Using a custom command instead of "bison_target" because bison doesn't like
  # being run in a read only directory.
  set(PARSER_CC_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
  set(PARSER_DEFINES_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
  add_custom_command(
      OUTPUT ${PARSER_CC_OUTPUT} ${PARSER_DEFINES_OUTPUT}
      COMMAND ${BISON_EXECUTABLE}
      --output=${PARSER_CC_OUTPUT}
      --defines=${PARSER_DEFINES_OUTPUT}
      ${CMAKE_SOURCE_DIR}/parser.y
      COMMENT "Building parser."
  )
endif ()

if (FLEX_FOUND)
  set(LEXER_CC_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
  add_custom_command(
      OUTPUT ${LEXER_CC_OUTPUT}
      COMMAND ${FLEX_EXECUTABLE}
      --outfile=${LEXER_CC_OUTPUT}
      ${CMAKE_SOURCE_DIR}/lexer.l
      COMMENT "Building lexer."
  )

  # Inlining the implementation of ADD_FLEX_BISON_DEPENDENCY(...):
  set_source_files_properties(
      ${LEXER_CC_OUTPUT}
      PROPERTIES OBJECT_DEPENDS ${PARSER_DEFINES_OUTPUT})
endif ()

add_executable(build_test
  main.cc
  ${PARSER_CC_OUTPUT}
  ${LEXER_CC_OUTPUT}
  )
