/**
 * @file player_test.h
 *
 * Helpers for player related tests.
 */
#pragma once

#include "items.h"
#include "player.h"

using namespace devilution;

static int CountItems(Item *items, int n)
{
	int count = n;
	for (int i = 0; i < n; i++)
		if (items[i].isEmpty())
			count--;

	return count;
}

static int Count8(int8_t *ints, int n)
{
	int count = n;
	for (int i = 0; i < n; i++)
		if (ints[i] == 0)
			count--;

	return count;
}

static int CountBool(bool *bools, int n)
{
	int count = n;
	for (int i = 0; i < n; i++)
		if (!bools[i])
			count--;

	return count;
}
