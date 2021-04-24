/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#pragma once

#include <cstdint>

#include "engine.h"

namespace devilution {

#define MAXDEAD 31

struct DeadStruct {
	uint8_t *_deadData[8];
	int _deadFrame;
	int _deadWidth;
	int _deadWidth2;
	uint8_t _deadtrans;
};

extern DeadStruct dead[MAXDEAD];
extern int8_t stonendx;

void InitDead();
void AddDead(int dx, int dy, int8_t dv, direction ddir);
void SetDead();

} // namespace devilution
