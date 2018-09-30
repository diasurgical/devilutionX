/*
SDL_FontCache v0.9.0: A font cache for SDL and SDL_ttf
by Jonathan Dearborn
Dedicated to the memory of Florian Hufsky

License:
    The short:
    Use it however you'd like, but keep the copyright and license notice
    whenever these files or parts of them are distributed in uncompiled form.

    The long:
Copyright (c) 2016 Jonathan Dearborn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _SDL_FONTCACHE_H__
#define _SDL_FONTCACHE_H__

#include "SDL.h"
#include "SDL_ttf.h"

#ifdef FC_USE_SDL_GPU
    #include "SDL_gpu.h"
#endif


#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


// Let's pretend this exists...
#define TTF_STYLE_OUTLINE	16



// Differences between SDL_Renderer and SDL_gpu
#ifdef FC_USE_SDL_GPU
#define FC_Rect GPU_Rect
#define FC_Target GPU_Target
#define FC_Image GPU_Image
#define FC_Log GPU_LogError
#else
#define FC_Rect SDL_Rect
#define FC_Target SDL_Renderer
#define FC_Image SDL_Texture
#define FC_Log SDL_Log
#endif


// SDL_FontCache types

typedef enum
{
    FC_ALIGN_LEFT,
    FC_ALIGN_CENTER,
    FC_ALIGN_RIGHT
} FC_AlignEnum;

typedef enum
{
    FC_FILTER_NEAREST,
    FC_FILTER_LINEAR
} FC_FilterEnum;

typedef struct FC_Scale
{
    float x;
    float y;

} FC_Scale;

typedef struct FC_Effect
{
    FC_AlignEnum alignment;
    FC_Scale scale;
    SDL_Color color;

} FC_Effect;

// Opaque type
typedef struct FC_Font FC_Font;


typedef struct FC_GlyphData
{
    SDL_Rect rect;
    int cache_level;

} FC_GlyphData;




// Object creation

FC_Rect FC_MakeRect(float x, float y, float w, float h);

FC_Scale FC_MakeScale(float x, float y);

SDL_Color FC_MakeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

FC_Effect FC_MakeEffect(FC_AlignEnum alignment, FC_Scale scale, SDL_Color color);

FC_GlyphData FC_MakeGlyphData(int cache_level, Sint16 x, Sint16 y, Uint16 w, Uint16 h);



// Font object

FC_Font* FC_CreateFont(void);

#ifdef FC_USE_SDL_GPU
Uint8 FC_LoadFont(FC_Font* font, const char* filename_ttf, Uint32 pointSize, SDL_Color color, int style);

Uint8 FC_LoadFontFromTTF(FC_Font* font, TTF_Font* ttf, SDL_Color color);

Uint8 FC_LoadFont_RW(FC_Font* font, SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, SDL_Color color, int style);
#else
Uint8 FC_LoadFont(FC_Font* font, SDL_Renderer* renderer, const char* filename_ttf, Uint32 pointSize, SDL_Color color, int style);

Uint8 FC_LoadFontFromTTF(FC_Font* font, SDL_Renderer* renderer, TTF_Font* ttf, SDL_Color color);

Uint8 FC_LoadFont_RW(FC_Font* font, SDL_Renderer* renderer, SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, SDL_Color color, int style);
#endif

void FC_ClearFont(FC_Font* font);

void FC_FreeFont(FC_Font* font);



// Built-in loading strings

char* FC_GetStringASCII(void);

char* FC_GetStringLatin1(void);

char* FC_GetStringASCII_Latin1(void);


// UTF-8 to SDL_FontCache codepoint conversion

/*!
Returns the Uint32 codepoint (not UTF-32) parsed from the given UTF-8 string.
\param c A pointer to a string of proper UTF-8 character values.
\param advance_pointer If true, the source pointer will be incremented to skip the extra bytes from multibyte codepoints.
*/
Uint32 FC_GetCodepointFromUTF8(const char** c, Uint8 advance_pointer);

/*!
Parses the given codepoint and stores the UTF-8 bytes in 'result'.  The result is NULL terminated.
\param result A memory buffer for the UTF-8 values.  Must be at least 5 bytes long.
\param codepoint The Uint32 codepoint to parse (not UTF-32).
*/
void FC_GetUTF8FromCodepoint(char* result, Uint32 codepoint);


// UTF-8 string operations

/*! Allocates a new string of 'size' bytes that is already NULL-terminated.  The NULL byte counts toward the size limit, as usual.  Returns NULL if size is 0. */
char* U8_alloc(unsigned int size);

/*! Deallocates the given string. */
void U8_free(char* string);

/*! Allocates a copy of the given string. */
char* U8_strdup(const char* string);

/*! Returns the number of UTF-8 characters in the given string. */
int U8_strlen(const char* string);

/*! Returns the number of bytes in the UTF-8 multibyte character pointed at by 'character'. */
int U8_charsize(const char* character);

/*! Copies the source multibyte character into the given buffer without overrunning it.  Returns 0 on failure. */
int U8_charcpy(char* buffer, const char* source, int buffer_size);

/*! Returns a pointer to the next UTF-8 character. */
const char* U8_next(const char* string);

/*! Inserts a UTF-8 string into 'string' at the given position.  Use a position of -1 to append.  Returns 0 when unable to insert the string. */
int U8_strinsert(char* string, int position, const char* source, int max_bytes);

/*! Erases the UTF-8 character at the given position, moving the subsequent characters down. */
void U8_strdel(char* string, int position);


// Internal settings

/*! Sets the string from which to load the initial glyphs.  Use this if you need upfront loading for any reason (such as lack of render-target support). */
void FC_SetLoadingString(FC_Font* font, const char* string);

/*! Returns the size of the internal buffer which is used for unpacking variadic text data.  This buffer is shared by all FC_Fonts. */
unsigned int FC_GetBufferSize(void);

/*! Changes the size of the internal buffer which is used for unpacking variadic text data.  This buffer is shared by all FC_Fonts. */
void FC_SetBufferSize(unsigned int size);

void FC_SetRenderCallback(FC_Rect (*callback)(FC_Image* src, FC_Rect* srcrect, FC_Target* dest, float x, float y, float xscale, float yscale));

FC_Rect FC_DefaultRenderCallback(FC_Image* src, FC_Rect* srcrect, FC_Target* dest, float x, float y, float xscale, float yscale);


// Custom caching

/*! Returns the number of cache levels that are active. */
int FC_GetNumCacheLevels(FC_Font* font);

/*! Returns the cache source texture at the given cache level. */
FC_Image* FC_GetGlyphCacheLevel(FC_Font* font, int cache_level);

// TODO: Specify ownership of the texture (should be shareable)
/*! Sets a cache source texture for rendering.  New cache levels must be sequential. */
Uint8 FC_SetGlyphCacheLevel(FC_Font* font, int cache_level, FC_Image* cache_texture);

/*! Copies the given surface to the given cache level as a texture.  New cache levels must be sequential. */
Uint8 FC_UploadGlyphCache(FC_Font* font, int cache_level, SDL_Surface* data_surface);


/*! Returns the number of codepoints that are stored in the font's glyph data map. */
unsigned int FC_GetNumCodepoints(FC_Font* font);

/*! Copies the stored codepoints into the given array. */
void FC_GetCodepoints(FC_Font* font, Uint32* result);

/*! Stores the glyph data for the given codepoint in 'result'.  Returns 0 if the codepoint was not found in the cache. */
Uint8 FC_GetGlyphData(FC_Font* font, FC_GlyphData* result, Uint32 codepoint);

/*! Sets the glyph data for the given codepoint.  Duplicates are not checked.  Returns a pointer to the stored data. */
FC_GlyphData* FC_SetGlyphData(FC_Font* font, Uint32 codepoint, FC_GlyphData glyph_data);


// Rendering

FC_Rect FC_Draw(FC_Font* font, FC_Target* dest, float x, float y, const char* formatted_text, ...);
FC_Rect FC_DrawAlign(FC_Font* font, FC_Target* dest, float x, float y, FC_AlignEnum align, const char* formatted_text, ...);
FC_Rect FC_DrawScale(FC_Font* font, FC_Target* dest, float x, float y, FC_Scale scale, const char* formatted_text, ...);
FC_Rect FC_DrawColor(FC_Font* font, FC_Target* dest, float x, float y, SDL_Color color, const char* formatted_text, ...);
FC_Rect FC_DrawEffect(FC_Font* font, FC_Target* dest, float x, float y, FC_Effect effect, const char* formatted_text, ...);

FC_Rect FC_DrawBox(FC_Font* font, FC_Target* dest, FC_Rect box, const char* formatted_text, ...);
FC_Rect FC_DrawBoxAlign(FC_Font* font, FC_Target* dest, FC_Rect box, FC_AlignEnum align, const char* formatted_text, ...);
FC_Rect FC_DrawBoxScale(FC_Font* font, FC_Target* dest, FC_Rect box, FC_Scale scale, const char* formatted_text, ...);
FC_Rect FC_DrawBoxColor(FC_Font* font, FC_Target* dest, FC_Rect box, SDL_Color color, const char* formatted_text, ...);
FC_Rect FC_DrawBoxEffect(FC_Font* font, FC_Target* dest, FC_Rect box, FC_Effect effect, const char* formatted_text, ...);

FC_Rect FC_DrawColumn(FC_Font* font, FC_Target* dest, float x, float y, Uint16 width, const char* formatted_text, ...);
FC_Rect FC_DrawColumnAlign(FC_Font* font, FC_Target* dest, float x, float y, Uint16 width, FC_AlignEnum align, const char* formatted_text, ...);
FC_Rect FC_DrawColumnScale(FC_Font* font, FC_Target* dest, float x, float y, Uint16 width, FC_Scale scale, const char* formatted_text, ...);
FC_Rect FC_DrawColumnColor(FC_Font* font, FC_Target* dest, float x, float y, Uint16 width, SDL_Color color, const char* formatted_text, ...);
FC_Rect FC_DrawColumnEffect(FC_Font* font, FC_Target* dest, float x, float y, Uint16 width, FC_Effect effect, const char* formatted_text, ...);


// Getters

FC_FilterEnum FC_GetFilterMode(FC_Font* font);
Uint16 FC_GetLineHeight(FC_Font* font);
Uint16 FC_GetHeight(FC_Font* font, const char* formatted_text, ...);
Uint16 FC_GetWidth(FC_Font* font, const char* formatted_text, ...);

// Returns a 1-pixel wide box in front of the character in the given position (index)
FC_Rect FC_GetCharacterOffset(FC_Font* font, Uint16 position_index, int column_width, const char* formatted_text, ...);
Uint16 FC_GetColumnHeight(FC_Font* font, Uint16 width, const char* formatted_text, ...);

int FC_GetAscent(FC_Font* font, const char* formatted_text, ...);
int FC_GetDescent(FC_Font* font, const char* formatted_text, ...);
int FC_GetBaseline(FC_Font* font);
int FC_GetSpacing(FC_Font* font);
int FC_GetLineSpacing(FC_Font* font);
Uint16 FC_GetMaxWidth(FC_Font* font);
SDL_Color FC_GetDefaultColor(FC_Font* font);

Uint8 FC_InRect(float x, float y, FC_Rect input_rect);
// Given an offset (x,y) from the text draw position (the upper-left corner), returns the character position (UTF-8 index)
Uint16 FC_GetPositionFromOffset(FC_Font* font, float x, float y, int column_width, FC_AlignEnum align, const char* formatted_text, ...);

// Setters

void FC_SetFilterMode(FC_Font* font, FC_FilterEnum filter);
void FC_SetSpacing(FC_Font* font, int LetterSpacing);
void FC_SetLineSpacing(FC_Font* font, int LineSpacing);
void FC_SetDefaultColor(FC_Font* font, SDL_Color color);


#ifdef __cplusplus
}
#endif



#endif