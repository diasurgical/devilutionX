set(BUILD_ASSETS_MPQ OFF)
set(DISABLE_ZERO_TIER ON)
set(USE_SDL1 ON)

# Do not warn about unknown attributes, such as [[nodiscard]].
# As this build uses an older compiler, there are lots of them.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")

# GKD350h IPU scaler is broken at the moment
set(DEFAULT_WIDTH 320)
set(DEFAULT_HEIGHT 240)

set(SDL1_VIDEO_MODE_BPP 16)
set(PREFILL_PLAYER_NAME ON)

# In joystick mode, GKD350h reports D-Pad as left stick,
# so we have to use keyboard mode instead.

set(HAS_KBCTRL 1)
set(KBCTRL_BUTTON_DPAD_LEFT SDLK_LEFT)
set(KBCTRL_BUTTON_DPAD_RIGHT SDLK_RIGHT)
set(KBCTRL_BUTTON_DPAD_UP SDLK_UP)
set(KBCTRL_BUTTON_DPAD_DOWN SDLK_DOWN)
set(KBCTRL_BUTTON_B SDLK_LCTRL)
set(KBCTRL_BUTTON_A SDLK_LALT)
set(KBCTRL_BUTTON_Y SDLK_SPACE)
set(KBCTRL_BUTTON_X SDLK_LSHIFT)
set(KBCTRL_BUTTON_RIGHTSHOULDER SDLK_BACKSPACE)
set(KBCTRL_BUTTON_LEFTSHOULDER SDLK_TAB)

# We swap Select and Start because Start + D-Pad is overtaken by the kernel.
set(KBCTRL_BUTTON_START SDLK_ESCAPE) # Select
set(KBCTRL_BUTTON_BACK SDLK_RETURN) # Start

set(JOY_AXIS_LEFTX 0)
set(JOY_AXIS_LEFTY 1)

# Unused joystick mappings (kept here for future reference).
set(JOY_HAT_DPAD_UP_HAT 0)
set(JOY_HAT_DPAD_UP 1)
set(JOY_HAT_DPAD_DOWN_HAT 0)
set(JOY_HAT_DPAD_DOWN 4)
set(JOY_HAT_DPAD_LEFT_HAT 0)
set(JOY_HAT_DPAD_LEFT 8)
set(JOY_HAT_DPAD_RIGHT_HAT 0)
set(JOY_HAT_DPAD_RIGHT 2)
set(JOY_BUTTON_A 0)
set(JOY_BUTTON_B 1)
set(JOY_BUTTON_Y 2)
set(JOY_BUTTON_X 3)
set(JOY_BUTTON_RIGHTSHOULDER 7)
set(JOY_BUTTON_LEFTSHOULDER 6)
set(JOY_BUTTON_START 5)
set(JOY_BUTTON_BACK 4)
