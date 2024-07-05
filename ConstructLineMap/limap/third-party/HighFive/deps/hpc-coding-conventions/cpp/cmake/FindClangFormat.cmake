#
# .rst: FindClangFormat
# ---------------
#
# The module defines the following variables:
#
# * ``ClangFormat_EXECUTABLE`` Path to clang-format executable.
# * ``ClangFormat_FOUND`` True if clang-format executable was found.
# * ``ClangFormat_VERSION`` The version of clang-format found.
# * ``ClangFormat_VERSION_MAJOR`` The clang-format major version if specified, 0 otherwise
# * ``ClangFormat_VERSION_MINOR`` The clang-format minor version if specified, 0 otherwise
# * ``ClangFormat_VERSION_PATCH`` The clang-format patch version if specified, 0 otherwise
# * ``ClangFormat_VERSION_COUNT`` Number of version components reported by clang-format
#
# Example usage:
#
# .. code-block:: cmake
#
# ~~~
# find_package(ClangFormat)
# if(ClangFormat_FOUND)
#   message(STATUS "clang-format executable found: "
#           "${ClangFormat_EXECUTABLE}\n"
# "version: ${ClangFormat_VERSION}")
# endif()
# ~~~

if(ClangFormat_FIND_VERSION AND ClangFormat_FIND_VERSION_EXACT)
  find_program(ClangFormat_EXECUTABLE
               NAMES clang-format-${ClangFormat_FIND_VERSION} clang-format
               DOC "clang-format executable")
else()
  find_program(ClangFormat_EXECUTABLE
               NAMES clang-format
                     clang-format-8
                     clang-format-7
                     clang-format-6.0
                     clang-format-5.0
                     clang-format-4.0
                     clang-format-3.9
                     clang-format-3.8
                     clang-format-3.7
                     clang-format-3.6
                     clang-format-3.5
                     clang-format-3.4
                     clang-format-3.3
               DOC "clang-format executable")
endif()
mark_as_advanced(ClangFormat_EXECUTABLE)

# Extract version from command "clang-format -version"
if(ClangFormat_EXECUTABLE)
  execute_process(COMMAND ${ClangFormat_EXECUTABLE} -version
                  OUTPUT_VARIABLE ClangFormat_version
                  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(ClangFormat_version MATCHES "^clang-format version .*")
    # ClangFormat_version sample: "clang-format version 3.9.1-4ubuntu3~16.04.1
    # (tags/RELEASE_391/rc2)"
    string(REGEX
           REPLACE "clang-format version ([.0-9]+).*"
                   "\\1"
                   ClangFormat_VERSION
                   "${ClangFormat_version}")
  elseif(ClangFormat_version MATCHES "  LLVM version .*$")
    # ClangFormat_version sample:
    # --- 8< ---
    # LLVM (http://llvm.org/): LLVM version 3.4.2 Optimized build. Built Dec  7 2015 (09:37:36).
    # Default target: x86_64-redhat-linux-gnu Host CPU: x86-64
    # --- >8 ---
    string(REGEX
           REPLACE ".* LLVM version ([.0-9]+).*"
                   "\\1"
                   ClangFormat_VERSION
                   "${ClangFormat_version}")
  endif()

  if(ClangFormat_VERSION)
    # ClangFormat_VERSION sample: "3.9.1"

    # Extract version components
    string(REPLACE "."
                   ";"
                   ClangFormat_version
                   "${ClangFormat_VERSION}")
    list(LENGTH ClangFormat_version ClangFormat_VERSION_COUNT)
    if(ClangFormat_VERSION_COUNT GREATER 0)
      list(GET ClangFormat_version 0 ClangFormat_VERSION_MAJOR)
    else()
      set(ClangFormat_VERSION_MAJOR 0)
    endif()
    if(ClangFormat_VERSION_COUNT GREATER 1)
      list(GET ClangFormat_version 1 ClangFormat_VERSION_MINOR)
    else()
      set(ClangFormat_VERSION_MINOR 0)
    endif()
    if(ClangFormat_VERSION_COUNT GREATER 2)
      list(GET ClangFormat_version 2 ClangFormat_VERSION_PATCH)
    else()
      set(ClangFormat_VERSION_PATCH 0)
    endif()
  endif()
  unset(ClangFormat_version)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangFormat
                                  FOUND_VAR
                                  ClangFormat_FOUND
                                  REQUIRED_VARS
                                  ClangFormat_EXECUTABLE
                                  VERSION_VAR
                                  ClangFormat_VERSION)
