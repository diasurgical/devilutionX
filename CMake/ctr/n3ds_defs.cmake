#General compilation options
set(NONET ON)
set(USE_SDL1 ON)

# Streaming audio is broken on the 3DS as of 25 Mar 2021:
# https://github.com/devkitPro/SDL/issues/72
set(DISABLE_STREAMING_MUSIC ON)
set(DISABLE_STREAMING_SOUNDS ON)

#3DS libraries
list(APPEND CMAKE_MODULE_PATH "${DevilutionX_SOURCE_DIR}/CMake/ctr/modules")
find_package(CITRO3D REQUIRED)
find_package(FREETYPE REQUIRED)
find_package(BZIP2 REQUIRED)
find_package(Tremor REQUIRED)
find_package(OGG REQUIRED)
find_package(MIKMOD REQUIRED)
find_package(MAD REQUIRED)
find_package(PNG REQUIRED)

#additional compilation definitions
add_definitions(-D__3DS__)
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE}\ -DTTF_FONT_PATH=\\"romfs:/CharisSILB.ttf\\")

#Force scaling, for now..
set(SDL1_VIDEO_MODE_FLAGS SDL_FULLSCREEN)
set(DEFAULT_WIDTH 800)
set(DEFAULT_HEIGHT 480)

#SDL Joystick axis mapping (circle-pad)
set(JOY_AXIS_LEFTX 0)
set(JOY_AXIS_LEFTY 1)
#SDL Joystick button mapping (A / B and X / Y inverted)
set(JOY_BUTTON_A 2)
set(JOY_BUTTON_B 1)
set(JOY_BUTTON_X 4)
set(JOY_BUTTON_Y 3)
set(JOY_BUTTON_LEFTSHOULDER 5)
set(JOY_BUTTON_RIGHTSHOULDER 6)
set(JOY_BUTTON_BACK 7)
set(JOY_BUTTON_START 0)
set(JOY_BUTTON_DPAD_DOWN 8)
set(JOY_BUTTON_DPAD_LEFT 9)
set(JOY_BUTTON_DPAD_UP 10)
set(JOY_BUTTON_DPAD_RIGHT 11)
set(JOY_BUTTON_TRIGGERLEFT 12)
set(JOY_BUTTON_TRIGGERRIGHT 13)
