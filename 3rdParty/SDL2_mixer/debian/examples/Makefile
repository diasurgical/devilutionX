# Makefile for playmus and playwave

CC = gcc
CFLAGS = $(shell sdl2-config --cflags) -Wall -O
LIBS = $(shell sdl2-config --libs) -lSDL_mixer
EXE = playmus playwave

all: $(EXE)

playmus: playmus.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

playwave: playwave.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

clean:
	-rm *.o $(EXE)
