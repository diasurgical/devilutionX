# This function defines a target that points to an Emscripten system library.
#
# Arguments:
#   LIB_NAME: a human-readable library name.
#   TARGET_NAME: the library target name
#   ...ARGN: Emscripten flags.
#
# Example:
#   emscripten_system_library("SDL2_image" SDL2::SDL2_image USE_SDL_IMAGE=2 "SDL2_IMAGE_FORMATS='[\"png\"]'")
function(emscripten_system_library LIB_NAME TARGET_NAME)
  add_library(${TARGET_NAME} INTERFACE IMPORTED GLOBAL)
  foreach(arg ${ARGN})
    target_compile_options(${TARGET_NAME} INTERFACE "SHELL:-s ${arg}")
    target_link_options(${TARGET_NAME} INTERFACE "SHELL:-s ${arg}")
  endforeach()
  message("-- 📚 ${LIB_NAME}: Emscripten system library via ${ARGN}")
endfunction()
