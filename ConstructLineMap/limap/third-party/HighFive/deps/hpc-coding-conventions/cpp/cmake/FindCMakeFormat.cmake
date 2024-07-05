# .rst: FindCMakeFormat.cmake
# ---------------------------
#
# The module defines the following variables:
#
# * ``CMakeFormat_EXECUTABLE`` Path to cmake-format executable.
# * ``CMakeFormat_FOUND`` True if cmake-format executable was found.
# * ``CMakeFormat_VERSION`` The version of cmake-format found.
# * ``CMakeFormat_VERSION_MAJOR`` The cmake-format major version
# * ``CMakeFormat_VERSION_MINOR`` The cmake-format minor version
# * ``CMakeFormat_VERSION_PATCH`` The cmake-format patch version
#
find_program(CMakeFormat_EXECUTABLE cmake-format DOC "Path to cmake formatter executable")
mark_as_advanced(CMakeFormat_EXECUTABLE)

if(CMakeFormat_EXECUTABLE)
  execute_process(COMMAND ${CMakeFormat_EXECUTABLE} --version
                  OUTPUT_VARIABLE CMakeFormat_VERSION
                  ERROR_VARIABLE CMakeFormat_VERSION
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Extract version components
  string(REPLACE "."
                 ";"
                 list_versions
                 "${CMakeFormat_VERSION}")
  list(GET list_versions 0 CMakeFormat_VERSION_MAJOR)
  list(GET list_versions 1 CMakeFormat_VERSION_MINOR)
  list(GET list_versions 2 CMakeFormat_VERSION_PATCH)
  unset(list_versions)

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CMakeFormat
                                  FOUND_VAR
                                  CMakeFormat_FOUND
                                  REQUIRED_VARS
                                  CMakeFormat_EXECUTABLE
                                  VERSION_VAR
                                  CMakeFormat_VERSION)
