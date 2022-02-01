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

# Action buttons (swapped A/B and X/Y)
set(JOY_BUTTON_B 1) # ×
set(JOY_BUTTON_A 2) # ○
set(JOY_BUTTON_Y 0) # □
set(JOY_BUTTON_X 3) # △

# Directional buttons
set(JOY_BUTTON_DPAD_LEFT  15)
set(JOY_BUTTON_DPAD_RIGHT 17)
set(JOY_BUTTON_DPAD_UP    16)
set(JOY_BUTTON_DPAD_DOWN  14)

# Shoulder buttons
set(JOY_BUTTON_LEFTSHOULDER  4) # L1
set(JOY_BUTTON_RIGHTSHOULDER 5) # R1

# Trigger buttons
set(JOY_BUTTON_TRIGGERLEFT  18) # L2
set(JOY_BUTTON_TRIGGERRIGHT 19) # R2

# Analog stick click buttons
set(JOY_BUTTON_LEFTSTICK  10) # L3
set(JOY_BUTTON_RIGHTSTICK 11) # R3

# Left analog stick
set(JOY_AXIS_LEFTX 0)
set(JOY_AXIS_LEFTY 1)

# Right analog stick
set(JOY_AXIS_RIGHTX 2)
set(JOY_AXIS_RIGHTY 5)

