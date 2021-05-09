#General compilation options
set(NONET ON)
set(USE_SDL1 ON)
set(NOSOUND ON)

# keeping the 3DS defaults for now
# Streaming audio is broken on the 3DS as of 25 Mar 2021:
# https://github.com/devkitPro/SDL/issues/72
set(DISABLE_STREAMING_MUSIC ON)
set(DISABLE_STREAMING_SOUNDS ON)

#Force scaling, for now..
set(SDL1_VIDEO_MODE_FLAGS SDL_FULLSCREEN)
set(DEFAULT_WIDTH 640)
set(DEFAULT_HEIGHT 480)

#SDL Joystick axis mapping (circle-pad)
# set(JOY_AXIS_LEFTX 0)
# set(JOY_AXIS_LEFTY 1)
#SDL Joystick button mapping (A / B and X / Y inverted)
# set(JOY_BUTTON_A 2)
# set(JOY_BUTTON_B 1)
# set(JOY_BUTTON_X 4)
# set(JOY_BUTTON_Y 3)
# set(JOY_BUTTON_LEFTSHOULDER 5)
# set(JOY_BUTTON_RIGHTSHOULDER 6)
# set(JOY_BUTTON_BACK 7)
# set(JOY_BUTTON_START 0)
# set(JOY_BUTTON_DPAD_DOWN 8)
# set(JOY_BUTTON_DPAD_LEFT 9)
# set(JOY_BUTTON_DPAD_UP 10)
# set(JOY_BUTTON_DPAD_RIGHT 11)
# set(JOY_BUTTON_TRIGGERLEFT 12)
# set(JOY_BUTTON_TRIGGERRIGHT 13)
