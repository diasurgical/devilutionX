// ref: 0x1000523E
BOOL __stdcall UiGetDefaultStats(int pclass, _uidefaultstats *pStats)
{
	if ( !pStats )
		return 0;
	pStats->strength = defstats[pclass][0];
	pStats->magic = defstats[pclass][1];
	pStats->dexterity = defstats[pclass][2];
	pStats->vitality = defstats[pclass][3];
	return 1;
}

// ref: 0x10005287
void __cdecl CreaStat_cpp_init()
{
	CreaStat_cpp_float = CreaStat_cpp_float_value;
}
// 1001F404: using guessed type int CreaStat_cpp_float_value;
// 100296E0: using guessed type int CreaStat_cpp_float;
