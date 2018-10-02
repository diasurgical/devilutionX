set(ORIGINAL_EXE "${CMAKE_SOURCE_DIR}/Diablo.exe")

if(EXISTS "${ORIGINAL_EXE}")
    message(STATUS "Using EXE at ${ORIGINAL_EXE}")

    file(MD5 ${ORIGINAL_EXE} MD5SUM)
    if(NOT MD5SUM STREQUAL "da62d5cd8bd71a0b66e6d4ef7a111233")
        message(FATAL_ERROR "MD5 of EXE is not correct (${MD5SUM})")
    endif()

    enable_language(ASM_NASM)

    set(HARNESS_ASM "${CMAKE_SOURCE_DIR}/Absolute/harness.asm")

    # This can not be an OBJECT library since those can not have link flags on older versions of cmake
    add_library(harness STATIC ${HARNESS_ASM})
    target_compile_options(harness PRIVATE -f elf -DEXE=\"${ORIGINAL_EXE}\")

    target_compile_options(harness INTERFACE -fno-pie -fno-pic)
    target_compile_definitions(harness INTERFACE -DNO_GLOBALS)
    target_link_libraries(harness INTERFACE
        -L${CMAKE_SOURCE_DIR}/Absolute -Tdefault.ld
    )
else()
    message(STATUS "Original .exe not found at ${ORIGINAL_EXE}")
endif()
