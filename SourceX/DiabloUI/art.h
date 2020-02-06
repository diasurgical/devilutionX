#pragma once

#include "devilution.h"

namespace dvl {

struct Art {
	SDL_Surface *surface = NULL;
	int frames = 1;
	int frame_height = 0;
	unsigned int palette_version = 0;

	int w() const
	{
		return surface ? surface->w : 0;
	}

	int h() const
	{
		return frame_height;
	}

	void Unload()
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
};

void LoadArt(const char *pszFile, Art *art, int frames = 1, SDL_Color *pPalette = NULL);
void LoadMaskedArt(const char *pszFile, Art *art, int frames = 1, int mask = 250);
void LoadArt(Art *art, const BYTE *artData, int w, int h, int frames = 1);

} // namespace dvl
