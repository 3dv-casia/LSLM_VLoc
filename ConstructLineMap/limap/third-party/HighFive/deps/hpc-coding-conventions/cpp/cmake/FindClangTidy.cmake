# (based on https://github.com/BlueBrain/git-cmake- format/blob/master/FindClangFormat.cmake)
# ---------------
# .rst: FindClangTidy
# ---------------
#
# The module defines the following variables
#
# * ``ClangTidy_EXECUTABLE`` Path to clang-tidy executable
# * ``ClangTidy_FOUND`` True if the clang-tidy executable was found
# * ``ClangTidy_VERSION`` The version of clang-tidy found as a string
# * ``ClangTidy_VERSION_MAJOR`` The clang-tidy major version
# * ``ClangTidy_VERSION_MINOR`` The clang-tidy minor version
# * ``ClangTidy_VERSION_PATCH`` The clang-tidy patch version

if(ClangTidy_FIND_VERSION AND ClangTidy_FIND_VERSION_EXACT)
  find_program(ClangTidy_EXECUTABLE
               NAMES clang-tidy-${ClangTidy_FIND_VERSION} clang-tidy
               DOC "clang-tidy executable")
else()
  find_program(ClangTidy_EXECUTABLE
               NAMES clang-tidy
                     clang-tidy-8
                     clang-tidy-7
                     clang-tidy-6.0
                     clang-tidy-5.0
                     clang-tidy-4.0
                     clang-tidy-3.9
                     clang-tidy-3.8
                     clang-tidy-3.7
                     clang-tidy-3.6
                     clang-tidy-3.5
               DOC "clang-tidy executable")
endif()

# Extract version from command "clang-tidy -version"
if(ClangTidy_EXECUTABLE)
  execute_process(COMMAND ${ClangTidy_EXECUTABLE} -version
                  OUTPUT_VARIABLE full_version_text
                  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  # full_version_text sample:
  # ~~~
  # LLVM (http://llvm.org/):
  #  LLVM version 7.0.1
  #
  #  Optimized build.
  #  Default target: x86_64-pc-linux-gnu
  #  Host CPU: skylake
  # ~~~

  if(full_version_text MATCHES "^LLVM .*")
    string(REGEX
           REPLACE ".*LLVM version ([0-9]\\.[0-9]\\.[0-9]).*"
                   "\\1"
                   ClangTidy_VERSION
                   "${full_version_text}")
    # ClangTidy_VERSION sample: "7.0.1"

    # Extract version components
    string(REPLACE "."
                   ";"
                   list_versions
                   "${ClangTidy_VERSION}")
    list(GET list_versions 0 ClangTidy_VERSION_MAJOR)
    list(GET list_versions 1 ClangTidy_VERSION_MINOR)
    list(GET list_versions 2 ClangTidy_VERSION_PATCH)
    unset(list_versions)
  endif()
  unset(full_version_text)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangTidy
                                  FOUND_VAR
                                  ClangTidy_FOUND
                                  REQUIRED_VARS
                                  ClangTidy_EXECUTABLE
                                  VERSION_VAR
                                  ClangTidy_VERSION)
