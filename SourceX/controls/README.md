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
