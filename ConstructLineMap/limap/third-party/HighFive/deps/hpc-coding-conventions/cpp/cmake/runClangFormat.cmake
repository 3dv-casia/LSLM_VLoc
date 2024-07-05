# TODO: should have 2 opeartiosn 1. format the file 2. check if file is formatted

foreach(var ClangFormat_EXECUTABLE ClangFormat_OPTIONS BBP_CPP_FORMAT_FILES_LIST)
  if(NOT ${var})
    message(FATAL_ERROR "runClangFormat.cmake: no ${var} variable given")
  endif()
endforeach()

function(format_files)
  file(READ "${BBP_CPP_FORMAT_FILES_LIST}" contents)
  # Convert file contents into a CMake list (where each element in the list is one line of the file)
  #
  string(REGEX
         REPLACE ";"
                 "\\\\;"
                 contents
                 "${contents}")
  string(REGEX
         REPLACE "\n"
                 ";"
                 contents
                 "${contents}")

  foreach(source ${contents})
    execute_process(${ClangFormat_EXECUTABLE} ${ClangFormat_OPTIONS} ${source})
  endforeach()
endfunction()

function(check_file)
  list(REMOVE_ITEM BBP_ClangFormat_OPTIONS "-i")
  file(READ "${BBP_CPP_FORMAT_FILES_LIST}" contents)
  foreach(source ${contents})
    execute_process(${ClangFormat_EXECUTABLE} ${ClangFormat_OPTIONS} ${source}
                    OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/runClangFormat.cmake.temp)
    execute_process(${CMAKE_COMMAND}
                    -E
                    compare_files
                    ${source}
                    ${CMAKE_CURRENT_BINARY_DIR}/runClangFormat.cmake.temp
                    RESULT_VARIABLE same_file
                    OUTPUT_QUIET ERROR_QUIET)
    if(same_file)
      message(SEND_ERROR "C/C++ file is not properly formatted: ${source}")
      set(fail True)
    endif()
  endforeach()
  file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/runClangFormat.cmake.temp)
endfunction()
