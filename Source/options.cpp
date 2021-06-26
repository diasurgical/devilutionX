/**
 * @file options.cpp
 *
 * Load and save options from the diablo.ini file.
 */

#include <fstream>
#include <cstdint>

#define SI_SUPPORT_IOSTREAMS
#include <SimpleIni.h>

#include "options.h"
#include "diablo.h"
#include "utils/file_util.h"
#include "utils/paths.h"

namespace devilution {

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 640
#endif
#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 480
#endif
#ifndef DEFAULT_AUDIO_SAMPLE_RATE
#define DEFAULT_AUDIO_SAMPLE_RATE 22050
#endif
#ifndef DEFAULT_AUDIO_CHANNELS
#define DEFAULT_AUDIO_CHANNELS 2
#endif
#ifndef DEFAULT_AUDIO_BUFFER_SIZE
#define DEFAULT_AUDIO_BUFFER_SIZE 2048
#endif
#ifndef DEFAULT_AUDIO_RESAMPLING_QUALITY
#define DEFAULT_AUDIO_RESAMPLING_QUALITY 5
#endif

namespace {

std::string getIniPath()
{
	auto path = paths::ConfigPath() + std::string("diablo.ini");
	return path;
}

CSimpleIni &getIni()
{
	static CSimpleIni ini;
	static bool isIniLoaded = false;
	if (!isIniLoaded) {
		auto path = getIniPath();
		auto stream = CreateFileStream(path.c_str(), std::fstream::in | std::fstream::binary);
		ini.SetSpaces(false);
		if (stream != nullptr)
			ini.LoadData(*stream);
		isIniLoaded = true;
	}
	return ini;
}

int getIniInt(const char *keyname, const char *valuename, int defaultValue)
{
	long value = getIni().GetLongValue(keyname, valuename, defaultValue);
	return value;
}

bool getIniBool(const char *sectionName, const char *keyName, bool defaultValue)
{
	bool value = getIni().GetBoolValue(sectionName, keyName, defaultValue);
	return value;
}

float getIniFloat(const char *sectionName, const char *keyName, float defaultValue)
{
	const double value = getIni().GetDoubleValue(sectionName, keyName, defaultValue);
	return (float)value;
}

void setIniValue(const char *keyname, const char *valuename, int value)
{
	getIni().SetLongValue(keyname, valuename, value);
}

void setIniValue(const char *keyname, const char *valuename, std::uint8_t value)
{
	getIni().SetLongValue(keyname, valuename, value);
}

void setIniValue(const char *keyname, const char *valuename, std::uint32_t value)
{
	getIni().SetLongValue(keyname, valuename, value);
}

void setIniValue(const char *keyname, const char *valuename, bool value)
{
	getIni().SetLongValue(keyname, valuename, value ? 1 : 0);
}

void setIniValue(const char *keyname, const char *valuename, float value)
{
	getIni().SetDoubleValue(keyname, valuename, value);
}

void SaveIni()
{
	auto iniPath = getIniPath();
	auto stream = CreateFileStream(iniPath.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
	getIni().Save(*stream, true);
}

}

void setIniValue(const char *sectionName, const char *keyName, const char *value, int len)
{
	auto &ini = getIni();
	std::string stringValue(value, len != 0 ? len : strlen(value));
	ini.SetValue(sectionName, keyName, stringValue.c_str());
}

bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString)
{
	const char *value = getIni().GetValue(sectionName, keyName);
	if (value == nullptr) {
		strncpy(string, defaultString, stringSize);
		return false;
	}
	strncpy(string, value, stringSize);
	return true;
}

/** Game options */
Options sgOptions;
bool sbWasOptionsLoaded = false;

/**
 * @brief Load game configurations from ini file
 */
void LoadOptions()
{
	sgOptions.Diablo.bIntro = getIniBool("Diablo", "Intro", true);
	sgOptions.Hellfire.bIntro = getIniBool("Hellfire", "Intro", true);
	getIniValue("Hellfire", "SItem", sgOptions.Hellfire.szItem, sizeof(sgOptions.Hellfire.szItem), "");

	sgOptions.Audio.nSoundVolume = getIniInt("Audio", "Sound Volume", VOLUME_MAX);
	sgOptions.Audio.nMusicVolume = getIniInt("Audio", "Music Volume", VOLUME_MAX);
	sgOptions.Audio.bWalkingSound = getIniBool("Audio", "Walking Sound", true);
	sgOptions.Audio.bAutoEquipSound = getIniBool("Audio", "Auto Equip Sound", false);

	sgOptions.Audio.nSampleRate = getIniInt("Audio", "Sample Rate", DEFAULT_AUDIO_SAMPLE_RATE);
	sgOptions.Audio.nChannels = getIniInt("Audio", "Channels", DEFAULT_AUDIO_CHANNELS);
	sgOptions.Audio.nBufferSize = getIniInt("Audio", "Buffer Size", DEFAULT_AUDIO_BUFFER_SIZE);
	sgOptions.Audio.nResamplingQuality = getIniInt("Audio", "Resampling Quality", DEFAULT_AUDIO_RESAMPLING_QUALITY);

	sgOptions.Graphics.nWidth = getIniInt("Graphics", "Width", DEFAULT_WIDTH);
	sgOptions.Graphics.nHeight = getIniInt("Graphics", "Height", DEFAULT_HEIGHT);
#ifndef __vita__
	sgOptions.Graphics.bFullscreen = getIniBool("Graphics", "Fullscreen", true);
#else
	sgOptions.Graphics.bFullscreen = true;
#endif
#if !defined(USE_SDL1)
	sgOptions.Graphics.bUpscale = getIniBool("Graphics", "Upscale", true);
#else
	sgOptions.Graphics.bUpscale = false;
#endif
	sgOptions.Graphics.bFitToScreen = getIniBool("Graphics", "Fit to Screen", true);
	getIniValue("Graphics", "Scaling Quality", sgOptions.Graphics.szScaleQuality, sizeof(sgOptions.Graphics.szScaleQuality), "2");
	sgOptions.Graphics.bIntegerScaling = getIniBool("Graphics", "Integer Scaling", false);
	sgOptions.Graphics.bVSync = getIniBool("Graphics", "Vertical Sync", true);
	sgOptions.Graphics.bBlendedTransparancy = getIniBool("Graphics", "Blended Transparency", true);
	sgOptions.Graphics.nGammaCorrection = getIniInt("Graphics", "Gamma Correction", 100);
	sgOptions.Graphics.bColorCycling = getIniBool("Graphics", "Color Cycling", true);
#ifndef USE_SDL1
	sgOptions.Graphics.bHardwareCursor = getIniBool("Graphics", "Hardware Cursor", false);
#else
	sgOptions.Graphics.bHardwareCursor = false;
#endif
	sgOptions.Graphics.bFPSLimit = getIniBool("Graphics", "FPS Limiter", true);
	sgOptions.Graphics.bShowFPS = (getIniInt("Graphics", "Show FPS", 0) != 0);

	sgOptions.Gameplay.nTickRate = getIniInt("Game", "Speed", 20);
	sgOptions.Gameplay.bRunInTown = getIniBool("Game", "Run in Town", false);
	sgOptions.Gameplay.bGrabInput = getIniBool("Game", "Grab Input", false);
	sgOptions.Gameplay.bTheoQuest = getIniBool("Game", "Theo Quest", false);
	sgOptions.Gameplay.bCowQuest = getIniBool("Game", "Cow Quest", false);
	sgOptions.Gameplay.bFriendlyFire = getIniBool("Game", "Friendly Fire", true);
	sgOptions.Gameplay.bTestBard = getIniBool("Game", "Test Bard", false);
	sgOptions.Gameplay.bTestBarbarian = getIniBool("Game", "Test Barbarian", false);
	sgOptions.Gameplay.bExperienceBar = getIniBool("Game", "Experience Bar", false);
	sgOptions.Gameplay.bEnemyHealthBar = getIniBool("Game", "Enemy Health Bar", false);
	sgOptions.Gameplay.bAutoGoldPickup = getIniBool("Game", "Auto Gold Pickup", false);
	sgOptions.Gameplay.bAdriaRefillsMana = getIniBool("Game", "Adria Refills Mana", false);
	sgOptions.Gameplay.bAutoEquipWeapons = getIniBool("Game", "Auto Equip Weapons", true);
	sgOptions.Gameplay.bAutoEquipArmor = getIniBool("Game", "Auto Equip Armor", false);
	sgOptions.Gameplay.bAutoEquipHelms = getIniBool("Game", "Auto Equip Helms", false);
	sgOptions.Gameplay.bAutoEquipShields = getIniBool("Game", "Auto Equip Shields", false);
	sgOptions.Gameplay.bAutoEquipJewelry = getIniBool("Game", "Auto Equip Jewelry", false);
	sgOptions.Gameplay.bRandomizeQuests = getIniBool("Game", "Randomize Quests", true);
	sgOptions.Gameplay.bShowMonsterType = getIniBool("Game", "Show Monster Type", false);
	sgOptions.Gameplay.bDisableCripplingShrines = getIniBool("Game", "Disable Crippling Shrines", false);

	getIniValue("Network", "Bind Address", sgOptions.Network.szBindAddress, sizeof(sgOptions.Network.szBindAddress), "0.0.0.0");
	sgOptions.Network.nPort = getIniInt("Network", "Port", 6112);
	getIniValue("Network", "Previous Host", sgOptions.Network.szPreviousHost, sizeof(sgOptions.Network.szPreviousHost), "");

	for (size_t i = 0; i < QUICK_MESSAGE_OPTIONS; i++)
		getIniValue("NetMsg", QuickMessages[i].key, sgOptions.Chat.szHotKeyMsgs[i], MAX_SEND_STR_LEN, "");

	getIniValue("Controller", "Mapping", sgOptions.Controller.szMapping, sizeof(sgOptions.Controller.szMapping), "");
	sgOptions.Controller.bSwapShoulderButtonMode = getIniBool("Controller", "Swap Shoulder Button Mode", false);
	sgOptions.Controller.bDpadHotkeys = getIniBool("Controller", "Dpad Hotkeys", false);
	sgOptions.Controller.fDeadzone = getIniFloat("Controller", "deadzone", 0.07);
#ifdef __vita__
	sgOptions.Controller.bRearTouch = getIniBool("Controller", "Enable Rear Touchpad", true);
#endif

	getIniValue("Language", "Code", sgOptions.Language.szCode, sizeof(sgOptions.Language.szCode), "en");

	keymapper.load();

	sbWasOptionsLoaded = true;
}

/**
 * @brief Save game configurations to ini file
 */
void SaveOptions()
{
	setIniValue("Diablo", "Intro", sgOptions.Diablo.bIntro);
	setIniValue("Hellfire", "Intro", sgOptions.Hellfire.bIntro);
	setIniValue("Hellfire", "SItem", sgOptions.Hellfire.szItem);

	setIniValue("Audio", "Sound Volume", sgOptions.Audio.nSoundVolume);
	setIniValue("Audio", "Music Volume", sgOptions.Audio.nMusicVolume);
	setIniValue("Audio", "Walking Sound", sgOptions.Audio.bWalkingSound);
	setIniValue("Audio", "Auto Equip Sound", sgOptions.Audio.bAutoEquipSound);

	setIniValue("Audio", "Sample Rate", sgOptions.Audio.nSampleRate);
	setIniValue("Audio", "Channels", sgOptions.Audio.nChannels);
	setIniValue("Audio", "Buffer Size", sgOptions.Audio.nBufferSize);
	setIniValue("Audio", "Resampling Quality", sgOptions.Audio.nResamplingQuality);
	setIniValue("Graphics", "Width", sgOptions.Graphics.nWidth);
	setIniValue("Graphics", "Height", sgOptions.Graphics.nHeight);
#ifndef __vita__
	setIniValue("Graphics", "Fullscreen", sgOptions.Graphics.bFullscreen);
#endif
#if !defined(USE_SDL1)
	setIniValue("Graphics", "Upscale", sgOptions.Graphics.bUpscale);
#endif
	setIniValue("Graphics", "Fit to Screen", sgOptions.Graphics.bFitToScreen);
	setIniValue("Graphics", "Scaling Quality", sgOptions.Graphics.szScaleQuality);
	setIniValue("Graphics", "Integer Scaling", sgOptions.Graphics.bIntegerScaling);
	setIniValue("Graphics", "Vertical Sync", sgOptions.Graphics.bVSync);
	setIniValue("Graphics", "Blended Transparency", sgOptions.Graphics.bBlendedTransparancy);
	setIniValue("Graphics", "Gamma Correction", sgOptions.Graphics.nGammaCorrection);
	setIniValue("Graphics", "Color Cycling", sgOptions.Graphics.bColorCycling);
#ifndef USE_SDL1
	setIniValue("Graphics", "Hardware Cursor", sgOptions.Graphics.bHardwareCursor);
#endif
	setIniValue("Graphics", "FPS Limiter", sgOptions.Graphics.bFPSLimit);
	setIniValue("Graphics", "Show FPS", sgOptions.Graphics.bShowFPS);

	setIniValue("Game", "Speed", sgOptions.Gameplay.nTickRate);
	setIniValue("Game", "Run in Town", sgOptions.Gameplay.bRunInTown);
	setIniValue("Game", "Grab Input", sgOptions.Gameplay.bGrabInput);
	setIniValue("Game", "Theo Quest", sgOptions.Gameplay.bTheoQuest);
	setIniValue("Game", "Cow Quest", sgOptions.Gameplay.bCowQuest);
	setIniValue("Game", "Friendly Fire", sgOptions.Gameplay.bFriendlyFire);
	setIniValue("Game", "Test Bard", sgOptions.Gameplay.bTestBard);
	setIniValue("Game", "Test Barbarian", sgOptions.Gameplay.bTestBarbarian);
	setIniValue("Game", "Experience Bar", sgOptions.Gameplay.bExperienceBar);
	setIniValue("Game", "Enemy Health Bar", sgOptions.Gameplay.bEnemyHealthBar);
	setIniValue("Game", "Auto Gold Pickup", sgOptions.Gameplay.bAutoGoldPickup);
	setIniValue("Game", "Adria Refills Mana", sgOptions.Gameplay.bAdriaRefillsMana);
	setIniValue("Game", "Auto Equip Weapons", sgOptions.Gameplay.bAutoEquipWeapons);
	setIniValue("Game", "Auto Equip Armor", sgOptions.Gameplay.bAutoEquipArmor);
	setIniValue("Game", "Auto Equip Helms", sgOptions.Gameplay.bAutoEquipHelms);
	setIniValue("Game", "Auto Equip Shields", sgOptions.Gameplay.bAutoEquipShields);
	setIniValue("Game", "Auto Equip Jewelry", sgOptions.Gameplay.bAutoEquipJewelry);
	setIniValue("Game", "Randomize Quests", sgOptions.Gameplay.bRandomizeQuests);
	setIniValue("Game", "Show Monster Type", sgOptions.Gameplay.bShowMonsterType);
	setIniValue("Game", "Disable Crippling Shrines", sgOptions.Gameplay.bDisableCripplingShrines);

	setIniValue("Network", "Bind Address", sgOptions.Network.szBindAddress);
	setIniValue("Network", "Port", sgOptions.Network.nPort);
	setIniValue("Network", "Previous Host", sgOptions.Network.szPreviousHost);

	for (size_t i = 0; i < QUICK_MESSAGE_OPTIONS; i++)
		setIniValue("NetMsg", QuickMessages[i].key, sgOptions.Chat.szHotKeyMsgs[i]);

	setIniValue("Controller", "Mapping", sgOptions.Controller.szMapping);
	setIniValue("Controller", "Swap Shoulder Button Mode", sgOptions.Controller.bSwapShoulderButtonMode);
	setIniValue("Controller", "Dpad Hotkeys", sgOptions.Controller.bDpadHotkeys);
	setIniValue("Controller", "deadzone", sgOptions.Controller.fDeadzone);
#ifdef __vita__
	setIniValue("Controller", "Enable Rear Touchpad", sgOptions.Controller.bRearTouch);
#endif

	setIniValue("Language", "Code", sgOptions.Language.szCode);

	keymapper.save();

	SaveIni();
}

} // namespace devilution
