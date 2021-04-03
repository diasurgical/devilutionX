set(NONET ON)
set(PREFILL_PLAYER_NAME ON)

# Streaming audio is broken on the Switch as of 25 Mar 2021:
# https://github.com/devkitPro/SDL/issues/72
set(DISABLE_STREAMING_MUSIC ON)
set(DISABLE_STREAMING_SOUNDS ON)

set(JOY_BUTTON_DPAD_LEFT 16)
set(JOY_BUTTON_DPAD_UP 17)
set(JOY_BUTTON_DPAD_RIGHT 18)
set(JOY_BUTTON_DPAD_DOWN 19)
