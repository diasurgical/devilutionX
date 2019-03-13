/*
  glfont:  An example of using the SDL_ttf library with OpenGL.
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

/* A simple program to test the text rendering feature of the TTF library */

/* quiet windows compiler warnings */
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"

#ifdef HAVE_OPENGL

#include "SDL_opengl.h"

#define DEFAULT_PTSIZE  18
#define DEFAULT_TEXT    "The quick brown fox jumped over the lazy dog"
#define WIDTH   640
#define HEIGHT  480

#define TTF_GLFONT_USAGE \
"Usage: %s [-utf8|-unicode] [-b] [-i] [-u] [-fgcol r,g,b] [-bgcol r,g,b] \
<font>.ttf [ptsize] [text]\n"

static void SDL_GL_Enter2DMode(int width, int height)
{
    /* Note, there may be other things you need to change,
       depending on how you have your OpenGL state set up.
    */
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    /* This allows alpha blending of 2D textures with the scene */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

static void SDL_GL_Leave2DMode()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();
}

/* Quick utility function for texture creation */
static int power_of_two(int input)
{
    int value = 1;

    while (value < input) {
        value <<= 1;
    }
    return value;
}

static GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord)
{
    GLuint texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect area;
    Uint8  saved_alpha;
    SDL_BlendMode saved_mode;

    /* Use the surface width and height expanded to powers of 2 */
    w = power_of_two(surface->w);
    h = power_of_two(surface->h);
    texcoord[0] = 0.0f;         /* Min X */
    texcoord[1] = 0.0f;         /* Min Y */
    texcoord[2] = (GLfloat)surface->w / w;  /* Max X */
    texcoord[3] = (GLfloat)surface->h / h;  /* Max Y */

    image = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
            0x000000FF,
            0x0000FF00,
            0x00FF0000,
            0xFF000000
#else
            0xFF000000,
            0x00FF0000,
            0x0000FF00,
            0x000000FF
#endif
            );
    if (image == NULL) {
        return 0;
    }

    /* Save the alpha blending attributes */
    SDL_GetSurfaceAlphaMod(surface, &saved_alpha);
    SDL_SetSurfaceAlphaMod(surface, 0xFF);
    SDL_GetSurfaceBlendMode(surface, &saved_mode);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

    /* Copy the surface into the GL texture image */
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

    /* Restore the alpha blending attributes */
    SDL_SetSurfaceAlphaMod(surface, saved_alpha);
    SDL_SetSurfaceBlendMode(surface, saved_mode);

    /* Create an OpenGL texture for the image */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA,
             w, h,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    return texture;
}

static void cleanup(int exitcode)
{
    TTF_Quit();
    SDL_Quit();
    exit(exitcode);
}

int main(int argc, char *argv[])
{
    char *argv0 = argv[0];
    SDL_Window *window;
    SDL_GLContext context;
    TTF_Font *font;
    SDL_Surface *text = NULL;
    int ptsize;
    int i, done;
    SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
    SDL_Color black = { 0x00, 0x00, 0x00, 0 };
    SDL_Color *forecol;
    SDL_Color *backcol;
    GLenum gl_error;
    GLuint texture;
    int x, y, w, h;
    GLfloat texcoord[4];
    GLfloat texMinX, texMinY;
    GLfloat texMaxX, texMaxY;
        float color[8][3]= {{ 1.0,  1.0,  0.0},
                { 1.0,  0.0,  0.0},
                { 0.0,  0.0,  0.0},
                { 0.0,  1.0,  0.0},
                { 0.0,  1.0,  1.0},
                { 1.0,  1.0,  1.0},
                { 1.0,  0.0,  1.0},
                { 0.0,  0.0,  1.0}};
    float cube[8][3]= {{ 0.5,  0.5, -0.5},
               { 0.5, -0.5, -0.5},
               {-0.5, -0.5, -0.5},
               {-0.5,  0.5, -0.5},
               {-0.5,  0.5,  0.5},
               { 0.5,  0.5,  0.5},
               { 0.5, -0.5,  0.5},
               {-0.5, -0.5,  0.5}};
    SDL_Event event;
    int renderstyle;
    int dump;
    enum {
        RENDER_LATIN1,
        RENDER_UTF8,
        RENDER_UNICODE
    } rendertype;
    char *message;

    /* Look for special execution mode */
    dump = 0;
    /* Look for special rendering types */
    renderstyle = TTF_STYLE_NORMAL;
    rendertype = RENDER_LATIN1;
    /* Default is black and white */
    forecol = &black;
    backcol = &white;
    for (i=1; argv[i] && argv[i][0] == '-'; ++i) {
        if (strcmp(argv[i], "-utf8") == 0) {
            rendertype = RENDER_UTF8;
        } else
        if (strcmp(argv[i], "-unicode") == 0) {
            rendertype = RENDER_UNICODE;
        } else
        if (strcmp(argv[i], "-b") == 0) {
            renderstyle |= TTF_STYLE_BOLD;
        } else
        if (strcmp(argv[i], "-i") == 0) {
            renderstyle |= TTF_STYLE_ITALIC;
        } else
        if (strcmp(argv[i], "-u") == 0) {
            renderstyle |= TTF_STYLE_UNDERLINE;
        } else
        if (strcmp(argv[i], "-dump") == 0) {
            dump = 1;
        } else
        if (strcmp(argv[i], "-fgcol") == 0) {
            int r, g, b;
            if (sscanf (argv[++i], "%d,%d,%d", &r, &g, &b) != 3) {
                fprintf(stderr, TTF_GLFONT_USAGE, argv0);
                return(1);
            }
            forecol->r = (Uint8)r;
            forecol->g = (Uint8)g;
            forecol->b = (Uint8)b;
        } else
        if (strcmp(argv[i], "-bgcol") == 0) {
            int r, g, b;
            if (sscanf (argv[++i], "%d,%d,%d", &r, &g, &b) != 3) {
                fprintf(stderr, TTF_GLFONT_USAGE, argv0);
                return(1);
            }
            backcol->r = (Uint8)r;
            backcol->g = (Uint8)g;
            backcol->b = (Uint8)b;
        } else {
            fprintf(stderr, TTF_GLFONT_USAGE, argv0);
            return(1);
        }
    }
    argv += i;
    argc -= i;

    /* Check usage */
    if (!argv[0]) {
        fprintf(stderr, TTF_GLFONT_USAGE, argv0);
        return(1);
    }

    /* Initialize the TTF library */
    if (TTF_Init() < 0) {
        fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
        SDL_Quit();
        return(2);
    }

    /* Open the font file with the requested point size */
    ptsize = 0;
    if (argc > 1) {
        ptsize = atoi(argv[1]);
    }
    if (ptsize == 0) {
        i = 2;
        ptsize = DEFAULT_PTSIZE;
    } else {
        i = 3;
    }
    font = TTF_OpenFont(argv[0], ptsize);
    if (font == NULL) {
        fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
                    ptsize, argv[0], SDL_GetError());
        cleanup(2);
    }
    TTF_SetFontStyle(font, renderstyle);

    if(dump) {
        for(i = 48; i < 123; i++) {
            SDL_Surface* glyph = NULL;

            glyph = TTF_RenderGlyph_Shaded(font, i, *forecol, *backcol);

            if(glyph) {
                char outname[64];
                sprintf(outname, "glyph-%d.bmp", i);
                SDL_SaveBMP(glyph, outname);
            }

        }
        cleanup(0);
    }

    /* Set a 640x480 video mode */
    window = SDL_CreateWindow("glfont",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
        cleanup(2);
    }

    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        fprintf(stderr, "Couldn't create OpenGL context: %s\n", SDL_GetError());
        cleanup(2);
    }

    /* Render and center the message */
    if (argc > 2) {
        message = argv[2];
    } else {
        message = DEFAULT_TEXT;
    }
    switch (rendertype) {
        case RENDER_LATIN1:
        text = TTF_RenderText_Blended(font, message, *forecol);
        break;

        case RENDER_UTF8:
        text = TTF_RenderUTF8_Blended(font, message, *forecol);
        break;

        case RENDER_UNICODE:
        {
            /* This doesn't actually work because you can't pass
               UNICODE text in via command line, AFAIK, but...
             */
            Uint16 unicode_text[BUFSIZ];
            int index;
            for (index = 0; (message[0] || message[1]); ++index) {
                unicode_text[index]  = ((Uint8 *)message)[0];
                unicode_text[index] <<= 8;
                unicode_text[index] |= ((Uint8 *)message)[1];
                message += 2;
            }
            text = TTF_RenderUNICODE_Blended(font,
                    unicode_text, *forecol);
        }
        break;
    }
    if (text == NULL) {
        fprintf(stderr, "Couldn't render text: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        cleanup(2);
    }
    x = (WIDTH - text->w)/2;
    y = (HEIGHT - text->h)/2;
    w = text->w;
    h = text->h;
    printf("Font is generally %d big, and string is %d big\n",
                        TTF_FontHeight(font), text->h);

    /* Convert the text into an OpenGL texture */
    glGetError();
    texture = SDL_GL_LoadTexture(text, texcoord);
    if ((gl_error = glGetError()) != GL_NO_ERROR) {
        /* If this failed, the text may exceed texture size limits */
        printf("Warning: Couldn't create texture: 0x%x\n", gl_error);
    }

    /* Make texture coordinates easy to understand */
    texMinX = texcoord[0];
    texMinY = texcoord[1];
    texMaxX = texcoord[2];
    texMaxY = texcoord[3];

    /* We don't need the original text surface anymore */
    SDL_FreeSurface(text);

    /* Initialize the GL state */
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-2.0, 2.0, -2.0, 2.0, -20.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    glShadeModel(GL_SMOOTH);

    /* Wait for a keystroke, and blit text on mouse press */
    done = 0;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                x = event.motion.x - w/2;
                y = event.motion.y - h/2;
                break;

                case SDL_KEYDOWN:
                case SDL_QUIT:
                done = 1;
                break;
                default:
                break;
            }
        }

        /* Clear the screen */
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw the spinning cube */
        glBegin(GL_QUADS);

            glColor3fv(color[0]);
            glVertex3fv(cube[0]);
            glColor3fv(color[1]);
            glVertex3fv(cube[1]);
            glColor3fv(color[2]);
            glVertex3fv(cube[2]);
            glColor3fv(color[3]);
            glVertex3fv(cube[3]);

            glColor3fv(color[3]);
            glVertex3fv(cube[3]);
            glColor3fv(color[4]);
            glVertex3fv(cube[4]);
            glColor3fv(color[7]);
            glVertex3fv(cube[7]);
            glColor3fv(color[2]);
            glVertex3fv(cube[2]);

            glColor3fv(color[0]);
            glVertex3fv(cube[0]);
            glColor3fv(color[5]);
            glVertex3fv(cube[5]);
            glColor3fv(color[6]);
            glVertex3fv(cube[6]);
            glColor3fv(color[1]);
            glVertex3fv(cube[1]);

            glColor3fv(color[5]);
            glVertex3fv(cube[5]);
            glColor3fv(color[4]);
            glVertex3fv(cube[4]);
            glColor3fv(color[7]);
            glVertex3fv(cube[7]);
            glColor3fv(color[6]);
            glVertex3fv(cube[6]);

            glColor3fv(color[5]);
            glVertex3fv(cube[5]);
            glColor3fv(color[0]);
            glVertex3fv(cube[0]);
            glColor3fv(color[3]);
            glVertex3fv(cube[3]);
            glColor3fv(color[4]);
            glVertex3fv(cube[4]);

            glColor3fv(color[6]);
            glVertex3fv(cube[6]);
            glColor3fv(color[1]);
            glVertex3fv(cube[1]);
            glColor3fv(color[2]);
            glVertex3fv(cube[2]);
            glColor3fv(color[7]);
            glVertex3fv(cube[7]);

        glEnd();

        /* Rotate the cube */
        glMatrixMode(GL_MODELVIEW);
        glRotatef(5.0, 1.0, 1.0, 1.0);

        /* Show the text on the screen */
        SDL_GL_Enter2DMode(WIDTH, HEIGHT);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y);
        glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y);
        glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
        glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
        glEnd();
        SDL_GL_Leave2DMode();

        /* Swap the buffers so everything is visible */
        SDL_GL_SwapWindow(window);
    }
    SDL_GL_DeleteContext(context);
    TTF_CloseFont(font);
    cleanup(0);

    /* Not reached, but fixes compiler warnings */
    return 0;
}

#else /* HAVE_OPENGL */

int main(int argc, char *argv[])
{
    printf("No OpenGL support on this system\n");
    return 1;
}

#endif /* HAVE_OPENGL */

/* vi: set ts=4 sw=4 expandtab: */
