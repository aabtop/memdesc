cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(memdesc VERSION 1.0)

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

add_library(memdesc_lib
  ast.h
  ast_functions.cc
  ast_functions.h
  command_line_arguments.cc
  command_line_arguments.h
  lexer_wrapper.cc
  lexer_wrapper.h
  parse_error.cc
  parse_error.h
  process_import_statement.cc
  process_import_statement.h
  parse_results.cc
  parse_results.h
  parser_helper_functions.cc
  parser_helper_functions.h
  parser_wrapper.cc
  parser_wrapper.h
  target_registry.h
  target_registry.cc
  targets/ast_dump.cc
  targets/c.cc
  ${PARSER_HEADER_OUTPUT}
  ${LEXER_HEADER_OUTPUT}
  )

include_directories(${GENERATED_SOURCE_DIRECTORY} ${CMAKE_SOURCE_DIR})

add_executable(memdesc
  main.cc
  )


target_link_libraries(memdesc memdesc_lib)

configure_file(.clang-tidy .clang-tidy COPYONLY)
configure_file(.clang-format .clang-format COPYONLY)

install(TARGETS memdesc
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/package)
install(DIRECTORY runtime_data_files/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/package
        PATTERN "README.md" EXCLUDE)

################################################################################
# Unit tests
################################################################################

set(UNIT_TEST_SRCS
  ast_test_helpers.cc
  ast_test_helpers.h
  ast_test.cc
  ast_functions_test.cc
  parse_file_tests.cc
)

# Override gtest's  aults.
SET(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
SET(BUILD_GMOCK ON CACHE BOOL "Builds the googlemock subproject")
SET(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest.")
SET(INSTALL_GMOCK OFF CACHE BOOL "Enable installation of googlemock.")
add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
SET(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib even when Google Test is built as static lib.")

enable_testing()
add_subdirectory(third_party/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${memdesc_INCLUDE_DIR})
link_directories(${memdesc_BINARY_DIR})
link_directories(${gtest_BINARY_DIR})

add_executable(memdesc_tests ${UNIT_TEST_SRCS})
target_link_libraries(memdesc_tests memdesc_lib gtest gtest_main)

file(COPY examples DESTINATION .)
file(COPY runtime_data_files/ DESTINATION .)
add_test(MemdescUnitTests memdesc_tests)
