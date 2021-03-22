#pragma once

DEVILUTION_BEGIN_NAMESPACE

typedef struct AudioOptions {
	/** @brief Movie and SFX volume. */
	Sint32 nSoundVolume;
	/** @brief Music volume. */
	Sint32 nMusicVolume;
	/** @brief Player emits sound when walking. */
	bool bWalkingSound;
	/** @brief Automatically equipping items on pickup emits the equipment sound. */
	bool bAutoEquipSound;
} AudioOptions;

typedef struct GraphicsOptions {
	/** @brief Render width. */
	Sint32 nWidth;
	/** @brief Render height. */
	Sint32 nHeight;
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
	Sint32 nGammaCorrection;
	/** @brief Enable color cycling animations. */
	bool bColorCycling;
	/** @brief Enable FPS Limit. */
	bool bFPSLimit;
} GraphicsOptions;

typedef struct GameplayOptions {
	/** @brief Game play ticks per secound. */
	Sint32 nTickRate;
	/** @brief Enable double walk speed when in town. */
	bool bJogInTown;
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
	/** @brief Automatically pick up goald when walking on to it. */
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
	/** @brief Only enable 2/3 quests in each game sessoin */
	bool bRandomizeQuests;
	/** @brief Indicates whether or not mosnter type (Animal, Demon, Undead) is shown along with other monster information. */
	bool bShowMonsterType;
} GameplayOptions;

typedef struct NetworkOptions {
	/** @brief Optionally bind to a specific network interface. */
	char szBindAddress[129];
	/** @brief What network port to use. */
	Uint16 nPort;
} NetworkOptions;

typedef struct Options {
	AudioOptions Audio;
	GameplayOptions Gameplay;
	GraphicsOptions Graphics;
	NetworkOptions Network;
} Options;

extern Options sgOptions;

DEVILUTION_END_NAMESPACE
