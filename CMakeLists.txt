cmake_minimum_required(VERSION 3.5)

# Turn on modern `try_compile()` policies.
if(POLICY CMP0066)
    cmake_policy(SET CMP0066 NEW)
endif()
if(POLICY CMP0067)
    cmake_policy(SET CMP0067 NEW)
endif()

include(CMake/out_of_tree.cmake)

# This *must* be included before calling `project()`, due to setting early compiler flags.
include(CMake/32bit.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(devil-miniwin
    VERSION 0.0.1
    LANGUAGES C CXX
)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(WARNING "sizeof(void*) == ${CMAKE_SIZEOF_VOID_P}.")
    message(FATAL_ERROR [[This project can only be compiled in 32-bit mode.]])
endif()

# Note: In Debug mode, GCC generates spurious memory references that upset Valgrind,
# these options fix that.
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer")
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang/LLVM optimizations break everything
#    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -Og")
#    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fvar-tracking-assignments -ggdb -gdwarf-4")
endif()

set(SOURCES
    Source/automap.cpp
    Source/codec.cpp
    Source/control.cpp
    Source/cursor.cpp
    Source/dead.cpp
    Source/debug.cpp
#   Source/diablo.cpp
    Source/doom.cpp
    Source/drlg_l1.cpp
    Source/drlg_l2.cpp
    Source/drlg_l3.cpp
    Source/drlg_l4.cpp
#   Source/effects.cpp
    Source/encrypt.cpp
    Source/engine.cpp
    Source/error.cpp
#   Source/gamemenu.cpp
    Source/gendung.cpp
    Source/gmenu.cpp
    Source/help.cpp
    Source/interfac.cpp
    Source/inv.cpp
    Source/items.cpp
    Source/lighting.cpp
    Source/loadsave.cpp
    Source/mainmenu.cpp
    Source/minitext.cpp
    Source/missiles.cpp
    Source/monster.cpp
    Source/mpqapi.cpp
    Source/msgcmd.cpp
    Source/msg.cpp
    Source/multi.cpp
    Source/objects.cpp
    Source/pack.cpp
    Source/palette.cpp
    Source/path.cpp
    Source/pfile.cpp
    Source/player.cpp
    Source/plrmsg.cpp
    Source/portal.cpp
    Source/quests.cpp
    Source/render.cpp
    Source/scrollrt.cpp
    Source/setmaps.cpp
    Source/sha.cpp
    Source/spells.cpp
    Source/stores.cpp
    Source/sync.cpp
    Source/textdat.cpp
    Source/themes.cpp
    Source/tmsg.cpp
    Source/town.cpp
    Source/towners.cpp
    Source/track.cpp
    Source/trigs.cpp
    Source/wave.cpp

    3rdParty/PKWare/explode.cpp
    3rdParty/PKWare/implode.cpp
)

# Stubbed out sources, for reference
set(ORIGINAL_SOURCES
    Source/capture.cpp
    Source/appfat.cpp
    Source/dthread.cpp
    Source/effects.cpp
    Source/dx.cpp
    Source/fault.cpp
    Source/init.cpp
    Source/logging.cpp
    Source/movie.cpp
    Source/nthread.cpp
    Source/restrict.cpp
    Source/sound.cpp
    Source/diablo.cpp
)

set(STUB_SOURCES
    Stub/miniwin.cpp
    Stub/miniwin_rand.cpp
    Stub/appfat.cpp
    Stub/capture.cpp
    Stub/dthread.cpp
    Stub/dx.cpp
    Stub/fault.cpp
    Stub/init.cpp
    Stub/movie.cpp
    Stub/nthread.cpp
    Stub/restrict.cpp
    Stub/sound.cpp
    Stub/storm.cpp
    Stub/diabloui.cpp
    Stub/miniwin_io.cpp
    Stub/miniwin_msg_sdl.cpp
    Stub/storm_net.cpp
    Stub/validate.cpp
    Stub/effects.cpp
    Stub/sdlrender.cpp
    Stub/SDL_FontCache.cpp
    Stub/diablo.cpp
    Stub/gamemenu.cpp

    3rdParty/StormLib/src/FileStream.cpp
    3rdParty/StormLib/src/SBaseCommon.cpp
    3rdParty/StormLib/src/SBaseFileTable.cpp
    3rdParty/StormLib/src/SBaseSubTypes.cpp
    3rdParty/StormLib/src/SCompression.cpp
    3rdParty/StormLib/src/SFileExtractFile.cpp
    3rdParty/StormLib/src/SFileFindFile.cpp
    3rdParty/StormLib/src/SFileGetFileInfo.cpp
    3rdParty/StormLib/src/SFileOpenArchive.cpp
    3rdParty/StormLib/src/SFileOpenFileEx.cpp
    3rdParty/StormLib/src/SFileReadFile.cpp
)

include(CMake/SDL2_fixed.cmake)
include(CMake/sanitize.cmake)
include(CMake/absolute.cmake)
include(CMake/CodeCoverage.cmake)

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(. Stub)

# FUTURE: use add_compile_definitions()
add_definitions(
    -D_DEBUG
    -DMINIWIN
    # Skip fades and other fluff
    -DFASTER
)

# Ignore serious warnings due to "quality" of decompiled code
add_compile_options(-fpermissive -Wno-write-strings -Wno-multichar)
# Disable optimizations that can break non-standards-conforming code
add_compile_options(-fno-aggressive-loop-optimizations -fno-strict-aliasing)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Style issues
    add_compile_options(-Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses)
    # Silence warnings about __int64 alignment hack not always being applicable
    add_compile_options(-Wno-ignored-attributes)
    # Fix: error: cast from pointer to smaller type 'unsigned char' loses information
    add_compile_options(-fms-extensions)
endif()

# Vanilla build
add_executable(devilution
    ${SOURCES}
    ${STUB_SOURCES}
    Stub/main.cpp
)

target_link_libraries(devilution PUBLIC
    m
    ${SDL2_LIBRARIES}
)

# Coverage tracing library (compiled separately so it itself is not traced & can be optimized)
add_library(sanitize_coverage STATIC Stub/sanitize_coverage.cpp)
target_compile_options(sanitize_coverage PRIVATE -O2 -fno-pie -fno-pic)

cmake_push_check_state()
set(SANITIZE_COVERAGE_FLAGS -fsanitize-coverage=trace-pc)
set(CMAKE_REQUIRED_FLAGS ${SANITIZE_COVERAGE_FLAGS})
check_cxx_source_compiles([[extern "C" void __sanitizer_cov_trace_pc(void) {} int main() { return 0; }]] HAS_SANITIZE_COVERAGE)
cmake_pop_check_state()


# xxhash fast hashing library
add_library(xxhash STATIC 3rdParty/xxhash/xxhash.c)
set_source_files_properties(3rdParty/xxhash/xxhash.c PROPERTIES LANGUAGE CXX)
target_include_directories(xxhash PUBLIC 3rdParty/xxhash)
target_compile_options(xxhash PRIVATE -O3 -fno-pie -fno-pic)

# Build with harness enabled (conflicts with sanitizers)
if(TARGET harness)
    add_executable(devil-harness
        ${SOURCES}
        ${STUB_SOURCES}
        # Stub/main_harness.cpp
        Stub/main_test.cpp
        Absolute/absolute.cpp
        Absolute/hook.cpp
        Stub/test_utils.cpp
    )

    if(HAS_SANITIZE_COVERAGE)
        target_compile_options(devil-harness PRIVATE ${SANITIZE_COVERAGE_FLAGS})
        target_compile_definitions(devil-harness PRIVATE -DHAVE_SANITIZE_COVERGE)
    endif()

    target_compile_definitions(devil-harness PUBLIC -DHAVE_HARNESS)

    target_link_libraries(devil-harness PUBLIC
        m
        harness
        xxhash
        sanitize_coverage
        ${SDL2_LIBRARIES}
    )
endif()

# Build with sanitizers enabled
add_executable(devil-sanitize
    ${SOURCES}
    ${STUB_SOURCES}
    Stub/main_test.cpp
    Stub/test_utils.cpp
    Stub/sanitize.cpp
)

target_compile_options(devil-sanitize PRIVATE
    ${SANITIZE_OPTIONS}
    ${COVERAGE_COMPILER_FLAGS}
)

target_link_libraries(devil-sanitize PUBLIC
    m
    xxhash
    sanitize_coverage
    ${SDL2_LIBRARIES}
    ${SANITIZE_OPTIONS}
    ${COVERAGE_COMPILER_FLAGS}
)

if(COVERAGE_COMPILER_FLAGS)
    SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
        NAME devil-sanitize-coverage
        EXECUTABLE devil-sanitize
        DEPENDENCIES devil-sanitize
    )
endif()
