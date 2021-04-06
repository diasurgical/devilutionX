/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DeadStruct {
	Uint8 *_deadData[8];
	int _deadFrame;
	int _deadWidth;
	int _deadWidth2;
	Uint8 _deadtrans;
} DeadStruct;

extern DeadStruct dead[MAXDEAD];
extern int stonendx;

void InitDead();
void AddDead(int dx, int dy, char dv, int ddir);
void SetDead();

#ifdef __cplusplus
}
#endif

}
