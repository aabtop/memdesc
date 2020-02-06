cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(BuildTest VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(GENERATED_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen)

add_custom_target(ensure_gen_source_directory_exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SOURCE_DIRECTORY})

if (BISON_FOUND)
  # Using a custom command instead of "bison_target" because bison doesn't like
  # being run in a read only directory.
  set(PARSER_CC_OUTPUT ${GENERATED_SOURCE_DIRECTORY}/parser.tab.cc)
  set(PARSER_HEADER_OUTPUT ${GENERATED_SOURCE_DIRECTORY}/parser.tab.h)
  set(PARSER_INPUT ${CMAKE_SOURCE_DIR}/parser.y)
  add_custom_command(
      OUTPUT ${PARSER_CC_OUTPUT} ${PARSER_HEADER_OUTPUT}
      DEPENDS ${PARSER_INPUT} ensure_gen_source_directory_exists
      COMMAND ${BISON_EXECUTABLE}
      --output=${PARSER_CC_OUTPUT}
      --defines=${PARSER_HEADER_OUTPUT}
      ${PARSER_INPUT}
      COMMENT "Building parser."
  )
endif ()

if (FLEX_FOUND)
  set(LEXER_CC_OUTPUT ${GENERATED_SOURCE_DIRECTORY}/lexer.yy.cc)
  set(LEXER_HEADER_OUTPUT ${GENERATED_SOURCE_DIRECTORY}/lexer.yy.h)
  set(LEXER_INPUT ${CMAKE_SOURCE_DIR}/lexer.l)
  add_custom_command(
      OUTPUT ${LEXER_CC_OUTPUT} ${LEXER_HEADER_OUTPUT}
      DEPENDS ${LEXER_INPUT} ensure_gen_source_directory_exists
      COMMAND ${FLEX_EXECUTABLE}
      --header-file=${LEXER_HEADER_OUTPUT}
      --outfile=${LEXER_CC_OUTPUT}
      ${LEXER_INPUT}
      COMMENT "Building lexer."
  )

  # Inlining the implementation of ADD_FLEX_BISON_DEPENDENCY(...):
  set_source_files_properties(
      ${LEXER_CC_OUTPUT}
      PROPERTIES OBJECT_DEPENDS ${PARSER_HEADER_OUTPUT})
endif ()

add_executable(build_test
  ast.cc
  ast.h
  main.cc
  lexer_wrapper.cc
  lexer_wrapper.h
  parser_wrapper.cc
  parser_wrapper.h
  ${PARSER_HEADER_OUTPUT}
  ${LEXER_HEADER_OUTPUT}
  )

target_include_directories(build_test PRIVATE ${GENERATED_SOURCE_DIRECTORY})
