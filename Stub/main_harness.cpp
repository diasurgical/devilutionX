#include "../types.h"
#include "Absolute/absolute.h"
#include "stubs.h"

#include <stddef.h>

int main(int argc, char **argv)
{
	init_absolute();

	d_WinMain(NULL, NULL, "", 0);
}
