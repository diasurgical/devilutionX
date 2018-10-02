include(CheckCXXCompilerFlag)
include(CMakePushCheckState)

set(SANITIZE_OPTIONS -fsanitize=null -fsanitize=return)
# TODO: use "-fsanitize=object-size"
# "-fsanitize=bounds" not enabled because the code often generates temporary pointers out-of-bounds of arrays

# Note: The compiler must always support recovery because the decompiled code is not ASAN-clean
set(SANITIZE_ADDRESS_FLAGS -fsanitize=address -fsanitize-recover=address)

cmake_push_check_state()
set(CMAKE_REQUIRED_LIBRARIES ${SANITIZE_ADDRESS_FLAGS})
check_cxx_compiler_flag("${SANITIZE_ADDRESS_FLAGS}" HAS_SANITIZE_ADDRESS)
cmake_pop_check_state()

if(HAS_SANITIZE_ADDRESS)
    list(APPEND SANITIZE_OPTIONS ${SANITIZE_ADDRESS_FLAGS})
endif()
