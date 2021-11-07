#pragma once

#include <cstdint>

#include <SDL_version.h>

#include "pack.h"
#include "utils/enum_traits.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum class StartUpGameOption {
	None,
	Hellfire,
	Diablo,
};

enum class ScalingQuality {
	NearestPixel,
	BilinearFiltering,
	AnisotropicFiltering,
};

enum class OptionEntryType {
	Boolean,
	List,
};

enum class OptionEntryFlags {
	/** @brief No special logic. */
	None = 0,
	/** @brief Shouldn't be shown in settings dialog. */
	Invisible = 1 << 0,
	/** @brief Need to restart the current running game (single- or multiplayer) to take effect. */
	CantChangeInGame = 1 << 1,
	/** @brief Need to restart the current running multiplayer game to take effect. */
	CantChangeInMultiPlayer = 1 << 2,
	/** @brief Option is only relevant for Hellfire. */
	OnlyHellfire = 1 << 3,
	/** @brief Option is only relevant for Diablo. */
	OnlyDiablo = 1 << 4,
};
use_enum_as_flags(OptionEntryFlags);

class OptionEntryBase {
public:
	OptionEntryBase(string_view key, OptionEntryFlags flags, string_view name, string_view description)
	    : key(key)
	    , flags(flags)
	    , name(name)
	    , description(description)
	{
	}
	[[nodiscard]] string_view GetName() const;
	[[nodiscard]] string_view GetDescription() const;
	[[nodiscard]] virtual OptionEntryType GetType() const = 0;
	[[nodiscard]] OptionEntryFlags GetFlags() const;

	void SetValueChangedCallback(std::function<void()> callback);

	[[nodiscard]] virtual string_view GetValueDescription() const = 0;
	virtual void LoadFromIni(string_view category) = 0;
	virtual void SaveToIni(string_view category) const = 0;

protected:
	string_view key;
	string_view name;
	string_view description;
	OptionEntryFlags flags;
	void NotifyValueChanged();

private:
	std::function<void()> callback;
};

class OptionEntryBoolean : public OptionEntryBase {
public:
	OptionEntryBoolean(string_view key, OptionEntryFlags flags, string_view name, string_view description, bool defaultValue)
	    : OptionEntryBase(key, flags, name, description)
	    , defaultValue(defaultValue)
	    , value(defaultValue)
	{
	}
	[[nodiscard]] bool operator*() const;
	void SetValue(bool value);

	[[nodiscard]] OptionEntryType GetType() const override;
	[[nodiscard]] string_view GetValueDescription() const override;
	void LoadFromIni(string_view category) override;
	void SaveToIni(string_view category) const override;

private:
	bool value;
	bool defaultValue;
};

class OptionEntryListBase : public OptionEntryBase {
public:
	[[nodiscard]] virtual size_t GetListSize() const = 0;
	[[nodiscard]] virtual string_view GetListDescription(size_t index) const = 0;
	[[nodiscard]] virtual size_t GetActiveListIndex() const = 0;
	virtual void SetActiveListIndex(size_t index) = 0;

	[[nodiscard]] OptionEntryType GetType() const override;
	[[nodiscard]] string_view GetValueDescription() const override;

protected:
	OptionEntryListBase(string_view key, OptionEntryFlags flags, string_view name, string_view description)
	    : OptionEntryBase(key, flags, name, description)
	{
	}
};

class OptionEntryEnumBase : public OptionEntryListBase {
public:
	void LoadFromIni(string_view category) override;
	void SaveToIni(string_view category) const override;

	[[nodiscard]] virtual size_t GetListSize() const override;
	[[nodiscard]] virtual string_view GetListDescription(size_t index) const override;
	[[nodiscard]] size_t GetActiveListIndex() const override;
	void SetActiveListIndex(size_t index) override;

protected:
	OptionEntryEnumBase(string_view key, OptionEntryFlags flags, string_view name, string_view description, int defaultValue)
	    : OptionEntryListBase(key, flags, name, description)
	    , defaultValue(defaultValue)
	    , value(defaultValue)
	{
	}

	[[nodiscard]] int GetValueInternal() const;

	void AddEntry(int value, string_view name);

private:
	int value;
	int defaultValue;
	std::vector<string_view> entryNames;
	std::vector<int> entryValues;
};

template <typename T>
class OptionEntryEnum : public OptionEntryEnumBase {
public:
	OptionEntryEnum(string_view key, OptionEntryFlags flags, string_view name, string_view description, T defaultValue, std::initializer_list<std::pair<T, string_view>> entries)
	    : OptionEntryEnumBase(key, flags, name, description, static_cast<int>(defaultValue))
	{
		for (auto entry : entries) {
			AddEntry(static_cast<int>(entry.first), entry.second);
		}
	}
	[[nodiscard]] T operator*() const
	{
		return static_cast<T>(GetValueInternal());
	}
};

struct OptionCategoryBase {
	OptionCategoryBase(string_view key, string_view name, string_view description);

	[[nodiscard]] string_view GetKey() const;
	[[nodiscard]] string_view GetName() const;
	[[nodiscard]] string_view GetDescription() const;

	virtual std::vector<OptionEntryBase *> GetEntries() = 0;

protected:
	string_view key;
	string_view name;
	string_view description;
};

struct DiabloOptions : OptionCategoryBase {
	DiabloOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Play game intro video on startup. */
	bool bIntro;
	/** @brief Remembers what singleplayer hero/save was last used. */
	std::uint32_t lastSinglePlayerHero;
	/** @brief Remembers what multiplayer hero/save was last used. */
	std::uint32_t lastMultiplayerHero;
};

struct HellfireOptions : OptionCategoryBase {
	HellfireOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Play game intro video on startup. */
	bool bIntro;
	/** @brief Cornerstone of the world item. */
	char szItem[sizeof(ItemPack) * 2 + 1];
	/** @brief Remembers what singleplayer hero/save was last used. */
	std::uint32_t lastSinglePlayerHero;
	/** @brief Remembers what multiplayer hero/save was last used. */
	std::uint32_t lastMultiplayerHero;

	StartUpGameOption startUpGameOption;
};

struct AudioOptions : OptionCategoryBase {
	AudioOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Movie and SFX volume. */
	int nSoundVolume;
	/** @brief Music volume. */
	int nMusicVolume;
	/** @brief Player emits sound when walking. */
	OptionEntryBoolean walkingSound;
	/** @brief Automatically equipping items on pickup emits the equipment sound. */
	OptionEntryBoolean autoEquipSound;
	/** @brief Picking up items emits the items pickup sound. */
	OptionEntryBoolean itemPickupSound;

	/** @brief Output sample rate (Hz) */
	std::uint32_t nSampleRate;
	/** @brief The number of output channels (1 or 2) */
	std::uint8_t nChannels;
	/** @brief Buffer size (number of frames per channel) */
	std::uint32_t nBufferSize;
	/** @brief Quality of the resampler, from 0 (lowest) to 10 (highest) */
	std::uint8_t nResamplingQuality;
};

struct GraphicsOptions : OptionCategoryBase {
	GraphicsOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

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
	OptionEntryEnum<ScalingQuality> scaleQuality;
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
#if SDL_VERSION_ATLEAST(2, 0, 0)
	/** @brief Use a hardware cursor (SDL2 only). */
	bool bHardwareCursor;
	/** @brief Use a hardware cursor for items. */
	bool bHardwareCursorForItems;
	/** @brief Maximum width / height for the hardware cursor. Larger cursors fall back to software. */
	int nHardwareCursorMaxSize;
#endif
	/** @brief Enable FPS Limit. */
	bool bFPSLimit;
	/** @brief Show FPS, even without the -f command line flag. */
	bool bShowFPS;
};

struct GameplayOptions : OptionCategoryBase {
	GameplayOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Gameplay ticks per second. */
	int nTickRate;
	/** @brief Enable double walk speed when in town. */
	OptionEntryBoolean runInTown;
	/** @brief Do not let the mouse leave the application window. */
	OptionEntryBoolean grabInput;
	/** @brief Enable the Theo quest. */
	OptionEntryBoolean theoQuest;
	/** @brief Enable the cow quest. */
	OptionEntryBoolean cowQuest;
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
	/** @brief Refill belt form inventory, or rather, use potions/scrolls from inventory first when belt item is consumed.  */
	bool bAutoRefillBelt;
	/** @brief Locally disable clicking on shrines which permanently cripple character. */
	bool bDisableCripplingShrines;
};

struct ControllerOptions : OptionCategoryBase {
	ControllerOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

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

struct NetworkOptions : OptionCategoryBase {
	NetworkOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Optionally bind to a specific network interface. */
	char szBindAddress[129];
	/** @brief Most recently entered Hostname in join dialog. */
	char szPreviousHost[129];
	/** @brief What network port to use. */
	uint16_t nPort;
};

struct ChatOptions : OptionCategoryBase {
	ChatOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Quick chat messages. */
	char szHotKeyMsgs[QUICK_MESSAGE_OPTIONS][MAX_SEND_STR_LEN];
};

struct LanguageOptions : OptionCategoryBase {
	LanguageOptions();
	std::vector<OptionEntryBase *> GetEntries() override;

	/** @brief Language code (IETF) for text. */
	char szCode[6];
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

	[[nodiscard]] std::vector<OptionCategoryBase *> GetCategories()
	{
		return {
			&Diablo,
			&Hellfire,
			&Audio,
			&Gameplay,
			&Graphics,
			&Controller,
			&Network,
			&Chat,
			&Language,
		};
	}
};

bool GetIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString = "");
void SetIniValue(const char *sectionName, const char *keyName, const char *value, int len = 0);

extern Options sgOptions;
extern bool sbWasOptionsLoaded;

/**
 * @brief Save game configurations to ini file
 */
void SaveOptions();

/**
 * @brief Load game configurations from ini file
 */
void LoadOptions();

} // namespace devilution
