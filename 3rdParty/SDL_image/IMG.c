/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2021 Sam Lantinga <slouken@libsdl.org>
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* This is a heavily reduced version of IMG.c including only PNG support */

#include "SDL_image.h"

extern int IMG_InitPNG(void);
extern void IMG_QuitPNG(void);

static int initialized = 0;

int IMG_Init(int flags) {
  int result = 0;

  /* Passing 0 returns the currently initialized loaders */
  if (!flags) {
    return initialized;
  }

  if (flags & IMG_INIT_PNG) {
    if ((initialized & IMG_INIT_PNG) || IMG_InitPNG() == 0) {
      result |= IMG_INIT_PNG;
    }
  }
  initialized |= result;

  return result;
}

void IMG_Quit() {
  if (initialized & IMG_INIT_PNG) {
    IMG_QuitPNG();
  }
  initialized = 0;
}
