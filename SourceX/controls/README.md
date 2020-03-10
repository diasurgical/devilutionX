# Controls handling

DevilutionX supports mouse & keyboard and gamepad input.

This directory currently mostly handles gamepad input.

Low-level gamepad handling is abstracted and 3 implementations are provided:

1. SDL2 controller API.

2. SDL 1&2 joystick API.

   This can be used in SDL1 joystick platforms and for mapping additional
   buttons not defined by SDL2 controller mappings (e.g. additional Nintendo
   Switch arrows).

3. Keyboard keys acting as controller buttons.

   This can be used for testing, or on devices where this is the
   only or the easiest API to use (e.g. RetroFW).

Example keyboard-as-controller build flags:

```bash
cmake .. -DUSE_SDL1=ON -DHAS_KBCTRL=1 -DPREFILL_PLAYER_NAME=ON \
-DKBCTRL_BUTTON_DPAD_LEFT=SDLK_LEFT \
-DKBCTRL_BUTTON_DPAD_RIGHT=SDLK_RIGHT \
-DKBCTRL_BUTTON_DPAD_UP=SDLK_UP \
-DKBCTRL_BUTTON_DPAD_DOWN=SDLK_DOWN \
-DKBCTRL_BUTTON_X=SDLK_y \
-DKBCTRL_BUTTON_Y=SDLK_x \
-DKBCTRL_BUTTON_B=SDLK_a \
-DKBCTRL_BUTTON_A=SDLK_b \
-DKBCTRL_BUTTON_RIGHTSHOULDER=SDLK_RIGHTBRACKET \
-DKBCTRL_BUTTON_LEFTSHOULDER=SDLK_LEFTBRACKET \
-DKBCTRL_BUTTON_LEFTSTICK=SDLK_TAB \
-DKBCTRL_BUTTON_START=SDLK_RETURN \
-DKBCTRL_BUTTON_BACK=SDLK_LSHIFT
```
