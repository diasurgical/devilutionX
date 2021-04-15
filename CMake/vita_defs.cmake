set(ASAN OFF)
set(UBSAN OFF)
set(NONET ON)
set(PREFILL_PLAYER_NAME ON)

# Streaming audio is broken on the Vita as of 15 Apr 2021:
# https://github.com/diasurgical/devilutionX/issues/1526
set(DISABLE_STREAMING_MUSIC ON)
set(DISABLE_STREAMING_SOUNDS ON)

set(DEFAULT_WIDTH 960)
set(DEFAULT_HEIGHT 544)
