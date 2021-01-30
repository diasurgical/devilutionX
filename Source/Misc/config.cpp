//Fluffy: For loading various options from the game's config file

#include "..\all.h"
#include "../../3rdParty/Storm/Source/storm.h"

//TODO: We could create a struct which contains all general options which can be defined in the config file to make them tidier

DEVILUTION_BEGIN_NAMESPACE

static void LoadGameSetupVariableFromConfig(char *name, BOOL *variable)
{
	int temp = *variable;
	if (SRegLoadValue("devilutionx", name, 0, &temp))
		*variable = (BOOL)temp;
	else
		SRegSaveValue("devilutionx", name, 0, temp);
}

void LoadOptionsFromConfig()
{
	if (!SRegLoadValue("devilutionx", "game speed", 0, &ticks_per_sec)) {
		SRegSaveValue("devilutionx", "game speed", 0, ticks_per_sec);
	}

	//Fluffy: Load game setup from config here when booting up singleplayer (if we fail to load it, then we save its default to the config)
	LoadGameSetupVariableFromConfig("Transparency", &options_transparency);
}

DEVILUTION_END_NAMESPACE
