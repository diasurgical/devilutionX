#pragma once

#include <memory>
#include <string>
#include <vector>
#include "all.h"
#include "SDL_stdinc.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

/**
 * @brief Base non-templated class for all options. Provides saving and loading functionality and a way to group every
 * type of option together.
*/
class OptionBase {
private:
	/**
	 * @brief Loads data from the ini into this instance.
	 * @param sectionName The name of the section that contains the data for this option.
	*/
	virtual void Load(const std::string& sectionName) = 0;

	/**
	 * @brief Saves the data contained in this instance to the appropriate section in the ini.
	 * @param sectionName The name of the section where the data from this instance will be saved to.
	*/
	virtual void Save(const std::string& sectionName) = 0;

	friend class OptionGroup;
};

/**
 * @brief Base templated class for all options. Stores information about a given option, including its current value,
 * default value and the name of the key inside the ini file that corresponds to it.
 * @tparam T The underlying type of the setting.
*/
template <typename T>
class Option : public OptionBase {
public:
	/**
	 * @brief Initializes a new instance of the Option<T> with the specified display name and default value.
	 * @param displayName The name to use when saving and loading the option to ini.
	 * @param defaultValue The default value to use when the option does not yet exist.
	*/
	Option(const std::string& displayName, const T& defaultValue)
	    : m_value(0)
	    , m_displayName(displayName)
	    , m_defaultValue(defaultValue)
	{
	}

	/**
	 * @brief Extracts the underlying value from this option instance.
	 * @return The underlying value of the option.
	*/
	const T& operator*() const
	{
		return m_value;
	}

protected:
	T m_value;
	std::string m_displayName;
	T m_defaultValue;
};

/**
 * @brief A class that models a boolean option.
*/
class BooleanOption final : public Option<bool> {
	using Option<bool>::Option;

public:
	/**
	 * @brief Sets the underlying value of this option instance.
	 * @param value The new value to be set as the current value of this option.
	 * @return This instance with the new value set.
	*/
	BooleanOption& operator=(bool value);

private:
	void Load(const std::string &sectionName);
	void Save(const std::string &sectionName);
};

/**
 * @brief A class that models a group of options persisted in an ini section.
*/
class OptionGroup {
public:
	/**
	 * @brief Initializes a new instance of the option group with the specified section name.
	 * @param sectionName The name of the ini section used to read and write values.
	*/
	OptionGroup(const std::string& sectionName);

	/**
	 * @brief Loads the data from the ini file into this option group.
	*/
	void Load();

	/**
	 * @brief Saves the data stored in this option group to the ini file.
	*/
	void Save();

protected:
	void AddOption(std::reference_wrapper<OptionBase>);
	void AddOptionGroup(std::reference_wrapper<OptionGroup>);

private:
	std::vector<std::reference_wrapper<OptionBase>> m_options;
	std::vector<std::reference_wrapper<OptionGroup>> m_optionGroups;
	std::string m_sectionName;
};

class AudioOptions final : public OptionGroup {
public:
	AudioOptions();

	/** @brief Movie and SFX volume. */
	Sint32 nSoundVolume;
	/** @brief Music volume. */
	Sint32 nMusicVolume;
	/** @brief Player emits sound when walking. */
	BooleanOption bWalkingSound = { "Walking Sound", true };
	/** @brief Automatically equipping items on pickup emits the equipment sound. */
	bool bAutoEquipSound;
};

class GraphicsOptions final {
public:
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
};

class GameplayOptions final {
public:
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
};

class NetworkOptions final {
public:
	/** @brief Optionally bind to a specific network interface. */
	char szBindAddress[129];
	/** @brief What network port to use. */
	Uint16 nPort;
};

class Options final : public OptionGroup {
public:
	Options();

	AudioOptions Audio;
	GameplayOptions Gameplay;
	GraphicsOptions Graphics;
	NetworkOptions Network;
};

extern Options sgOptions;

DEVILUTION_END_NAMESPACE
