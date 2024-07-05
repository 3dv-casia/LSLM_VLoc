list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE)
include(cmake/bob.cmake)

function(bbp_enable_precommit)
  find_package(PythonInterp 3.5 REQUIRED)
  find_package(PreCommit REQUIRED)
  execute_process(COMMAND ${PreCommit_EXECUTABLE} install)
  execute_process(COMMAND ${PYTHON_EXECUTABLE}
                          "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbp-setup-pre-commit-config.py" --force
                          ${${PROJECT_NAME}_PRECOMMIT_REGEN} --clang-format
                          ${${PROJECT_NAME}_FORMATTING} --cmake-format ${${PROJECT_NAME}_FORMATTING}
                          --clang-tidy ${${PROJECT_NAME}_STATIC_ANALYSIS} ${CMAKE_SOURCE_DIR}
                          ${CMAKE_BINARY_DIR})
  add_custom_target(git-pre-commits ${PreCommit_EXECUTABLE} run --all-files)
endfunction(bbp_enable_precommit)

function(bbp_disable_precommit)
  if(EXISTS ${CMAKE_SOURCE_DIR}/.pre-commit-config.yaml)
    find_package(PythonInterp 3.5 REQUIRED)
    execute_process(COMMAND ${PYTHON_EXECUTABLE}
                            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbp-setup-pre-commit-config.py"
                            --clang-format off --cmake-format off --clang-tidy off
                            ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
  endif()
endfunction(bbp_disable_precommit)

function(bbp_enable_clang_formatting)
  find_package(PythonInterp 3.5 REQUIRED)
  find_package(ClangFormat 7 EXACT REQUIRED)
  set(${PROJECT_NAME}_ClangFormat_OPTIONS "" CACHE STRING "clang-format command options")
  set(${PROJECT_NAME}_ClangFormat_FILES_RE "^.*\\\\.[ch]$$" "^.*\\\\.[chi]pp$$"
      CACHE STRING "List of regular expressions matching C/C++ filenames")
  set(${PROJECT_NAME}_ClangFormat_EXCLUDES_RE ".*/third[-_]parties/.*$$" ".*/third[-_]party/.*$$"
      CACHE STRING "list of regular expressions to exclude C/C++ files from formatting")
  set(${PROJECT_NAME}_ClangFormat_DEPENDENCIES ""
      CACHE STRING "list of CMake targets to build before formatting C/C++ code")
  mark_as_advanced(${PROJECT_NAME}_ClangFormat_OPTIONS ${PROJECT_NAME}_ClangFormat_FILES_RE
                   ${PROJECT_NAME}_ClangFormat_EXCLUDES_RE)
  configure_file(.clang-format ${PROJECT_SOURCE_DIR})
  set(clang_format_command_prefix
      ${PYTHON_EXECUTABLE}
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbp-clang-format.py"
      -S
      "${CMAKE_SOURCE_DIR}"
      -B
      "${CMAKE_BINARY_DIR}"
      "--executable=${ClangFormat_EXECUTABLE}"
      --files-re
      ${${PROJECT_NAME}_ClangFormat_FILES_RE}
      $<$<BOOL:${PROJECT_NAME}_FORMATTING_NO_SUBMODULES>:--git-modules>
      --excludes-re
      ${${PROJECT_NAME}_ClangFormat_EXCLUDES_RE}
      -p
      ${CMAKE_BINARY_DIR}/compile_commands.json)
  add_custom_target(clang-format
                    ${clang_format_command_prefix}
                    --action
                    format
                    --
                    "${${PROJECT_NAME}_ClangFormat_OPTIONS}")
  add_custom_target(check-clang-format
                    ${clang_format_command_prefix}
                    --action
                    check
                    --
                    "${${PROJECT_NAME}_ClangFormat_OPTIONS}")
  if(${PROJECT_NAME}_TEST_FORMATTING)
    add_test(NAME ClangFormat
             COMMAND ${clang_format_command_prefix}
                     --action check
                     --make-unescape-re
                     -- "${${PROJECT_NAME}_ClangFormat_OPTIONS}")
  endif()
  if(${PROJECT_NAME}_ClangFormat_DEPENDENCIES)
    add_dependencies(clang-format ${${PROJECT_NAME}_ClangFormat_DEPENDENCIES})
    add_dependencies(check-clang-format ${${PROJECT_NAME}_ClangFormat_DEPENDENCIES})
  endif()
endfunction(bbp_enable_clang_formatting)

function(bbp_enable_cmake_formatting)
  find_package(PythonInterp 3.5 REQUIRED)
  find_package(CMakeFormat 0.4.5 EXACT REQUIRED)
  set(${PROJECT_NAME}_CMakeFormat_OPTIONS "" CACHE STRING "cmake-format options")
  set(${PROJECT_NAME}_CMakeFormat_FILES_RE "^.*\\\\.cmake$$" "^.*CMakeLists.txt$$"
      CACHE STRING "List of regular expressions matching CMake files")
  set(${PROJECT_NAME}_CMakeFormat_EXCLUDES_RE ".*/third[-_]parties/.*$$" ".*/third[-_]party/.*$$"
      CACHE STRING "list of regular expressions to exclude CMake files from formatting")
  mark_as_advanced(${PROJECT_NAME}_CMakeFormat_OPTIONS ${PROJECT_NAME}_CMakeFormat_FILES_RE
                   ${PROJECT_NAME}_CMakeFormat_EXCLUDES_RE)
  configure_file(.cmake-format.yaml ${PROJECT_SOURCE_DIR})
  set(cmake_format_command_prefix
      ${PYTHON_EXECUTABLE}
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbp-cmake-format.py"
      -S
      "${CMAKE_SOURCE_DIR}"
      -B
      "${CMAKE_BINARY_DIR}"
      "--executable=${CMakeFormat_EXECUTABLE}"
      --files-re
      ${${PROJECT_NAME}_CMakeFormat_FILES_RE}
      $<$<BOOL:${PROJECT_NAME}_FORMATTING_NO_SUBMODULES>:--git-modules>
      --excludes-re
      ${${PROJECT_NAME}_CMakeFormat_EXCLUDES_RE})
  add_custom_target(cmake-format
                    ${cmake_format_command_prefix}
                    --action
                    format
                    --
                    "${${PROJECT_NAME}_CMakeFormat_OPTIONS}")
  add_custom_target(check-cmake-format
                    ${cmake_format_command_prefix}
                    --action
                    check
                    --
                    ${${PROJECT_NAME}_CMakeFormat_OPTIONS})
  if(${PROJECT_NAME}_TEST_FORMATTING)
    add_test(NAME CMakeFormat
             COMMAND ${cmake_format_command_prefix}
                     --action check
                     --make-unescape-re
                     -- ${${PROJECT_NAME}_CMakeFormat_OPTIONS})
  endif()

endfunction(bbp_enable_cmake_formatting)

function(bbp_enable_static_analysis)
  find_package(PythonInterp 3.5 REQUIRED)
  find_package(ClangTidy 7 EXACT REQUIRED)
  set(${PROJECT_NAME}_ClangTidy_OPTIONS -extra-arg=-Wno-unknown-warning-option
      CACHE STRING "clang-tidy command options")
  mark_as_advanced(${PROJECT_NAME}_ClangTidy_OPTIONS)
  set(${PROJECT_NAME}_ClangTidy_FILES_RE "^.*\\\\.c$$"  "^.*\\\\.cpp$$"
      CACHE STRING "List of regular expressions matching C/C++ filenames")
  set(${PROJECT_NAME}_ClangTidy_EXCLUDES_RE ".*/third[-_]parties/.*$$" ".*/third[-_]party/.*$$"
      CACHE STRING "list of regular expressions to exclude C/C++ files from formatting")
  set(${PROJECT_NAME}_ClangTidy_DEPENDENCIES ""
      CACHE STRING "list of CMake targets to build before formatting C/C++ code")
  mark_as_advanced(${PROJECT_NAME}_ClangTidy_OPTIONS ${PROJECT_NAME}_ClangTidy_FILES_RE
                   ${PROJECT_NAME}_ClangTidy_EXCLUDES_RE)
  configure_file(.clang-tidy ${PROJECT_SOURCE_DIR})
  set(clang_tidy_command_prefix
          ${PYTHON_EXECUTABLE}
          "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbp-clang-tidy.py"
          -S
          "${CMAKE_SOURCE_DIR}"
          -B
          "${CMAKE_BINARY_DIR}"
          "--executable=${ClangTidy_EXECUTABLE}"
          --files-re
          "${${PROJECT_NAME}_ClangTidy_FILES_RE}"
          $<$<BOOL:${PROJECT_NAME}_FORMATTING_NO_SUBMODULES>:--git-modules>
          --excludes-re
          ${${PROJECT_NAME}_ClangTidy_EXCLUDES_RE}
          -p
          ${CMAKE_BINARY_DIR}/compile_commands.json
          --action
          check
          )
  add_custom_target(clang-tidy ${clang_tidy_command_prefix} -- ${${PROJECT_NAME}_ClangTidy_OPTIONS})
  if(${PROJECT_NAME}_TEST_STATIC_ANALYSIS)
    add_test(NAME ClangTidy COMMAND ${clang_tidy_command_prefix} --make-unescape-re -- ${${PROJECT_NAME}_ClangTidy_OPTIONS})
  endif()
  if(${PROJECT_NAME}_ClangTidy_DEPENDENCIES)
    add_dependencies(clang-tidy ${${PROJECT_NAME}_ClangTidy_DEPENDENCIES})
  endif()

endfunction(bbp_enable_static_analysis)

bob_option(${PROJECT_NAME}_FORMATTING "Enable helpers to keep CMake and C++ code properly formatted"
           OFF)
bob_option(${PROJECT_NAME}_TEST_FORMATTING "Add CTest formatting test" OFF)
bob_option(${PROJECT_NAME}_FORMATTING_NO_SUBMODULES "Exclude git submodules from formatting" ON)
bob_option(${PROJECT_NAME}_CLANG_FORMAT "Enable helper to keep C++ code properly formatted" OFF)
bob_option(${PROJECT_NAME}_CMAKE_FORMAT "Enable helper to keep CMake code properly formatted" OFF)

bob_option(${PROJECT_NAME}_PRECOMMIT "Enable automatic checks before git commits" OFF)
bob_option(${PROJECT_NAME}_PRECOMMIT_REGEN "Force precommit hook regeneration" OFF)

bob_option(${PROJECT_NAME}_STATIC_ANALYSIS "Enable C++ static analysis during compilation" OFF)
bob_option(${PROJECT_NAME}_TEST_STATIC_ANALYSIS "Add CTest static analysis test" OFF)

if(${PROJECT_NAME}_CLANG_FORMAT)
  bbp_enable_clang_formatting()
endif(${PROJECT_NAME}_CLANG_FORMAT)

if(${PROJECT_NAME}_CMAKE_FORMAT)
  bbp_enable_cmake_formatting()
endif(${PROJECT_NAME}_CMAKE_FORMAT)

if(${PROJECT_NAME}_FORMATTING)
  bbp_enable_clang_formatting()
  bbp_enable_cmake_formatting()
endif(${PROJECT_NAME}_FORMATTING)

if(${PROJECT_NAME}_PRECOMMIT)
  bbp_enable_precommit()
else(${PROJECT_NAME}_PRECOMMIT)
  bbp_disable_precommit()
endif(${PROJECT_NAME}_PRECOMMIT)

if(${PROJECT_NAME}_STATIC_ANALYSIS)
  cmake_minimum_required(VERSION 3.6)
  bbp_enable_static_analysis()
  set(CMAKE_CXX_CLANG_TIDY
      "${ClangTidy_EXECUTABLE}"
      -p
      "${CMAKE_BINARY_DIR}/compile_commands.json"
      ${${PROJECT_NAME}_ClangTidy_OPTIONS}
      PARENT_SCOPE)
endif(${PROJECT_NAME}_STATIC_ANALYSIS)
