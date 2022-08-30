cmake_minimum_required(VERSION 3.13)

foreach(_policy CMP0083 CMP0092 CMP0111 CMP0126 CMP0135)
  if(POLICY ${_policy})
    cmake_policy(SET ${_policy} NEW)
    set(CMAKE_POLICY_DEFAULT_${_policy} NEW)
  endif()
endforeach()

# Projects added via `add_subdirectory` or `FetchContent` may have a lower
# `cmake_minimum_required` than we set here. Set policies that we require
# to their new value so that they still apply.
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist")
  message("-- Detected a source distribution with the required FetchContent dependencies and devilutionx.mpq included")
  set(SRC_DIST ON)
  add_subdirectory(dist)
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(WARNING [[In-source build detected, please eg. create a new directory and use `cmake ..`]])
endif()

include(CMakeDependentOption)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/finders")
include(functions/genex)

# Options required by `VcPkgManifestFeatures`, which must be included before the `project` call.
option(USE_SDL1 "Use SDL1.2 instead of SDL2" OFF)
option(NONET "Disable network support" OFF)
cmake_dependent_option(PACKET_ENCRYPTION "Encrypt network packets" ON "NOT NONET" OFF)
# The gettext[tools] package takes a very long time to install
if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake$")
  option(USE_GETTEXT_FROM_VCPKG "Add vcpkg dependency for gettext[tools] for compiling translations" OFF)
endif()
option(BUILD_TESTING "Build tests." ON)

# These must be included after the options above but before the `project` call.
include(VcPkgManifestFeatures)

# Set up the `project` before the rest of the options so that:
#
# 1. Properties such as `TARGET_SUPPORTS_SHARED_LIBS` are defined.
# 2. Toolchain file is evaluated, required for `Platforms.cmake`,
#    which can override the options.
if(NOT VERSION_NUM)
  include(functions/git)
  get_git_tag(VERSION_NUM)
  if(NOT "${VERSION_NUM}" STREQUAL "")
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" VERSION_NUM ${VERSION_NUM} )
  endif()
  if(NOT VERSION_SUFFIX)
    get_git_commit_hash(GIT_COMMIT_HASH)
    set(VERSION_SUFFIX "${GIT_COMMIT_HASH}")
  endif()
endif()
if(NOT VERSION_SUFFIX)
  set(VERSION_SUFFIX "debug")
endif()
if(VERSION_NUM MATCHES untagged)
  project(DevilutionX LANGUAGES C CXX)
else()
  project(DevilutionX
    VERSION ${VERSION_NUM}
    LANGUAGES C CXX)
endif()
set(PROJECT_VERSION_WITH_SUFFIX "${PROJECT_VERSION}$<$<CONFIG:Debug>:-${VERSION_SUFFIX}>")

# Platform definitions can override options and we want `cmake_dependent_option` to see the effects.
# Note that a few options are still defined before this because they're needed by `VcPkgManifestFeatures.cmake`.
include(Platforms)

# This built-in CMake module adds a BUILD_TESTING option (ON by default).
# Must be included in the top-level `CMakeLists.txt` after calling `project`.
# Because we must include `VcPkgManifestFeatures` before the `project` call,
# we add a BUILD_TESTING option ourselves above as well.
include(CTest)

# Debugging / profiling options
DEBUG_OPTION(ASAN "Enable address sanitizer")
DEBUG_OPTION(UBSAN "Enable undefined behaviour sanitizer")
option(TSAN "Enable thread sanitizer (not compatible with ASAN=ON)" OFF)
DEBUG_OPTION(DEBUG "Enable debug mode in engine")
option(GPERF "Build with GPerfTools profiler" OFF)
cmake_dependent_option(GPERF_HEAP_FIRST_GAME_ITERATION "Save heap profile of the first game iteration" OFF "GPERF" OFF)
option(ENABLE_CODECOVERAGE "Instrument code for code coverage (only enabled with BUILD_TESTING)" OFF)

# Packaging options
RELEASE_OPTION(CPACK "Configure CPack")
option(MACOSX_STANDALONE_APP_BUNDLE "Generate a portable app bundle to use on other devices (requires sudo)" OFF)

# Network options
cmake_dependent_option(DISABLE_TCP "Disable TCP multiplayer option" OFF "NOT NONET" ON)
cmake_dependent_option(DISABLE_ZERO_TIER "Disable ZeroTier multiplayer option" OFF "NOT NONET" ON)

# Sound options
option(NOSOUND "Disable sound support" OFF)
option(DEVILUTIONX_RESAMPLER_SPEEX "Build with Speex resampler" ON)
cmake_dependent_option(DEVILUTIONX_RESAMPLER_SDL "Build with SDL resampler" ON "NOT USE_SDL1" OFF)
if(DEVILUTIONX_RESAMPLER_SPEEX)
  list(APPEND _resamplers Speex)
endif()
if(DEVILUTIONX_RESAMPLER_SDL)
  list(APPEND _resamplers SDL)
endif()
list(GET _resamplers 0 _default_resampler)
set(DEVILUTIONX_DEFAULT_RESAMPLER ${_default_resampler} CACHE STRING "Default resampler")
set_property(CACHE DEVILUTIONX_DEFAULT_RESAMPLER PROPERTY STRINGS ${_resamplers})

# Optimization / link options
option(DISABLE_LTO "Disable link-time optimization (by default enabled in release mode)" OFF)
option(PIE "Generate position-independent code" OFF)
cmake_dependent_option(DEVILUTIONX_DISABLE_RTTI "Disable RTTI" ON "NONET" OFF)
cmake_dependent_option(DEVILUTIONX_DISABLE_EXCEPTIONS "Disable exceptions" ON "NONET" OFF)
RELEASE_OPTION(DEVILUTIONX_STATIC_CXX_STDLIB "Link C++ standard library statically (if available)")

include(MoldLinker)

# Memory / performance trade-off options
option(DISABLE_STREAMING_MUSIC "Disable streaming music (to work around broken platform implementations)" OFF)
mark_as_advanced(DISABLE_STREAMING_MUSIC)
option(DISABLE_STREAMING_SOUNDS "Disable streaming sounds (to work around broken platform implementations)" OFF)
mark_as_advanced(DISABLE_STREAMING_SOUNDS)
option(STREAM_ALL_AUDIO "Stream all the audio. For extremely RAM-constrained platforms.")
mark_as_advanced(STREAM_ALL_AUDIO)
option(DEVILUTIONX_PALETTE_TRANSPARENCY_BLACK_16_LUT "Whether to use a lookup table for transparency blending with black. This improves performance of blending transparent black overlays, such as quest dialog background, at the cost of 128 KiB of RAM." ON)
mark_as_advanced(DEVILUTIONX_PALETTE_TRANSPARENCY_BLACK_16_LUT)

# Additional features
option(DISABLE_DEMOMODE "Disable demo mode support" OFF)
option(DISCORD_INTEGRATION "Build with Discord SDK for rich presence support" OFF)

# By default, devilutionx.mpq is built only if smpq is installed.
if(NOT DEFINED BUILD_ASSETS_MPQ AND NOT SRC_DIST)
  find_program(SMPQ smpq)
elseif(BUILD_ASSETS_MPQ)
  find_program(SMPQ smpq REQUIRED)
endif()
if(SMPQ)
  set(_has_smpq ON)
else()
  set(_has_smpq OFF)
endif()
option(BUILD_ASSETS_MPQ "If true, assets are packaged into devilutionx.mpq." ${_has_smpq})

# === Option overrides ===
# TSAN is not compatible with ASAN.
if(TSAN)
  set(ASAN OFF)
endif()

if(MSVC AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DISABLE_LTO)
  # Work around MSVC + CMake bug when LTO is enabled.
  # See https://github.com/diasurgical/devilutionX/issues/3778
  # and https://gitlab.kitware.com/cmake/cmake/-/issues/23035
  set(BUILD_TESTING OFF)
endif()

# Note: `CMAKE_CROSSCOMPILING` is only available after the `project` call.
if(CMAKE_CROSSCOMPILING)
  set(BUILD_TESTING OFF)
endif()

if(DISABLE_DEMOMODE)
  # Testing requires demomode.
  set(BUILD_TESTING OFF)
endif()

if(BUILD_TESTING)
  # When tests are enabled, we build a shared devilutionx_so library, which needs to be PIC to link.
  set(PIE ON)
endif()

# Recalculate the dependent options that are defined before `include(Platforms)`:
if(NONET)
  # PACKET_ENCRYPTION is defined before `Platforms.cmake` is included.
  # This means that if a `Platforms.cmake` sets NONET to OFF, PACKET_ENCRYPTION will not automatically
  # reflect that.
  set(PACKET_ENCRYPTION OFF)
endif()
# === End of option overrides ===

if(PIE)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

if(DEVILUTIONX_DISABLE_RTTI)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
  elseif(MSVC)
    string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  endif()
endif()

if(DEVILUTIONX_DISABLE_EXCEPTIONS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
  elseif(MSVC)
    string(REGEX REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  endif()
endif()

# Remove unused symbols in non-debug mode.
# This is useful even with LTO (-84 KiB with MinSizeRel).
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # For some reason, adding to CMAKE_CXX_FLAGS results in a slightly smaller
  # binary than using `add_compile/link_options`

  set(_extra_flags "-ffunction-sections -fdata-sections")
  if(APPLE)
    set(_extra_flags "${_extra_flags} -Wl,-dead_strip")
  else()
    set(_extra_flags "${_extra_flags} -Wl,--gc-sections,--as-needed")
  endif()

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${_extra_flags}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${_extra_flags}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${_extra_flags}")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${_extra_flags}")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${_extra_flags}")
  set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${_extra_flags}")
endif()

# Not a genexp because CMake doesn't support it
# https://gitlab.kitware.com/cmake/cmake/-/issues/20546
if(NOT DISABLE_LTO)
  # LTO if supported:
  include(CheckIPOSupported)
  check_ipo_supported(RESULT is_ipo_supported OUTPUT lto_error)
  if(is_ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
  endif()
endif()

if(GPERF)
  set(DEVILUTIONX_STATIC_CXX_STDLIB OFF)

  if(GPERF_HEAP_FIRST_GAME_ITERATION)
    set(GPERF_HEAP_MAIN ON)
  endif()

  # Compile with information about file and line numbers for everything
  # even in non-Debug build types.
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options("$<$<NOT:$<CONFIG:Debug>>:-g2>")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Use the more size-efficient `-gmlt` option on clang.
    add_compile_options("$<$<NOT:$<CONFIG:Debug>>:-gmlt>")
  endif()
endif()

# Despite setting C++ standard to 20, features from this version are not being used.
# The oldest compiler used is GCC 6.5 - and that defines our C++ feature set (meaning most of C++17).
# It's present only to take advantage of fmt::format build time errors.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Dependencies must be included after Platforms.
include(Dependencies)

add_subdirectory(Source)

set(BIN_TARGET devilutionx)
if(NINTENDO_3DS)
  set(BIN_TARGET ${BIN_TARGET}.elf)
endif()

if(ANDROID)
  add_library(${BIN_TARGET} SHARED Source/main.cpp)
elseif(UWP_LIB)
  add_custom_target(${BIN_TARGET})  # we only need libdevilutionx
else()
  add_executable(${BIN_TARGET}
    WIN32
    MACOSX_BUNDLE
    Source/main.cpp
    Packaging/windows/devilutionx.exe.manifest
    Packaging/windows/devilutionx.rc
    Packaging/apple/AppIcon.icns
    Packaging/apple/LaunchScreen.storyboard)

    if(CMAKE_STRIP AND NOT DEVILUTIONX_DISABLE_STRIP)
      add_custom_command(
        TARGET ${BIN_TARGET} POST_BUILD
        DEPENDS ${BIN_TARGET}
        COMMAND $<$<CONFIG:Release>:${CMAKE_STRIP}>
        ARGS --strip-all $<TARGET_FILE:${BIN_TARGET}>)
    endif()
endif()

if(NOT UWP_LIB)
  target_link_libraries(${BIN_TARGET} PRIVATE libdevilutionx)
endif()

if(GPERF)
  target_link_libraries(${BIN_TARGET} PUBLIC ${GPERFTOOLS_LIBRARIES})
endif()

# Must be included after `BIN_TARGET` and `libdevilutionx` are defined.
include(Assets)

if(EMSCRIPTEN)
  target_link_options(${BIN_TARGET} PRIVATE --preload-file assets)
endif()

if(NOT USE_SDL1 AND NOT UWP_LIB)
  target_link_libraries(${BIN_TARGET} PUBLIC ${SDL2_MAIN})
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

include(functions/set_relative_file_macro)
set_relative_file_macro(${BIN_TARGET})

if(APPLE)
  set_source_files_properties("./Packaging/apple/AppIcon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set(MACOSX_BUNDLE_GUI_IDENTIFIER com.diasurgical.devilutionx)
  set(MACOSX_BUNDLE_COPYRIGHT Unlicense)
  set(MACOSX_BUNDLE_BUNDLE_NAME devilutionx)
  set(MACOSX_BUNDLE_DISPLAY_NAME DevilutionX)
  set(MACOSX_BUNDLE_INFO_STRING ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}")
  if(IOS)
    set(MACOSX_BUNDLE_REQUIRED_PLATFORM IPhoneOS)
    set_target_properties(${BIN_TARGET} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "9.0")
  else()
    set(MACOSX_BUNDLE_REQUIRED_PLATFORM Carbon)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12.0")
  endif()
  set_target_properties(${BIN_TARGET} PROPERTIES MACOSX_BUNDLE_ICON_FILE "AppIcon.icns")
  set_target_properties(${BIN_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Packaging/apple/Info.plist")

  install (TARGETS ${BIN_TARGET} DESTINATION ./)

  if(MACOSX_STANDALONE_APP_BUNDLE)
      install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_BINARY_DIR}/${MACOSX_BUNDLE_BUNDLE_NAME}.app \"\" \"\")
        "
        COMPONENT Runtime)
  endif()

  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  if(COREFOUNDATION_LIBRARY)
    target_link_libraries(libdevilutionx PUBLIC "${COREFOUNDATION_LIBRARY}")
    target_compile_definitions(libdevilutionx PRIVATE USE_COREFOUNDATION)
  endif()

  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}")
  set(CPACK On)
endif()

if(NINTENDO_SWITCH)
  nx_generate_nacp (${BIN_TARGET}.nacp
    NAME    "DevilutionX"
    AUTHOR  "Devilution Team"
    VERSION "${PROJECT_VERSION}"
  )

  nx_create_nro(${BIN_TARGET}
    NACP  ${BIN_TARGET}.nacp
    ICON  "${PROJECT_SOURCE_DIR}/Packaging/switch/icon.jpg"
    ROMFS ${DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY}
  )
endif()

if(VITA)
  set(VITA_APP_NAME "devilutionX")
  set(VITA_TITLEID  "DVLX00001")
  set(VITA_VERSION  "01.00")
  set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")
  set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")
  vita_create_self(devilutionx.self devilutionx UNSAFE)
  if(BUILD_ASSETS_MPQ OR SRC_DIST)
    vita_create_vpk(devilutionx.vpk ${VITA_TITLEID} devilutionx.self
      VERSION ${VITA_VERSION}
      NAME ${VITA_APP_NAME}
      FILE Packaging/vita/sce_sys sce_sys
      FILE ${DEVILUTIONX_MPQ} devilutionx.mpq
    )
  else()
    vita_create_vpk(devilutionx.vpk ${VITA_TITLEID} devilutionx.self
      VERSION ${VITA_VERSION}
      NAME ${VITA_APP_NAME}
      FILE Packaging/vita/sce_sys sce_sys
      FILE Packaging/resources/assets assets
      ${VITA_TRANSLATIONS_LIST}
    )
  endif()

endif()

if(PS4)
  add_custom_command(
    TARGET devilutionx_mpq POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${DEVILUTIONX_MPQ} "${PROJECT_SOURCE_DIR}/Packaging/ps4/")
  add_self(${BIN_TARGET})
  add_pkg(${BIN_TARGET} "${PROJECT_SOURCE_DIR}/Packaging/ps4"
    "DVLX00001" "DevilutionX" "${PROJECT_VERSION}")
endif()

if(NINTENDO_3DS)
  set(APP_TITLE       "DevilutionX")
  set(APP_DESCRIPTION "DevilutionX port for 3DS")
  set(APP_AUTHOR      "Diasurgical Team")
  set(APP_ICON        "${PROJECT_SOURCE_DIR}/Packaging/ctr/icon.png")
  set(APP_BANNER      "${PROJECT_SOURCE_DIR}/Packaging/ctr/banner.png")
  set(APP_AUDIO       "${CMAKE_BINARY_DIR}/banner_audio.wav")
  set(APP_RSF         "${PROJECT_SOURCE_DIR}/Packaging/ctr/template.rsf")
  set(APP_VERSION ${PROJECT_VERSION})

  find_program(FFMPEG ffmpeg)
  if(FFMPEG)
    add_custom_command(OUTPUT ${APP_AUDIO}
      COMMAND ${FFMPEG} -y -ss 3.3 -t 3 -i "${PROJECT_SOURCE_DIR}/Packaging/resources/shareware-startup.wav" -af "afade=t=in:st=0:d=0.1,afade=t=out:st=2.9:d=0.1" ${APP_AUDIO}
      DEPENDS ${PROJECT_SOURCE_DIR}/Packaging/resources/shareware-startup.wav
      VERBATIM)
  else()
    add_custom_command(OUTPUT ${APP_AUDIO}
      COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/Packaging/ctr/audio_silent.wav ${APP_AUDIO}
      DEPENDS ${PROJECT_SOURCE_DIR}/Packaging/ctr/audio_silent.wav
      VERBATIM)
  endif()

  include(Tools3DS)
  add_3dsx_target(${BIN_TARGET})
  add_cia_target(${BIN_TARGET} ${APP_RSF} ${APP_BANNER} ${APP_AUDIO})
endif()

if(UWP_LIB)
  target_compile_definitions(libdevilutionx PRIVATE __UWP__=1)
endif()

if(NXDK)
  target_link_libraries(${BIN_TARGET} PRIVATE "${NXDK_DIR}/lib/libnxdk_automount_d.lib")
  target_link_options(${BIN_TARGET} PRIVATE "-include:_automount_d_drive")

  set(_nxdk_pkg_dir "${CMAKE_BINARY_DIR}/pkg")
  set(_xbe_path "${_nxdk_pkg_dir}/default.xbe")
  add_custom_command(
    OUTPUT "${_xbe_path}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${_nxdk_pkg_dir}"
    COMMAND "${NXDK_DIR}/tools/cxbe/cxbe" "-OUT:${_xbe_path}" -TITLE:DevilutionX "-Logo:${PROJECT_SOURCE_DIR}/Packaging/xbox_nxdk/xbe_logo.pgm" "${CMAKE_BINARY_DIR}/${BIN_TARGET}.exe"
    DEPENDS "${BIN_TARGET}"
  )
  add_custom_target(nxdk_xbe DEPENDS "${_xbe_path}")
endif()

if(CPACK AND (APPLE OR BUILD_ASSETS_MPQ OR SRC_DIST))
  if(WIN32)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
      set(SDL2_WIN32_DLLS_DIR "${CMAKE_BINARY_DIR}")
    else()
      set(SDL2_WIN32_DLLS_DIR "${SDL2_EXEC_PREFIX}/bin")
    endif()
    set(SDL2_WIN32_LICENSES_DIR "${PROJECT_SOURCE_DIR}/Packaging/resources")

    file(GLOB SDL2_WIN32_ALL_DLLS
      LIST_DIRECTORIES false
      "${SDL2_WIN32_DLLS_DIR}/*.dll")
    file(GLOB SDL2_WIN32_ALL_LICENSES
      LIST_DIRECTORIES false
      "${SDL2_WIN32_LICENSES_DIR}/LICENSE*.txt"
      "${SDL2_WIN32_LICENSES_DIR}/README*.txt")

    set(CPACK_PACKAGE_FILE_NAME "devilutionx")
    set(CPACK_PACKAGE_NAME ${project_name})
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_STRIP_FILES TRUE)
    install(TARGETS ${BIN_TARGET} DESTINATION .)
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/windows/README.txt"
      DESTINATION "."
    )
    install(FILES "${DEVILUTIONX_MPQ}"
      DESTINATION "."
    )

    foreach(_SDL2_WIN32_DLL_PATH ${SDL2_WIN32_ALL_DLLS})
      install(FILES "${_SDL2_WIN32_DLL_PATH}"
        DESTINATION "."
      )
    endforeach()

    foreach(_SDL2_WIN32_LICENSE_PATH ${SDL2_WIN32_ALL_LICENSES})
      install(FILES "${_SDL2_WIN32_LICENSE_PATH}"
        DESTINATION "LICENSE"
      )
    endforeach()

    if(DISCORD_SHARED_LIB)
      install(FILES "${DISCORD_SHARED_LIB}"
        DESTINATION "."
      )
    endif()

  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    string(TOLOWER ${PROJECT_NAME} project_name)
    set(CPACK_PACKAGE_NAME ${project_name})
    set(CPACK_GENERATOR "7Z")
    # Common *nix files
    set(CPACK_STRIP_FILES TRUE)
    install(TARGETS ${BIN_TARGET} DESTINATION bin)
    set(desktop_file "${PROJECT_SOURCE_DIR}/Packaging/nix/${project_name}.desktop")
    set(desktop_file_hellfire "${PROJECT_SOURCE_DIR}/Packaging/nix/${project_name}-hellfire.desktop")

    find_program(DFI desktop-file-install)
    if(DFI)
      execute_process(COMMAND ${DFI} --dir=${CMAKE_BINARY_DIR} ${desktop_file})
      set(desktop_file "${CMAKE_BINARY_DIR}/${project_name}.desktop")
      execute_process(COMMAND ${DFI} --dir=${CMAKE_BINARY_DIR} ${desktop_file_hellfire})
      set(desktop_file_hellfire "${CMAKE_BINARY_DIR}/${project_name}-hellfire.desktop")
    endif()

    install(FILES "${desktop_file}"
      DESTINATION "share/applications"
    )
    install(FILES "${desktop_file_hellfire}"
      DESTINATION "share/applications"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/nix/README.txt"
      DESTINATION "share/diasurgical/${project_name}"
    )
    install(FILES "${DEVILUTIONX_MPQ}"
      DESTINATION "share/diasurgical/${project_name}"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/icon.png"
      DESTINATION "share/icons/hicolor/512x512/apps"
      RENAME "${project_name}.png"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/hellfire.png"
      DESTINATION "share/icons/hicolor/512x512/apps"
      RENAME "${project_name}-hellfire.png"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/Packaging/nix/devilutionx.metainfo.xml"
      DESTINATION "share/metainfo"
      RENAME "${project_name}.metainfo.xml"
    )

    # -G DEB
    set(CPACK_PACKAGE_CONTACT "anders@jenbo.dk")
    if(USE_SDL1)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2debian")
    else()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0")
    endif()
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

    # -G RPM
    set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

    find_program(RPMBUILD rpmbuild)
    if(RPMBUILD)
      list(APPEND CPACK_GENERATOR "RPM")
    endif()
    find_program(DPKG dpkg)
    if(DPKG)
      list(APPEND CPACK_GENERATOR "DEB")
    endif()

  elseif(APPLE)
    set(CPACK_PACKAGE_FILE_NAME "devilutionx")
    set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK "ON")
    set(CPACK_STRIP_FILES TRUE)
    set(CPACK_GENERATOR "DragNDrop")
  endif()

  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  include(CPack)
endif()
