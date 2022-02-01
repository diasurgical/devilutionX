set(DISCORD_INTEGRATION OFF)
set(BUILD_TESTING OFF)
set(ASAN OFF)
set(UBSAN OFF)
SET(DISABLE_LTO ON)
set(NONET ON)
set(NOEXIT ON)

# Packbrew SDK provides SDL_image, but FindSDL2_image() fails to
# pick up its dependencies (with includes libjpeg, libwebp etc).
# One way to address this, is to do the following:
#
#   target_link_libraries(${BIN_TARGET} PUBLIC ${PC_SDL2_image_LIBRARIES})
#
# or simply use the in-tree copy as follows:
set(DEVILUTIONX_SYSTEM_SDL_IMAGE OFF)


