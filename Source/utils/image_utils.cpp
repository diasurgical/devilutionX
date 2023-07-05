// image_utils.cpp
#include "utils/image_utils.hpp"
#include <SDL.h>
#include <png.h>

bool SDL_SavePNG(SDL_Surface *surface, const char *file)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte **row_pointers;
	int y;

	/* Open file for writing (binary mode) */
	fp = fopen(file, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", file);
		return false;
	}

	/* Initialize write structure */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		fclose(fp);
		return false;
	}

	/* Initialize info structure */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(fp);
		return false;
	}

	/* Setup Exception handling */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		return false;
	}

	png_init_io(png_ptr, fp);

	/* Write header */
	png_set_IHDR(png_ptr, info_ptr, surface->w, surface->h,
	    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* Allocate memory for one row (3 bytes per pixel - RGB) */
	row_pointers = (png_byte **)png_malloc(png_ptr, surface->h * sizeof(png_byte *));

	/* Set the individual row_pointers to point at the correct offsets */
	for (y = 0; y < surface->h; y++) {
		row_pointers[y] = (png_byte *)(surface->pixels) + y * surface->pitch;
	}

	/* Write image data */
	png_write_image(png_ptr, row_pointers);

	/* End write */
	png_write_end(png_ptr, NULL);

	/* Cleanup */
	if (png_ptr && info_ptr)
		png_destroy_write_struct(&png_ptr, &info_ptr);
	if (row_pointers)
		png_free(png_ptr, row_pointers);
	fclose(fp);

	return true;
}
