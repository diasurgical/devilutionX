# Makefile for showfont

CC = gcc
CFLAGS = $(shell sdl2-config --cflags) -Wall -O
LIBS = $(shell sdl2-config --libs) -lSDL2_ttf
EXE = showfont #glfont

all: $(EXE)

showfont: showfont.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

glfont: glfont.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

clean:
	-rm *.o $(EXE)
