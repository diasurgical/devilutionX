/*
  SDL_ttf:  A companion library to SDL for working with TrueType (tm) fonts
  Copyright (C) 2001-2019 Sam Lantinga <slouken@libsdl.org>

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H

#include "SDL.h"
#include "SDL_endian.h"
#include "SDL_ttf.h"

/* FIXME: Right now we assume the gray-scale renderer Freetype is using
   supports 256 shades of gray, but we should instead key off of num_grays
   in the result FT_Bitmap after the FT_Render_Glyph() call. */
#define NUM_GRAYS       256


/* x offset = cos(((90.0-12)/360)*2*M_PI), or 12 degree angle */
#define GLYPH_ITALICS   0.207f

/* Handy routines for converting from fixed point */
#define FT_FLOOR(X) ((X & -64) / 64)
#define FT_CEIL(X)  (((X + 63) & -64) / 64)

#define CACHED_METRICS  0x10
#define CACHED_BITMAP   0x01
#define CACHED_PIXMAP   0x02

/* Cached glyph information */
typedef struct cached_glyph {
    int stored;
    FT_UInt index;
    FT_Bitmap bitmap;
    FT_Bitmap pixmap;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int yoffset;
    int advance;
    SDL_bool is_cached;
} c_glyph;

/* The structure used to hold internal font information */
struct _TTF_Font {
    /* Freetype2 maintains all sorts of useful info itself */
    FT_Face face;

    /* We'll cache these ourselves */
    int height;
    int ascent;
    int descent;
    int lineskip;

    /* The font style */
    int face_style;
    int style;
    int outline;

    /* Whether kerning is desired */
    int kerning;
    int use_kerning;

    /* Extra width in glyph bounds for text styles */
    int glyph_overhang;

    /* Information in the font for underlining */
    int underline_offset;
    int underline_height;

    int underline_top_row;
    int strikethrough_top_row;

    /* Cache for style-transformed glyphs */
    c_glyph *current;
    c_glyph cache[257]; /* 257 is a prime */

    /* We are responsible for closing the font stream */
    SDL_RWops *src;
    int freesrc;
    FT_Open_Args args;

    /* really just flags passed into FT_Load_Glyph */
    int hinting;
};

/* Handle a style only if the font does not already handle it */
#define TTF_HANDLE_STYLE_BOLD(font) (((font)->style & TTF_STYLE_BOLD) && \
                                    !((font)->face_style & TTF_STYLE_BOLD))
#define TTF_HANDLE_STYLE_ITALIC(font) (((font)->style & TTF_STYLE_ITALIC) && \
                                      !((font)->face_style & TTF_STYLE_ITALIC))
#define TTF_HANDLE_STYLE_UNDERLINE(font) ((font)->style & TTF_STYLE_UNDERLINE)
#define TTF_HANDLE_STYLE_STRIKETHROUGH(font) ((font)->style & TTF_STYLE_STRIKETHROUGH)

/* Font styles that does not impact glyph drawing */
#define TTF_STYLE_NO_GLYPH_CHANGE   (TTF_STYLE_UNDERLINE | TTF_STYLE_STRIKETHROUGH)

/* The FreeType font engine/library */
static FT_Library library;
static int TTF_initialized = 0;
static int TTF_byteswapped = 0;

#define TTF_CHECKPOINTER(p, errval)                 \
    if (!TTF_initialized) {                   \
        TTF_SetError("Library not initialized");        \
        return errval;                      \
    }                               \
    if (!p) {                         \
        TTF_SetError("Passed a NULL pointer");          \
        return errval;                      \
    }


static int TTF_initFontMetrics(TTF_Font *font);

/* Draw a solid or shaded line of underline_height at the given row. */
static void TTF_drawLine(const TTF_Font *font, const SDL_Surface *textbuf, int row, int color)
{
    int line;
    Uint8 *dst = (Uint8 *)textbuf->pixels + row * textbuf->pitch;

    /* Draw line */
    for (line = font->underline_height; line > 0; --line) {
        SDL_memset(dst, color, textbuf->w);
        dst += textbuf->pitch;
    }
}

/* Draw a blended line of underline_height */
static void TTF_drawLine_Blended(const TTF_Font *font, const SDL_Surface *textbuf, int row, int line_width, Uint32 color)
{
    int line;
    Uint32 *dst = (Uint32 *)textbuf->pixels + row * textbuf->pitch/4;
    int col;

    /* Draw line */
    for (line = font->underline_height; line > 0; --line) {
        for (col = 0; col < line_width; ++col) {
            dst[col] = color;
        }
        dst += textbuf->pitch/4;
    }
}

/* rcg06192001 get linked library's version. */
const SDL_version *TTF_Linked_Version(void)
{
    static SDL_version linked_version;
    SDL_TTF_VERSION(&linked_version);
    return &linked_version;
}

/* This function tells the library whether UNICODE text is generally
   byteswapped.  A UNICODE BOM character at the beginning of a string
   will override this setting for that string.
 */
void TTF_ByteSwappedUNICODE(int swapped)
{
    TTF_byteswapped = swapped;
}

static void TTF_SetFTError(const char *msg, FT_Error error)
{
#ifdef USE_FREETYPE_ERRORS
#undef FTERRORS_H
#define FT_ERRORDEF(e, v, s)  { e, s },
    static const struct
    {
      int          err_code;
      const char  *err_msg;
    } ft_errors[] = {
#include <freetype/fterrors.h>
    };
    int i;
    const char *err_msg;
    char buffer[1024];

    err_msg = NULL;
    for (i=0; i<((sizeof ft_errors)/(sizeof ft_errors[0])); ++i) {
        if (error == ft_errors[i].err_code) {
            err_msg = ft_errors[i].err_msg;
            break;
        }
    }
    if (!err_msg) {
        err_msg = "unknown FreeType error";
    }
    TTF_SetError("%s: %s", msg, err_msg);
#else
    TTF_SetError("%s", msg);
#endif /* USE_FREETYPE_ERRORS */
}

int TTF_Init(void)
{
    int status = 0;

    if (!TTF_initialized) {
        FT_Error error = FT_Init_FreeType(&library);
        if (error) {
            TTF_SetFTError("Couldn't init FreeType engine", error);
            status = -1;
        }
    }
    if (status == 0) {
        ++TTF_initialized;
    }
    return status;
}

static unsigned long RWread(
    FT_Stream stream,
    unsigned long offset,
    unsigned char *buffer,
    unsigned long count
)
{
    SDL_RWops *src;

    src = (SDL_RWops *)stream->descriptor.pointer;
    SDL_RWseek(src, (int)offset, RW_SEEK_SET);
    if (count == 0) {
        return 0;
    }
    return (unsigned long)SDL_RWread(src, buffer, 1, (int)count);
}

TTF_Font* TTF_OpenFontIndexRW(SDL_RWops *src, int freesrc, int ptsize, long index)
{
    TTF_Font *font;
    FT_Error error;
    FT_Face face;
    FT_Stream stream;
    FT_CharMap found;
    Sint64 position;
    int i;

    if (!TTF_initialized) {
        TTF_SetError("Library not initialized");
        if (src && freesrc) {
            SDL_RWclose(src);
        }
        return NULL;
    }

    if (!src) {
        TTF_SetError("Passed a NULL font source");
        return NULL;
    }

    /* Check to make sure we can seek in this stream */
    position = SDL_RWtell(src);
    if (position < 0) {
        TTF_SetError("Can't seek in stream");
        if (freesrc) {
            SDL_RWclose(src);
        }
        return NULL;
    }

    font = (TTF_Font*)SDL_malloc(sizeof *font);
    if (font == NULL) {
        TTF_SetError("Out of memory");
        if (freesrc) {
            SDL_RWclose(src);
        }
        return NULL;
    }
    SDL_memset(font, 0, sizeof(*font));

    font->src = src;
    font->freesrc = freesrc;

    stream = (FT_Stream)SDL_malloc(sizeof(*stream));
    if (stream == NULL) {
        TTF_SetError("Out of memory");
        TTF_CloseFont(font);
        return NULL;
    }
    SDL_memset(stream, 0, sizeof(*stream));

    stream->read = RWread;
    stream->descriptor.pointer = src;
    stream->pos = (unsigned long)position;
    stream->size = (unsigned long)(SDL_RWsize(src) - position);

    font->args.flags = FT_OPEN_STREAM;
    font->args.stream = stream;

    error = FT_Open_Face(library, &font->args, index, &font->face);
    if (error) {
        TTF_SetFTError("Couldn't load font file", error);
        TTF_CloseFont(font);
        return NULL;
    }
    face = font->face;

    /* Set charmap for loaded font */
    found = 0;
#if 0 /* Font debug code */
    for (i = 0; i < face->num_charmaps; i++) {
        FT_CharMap charmap = face->charmaps[i];
        SDL_Log("Found charmap: platform id %d, encoding id %d", charmap->platform_id, charmap->encoding_id);
    }
#endif
    if (!found) {
        for (i = 0; i < face->num_charmaps; i++) {
            FT_CharMap charmap = face->charmaps[i];
            if (charmap->platform_id == 3 && charmap->encoding_id == 10) { /* UCS-4 Unicode */
                found = charmap;
                break;
            }
        }
    }
    if (!found) {
        for (i = 0; i < face->num_charmaps; i++) {
            FT_CharMap charmap = face->charmaps[i];
            if ((charmap->platform_id == 3 && charmap->encoding_id == 1) /* Windows Unicode */
             || (charmap->platform_id == 3 && charmap->encoding_id == 0) /* Windows Symbol */
             || (charmap->platform_id == 2 && charmap->encoding_id == 1) /* ISO Unicode */
             || (charmap->platform_id == 0)) { /* Apple Unicode */
                found = charmap;
                break;
            }
        }
    }
    if (found) {
        /* If this fails, continue using the default charmap */
        FT_Set_Charmap(face, found);
    }

    font->outline = 0;
    TTF_SetFontKerning(font, 1);

    /* Initialize the font face style */
    font->face_style = TTF_STYLE_NORMAL;
    if (font->face->style_flags & FT_STYLE_FLAG_BOLD) {
        font->face_style |= TTF_STYLE_BOLD;
    }
    if (font->face->style_flags & FT_STYLE_FLAG_ITALIC) {
        font->face_style |= TTF_STYLE_ITALIC;
    }

    /* Set the default font style */
    font->style = font->face_style;

    /* Make sure that our font face is scalable (global metrics) */
    if (FT_IS_SCALABLE(face)) {
        /* Set the character size and use default DPI (72) */
        error = FT_Set_Char_Size(font->face, 0, ptsize * 64, 0, 0);
        if (error) {
            TTF_SetFTError("Couldn't set font size", error);
            TTF_CloseFont(font);
            return NULL;
        }
    } else {
        /* Non-scalable font case.  ptsize determines which family
         * or series of fonts to grab from the non-scalable format.
         * It is not the point size of the font.
         * */
        if (font->face->num_fixed_sizes <= 0) {
            TTF_SetError("Couldn't select size : no num_fixed_sizes");
            TTF_CloseFont(font);
            return NULL;
        }

        /* within [0; num_fixed_sizes - 1] */
        ptsize = SDL_max(ptsize, 0);
        ptsize = SDL_min(ptsize, font->face->num_fixed_sizes - 1);

        error = FT_Select_Size(face, ptsize);
        if (error) {
            TTF_SetFTError("Couldn't select size", error);
            TTF_CloseFont(font);
            return NULL;
        }
    }

    if (TTF_initFontMetrics(font) < 0) {
        return NULL;
    }

    return font;
}

/* Update font parameter depending on a style change */
static int TTF_initFontMetrics(TTF_Font *font)
{
    FT_Face face = font->face;

    /* Make sure that our font face is scalable (global metrics) */
    if (FT_IS_SCALABLE(face)) {

        /* Get the scalable font metrics for this font */
        FT_Fixed scale = face->size->metrics.y_scale;
        font->ascent   = FT_CEIL(FT_MulFix(face->ascender, scale));
        font->descent  = FT_CEIL(FT_MulFix(face->descender, scale));
        font->height   = FT_CEIL(FT_MulFix(face->ascender - face->descender, scale));
        font->lineskip = FT_CEIL(FT_MulFix(face->height, scale));
        font->underline_offset = FT_FLOOR(FT_MulFix(face->underline_position, scale));
        font->underline_height = FT_FLOOR(FT_MulFix(face->underline_thickness, scale));

    } else {
        /* Get the font metrics for this font, for the selected size */
        font->ascent   = FT_CEIL(face->size->metrics.ascender);
        font->descent  = FT_CEIL(face->size->metrics.descender);
        font->height   = FT_CEIL(face->size->metrics.height);
        font->lineskip = FT_CEIL(face->size->metrics.height);
        /* face->underline_position and face->underline_height are only
         * relevant for scalable formats (see freetype.h FT_FaceRec)*/
        font->underline_offset = font->descent / 2;
        font->underline_height = 1;
    }

    if (font->underline_height < 1) {
        font->underline_height = 1;
    }

    /* Adjust OutlineStyle, only for scalable fonts */
    if (font->outline > 0 && FT_IS_SCALABLE(face)) {
        int fo = font->outline;
        font->underline_height += 2 * fo;
        font->underline_offset += 2 * fo;
        font->ascent += 2 * fo;
    }

    font->underline_top_row = font->ascent - font->underline_offset - 1;
    font->strikethrough_top_row = font->height / 2;

    /* Robustness */
    if (font->underline_top_row < 0) {
        font->underline_top_row = 0;
    }
    if (font->strikethrough_top_row < 0) {
        font->strikethrough_top_row = 0;
    }

    /* Update height according to the needs of the underline style */
    if (TTF_HANDLE_STYLE_UNDERLINE(font)) {
        int bottom_row = font->underline_top_row + font->underline_height;
        font->height = SDL_max(font->height, bottom_row);
    }
    /* Update height according to the needs of the strikethrough style */
    if (TTF_HANDLE_STYLE_STRIKETHROUGH(font)) {
        int bottom_row = font->strikethrough_top_row + font->underline_height;
        font->height = SDL_max(font->height, bottom_row);
    }

#ifdef DEBUG_FONTS
    SDL_Log("Font metrics:");
    SDL_Log("ascent = %d, descent = %d", font->ascent, font->descent);
    SDL_Log("height = %d, lineskip = %d", font->height, font->lineskip);
    SDL_Log("underline_offset = %d, underline_height = %d", font->underline_offset, font->underline_height);
    SDL_Log("underline_top_row = %d, strikethrough_top_row = %d", font->underline_top_row, font->strikethrough_top_row);
    SDL_Log("scalable=%d", FT_IS_SCALABLE(face));
#endif

    font->glyph_overhang = face->size->metrics.y_ppem / 10;

    return 0;
}

TTF_Font* TTF_OpenFontRW(SDL_RWops *src, int freesrc, int ptsize)
{
    return TTF_OpenFontIndexRW(src, freesrc, ptsize, 0);
}

TTF_Font* TTF_OpenFontIndex(const char *file, int ptsize, long index)
{
    SDL_RWops *rw = SDL_RWFromFile(file, "rb");
    if (rw == NULL) {
        return NULL;
    }
    return TTF_OpenFontIndexRW(rw, 1, ptsize, index);
}

TTF_Font* TTF_OpenFont(const char *file, int ptsize)
{
    return TTF_OpenFontIndex(file, ptsize, 0);
}

static void Flush_Glyph(c_glyph *glyph)
{
    glyph->stored = 0;
    glyph->index = 0;
    if (glyph->bitmap.buffer) {
        SDL_free(glyph->bitmap.buffer);
        glyph->bitmap.buffer = 0;
    }
    if (glyph->pixmap.buffer) {
        SDL_free(glyph->pixmap.buffer);
        glyph->pixmap.buffer = 0;
    }
    glyph->is_cached = SDL_FALSE;
}

static void Flush_Cache(TTF_Font *font)
{
    int i;
    int size = sizeof(font->cache) / sizeof(font->cache[0]);

    for (i = 0; i < size; ++i) {
        if (font->cache[i].is_cached) {
            Flush_Glyph(&font->cache[i]);
        }
    }
}

static FT_Error Load_Glyph(TTF_Font *font, Uint32 idx, c_glyph *cached, int want)
{
    FT_Face face;
    FT_Error error;
    FT_GlyphSlot glyph;
    FT_Glyph_Metrics *metrics;
    FT_Outline *outline;

    if (!font || !font->face) {
        return FT_Err_Invalid_Handle;
    }

    face = font->face;

    /* Load the glyph */
    cached->index = idx;

    error = FT_Load_Glyph(face, cached->index, FT_LOAD_DEFAULT | font->hinting);
    if (error) {
        return error;
    }

    /* Get our glyph shortcuts */
    glyph = face->glyph;
    metrics = &glyph->metrics;
    outline = &glyph->outline;

    /* Get the glyph metrics if desired */
    if ((want & CACHED_METRICS) && !(cached->stored & CACHED_METRICS)) {

        cached->minx = FT_FLOOR(metrics->horiBearingX);
        cached->maxx = FT_CEIL(metrics->horiBearingX + metrics->width);
        cached->maxy = FT_FLOOR(metrics->horiBearingY);
        cached->miny = cached->maxy - FT_CEIL(metrics->height);
        cached->yoffset = font->ascent - cached->maxy;
        cached->advance = FT_CEIL(metrics->horiAdvance);

#ifdef DEBUG_FONTS
        SDL_Log("Index=%d minx=%d maxx=%d miny=%d maxy=%d yoffset=%d advance=%d", 
                cached->index, cached->minx, cached->maxx, cached->miny, cached->maxy, cached->yoffset, cached->advance);
#endif
        /* Robustness */
        if (cached->maxx - cached->minx < 0) {
            int tmp = cached->maxx;
            cached->maxx = cached->minx;
            cached->minx = tmp;
        }
        if (cached->maxy - cached->miny < 0) {
            int tmp = cached->maxy;
            cached->maxy = cached->miny;
            cached->miny = tmp;
        }

        /* Adjust for bold and italic text */
        if (TTF_HANDLE_STYLE_BOLD(font)) {
            cached->maxx += font->glyph_overhang;
            cached->advance += font->glyph_overhang;
        }
        if (TTF_HANDLE_STYLE_ITALIC(font) && FT_IS_SCALABLE(face)) {
            int bump = (int)SDL_ceilf(GLYPH_ITALICS * font->height);
            cached->maxx += bump;
        }

        /* Adjust OutlineStyle, only for scalable fonts */
        if (font->outline > 0 && FT_IS_SCALABLE(face)) {
            int fo = font->outline;
            /* we could have updated minx/miny by -fo, but that would shift the text left  */
            cached->maxx += 2.1f * fo;
            cached->maxy += 2.1f * fo;
            cached->yoffset -= 2 * fo;
        }

        cached->stored |= CACHED_METRICS;
    }

    if (((want & CACHED_BITMAP) && !(cached->stored & CACHED_BITMAP)) ||
         ((want & CACHED_PIXMAP) && !(cached->stored & CACHED_PIXMAP))) {
        int mono = (want & CACHED_BITMAP);
        unsigned int i;
        FT_Bitmap *src;
        FT_Bitmap *dst;
        FT_Glyph bitmap_glyph = NULL;

        /* Handle the italic style, only for scalable fonts */
        if (TTF_HANDLE_STYLE_ITALIC(font) && FT_IS_SCALABLE(face)) {
            FT_Matrix shear;

            shear.xx = 1 << 16;
            shear.xy = (int) (GLYPH_ITALICS * (1 << 16));
            shear.yx = 0;
            shear.yy = 1 << 16;

            FT_Outline_Transform(outline, &shear);
        }

        /* Render as outline */
        if ((font->outline > 0) && glyph->format != FT_GLYPH_FORMAT_BITMAP) {
            FT_Stroker stroker;
            FT_Get_Glyph(glyph, &bitmap_glyph);
            error = FT_Stroker_New(library, &stroker);
            if (error) {
                return error;
            }
            FT_Stroker_Set(stroker, font->outline * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
            FT_Glyph_Stroke(&bitmap_glyph, stroker, 1 /* delete the original glyph */);
            FT_Stroker_Done(stroker);
            /* Render the glyph */
            error = FT_Glyph_To_Bitmap(&bitmap_glyph, mono ? ft_render_mode_mono : ft_render_mode_normal, 0, 1);
            if (error) {
                FT_Done_Glyph(bitmap_glyph);
                return error;
            }
            src = &((FT_BitmapGlyph)bitmap_glyph)->bitmap;
        } else {
            /* Render the glyph */
            error = FT_Render_Glyph(glyph, mono ? ft_render_mode_mono : ft_render_mode_normal);
            if (error) {
                return error;
            }
            src = &glyph->bitmap;
        }
        /* Copy over information to cache */
        if (mono) {
            dst = &cached->bitmap;
        } else {
            dst = &cached->pixmap;
        }
        SDL_memcpy(dst, src, sizeof(*dst));

        /* FT_Render_Glyph() and .fon fonts always generate a
         * two-color (black and white) glyphslot surface, even
         * when rendered in ft_render_mode_normal. */
        /* FT_IS_SCALABLE() means that the font is in outline format,
         * but does not imply that outline is rendered as 8-bit
         * grayscale, because embedded bitmap/graymap is preferred
         * (see FT_LOAD_DEFAULT section of FreeType2 API Reference).
         * FT_Render_Glyph() canreturn two-color bitmap or 4/16/256-
         * color graymap according to the format of embedded bitmap/
         * graymap. */
        if (src->pixel_mode == FT_PIXEL_MODE_MONO) {
            dst->pitch *= 8;
        } else if (src->pixel_mode == FT_PIXEL_MODE_GRAY2) {
            dst->pitch *= 4;
        } else if (src->pixel_mode == FT_PIXEL_MODE_GRAY4) {
            dst->pitch *= 2;
        }

        /* Adjust for bold text */
        if (TTF_HANDLE_STYLE_BOLD(font)) {
            int bump = font->glyph_overhang;
            dst->pitch += bump;
            dst->width += bump;
        }

        if (dst->rows != 0) {
            dst->buffer = (unsigned char *)SDL_malloc(dst->pitch * dst->rows);
            if (!dst->buffer) {
                return FT_Err_Out_Of_Memory;
            }
            SDL_memset(dst->buffer, 0, dst->pitch * dst->rows);

            for (i = 0; i < src->rows; i++) {
                int soffset = i * src->pitch;
                int doffset = i * dst->pitch;
                if (mono) {
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned int j;
                    if (src->pixel_mode == FT_PIXEL_MODE_MONO) {
                        for (j = 0; j < src->width; j += 8) {
                            unsigned char c = *srcp++;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                        }
                    }  else if (src->pixel_mode == FT_PIXEL_MODE_GRAY2) {
                        for (j = 0; j < src->width; j += 4) {
                            unsigned char c = *srcp++;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                        }
                    } else if (src->pixel_mode == FT_PIXEL_MODE_GRAY4) {
                        for (j = 0; j < src->width; j += 2) {
                            unsigned char c = *srcp++;
                            *dstp++ = (((c&0xF0) >> 4) >= 0x8) ? 1 : 0;
                            c <<= 4;
                            *dstp++ = (((c&0xF0) >> 4) >= 0x8) ? 1 : 0;
                        }
                    } else {
                        for (j = 0; j < src->width; j++) {
                            unsigned char c = *srcp++;
                            *dstp++ = (c >= 0x80) ? 1 : 0;
                        }
                    }
                } else if (src->pixel_mode == FT_PIXEL_MODE_MONO) {
                    /* This special case wouldn't
                     * be here if the FT_Render_Glyph()
                     * function wasn't buggy when it tried
                     * to render a .fon font with 256
                     * shades of gray.  Instead, it
                     * returns a black and white surface
                     * and we have to translate it back
                     * to a 256 gray shaded surface.
                     * */
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned char c;
                    unsigned int j, k;
                    for (j = 0; j < src->width; j += 8) {
                        c = *srcp++;
                        for (k = 0; k < 8; ++k) {
                            if ((c&0x80) >> 7) {
                                *dstp++ = NUM_GRAYS - 1;
                            } else {
                                *dstp++ = 0x00;
                            }
                            c <<= 1;
                        }
                    }
                } else if (src->pixel_mode == FT_PIXEL_MODE_GRAY2) {
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned char c;
                    unsigned int j, k;
                    for (j = 0; j < src->width; j += 4) {
                        c = *srcp++;
                        for (k = 0; k < 4; ++k) {
                            if ((c&0xA0) >> 6) {
                                *dstp++ = NUM_GRAYS * ((c&0xA0) >> 6) / 3 - 1;
                            } else {
                                *dstp++ = 0x00;
                            }
                            c <<= 2;
                        }
                    }
                } else if (src->pixel_mode == FT_PIXEL_MODE_GRAY4) {
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned char c;
                    unsigned int j, k;
                    for (j = 0; j < src->width; j += 2) {
                        c = *srcp++;
                        for (k = 0; k < 2; ++k) {
                            if ((c&0xF0) >> 4) {
                                *dstp++ = NUM_GRAYS * ((c&0xF0) >> 4) / 15 - 1;
                            } else {
                                *dstp++ = 0x00;
                            }
                            c <<= 4;
                        }
                    }
                } else {
                    SDL_memcpy(dst->buffer+doffset,
                           src->buffer+soffset, src->pitch);
                }
            }
        }

        /* Handle the bold style */
        if (TTF_HANDLE_STYLE_BOLD(font)) {
            int row;
            int col;
            int offset;
            int pixel;
            Uint8 *pixmap;

            /* The pixmap is a little hard, we have to add and clamp */
            for (row = dst->rows - 1; row >= 0; --row) {
                pixmap = (Uint8*) dst->buffer + row * dst->pitch;
                for (offset = 1; offset <= font->glyph_overhang; ++offset) {
                    for (col = dst->width - 1; col > 0; --col) {
                        if (mono) {
                            pixmap[col] |= pixmap[col-1];
                        } else {
                            pixel = (pixmap[col] + pixmap[col-1]);
                            if (pixel > NUM_GRAYS - 1) {
                                pixel = NUM_GRAYS - 1;
                            }
                            pixmap[col] = (Uint8) pixel;
                        }
                    }
                }
            }
        }

        /* Mark that we rendered this format */
        if (mono) {
            cached->stored |= CACHED_BITMAP;
        } else {
            cached->stored |= CACHED_PIXMAP;
        }

        /* Free outlined glyph */
        if (bitmap_glyph) {
            FT_Done_Glyph(bitmap_glyph);
        }

        /* Freetype may report a larger pixmap than expected. Make sure we don't exceed
         * the size that will be computed in TTF_SizeUTF8_Internal() */
        dst->width = SDL_min((int)dst->width, cached->maxx - cached->minx);
        dst->rows  = SDL_min((int)dst->rows,  cached->maxy - cached->miny);
    }

    /* We're done, mark this glyph cached */
    cached->is_cached = SDL_TRUE;

    return 0;
}

static FT_Error Find_GlyphByIndex(TTF_Font *font, FT_UInt idx, int want)
{
    int retval = 0;
    int hsize = sizeof(font->cache) / sizeof(font->cache[0]);

    int h = idx % hsize;
    font->current = &font->cache[h];

    if (font->current->is_cached && font->current->index != idx) {
        Flush_Glyph(font->current);
    }

    if ((font->current->stored & want) != want) {
        retval = Load_Glyph(font, idx, font->current, want);
    }
    return retval;
}

static FT_Error Find_Glyph(TTF_Font *font, Uint32 ch, int want) {
    Uint32 idx = FT_Get_Char_Index(font->face, ch);
    return Find_GlyphByIndex(font, idx, want);
}

void TTF_CloseFont(TTF_Font *font)
{
    if (font) {
        Flush_Cache(font);
        if (font->face) {
            FT_Done_Face(font->face);
        }
        if (font->args.stream) {
            SDL_free(font->args.stream);
        }
        if (font->freesrc) {
            SDL_RWclose(font->src);
        }
        SDL_free(font);
    }
}

/* Gets the number of bytes needed to convert a Latin-1 string to UTF-8 */
static size_t LATIN1_to_UTF8_len(const char *text)
{
    size_t bytes = 1;
    while (*text) {
        Uint8 ch = *(const Uint8*)text++;
        if (ch <= 0x7F) {
            bytes += 1;
        } else {
            bytes += 2;
        }
    }
    return bytes;
}

/* Gets the number of bytes needed to convert a UCS2 string to UTF-8 */
static size_t UCS2_to_UTF8_len(const Uint16 *text)
{
    size_t bytes = 1;
    while (*text) {
        Uint16 ch = *text++;
        if (ch <= 0x7F) {
            bytes += 1;
        } else if (ch <= 0x7FF) {
            bytes += 2;
        } else {
            bytes += 3;
        }
    }
    return bytes;
}

/* Convert a Latin-1 string to a UTF-8 string */
static void LATIN1_to_UTF8(const char *src, Uint8 *dst)
{
    while (*src) {
        Uint8 ch = *(const Uint8*)src++;
        if (ch <= 0x7F) {
            *dst++ = ch;
        } else {
            *dst++ = 0xC0 | ((ch >> 6) & 0x1F);
            *dst++ = 0x80 | (ch & 0x3F);
        }
    }
    *dst = '\0';
}

/* Convert a UCS-2 string to a UTF-8 string */
static void UCS2_to_UTF8(const Uint16 *src, Uint8 *dst)
{
    int swapped = TTF_byteswapped;

    while (*src) {
        Uint16 ch = *src++;
        if (ch == UNICODE_BOM_NATIVE) {
            swapped = 0;
            continue;
        }
        if (ch == UNICODE_BOM_SWAPPED) {
            swapped = 1;
            continue;
        }
        if (swapped) {
            ch = SDL_Swap16(ch);
        }
        if (ch <= 0x7F) {
            *dst++ = (Uint8) ch;
        } else if (ch <= 0x7FF) {
            *dst++ = 0xC0 | (Uint8) ((ch >> 6) & 0x1F);
            *dst++ = 0x80 | (Uint8) (ch & 0x3F);
        } else {
            *dst++ = 0xE0 | (Uint8) ((ch >> 12) & 0x0F);
            *dst++ = 0x80 | (Uint8) ((ch >> 6) & 0x3F);
            *dst++ = 0x80 | (Uint8) (ch & 0x3F);
        }
    }
    *dst = '\0';
}

/* Gets a unicode value from a UTF-8 encoded string and advance the string */
#define UNKNOWN_UNICODE 0xFFFD
static Uint32 UTF8_getch(const char **src, size_t *srclen)
{
    const Uint8 *p = *(const Uint8**)src;
    size_t left = 0;
    SDL_bool overlong = SDL_FALSE;
    SDL_bool underflow = SDL_FALSE;
    Uint32 ch = UNKNOWN_UNICODE;

    if (*srclen == 0) {
        return UNKNOWN_UNICODE;
    }
    if (p[0] >= 0xFC) {
        if ((p[0] & 0xFE) == 0xFC) {
            if (p[0] == 0xFC && (p[1] & 0xFC) == 0x80) {
                overlong = SDL_TRUE;
            }
            ch = (Uint32) (p[0] & 0x01);
            left = 5;
        }
    } else if (p[0] >= 0xF8) {
        if ((p[0] & 0xFC) == 0xF8) {
            if (p[0] == 0xF8 && (p[1] & 0xF8) == 0x80) {
                overlong = SDL_TRUE;
            }
            ch = (Uint32) (p[0] & 0x03);
            left = 4;
        }
    } else if (p[0] >= 0xF0) {
        if ((p[0] & 0xF8) == 0xF0) {
            if (p[0] == 0xF0 && (p[1] & 0xF0) == 0x80) {
                overlong = SDL_TRUE;
            }
            ch = (Uint32) (p[0] & 0x07);
            left = 3;
        }
    } else if (p[0] >= 0xE0) {
        if ((p[0] & 0xF0) == 0xE0) {
            if (p[0] == 0xE0 && (p[1] & 0xE0) == 0x80) {
                overlong = SDL_TRUE;
            }
            ch = (Uint32) (p[0] & 0x0F);
            left = 2;
        }
    } else if (p[0] >= 0xC0) {
        if ((p[0] & 0xE0) == 0xC0) {
            if ((p[0] & 0xDE) == 0xC0) {
                overlong = SDL_TRUE;
            }
            ch = (Uint32) (p[0] & 0x1F);
            left = 1;
        }
    } else {
        if ((p[0] & 0x80) == 0x00) {
            ch = (Uint32) p[0];
        }
    }
    ++*src;
    --*srclen;
    while (left > 0 && *srclen > 0) {
        ++p;
        if ((p[0] & 0xC0) != 0x80) {
            ch = UNKNOWN_UNICODE;
            break;
        }
        ch <<= 6;
        ch |= (p[0] & 0x3F);
        ++*src;
        --*srclen;
        --left;
    }
    if (left > 0) {
        underflow = SDL_TRUE;
    }
    /* Technically overlong sequences are invalid and should not be interpreted.
       However, it doesn't cause a security risk here and I don't see any harm in
       displaying them. The application is responsible for any other side effects
       of allowing overlong sequences (e.g. string compares failing, etc.)
       See bug 1931 for sample input that triggers this.
    */
    /*if (overlong) return UNKNOWN_UNICODE;*/
    if (underflow ||
        (ch >= 0xD800 && ch <= 0xDFFF) ||
        (ch == 0xFFFE || ch == 0xFFFF) || ch > 0x10FFFF) {
        ch = UNKNOWN_UNICODE;
    }
    return ch;
}

int TTF_FontHeight(const TTF_Font *font)
{
    return font->height;
}

int TTF_FontAscent(const TTF_Font *font)
{
    return font->ascent;
}

int TTF_FontDescent(const TTF_Font *font)
{
    return font->descent;
}

int TTF_FontLineSkip(const TTF_Font *font)
{
    return font->lineskip;
}

int TTF_GetFontKerning(const TTF_Font *font)
{
    return font->kerning;
}

void TTF_SetFontKerning(TTF_Font *font, int allowed)
{
    font->kerning = allowed;
    font->use_kerning = FT_HAS_KERNING(font->face) && font->kerning;
}

long TTF_FontFaces(const TTF_Font *font)
{
    return font->face->num_faces;
}

int TTF_FontFaceIsFixedWidth(const TTF_Font *font)
{
    return FT_IS_FIXED_WIDTH(font->face);
}

char *TTF_FontFaceFamilyName(const TTF_Font *font)
{
    return font->face->family_name;
}

char *TTF_FontFaceStyleName(const TTF_Font *font)
{
    return font->face->style_name;
}

int TTF_GlyphIsProvided(const TTF_Font *font, Uint16 ch)
{
    return FT_Get_Char_Index(font->face, ch);
}

int TTF_GlyphMetrics(TTF_Font *font, Uint16 ch,
                     int *minx, int *maxx, int *miny, int *maxy, int *advance)
{
    FT_Error error;

    error = Find_Glyph(font, ch, CACHED_METRICS);
    if (error) {
        TTF_SetFTError("Couldn't find glyph", error);
        return -1;
    }

    if (minx) {
        *minx = font->current->minx;
    }
    if (maxx) {
        *maxx = font->current->maxx;
    }
    if (miny) {
        *miny = font->current->miny;
    }
    if (maxy) {
        *maxy = font->current->maxy;
    }
    if (advance) {
        *advance = font->current->advance;
    }
    return 0;
}

int TTF_SizeText(TTF_Font *font, const char *text, int *w, int *h)
{
    int status = -1;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, -1);

    utf8 = SDL_stack_alloc(Uint8, LATIN1_to_UTF8_len(text));
    if (utf8) {
        LATIN1_to_UTF8(text, utf8);
        status = TTF_SizeUTF8(font, (char *)utf8, w, h);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return status;
}

static int TTF_SizeUTF8_Internal(TTF_Font *font, const char *text, int *w, int *h, int *xstart, int *ystart)
{
    int x = 0;
    int minx = 0, maxx = 0;
    int miny = 0, maxy = 0;
    c_glyph *glyph;
    FT_Error error;
    FT_UInt prev_index = 0;
    size_t textlen;

    TTF_CHECKPOINTER(text, -1);

    maxy = font->height;

    /* Load each character and sum it's bounding box */
    textlen = SDL_strlen(text);
    while (textlen > 0) {
        Uint32 c = UTF8_getch(&text, &textlen);
        if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED) {
            continue;
        }

        error = Find_Glyph(font, c, CACHED_METRICS);
        if (error) {
            TTF_SetFTError("Couldn't find glyph", error);
            return -1;
        }
        glyph = font->current;

        /* handle kerning */
        if (font->use_kerning && prev_index && glyph->index) {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            x += delta.x >> 6;
        }

        minx = SDL_min(minx, x + glyph->minx);
        maxx = SDL_max(maxx, x + glyph->maxx);
        /* Allows to render a string with only one space " ". (bug 4344). */
        maxx = SDL_max(maxx, x + glyph->advance);

        miny = SDL_min(miny, glyph->yoffset);
        maxy = SDL_max(maxy, glyph->yoffset + glyph->maxy - glyph->miny);

        x += glyph->advance;
        prev_index = glyph->index;
    }

    /* Initial x start position: often 0, except when a glyph would be written at
     * a negative position. In this case an offset is needed for the whole line.*/
    if (xstart) {
        *xstart = (minx < 0)? -minx : 0;
    }

    /* Initial y start: compensation for a negative yoffset */
    if (ystart) {
        *ystart = (miny < 0)? -miny : 0;
    }

    /* Fill the bounds rectangle */
    if (w) {
        *w = (maxx - minx);
    }
    if (h) {
        *h = (maxy - miny);
    }
    return 0;
}

int TTF_SizeUTF8(TTF_Font *font, const char *text, int *w, int *h) {
    return TTF_SizeUTF8_Internal(font, text, w, h, NULL, NULL);
}

int TTF_SizeUNICODE(TTF_Font *font, const Uint16 *text, int *w, int *h)
{
    int status = -1;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, -1);

    utf8 = SDL_stack_alloc(Uint8, UCS2_to_UTF8_len(text));
    if (utf8) {
        UCS2_to_UTF8(text, utf8);
        status = TTF_SizeUTF8(font, (char *)utf8, w, h);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return status;
}

SDL_Surface *TTF_RenderText_Solid(TTF_Font *font,
                const char *text, SDL_Color fg)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, LATIN1_to_UTF8_len(text));
    if (utf8) {
        LATIN1_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Solid(font, (char *)utf8, fg);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

SDL_Surface *TTF_RenderUTF8_Solid(TTF_Font *font,
                const char *text, SDL_Color fg)
{
    int xstart, ystart;
    int width;
    int height;
    SDL_Surface* textbuf;
    SDL_Palette* palette;
    Uint8* src;
    Uint8* dst;
    unsigned int row, col;
    c_glyph *glyph;
    FT_Bitmap *current;
    FT_Error error;
    FT_UInt prev_index = 0;
    size_t textlen;

    TTF_CHECKPOINTER(text, NULL);

    /* Get the dimensions of the text surface */
    if ((TTF_SizeUTF8_Internal(font, text, &width, &height, &xstart, &ystart) < 0) || !width) {
        TTF_SetError("Text has zero width");
        return NULL;
    }

    /* Create the target surface */
    textbuf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);
    if (textbuf == NULL) {
        return NULL;
    }

    /* Fill the palette with the foreground color */
    palette = textbuf->format->palette;
    palette->colors[0].r = 255 - fg.r;
    palette->colors[0].g = 255 - fg.g;
    palette->colors[0].b = 255 - fg.b;
    palette->colors[1].r = fg.r;
    palette->colors[1].g = fg.g;
    palette->colors[1].b = fg.b;
    palette->colors[1].a = fg.a ? fg.a : SDL_ALPHA_OPAQUE;
    SDL_SetColorKey(textbuf, SDL_TRUE, 0);

    /* Load and render each character */
    textlen = SDL_strlen(text);
    while (textlen > 0) {
        Uint32 c = UTF8_getch(&text, &textlen);
        if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED) {
            continue;
        }

        error = Find_Glyph(font, c, CACHED_METRICS|CACHED_BITMAP);
        if (error) {
            TTF_SetFTError("Couldn't find glyph", error);
            SDL_FreeSurface(textbuf);
            return NULL;
        }
        glyph = font->current;
        current = &glyph->bitmap;

        /* handle kerning */
        if (font->use_kerning && prev_index && glyph->index) {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            xstart += delta.x >> 6;
        }

        for (row = 0; row < current->rows; ++row) {
            dst = (Uint8 *)textbuf->pixels +
                (row + ystart + glyph->yoffset) * textbuf->pitch +
                xstart + glyph->minx;
            src = current->buffer + row * current->pitch;
            for (col = current->width; col > 0; --col) {
                *dst++ |= *src++;
            }
        }

        xstart += glyph->advance;
        prev_index = glyph->index;
    }

    /* Handle the underline style */
    if (TTF_HANDLE_STYLE_UNDERLINE(font)) {
        int first_row = font->underline_top_row + ystart;
        TTF_drawLine(font, textbuf, first_row, 1 /* 1 because 0 is the bg color */ );
    }

    /* Handle the strikethrough style */
    if (TTF_HANDLE_STYLE_STRIKETHROUGH(font)) {
        int first_row = font->strikethrough_top_row + ystart;
        TTF_drawLine(font, textbuf, first_row, 1 /* 1 because 0 is the bg color */ );
    }
    return textbuf;
}

SDL_Surface *TTF_RenderUNICODE_Solid(TTF_Font *font,
                const Uint16 *text, SDL_Color fg)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, UCS2_to_UTF8_len(text));
    if (utf8) {
        UCS2_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Solid(font, (char *)utf8, fg);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

SDL_Surface *TTF_RenderGlyph_Solid(TTF_Font *font, Uint16 ch, SDL_Color fg)
{
    Uint16 ucs2[2];
    Uint8 utf8[4];

    ucs2[0] = ch;
    ucs2[1] = 0;
    UCS2_to_UTF8(ucs2, utf8);
    return TTF_RenderUTF8_Solid(font, (char *)utf8, fg);
}

SDL_Surface *TTF_RenderText_Shaded(TTF_Font *font,
                const char *text, SDL_Color fg, SDL_Color bg)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, LATIN1_to_UTF8_len(text));
    if (utf8) {
        LATIN1_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Shaded(font, (char *)utf8, fg, bg);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

/* Convert the UTF-8 text to UNICODE and render it
*/
SDL_Surface *TTF_RenderUTF8_Shaded(TTF_Font *font,
                const char *text, SDL_Color fg, SDL_Color bg)
{
    int xstart, ystart;
    int width;
    int height;
    SDL_Surface* textbuf;
    SDL_Palette* palette;
    int index;
    int rdiff;
    int gdiff;
    int bdiff;
    int adiff;
    Uint8* src;
    Uint8* dst;
    unsigned int row, col;
    c_glyph *glyph;
    FT_Bitmap *current;
    FT_Error error;
    FT_UInt prev_index = 0;
    size_t textlen;
    Uint8 bg_alpha;

    TTF_CHECKPOINTER(text, NULL);

    /* Get the dimensions of the text surface */
    if ((TTF_SizeUTF8_Internal(font, text, &width, &height, &xstart, &ystart) < 0) || !width) {
        TTF_SetError("Text has zero width");
        return NULL;
    }

    /* Create the target surface */
    textbuf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);
    if (textbuf == NULL) {
        return NULL;
    }

    /* Support alpha blending */
    if (!fg.a) {
        fg.a = SDL_ALPHA_OPAQUE;
    }
    if (!bg.a) {
        bg.a = SDL_ALPHA_OPAQUE;
    }

    /* Save background alpha value */
    bg_alpha = bg.a;

    if (fg.a != SDL_ALPHA_OPAQUE || bg.a != SDL_ALPHA_OPAQUE) {
        SDL_SetSurfaceBlendMode(textbuf, SDL_BLENDMODE_BLEND);

        /* Would disturb alpha palette */
        if (bg.a == SDL_ALPHA_OPAQUE) {
            bg.a = 0;
        }
    }

    /* Fill the palette with NUM_GRAYS levels of shading from bg to fg */
    palette = textbuf->format->palette;
    rdiff = fg.r - bg.r;
    gdiff = fg.g - bg.g;
    bdiff = fg.b - bg.b;
    adiff = fg.a - bg.a;

    for (index = 0; index < NUM_GRAYS; ++index) {
        palette->colors[index].r = bg.r + (index*rdiff) / (NUM_GRAYS-1);
        palette->colors[index].g = bg.g + (index*gdiff) / (NUM_GRAYS-1);
        palette->colors[index].b = bg.b + (index*bdiff) / (NUM_GRAYS-1);
        palette->colors[index].a = bg.a + (index*adiff) / (NUM_GRAYS-1);
    }

    /* Make sure background has the correct alpha value */
    palette->colors[0].a = bg_alpha;

    /* Load and render each character */
    textlen = SDL_strlen(text);
    while (textlen > 0) {
        Uint32 c = UTF8_getch(&text, &textlen);
        if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED) {
            continue;
        }

        error = Find_Glyph(font, c, CACHED_METRICS|CACHED_PIXMAP);
        if (error) {
            TTF_SetFTError("Couldn't find glyph", error);
            SDL_FreeSurface(textbuf);
            return NULL;
        }
        glyph = font->current;
        current = &glyph->pixmap;

        /* handle kerning */
        if (font->use_kerning && prev_index && glyph->index) {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            xstart += delta.x >> 6;
        }

        for (row = 0; row < current->rows; ++row) {
            dst = (Uint8 *)textbuf->pixels +
                (row + ystart + glyph->yoffset) * textbuf->pitch +
                xstart + glyph->minx;
            src = current->buffer + row * current->pitch;
            for (col = current->width; col > 0; --col) {
                *dst++ |= *src++;
            }
        }

        xstart += glyph->advance;
        prev_index = glyph->index;
    }

    /* Handle the underline style */
    if (TTF_HANDLE_STYLE_UNDERLINE(font)) {
        int first_row = font->underline_top_row + ystart;
        TTF_drawLine(font, textbuf, first_row, NUM_GRAYS - 1);
    }

    /* Handle the strikethrough style */
    if (TTF_HANDLE_STYLE_STRIKETHROUGH(font)) {
        int first_row = font->strikethrough_top_row + ystart;
        TTF_drawLine(font, textbuf, first_row, NUM_GRAYS - 1);
    }
    return textbuf;
}

SDL_Surface* TTF_RenderUNICODE_Shaded(TTF_Font* font,
                       const Uint16* text,
                       SDL_Color fg,
                       SDL_Color bg)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, UCS2_to_UTF8_len(text));
    if (utf8) {
        UCS2_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Shaded(font, (char *)utf8, fg, bg);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

SDL_Surface* TTF_RenderGlyph_Shaded(TTF_Font* font,
                     Uint16 ch,
                     SDL_Color fg,
                     SDL_Color bg)
{
    Uint16 ucs2[2];
    Uint8 utf8[4];

    ucs2[0] = ch;
    ucs2[1] = 0;
    UCS2_to_UTF8(ucs2, utf8);
    return TTF_RenderUTF8_Shaded(font, (char *)utf8, fg, bg);
}

SDL_Surface *TTF_RenderText_Blended(TTF_Font *font,
                const char *text, SDL_Color fg)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, LATIN1_to_UTF8_len(text));
    if (utf8) {
        LATIN1_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Blended(font, (char *)utf8, fg);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

SDL_Surface *TTF_RenderUTF8_Blended(TTF_Font *font,
                const char *text, SDL_Color fg)
{
    unsigned int i;
    int xstart,  ystart;
    int width, height;
    SDL_Surface *textbuf;
    Uint8 alpha;
    Uint8 alpha_table[256];
    Uint32 pixel;
    Uint8 *src;
    Uint32 *dst;
    unsigned int row, col;
    c_glyph *glyph;
    FT_Bitmap *current;
    FT_Error error;
    FT_UInt prev_index = 0;
    size_t textlen;

    TTF_CHECKPOINTER(text, NULL);

    /* Get the dimensions of the text surface */
    if ((TTF_SizeUTF8_Internal(font, text, &width, &height, &xstart, &ystart) < 0) || !width) {
        TTF_SetError("Text has zero width");
        return NULL;
    }

    /* Create the target surface */
    textbuf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (textbuf == NULL) {
        return NULL;
    }

    /* Support alpha blending */
    if (!fg.a) {
        fg.a = SDL_ALPHA_OPAQUE;
    }
    if (fg.a == SDL_ALPHA_OPAQUE) {
        for (i = 0; i < SDL_arraysize(alpha_table); ++i) {
            alpha_table[i] = (Uint8)i;
        }
    } else {
        for (i = 0; i < SDL_arraysize(alpha_table); ++i) {
            alpha_table[i] = (Uint8)(i * fg.a / 255);
        }
        SDL_SetSurfaceBlendMode(textbuf, SDL_BLENDMODE_BLEND);
    }

    /* Load and render each character */
    textlen = SDL_strlen(text);
    pixel = (fg.r<<16)|(fg.g<<8)|fg.b;
    SDL_FillRect(textbuf, NULL, pixel); /* Initialize with fg and 0 alpha */
    while (textlen > 0) {
        Uint32 c = UTF8_getch(&text, &textlen);
        if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED) {
            continue;
        }

        error = Find_Glyph(font, c, CACHED_METRICS|CACHED_PIXMAP);
        if (error) {
            TTF_SetFTError("Couldn't find glyph", error);
            SDL_FreeSurface(textbuf);
            return NULL;
        }
        glyph = font->current;
        current = &glyph->pixmap;

        /* handle kerning */
        if (font->use_kerning && prev_index && glyph->index) {
            FT_Vector delta;
            FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
            xstart += delta.x >> 6;
        }

        for (row = 0; row < current->rows; ++row) {
            dst = (Uint32 *)textbuf->pixels +
                (row + ystart + glyph->yoffset) * textbuf->pitch/4 +
                xstart + glyph->minx;
            src = (Uint8*)current->buffer + row * current->pitch;
            for (col = current->width; col > 0; --col) {
                alpha = *src++;
                *dst++ |= pixel | ((Uint32)alpha_table[alpha] << 24);
            }
        }

        xstart += glyph->advance;
        prev_index = glyph->index;
    }

    /* Handle the underline style */
    if (TTF_HANDLE_STYLE_UNDERLINE(font)) {
        int first_row = font->underline_top_row + ystart;
        TTF_drawLine_Blended(font, textbuf, first_row, textbuf->w, pixel | (alpha_table[255] << 24));
    }

    /* Handle the strikethrough style */
    if (TTF_HANDLE_STYLE_STRIKETHROUGH(font)) {
        int first_row = font->strikethrough_top_row + ystart;
        TTF_drawLine_Blended(font, textbuf, first_row, textbuf->w, pixel | (alpha_table[255] << 24));
    }
    return textbuf;
}

SDL_Surface *TTF_RenderUNICODE_Blended(TTF_Font *font,
                const Uint16 *text, SDL_Color fg)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, UCS2_to_UTF8_len(text));
    if (utf8) {
        UCS2_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Blended(font, (char *)utf8, fg);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}


SDL_Surface *TTF_RenderText_Blended_Wrapped(TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, LATIN1_to_UTF8_len(text));
    if (utf8) {
        LATIN1_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Blended_Wrapped(font, (char *)utf8, fg, wrapLength);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

static SDL_bool CharacterIsDelimiter(char c, const char *delimiters)
{
    while (*delimiters) {
        if (c == *delimiters) {
            return SDL_TRUE;
        }
        ++delimiters;
    }
    return SDL_FALSE;
}

SDL_Surface *TTF_RenderUTF8_Blended_Wrapped(TTF_Font *font,
                                    const char *text, SDL_Color fg, Uint32 wrapLength)
{
    unsigned int i;
    int xstart, ystart;
    int width, height;
    SDL_Surface *textbuf;
    Uint8 alpha;
    Uint8 alpha_table[256];
    Uint32 pixel;
    Uint8 *src;
    Uint32 *dst;
    unsigned int row, col;
    c_glyph *glyph;
    FT_Bitmap *current;
    FT_Error error;
    int line, numLines, rowHeight, lineskip;
    char *str, **strLines, **newLines;
    size_t textlen;

    TTF_CHECKPOINTER(text, NULL);

    /* Get the dimensions of the text surface */
    if ((TTF_SizeUTF8(font, text, &width, &height) < 0) || !width) {
        TTF_SetError("Text has zero width");
        return NULL;
    }

    numLines = 1;
    str = NULL;
    strLines = NULL;
    if (wrapLength > 0 && *text) {
        const char *wrapDelims = " \t\r\n";
        int w, h;
        char *spot, *tok, *next_tok, *end;
        char delim;
        size_t str_len = SDL_strlen(text);

        numLines = 0;

        str = SDL_stack_alloc(char, str_len+1);
        if (str == NULL) {
            TTF_SetError("Out of memory");
            return NULL;
        }

        SDL_strlcpy(str, text, str_len+1);
        tok = str;
        end = str + str_len;
        do {
            newLines = (char **)SDL_realloc(strLines, (numLines+1)*sizeof(*strLines));
            if (!newLines) {
                TTF_SetError("Out of memory");
                SDL_free(strLines);
                SDL_stack_free(str);
                return NULL;
            }
            strLines = newLines;
            strLines[numLines++] = tok;

            /* Look for the end of the line */
            if ((spot = SDL_strchr(tok, '\r')) != NULL ||
                (spot = SDL_strchr(tok, '\n')) != NULL) {
                if (*spot == '\r') {
                    ++spot;
                }
                if (*spot == '\n') {
                    ++spot;
                }
            } else {
                spot = end;
            }
            next_tok = spot;

            /* Get the longest string that will fit in the desired space */
            for (; ;) {
                /* Strip trailing whitespace */
                while (spot > tok &&
                        CharacterIsDelimiter(spot[-1], wrapDelims)) {
                    --spot;
                }
                if (spot == tok) {
                    if (CharacterIsDelimiter(*spot, wrapDelims)) {
                        *spot = '\0';
                    }
                    break;
                }
                delim = *spot;
                *spot = '\0';

                TTF_SizeUTF8(font, tok, &w, &h);
                if ((Uint32)w <= wrapLength) {
                    break;
                } else {
                    /* Back up and try again... */
                    *spot = delim;
                }

                while (spot > tok &&
                        !CharacterIsDelimiter(spot[-1], wrapDelims)) {
                    --spot;
                }
                if (spot > tok) {
                    next_tok = spot;
                }
            }
            tok = next_tok;
        } while (tok < end);
    }

    lineskip = TTF_FontLineSkip(font);
    rowHeight = SDL_max(height, lineskip);

    width = (numLines > 1) ? wrapLength : width;

    /* Don't go above wrapLength if you have only 1 line which hasn't been cut */
    width  = SDL_min((int)wrapLength, width);

    /* Create the target surface */
    textbuf = SDL_CreateRGBSurface(SDL_SWSURFACE,
            width,
            rowHeight + lineskip * (numLines - 1),
            32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (textbuf == NULL) {
        if (strLines) {
            SDL_free(strLines);
            SDL_stack_free(str);
        }
        return NULL;
    }

    /* Support alpha blending */
    if (!fg.a) {
        fg.a = SDL_ALPHA_OPAQUE;
    }
    if (fg.a == SDL_ALPHA_OPAQUE) {
        for (i = 0; i < SDL_arraysize(alpha_table); ++i) {
            alpha_table[i] = (Uint8)i;
        }
    } else {
        for (i = 0; i < SDL_arraysize(alpha_table); ++i) {
            alpha_table[i] = (Uint8)(i * fg.a / 255);
        }
        SDL_SetSurfaceBlendMode(textbuf, SDL_BLENDMODE_BLEND);
    }

    /* Load and render each character */
    pixel = (fg.r<<16)|(fg.g<<8)|fg.b;
    SDL_FillRect(textbuf, NULL, pixel); /* Initialize with fg and 0 alpha */

    for (line = 0; line < numLines; line++) {
        FT_UInt prev_index = 0; /* clear kerning at beginning of line */
        int line_width = 0; /*  underline and strikethrough styles */

        if (strLines) {
            text = strLines[line];
        }
        textlen = SDL_strlen(text);

        /* Initialize xstart, ystart */
        TTF_SizeUTF8_Internal(font, text, &line_width, NULL, &xstart, &ystart);
        while (textlen > 0) {
            Uint32 c = UTF8_getch(&text, &textlen);
            if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED) {
                continue;
            }

            error = Find_Glyph(font, c, CACHED_METRICS|CACHED_PIXMAP);
            if (error) {
                TTF_SetFTError("Couldn't find glyph", error);
                SDL_FreeSurface(textbuf);
                if (strLines) {
                    SDL_free(strLines);
                    SDL_stack_free(str);
                }
                return NULL;
            }
            glyph = font->current;
            current = &glyph->pixmap;

            /* handle kerning */
            if (font->use_kerning && prev_index && glyph->index) {
                FT_Vector delta;
                FT_Get_Kerning(font->face, prev_index, glyph->index, ft_kerning_default, &delta);
                xstart += delta.x >> 6;
            }

            /* workaround: an unbreakable line doesn't render overlapped */
            if (xstart + glyph->minx + current->width > textbuf->w) {
                break;
            }

            for (row = 0; row < current->rows; ++row) {
                dst = (Uint32*)textbuf->pixels +
                    (lineskip * line + row + ystart + glyph->yoffset) * textbuf->pitch/4 +
                    xstart + glyph->minx;
                src = (Uint8*)current->buffer + row * current->pitch;
                for (col = current->width; col > 0; --col) {
                    alpha = *src++;
                    *dst++ |= pixel | ((Uint32)alpha_table[alpha] << 24);
                }
            }

            xstart += glyph->advance;
            prev_index = glyph->index;
        }

        /* Handle the underline style */
        if (TTF_HANDLE_STYLE_UNDERLINE(font)) {
            int first_row = lineskip * line + font->underline_top_row + ystart;
            TTF_drawLine_Blended(font, textbuf, first_row, SDL_min(line_width, textbuf->w), pixel | (alpha_table[255] << 24));
        }

        /* Handle the strikethrough style */
        if (TTF_HANDLE_STYLE_STRIKETHROUGH(font)) {
            int first_row = lineskip * line + font->strikethrough_top_row + ystart;
            TTF_drawLine_Blended(font, textbuf, first_row, SDL_min(line_width, textbuf->w), pixel | (alpha_table[255] << 24));
        }
    }

    if (strLines) {
        SDL_free(strLines);
        SDL_stack_free(str);
    }
    return textbuf;
}

SDL_Surface *TTF_RenderUNICODE_Blended_Wrapped(TTF_Font *font, const Uint16* text,
                                               SDL_Color fg, Uint32 wrapLength)
{
    SDL_Surface *surface = NULL;
    Uint8 *utf8;

    TTF_CHECKPOINTER(text, NULL);

    utf8 = SDL_stack_alloc(Uint8, UCS2_to_UTF8_len(text));
    if (utf8) {
        UCS2_to_UTF8(text, utf8);
        surface = TTF_RenderUTF8_Blended_Wrapped(font, (char *)utf8, fg, wrapLength);
        SDL_stack_free(utf8);
    } else {
        SDL_OutOfMemory();
    }
    return surface;
}

SDL_Surface *TTF_RenderGlyph_Blended(TTF_Font *font, Uint16 ch, SDL_Color fg)
{
    Uint16 ucs2[2];
    Uint8 utf8[4];

    ucs2[0] = ch;
    ucs2[1] = 0;
    UCS2_to_UTF8(ucs2, utf8);
    return TTF_RenderUTF8_Blended(font, (char *)utf8, fg);
}

void TTF_SetFontStyle(TTF_Font* font, int style)
{
    int prev_style = font->style;
    font->style = style | font->face_style;

    TTF_initFontMetrics(font);

    /* Flush the cache if the style has changed.
     * Ignore UNDERLINE which does not impact glyph drawning.
     * */
    if ((font->style | TTF_STYLE_NO_GLYPH_CHANGE) != (prev_style | TTF_STYLE_NO_GLYPH_CHANGE)) {
        Flush_Cache(font);
    }
}

int TTF_GetFontStyle(const TTF_Font* font)
{
    return font->style;
}

void TTF_SetFontOutline(TTF_Font* font, int outline)
{
    font->outline = SDL_max(0, outline);
    TTF_initFontMetrics(font);
    Flush_Cache(font);
}

int TTF_GetFontOutline(const TTF_Font* font)
{
    return font->outline;
}

void TTF_SetFontHinting(TTF_Font* font, int hinting)
{
    if (hinting == TTF_HINTING_LIGHT)
        font->hinting = FT_LOAD_TARGET_LIGHT;
    else if (hinting == TTF_HINTING_MONO)
        font->hinting = FT_LOAD_TARGET_MONO;
    else if (hinting == TTF_HINTING_NONE)
        font->hinting = FT_LOAD_NO_HINTING;
    else
        font->hinting = 0;

    Flush_Cache(font);
}

int TTF_GetFontHinting(const TTF_Font* font)
{
    if (font->hinting == FT_LOAD_TARGET_LIGHT)
        return TTF_HINTING_LIGHT;
    else if (font->hinting == FT_LOAD_TARGET_MONO)
        return TTF_HINTING_MONO;
    else if (font->hinting == FT_LOAD_NO_HINTING)
        return TTF_HINTING_NONE;
    return 0;
}

void TTF_Quit(void)
{
    if (TTF_initialized) {
        if (--TTF_initialized == 0) {
            FT_Done_FreeType(library);
        }
    }
}

int TTF_WasInit(void)
{
    return TTF_initialized;
}

/* don't use this function. It's just here for binary compatibility. */
int TTF_GetFontKerningSize(TTF_Font* font, int prev_index, int index)
{
    FT_Vector delta;
    FT_Get_Kerning(font->face, prev_index, index, ft_kerning_default, &delta);
    return (delta.x >> 6);
}

int TTF_GetFontKerningSizeGlyphs(TTF_Font *font, Uint16 previous_ch, Uint16 ch)
{
    int error;
    FT_UInt glyph_index, prev_index;
    FT_Vector delta;

    if (ch == UNICODE_BOM_NATIVE || ch == UNICODE_BOM_SWAPPED) {
        return 0;
    }

    if (previous_ch == UNICODE_BOM_NATIVE || previous_ch == UNICODE_BOM_SWAPPED) {
        return 0;
    }

    error = Find_Glyph(font, ch, CACHED_METRICS);
    if (error) {
        TTF_SetFTError("Couldn't find glyph", error);
        return -1;
    }
    glyph_index = font->current->index;

    error = Find_Glyph(font, previous_ch, CACHED_METRICS);
    if (error) {
        TTF_SetFTError("Couldn't find glyph", error);
        return -1;
    }
    prev_index = font->current->index;

    error = FT_Get_Kerning(font->face, prev_index, glyph_index, ft_kerning_default, &delta);
    if (error) {
        TTF_SetFTError("Couldn't get glyph kerning", error);
        return -1;
    }
    return (delta.x >> 6);
}

/* vi: set ts=4 sw=4 expandtab: */
