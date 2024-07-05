# .rst: FindPreCommit.cmake
# -------------------------
#
# The module defines the following variables:
#
# * ``PreCommit_EXECUTABLE`` Path to pre-commit executable.
# * ``PreCommit_FOUND`` True if pre-commit executable was found.
# * ``PreCommit_VERSION`` The version of pre-commit found.
# * ``PreCommit_VERSION_MAJOR`` The pre-commit major version
# * ``PreCommit_VERSION_MINOR`` The pre-commit minor version
# * ``PreCommit_VERSION_PATCH`` The pre-commit patch version
#
find_program(PreCommit_EXECUTABLE pre-commit DOC "Path to pre-commit Python utility")
mark_as_advanced(PreCommit_EXECUTABLE)

if(PreCommit_EXECUTABLE)
  execute_process(COMMAND ${PreCommit_EXECUTABLE} --version
                  OUTPUT_VARIABLE full_version_text
                  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

  # full_version_text sample: "pre-commit 1.14.2"
  if(full_version_text MATCHES "^pre-commit .*")
    string(REGEX
           REPLACE "pre-commit ([0-9.]+)"
                   "\\1"
                   PreCommit_VERSION
                   "${full_version_text}")
    # PreCommit_VERSION sample: "1.14.2"

    # Extract version components
    string(REPLACE "."
                   ";"
                   list_versions
                   "${PreCommit_VERSION}")
    list(GET list_versions 0 PreCommit_VERSION_MAJOR)
    list(GET list_versions 1 PreCommit_VERSION_MINOR)
    list(GET list_versions 2 PreCommit_VERSION_PATCH)
    unset(list_versions)
  endif()
  unset(full_version_text)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PreCommit
                                  FOUND_VAR
                                  PreCommit_FOUND
                                  REQUIRED_VARS
                                  PreCommit_EXECUTABLE
                                  VERSION_VAR
                                  PreCommit_VERSION)
