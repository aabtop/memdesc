cmake_minimum_required(VERSION 3.13)

################################################################################
# Helper Functions
################################################################################

function(copy_file_with_custom_command)
  cmake_parse_arguments(PARAM "" "TARGET;SRC_FILE;DST_FILE" "ADDITIONAL_DEPENDS" ${ARGN})
  add_custom_command(
    TARGET ${PARAM_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PARAM_SRC_FILE} ${PARAM_DST_FILE}
    DEPENDS ${PARAM_SRC_FILE} ${PARAM_ADDITIONAL_DEPENDS}
  )
endfunction()

function(copy_glob_to_dir_with_custom_command)
  cmake_parse_arguments(PARAM "" "TARGET;GLOB_PATTERN;DST_DIR" "ADDITIONAL_DEPENDS" ${ARGN})
  file(GLOB GLOB_FILES ${PARAM_GLOB_PATTERN})
  foreach(GLOB_FILE ${GLOB_FILES})
    get_filename_component(GLOB_FILENAME_ONLY ${GLOB_FILE} NAME)
    set(COPIED_GLOB_FILE ${PARAM_DST_DIR}/${GLOB_FILENAME_ONLY})
    copy_file_with_custom_command(
      TARGET ${PARAM_TARGET}
      SRC_FILE ${GLOB_FILE}
      DST_FILE ${COPIED_GLOB_FILE}
      ADDITIONAL_DEPENDS ${PARAM_ADDITIONAL_DEPENDS})
  endforeach()
endfunction()

################################################################################
# Main Project Definition
################################################################################

# set the project name and version
project(memdesc VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/package)
set(GENERATED_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen)
set(GENERATED_SOURCE_TEST_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/test)

add_custom_target(ensure_package_directory_exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIRECTORY})
add_custom_target(ensure_gen_source_directory_exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SOURCE_DIRECTORY})
add_custom_target(ensure_gen_source_test_directory_exists
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SOURCE_TEST_DIRECTORY})

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

# Setup the output executable as well as its dependent data files to all be
# copied into a package directory together.
set(PACKAGE_MEMDESC_EXECUTABLE ${PACKAGE_DIRECTORY}/memdesc${CMAKE_EXECUTABLE_SUFFIX})

copy_file_with_custom_command(
  TARGET memdesc
  SRC_FILE $<TARGET_FILE:memdesc>
  DST_FILE ${PACKAGE_MEMDESC_EXECUTABLE}
  ADDITIONAL_DEPENDS ensure_package_directory_exists)

copy_glob_to_dir_with_custom_command(
  TARGET memdesc
  GLOB_PATTERN ${CMAKE_SOURCE_DIR}/runtime_data_files/*.mdesc
  DST_DIR ${PACKAGE_DIRECTORY}
  ADDITIONAL_DEPENDS ensure_package_directory_exists)

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

################################################################################
# C end-to-end tests
################################################################################

# The high-level target to build and run all tests.
add_custom_target(end_to_end_c_tests)

# These tests will invoke a freshly built version of memdesc on a list of
# test datas, and then compile a C program that uses the test data.  This
# ensures that we have end-to-end functionality for C.
include_directories(${GENERATED_SOURCE_TEST_DIRECTORY})

# This function setups up the machinery to use a freshly built version of
# the memdesc project on data files in order to translate them into C headers.
# The C headers are then #included by C (or C++) source files to ensure that
# they can compile and that everything works end-to-end.
# MEMDESC_SOURCES and CC_SOURCES are both relative to
# ${GENERATED_SOURCE_TEST_DIRECTORY}
function(define_end_to_end_c_test PARAM_NAME)
  cmake_parse_arguments(PARAM "AS_CXX" "" "CC_SOURCES;MEMDESC_SOURCES" ${ARGN})
  set(TEST_OUTPUT_TARGET "end_to_end_c_test_${PARAM_NAME}")

  SET(ABSOLUTE_CC_SOURCES "")
  foreach(SRC ${PARAM_CC_SOURCES})
    LIST(APPEND ABSOLUTE_CC_SOURCES "end_to_end_c_tests/${SRC}")
    set_source_files_properties(filename.c PROPERTIES LANGUAGE CXX )
  endforeach()

  add_executable(${TEST_OUTPUT_TARGET} ${ABSOLUTE_CC_SOURCES})

  foreach(MEMDESC_SOURCE ${PARAM_MEMDESC_SOURCES})
    get_filename_component(MEMDESC_SOURCE_WE ${MEMDESC_SOURCE} NAME_WE)
    set(OUTPUT_MEMDESC_FILE
        ${GENERATED_SOURCE_TEST_DIRECTORY}/${MEMDESC_SOURCE_WE}.h)

    add_custom_command(
      OUTPUT ${OUTPUT_MEMDESC_FILE}
      COMMAND ${PACKAGE_MEMDESC_EXECUTABLE}
              ${CMAKE_SOURCE_DIR}/end_to_end_c_tests/${MEMDESC_SOURCE}
              -t c
              -o ${OUTPUT_MEMDESC_FILE}
      DEPENDS ensure_gen_source_test_directory_exists memdesc)

    set(MDESC_OUTPUT_TARGET "${TEST_OUTPUT_TARGET}_${MEMDESC_SOURCE_WE}")
    add_custom_target(${MDESC_OUTPUT_TARGET}
                      DEPENDS ${OUTPUT_MEMDESC_FILE} memdesc)

    add_dependencies(${TEST_OUTPUT_TARGET} ${MDESC_OUTPUT_TARGET})
  endforeach()

  add_dependencies(end_to_end_c_tests ${TEST_OUTPUT_TARGET})
endfunction()

define_end_to_end_c_test(
  simple_c CC_SOURCES simple.c MEMDESC_SOURCES simple_desc.mdesc)
define_end_to_end_c_test(
  simple_cc AS_CXX CC_SOURCES simple.cc MEMDESC_SOURCES simple_desc.mdesc)
  