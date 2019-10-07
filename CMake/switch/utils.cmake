# https://github.com/switchpy/libnx-template/blob/7037982c77e1767410143103d5963d0ddc77fb64/cmake/utils.cmake

function(cmake_info message)
    if (cmake_VERBOSE)
        message("Build-Info: ${message}")
    endif ()
endfunction()

function(cmake_warning message)
    if (cmake_VERBOSE)
        message(WARNING "${message}")
    endif ()
endfunction()

function(cmake_panic message)
    message(FATAL_ERROR "${message}")
endfunction()
