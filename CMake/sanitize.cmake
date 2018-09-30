include(CheckCXXCompilerFlag)

set(SANITIZE_OPTIONS -fsanitize=null -fsanitize=return)

check_cxx_compiler_flag(HAS_SANITIZE_ADDRESS -fsanitize-recover=address)
if(HAS_SANITIZE_ADDRESS)
    list(APPEND SANITIZE_OPTIONS -fsanitize=address -fsanitize-recover=address)
endif()
