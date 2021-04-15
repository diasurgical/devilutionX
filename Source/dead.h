/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#pragma once

#include <SDL.h>

namespace devilution {

#define MAXDEAD 31

struct DeadStruct {
	Uint8 *_deadData[8];
	int _deadFrame;
	int _deadWidth;
	int _deadWidth2;
	Uint8 _deadtrans;
};

extern DeadStruct dead[MAXDEAD];
extern int stonendx;

void InitDead();
void AddDead(int dx, int dy, char dv, int ddir);
void SetDead();

}
