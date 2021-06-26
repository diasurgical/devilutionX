#pragma once

#include <cstdint>

#include "pack.h"

namespace devilution {

struct DiabloOptions {
	/** @brief Play game intro video on startup. */
	bool bIntro;
};

struct HellfireOptions {
	/** @brief Play game intro video on startup. */
	bool bIntro;
	/** @brief Cornerstone of the world item. */
	char szItem[sizeof(PkItemStruct) * 2 + 1];
};

struct AudioOptions {
	/** @brief Movie and SFX volume. */
	int nSoundVolume;
	/** @brief Music volume. */
	int nMusicVolume;
	/** @brief Player emits sound when walking. */
	bool bWalkingSound;
	/** @brief Automatically equipping items on pickup emits the equipment sound. */
	bool bAutoEquipSound;

	/** @brief Output sample rate (Hz) */
	std::uint32_t nSampleRate;
	/** @brief The number of output channels (1 or 2) */
	std::uint8_t nChannels;
	/** @brief Buffer size (number of frames per channel) */
	std::uint32_t nBufferSize;
	/** @brief Quality of the resampler, from 0 (lowest) to 10 (highest) */
	std::uint8_t nResamplingQuality;
};

struct GraphicsOptions {
	/** @brief Render width. */
	int nWidth;
	/** @brief Render height. */
	int nHeight;
	/** @brief Run in fullscreen or windowed mode. */
	bool bFullscreen;
	/** @brief Scale the image after rendering. */
	bool bUpscale;
	/** @brief Expand the aspect ratio to match the screen. */
	bool bFitToScreen;
	/** @brief See SDL_HINT_RENDER_SCALE_QUALITY. */
	char szScaleQuality[2];
	/** @brief Only scale by values divisible by the width and height. */
	bool bIntegerScaling;
	/** @brief Enable vsync on the output. */
	bool bVSync;
	/** @brief Use blended transparency rather than stippled. */
	bool bBlendedTransparancy;
	/** @brief Gamma correction level. */
	int nGammaCorrection;
	/** @brief Enable color cycling animations. */
	bool bColorCycling;
	/** @brief Use a hardware cursor (SDL2 only). */
	bool bHardwareCursor;
	/** @brief Enable FPS Limit. */
	bool bFPSLimit;
	/** @brief Show FPS, even without the -f command line flag. */
	bool bShowFPS;
};

struct GameplayOptions {
	/** @brief Gameplay ticks per second. */
	int nTickRate;
	/** @brief Enable double walk speed when in town. */
	bool bRunInTown;
	/** @brief Do not let the mouse leave the application window. */
	bool bGrabInput;
	/** @brief Enable the Theo quest. */
	bool bTheoQuest;
	/** @brief Enable the cow quest. */
	bool bCowQuest;
	/** @brief Will players still damage other players in non-PvP mode. */
	bool bFriendlyFire;
	/** @brief Enable the bard hero class. */
	bool bTestBard;
	/** @brief Enable the babarian hero class. */
	bool bTestBarbarian;
	/** @brief Show the current level progress. */
	bool bExperienceBar;
	/** @brief Show enemy health at the top of the screen. */
	bool bEnemyHealthBar;
	/** @brief Automatically pick up gold when walking over it. */
	bool bAutoGoldPickup;
	/** @brief Recover mana when talking to Adria. */
	bool bAdriaRefillsMana;
	/** @brief Automatically attempt to equip weapon-type items when picking them up. */
	bool bAutoEquipWeapons;
	/** @brief Automatically attempt to equip armor-type items when picking them up. */
	bool bAutoEquipArmor;
	/** @brief Automatically attempt to equip helm-type items when picking them up. */
	bool bAutoEquipHelms;
	/** @brief Automatically attempt to equip shield-type items when picking them up. */
	bool bAutoEquipShields;
	/** @brief Automatically attempt to equip jewelry-type items when picking them up. */
	bool bAutoEquipJewelry;
	/** @brief Only enable 2/3 quests in each game session */
	bool bRandomizeQuests;
	/** @brief Indicates whether or not monster type (Animal, Demon, Undead) is shown along with other monster information. */
	bool bShowMonsterType;
	/** @brief Locally disable clicking on shrines which permanently cripple character. */
	bool bDisableCripplingShrines;
};

struct ControllerOptions {
	/** @brief SDL Controller mapping, see SDL_GameControllerDB. */
	char szMapping[1024];
	/** @brief Use dpad for spell hotkeys without holding "start" */
	bool bDpadHotkeys;
	/** @brief Shoulder gamepad shoulder buttons act as potions by default */
	bool bSwapShoulderButtonMode;
	/** @brief Configure gamepad joysticks deadzone */
	float fDeadzone;
#ifdef __vita__
	/** @brief Enable input via rear touchpad */
	bool bRearTouch;
#endif
};

struct NetworkOptions {
	/** @brief Optionally bind to a specific network interface. */
	char szBindAddress[129];
	/** @brief Most recently entered Hostname in join dialog. */
	char szPreviousHost[129];
	/** @brief What network port to use. */
	uint16_t nPort;
};

struct ChatOptions {
	/** @brief Quick chat messages. */
	char szHotKeyMsgs[QUICK_MESSAGE_OPTIONS][MAX_SEND_STR_LEN];
};

struct LanguageOptions {
	/** @brief Language code (IETF) for text. */
	char szCode[5];
};

struct Options {
	DiabloOptions Diablo;
	HellfireOptions Hellfire;
	AudioOptions Audio;
	GameplayOptions Gameplay;
	GraphicsOptions Graphics;
	ControllerOptions Controller;
	NetworkOptions Network;
	ChatOptions Chat;
	LanguageOptions Language;
};

bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString = "");
void setIniValue(const char *sectionName, const char *keyName, const char *value, int len = 0);

extern Options sgOptions;
extern bool sbWasOptionsLoaded;

void SaveOptions();
void LoadOptions();

} // namespace devilution
