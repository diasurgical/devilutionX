# Use globbing to find the source directory regardless of the version number
file(GLOB freetype_SOURCE_DIR ${SDL_ttf_SOURCE_DIR}/external/freetype*)
add_subdirectory(${freetype_SOURCE_DIR})

# freetype only provides an INSTALL_INTERFACE directory
# so use the source directory for the BUILD_INTERFACE
target_include_directories(freetype PUBLIC $<BUILD_INTERFACE:${freetype_SOURCE_DIR}/include>)

add_library(Freetype::Freetype ALIAS freetype)