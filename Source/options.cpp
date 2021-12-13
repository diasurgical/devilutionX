/**
 * @file options.cpp
 *
 * Load and save options from the diablo.ini file.
 */

#include <cstdint>
#include <fstream>
#include <locale>

#ifdef __ANDROID__
#include "SDL.h"
#include <jni.h>
#endif

#ifdef __vita__
#include <psp2/apputil.h>
#include <psp2/system_param.h>
#endif

#ifdef __3DS__
#include "platform/ctr/locale.hpp"
#endif

#ifdef _WIN32
// clang-format off
#include <windows.h>
#include <winnls.h>
// clang-format on
#endif

#define SI_SUPPORT_IOSTREAMS
#include <SimpleIni.h>

#include "diablo.h"
#include "engine/demomode.h"
#include "options.h"
#include "qol/monhealthbar.h"
#include "qol/xpbar.h"
#include "utils/file_util.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/utf8.hpp"

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

#ifdef __ANDROID__
constexpr OptionEntryFlags InvisibleOnAndroid = OptionEntryFlags::Invisible;
#else
constexpr OptionEntryFlags InvisibleOnAndroid = OptionEntryFlags::None;
#endif

std::string GetIniPath()
{
	auto path = paths::ConfigPath() + std::string("diablo.ini");
	return path;
}

CSimpleIni &GetIni()
{
	static CSimpleIni ini;
	static bool isIniLoaded = false;
	if (!isIniLoaded) {
		auto path = GetIniPath();
		auto stream = CreateFileStream(path.c_str(), std::fstream::in | std::fstream::binary);
		ini.SetSpaces(false);
		if (stream)
			ini.LoadData(*stream);
		isIniLoaded = true;
	}
	return ini;
}

bool IniChanged = false;

/**
 * @brief Checks if a ini entry is changed by comparing value before and after
 */
class IniChangedChecker {
public:
	IniChangedChecker(const char *sectionName, const char *keyName)
	{
		this->sectionName_ = sectionName;
		this->keyName_ = keyName;
		std::list<CSimpleIni::Entry> values;
		if (!GetIni().GetAllValues(sectionName, keyName, values)) {
			// No entry found in original ini => new entry => changed
			IniChanged = true;
		}
		const auto *value = GetIni().GetValue(sectionName, keyName);
		if (value != nullptr)
			oldValue_ = value;
	}
	~IniChangedChecker()
	{
		const auto *value = GetIni().GetValue(sectionName_, keyName_);
		std::string newValue;
		if (value != nullptr)
			newValue = value;
		if (oldValue_ != newValue)
			IniChanged = true;
	}

private:
	std::string oldValue_;
	const char *sectionName_;
	const char *keyName_;
};

int GetIniInt(const char *keyname, const char *valuename, int defaultValue)
{
	return GetIni().GetLongValue(keyname, valuename, defaultValue);
}

bool GetIniBool(const char *sectionName, const char *keyName, bool defaultValue)
{
	return GetIni().GetBoolValue(sectionName, keyName, defaultValue);
}

float GetIniFloat(const char *sectionName, const char *keyName, float defaultValue)
{
	return (float)GetIni().GetDoubleValue(sectionName, keyName, defaultValue);
}

void SetIniValue(const char *keyname, const char *valuename, int value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value);
}

void SetIniValue(const char *keyname, const char *valuename, std::uint8_t value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value);
}

void SetIniValue(const char *keyname, const char *valuename, std::uint32_t value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value);
}

void SetIniValue(const char *keyname, const char *valuename, bool value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value ? 1 : 0);
}

void SetIniValue(const char *keyname, const char *valuename, float value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetDoubleValue(keyname, valuename, value);
}

void SaveIni()
{
	if (!IniChanged)
		return;
	auto iniPath = GetIniPath();
	auto stream = CreateFileStream(iniPath.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
	GetIni().Save(*stream, true);
	IniChanged = false;
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
bool HardwareCursorDefault()
{
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
	// See https://github.com/diasurgical/devilutionX/issues/2502
	return false;
#else
	SDL_version v;
	SDL_GetVersion(&v);
	return SDL_VERSIONNUM(v.major, v.minor, v.patch) >= SDL_VERSIONNUM(2, 0, 12);
#endif
}
#endif

void OptionGrabInputChanged()
{
#ifdef USE_SDL1
	SDL_WM_GrabInput(*sgOptions.Gameplay.grabInput ? SDL_GRAB_ON : SDL_GRAB_OFF);
#else
	if (ghMainWnd != nullptr)
		SDL_SetWindowGrab(ghMainWnd, *sgOptions.Gameplay.grabInput ? SDL_TRUE : SDL_FALSE);
#endif
}

void OptionExperienceBarChanged()
{
	if (!gbRunGame)
		return;
	if (*sgOptions.Gameplay.experienceBar)
		InitXPBar();
	else
		FreeXPBar();
}

void OptionEnemyHealthBarChanged()
{
	if (!gbRunGame)
		return;
	if (*sgOptions.Gameplay.enemyHealthBar)
		InitMonsterHealthBar();
	else
		FreeMonsterHealthBar();
}

void OptionShowFPSChanged()
{
	if (*sgOptions.Graphics.showFPS)
		EnableFrameCount();
	else
		frameflag = false;
}

void OptionLanguageCodeChanged()
{
	LanguageInitialize();
	init_language_archives();
}

void OptionGameModeChanged()
{
	gbIsHellfire = *sgOptions.StartUp.gameMode == StartUpGameMode::Hellfire;
}

void OptionSharewareChanged()
{
	gbIsSpawn = *sgOptions.StartUp.shareware;
}

} // namespace

void SetIniValue(const char *sectionName, const char *keyName, const char *value, int len)
{
	IniChangedChecker changedChecker(sectionName, keyName);
	auto &ini = GetIni();
	std::string stringValue(value, len != 0 ? len : strlen(value));
	ini.SetValue(sectionName, keyName, stringValue.c_str());
}

bool GetIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString)
{
	const char *value = GetIni().GetValue(sectionName, keyName);
	if (value == nullptr) {
		CopyUtf8(string, defaultString, stringSize);
		return false;
	}
	CopyUtf8(string, value, stringSize);
	return true;
}

/** Game options */
Options sgOptions;
bool sbWasOptionsLoaded = false;

void LoadOptions()
{
	for (OptionCategoryBase *pCategory : sgOptions.GetCategories()) {
		for (OptionEntryBase *pEntry : pCategory->GetEntries()) {
			pEntry->LoadFromIni(pCategory->GetKey());
		}
	}

	sgOptions.Diablo.lastSinglePlayerHero = GetIniInt("Diablo", "LastSinglePlayerHero", 0);
	sgOptions.Diablo.lastMultiplayerHero = GetIniInt("Diablo", "LastMultiplayerHero", 0);
	sgOptions.Hellfire.lastSinglePlayerHero = GetIniInt("Hellfire", "LastSinglePlayerHero", 0);
	sgOptions.Hellfire.lastMultiplayerHero = GetIniInt("Hellfire", "LastMultiplayerHero", 0);
	GetIniValue("Hellfire", "SItem", sgOptions.Hellfire.szItem, sizeof(sgOptions.Hellfire.szItem), "");

	sgOptions.Audio.nSoundVolume = GetIniInt("Audio", "Sound Volume", VOLUME_MAX);
	sgOptions.Audio.nMusicVolume = GetIniInt("Audio", "Music Volume", VOLUME_MAX);

	sgOptions.Audio.nSampleRate = GetIniInt("Audio", "Sample Rate", DEFAULT_AUDIO_SAMPLE_RATE);
	sgOptions.Audio.nChannels = GetIniInt("Audio", "Channels", DEFAULT_AUDIO_CHANNELS);
	sgOptions.Audio.nBufferSize = GetIniInt("Audio", "Buffer Size", DEFAULT_AUDIO_BUFFER_SIZE);
	sgOptions.Audio.nResamplingQuality = GetIniInt("Audio", "Resampling Quality", DEFAULT_AUDIO_RESAMPLING_QUALITY);

	sgOptions.Graphics.nGammaCorrection = GetIniInt("Graphics", "Gamma Correction", 100);
#if SDL_VERSION_ATLEAST(2, 0, 0)
	sgOptions.Graphics.bHardwareCursor = GetIniBool("Graphics", "Hardware Cursor", HardwareCursorDefault());
	sgOptions.Graphics.bHardwareCursorForItems = GetIniBool("Graphics", "Hardware Cursor For Items", false);
	sgOptions.Graphics.nHardwareCursorMaxSize = GetIniInt("Graphics", "Hardware Cursor Maximum Size", 128);
#endif

	sgOptions.Gameplay.nTickRate = GetIniInt("Game", "Speed", 20);

	GetIniValue("Network", "Bind Address", sgOptions.Network.szBindAddress, sizeof(sgOptions.Network.szBindAddress), "0.0.0.0");
	sgOptions.Network.nPort = GetIniInt("Network", "Port", 6112);
	GetIniValue("Network", "Previous Host", sgOptions.Network.szPreviousHost, sizeof(sgOptions.Network.szPreviousHost), "");

	for (size_t i = 0; i < QUICK_MESSAGE_OPTIONS; i++)
		GetIniValue("NetMsg", QuickMessages[i].key, sgOptions.Chat.szHotKeyMsgs[i], MAX_SEND_STR_LEN, "");

	GetIniValue("Controller", "Mapping", sgOptions.Controller.szMapping, sizeof(sgOptions.Controller.szMapping), "");
	sgOptions.Controller.bSwapShoulderButtonMode = GetIniBool("Controller", "Swap Shoulder Button Mode", false);
	sgOptions.Controller.bDpadHotkeys = GetIniBool("Controller", "Dpad Hotkeys", false);
	sgOptions.Controller.fDeadzone = GetIniFloat("Controller", "deadzone", 0.07F);
#ifdef __vita__
	sgOptions.Controller.bRearTouch = GetIniBool("Controller", "Enable Rear Touchpad", true);
#endif

	keymapper.Load();

	if (demo::IsRunning())
		demo::OverrideOptions();

	sbWasOptionsLoaded = true;
}

void SaveOptions()
{
	for (OptionCategoryBase *pCategory : sgOptions.GetCategories()) {
		for (OptionEntryBase *pEntry : pCategory->GetEntries()) {
			pEntry->SaveToIni(pCategory->GetKey());
		}
	}

	SetIniValue("Diablo", "LastSinglePlayerHero", sgOptions.Diablo.lastSinglePlayerHero);
	SetIniValue("Diablo", "LastMultiplayerHero", sgOptions.Diablo.lastMultiplayerHero);
	SetIniValue("Hellfire", "SItem", sgOptions.Hellfire.szItem);
	SetIniValue("Hellfire", "LastSinglePlayerHero", sgOptions.Hellfire.lastSinglePlayerHero);
	SetIniValue("Hellfire", "LastMultiplayerHero", sgOptions.Hellfire.lastMultiplayerHero);

	SetIniValue("Audio", "Sound Volume", sgOptions.Audio.nSoundVolume);
	SetIniValue("Audio", "Music Volume", sgOptions.Audio.nMusicVolume);

	SetIniValue("Audio", "Sample Rate", sgOptions.Audio.nSampleRate);
	SetIniValue("Audio", "Channels", sgOptions.Audio.nChannels);
	SetIniValue("Audio", "Buffer Size", sgOptions.Audio.nBufferSize);
	SetIniValue("Audio", "Resampling Quality", sgOptions.Audio.nResamplingQuality);
	SetIniValue("Graphics", "Gamma Correction", sgOptions.Graphics.nGammaCorrection);
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SetIniValue("Graphics", "Hardware Cursor", sgOptions.Graphics.bHardwareCursor);
	SetIniValue("Graphics", "Hardware Cursor For Items", sgOptions.Graphics.bHardwareCursorForItems);
	SetIniValue("Graphics", "Hardware Cursor Maximum Size", sgOptions.Graphics.nHardwareCursorMaxSize);
#endif

	SetIniValue("Game", "Speed", sgOptions.Gameplay.nTickRate);

	SetIniValue("Network", "Bind Address", sgOptions.Network.szBindAddress);
	SetIniValue("Network", "Port", sgOptions.Network.nPort);
	SetIniValue("Network", "Previous Host", sgOptions.Network.szPreviousHost);

	for (size_t i = 0; i < QUICK_MESSAGE_OPTIONS; i++)
		SetIniValue("NetMsg", QuickMessages[i].key, sgOptions.Chat.szHotKeyMsgs[i]);

	SetIniValue("Controller", "Mapping", sgOptions.Controller.szMapping);
	SetIniValue("Controller", "Swap Shoulder Button Mode", sgOptions.Controller.bSwapShoulderButtonMode);
	SetIniValue("Controller", "Dpad Hotkeys", sgOptions.Controller.bDpadHotkeys);
	SetIniValue("Controller", "deadzone", sgOptions.Controller.fDeadzone);
#ifdef __vita__
	SetIniValue("Controller", "Enable Rear Touchpad", sgOptions.Controller.bRearTouch);
#endif

	keymapper.Save();

	SaveIni();
}

string_view OptionEntryBase::GetName() const
{
	return _(name.data());
}
string_view OptionEntryBase::GetDescription() const
{
	return _(description.data());
}
OptionEntryFlags OptionEntryBase::GetFlags() const
{
	return flags;
}
void OptionEntryBase::SetValueChangedCallback(std::function<void()> callback)
{
	this->callback = callback;
}
void OptionEntryBase::NotifyValueChanged()
{
	if (callback)
		callback();
}

void OptionEntryBoolean::LoadFromIni(string_view category)
{
	value = GetIniBool(category.data(), key.data(), defaultValue);
}
void OptionEntryBoolean::SaveToIni(string_view category) const
{
	SetIniValue(category.data(), key.data(), value);
}
void OptionEntryBoolean::SetValue(bool value)
{
	this->value = value;
	this->NotifyValueChanged();
}
OptionEntryType OptionEntryBoolean::GetType() const
{
	return OptionEntryType::Boolean;
}
string_view OptionEntryBoolean::GetValueDescription() const
{
	return value ? _("ON") : _("OFF");
}

OptionEntryType OptionEntryListBase::GetType() const
{
	return OptionEntryType::List;
}
string_view OptionEntryListBase::GetValueDescription() const
{
	return GetListDescription(GetActiveListIndex());
}

void OptionEntryEnumBase::LoadFromIni(string_view category)
{
	value = GetIniInt(category.data(), key.data(), defaultValue);
}
void OptionEntryEnumBase::SaveToIni(string_view category) const
{
	SetIniValue(category.data(), key.data(), value);
}
void OptionEntryEnumBase::SetValueInternal(int value)
{
	this->value = value;
	this->NotifyValueChanged();
}
void OptionEntryEnumBase::AddEntry(int value, string_view name)
{
	entryValues.push_back(value);
	entryNames.push_back(name);
}
size_t OptionEntryEnumBase::GetListSize() const
{
	return entryValues.size();
}
string_view OptionEntryEnumBase::GetListDescription(size_t index) const
{
	return _(entryNames[index].data());
}
size_t OptionEntryEnumBase::GetActiveListIndex() const
{
	auto iterator = std::find(entryValues.begin(), entryValues.end(), value);
	if (iterator == entryValues.end())
		return 0;
	return std::distance(entryValues.begin(), iterator);
}
void OptionEntryEnumBase::SetActiveListIndex(size_t index)
{
	this->value = entryValues[index];
	this->NotifyValueChanged();
}

void OptionEntryIntBase::LoadFromIni(string_view category)
{
	value = GetIniInt(category.data(), key.data(), defaultValue);
	if (std::find(entryValues.begin(), entryValues.end(), value) == entryValues.end()) {
		entryValues.push_back(value);
		std::sort(entryValues.begin(), entryValues.end());
		entryNames.clear();
	}
}
void OptionEntryIntBase::SaveToIni(string_view category) const
{
	SetIniValue(category.data(), key.data(), value);
}
void OptionEntryIntBase::SetValueInternal(int value)
{
	this->value = value;
	this->NotifyValueChanged();
}
void OptionEntryIntBase::AddEntry(int value)
{
	entryValues.push_back(value);
}
size_t OptionEntryIntBase::GetListSize() const
{
	return entryValues.size();
}
string_view OptionEntryIntBase::GetListDescription(size_t index) const
{
	if (entryNames.empty()) {
		for (auto value : entryValues) {
			entryNames.push_back(fmt::format("{}", value));
		}
	}
	return entryNames[index].data();
}
size_t OptionEntryIntBase::GetActiveListIndex() const
{
	auto iterator = std::find(entryValues.begin(), entryValues.end(), value);
	if (iterator == entryValues.end())
		return 0;
	return std::distance(entryValues.begin(), iterator);
}
void OptionEntryIntBase::SetActiveListIndex(size_t index)
{
	this->value = entryValues[index];
	this->NotifyValueChanged();
}

OptionCategoryBase::OptionCategoryBase(string_view key, string_view name, string_view description)
    : key(key)
    , name(name)
    , description(description)
{
}
string_view OptionCategoryBase::GetKey() const
{
	return key;
}
string_view OptionCategoryBase::GetName() const
{
	return _(name.data());
}
string_view OptionCategoryBase::GetDescription() const
{
	return _(description.data());
}

StartUpOptions::StartUpOptions()
    : OptionCategoryBase("StartUp", N_("Start Up"), N_("Start Up Settings"))
    , gameMode("Game", OptionEntryFlags::NeedHellfireMpq | OptionEntryFlags::RecreateUI, N_("Game Mode"), N_("Play Diablo or Hellfire."), StartUpGameMode::Ask,
          {
              { StartUpGameMode::Diablo, N_("Diablo") },
              // Ask is missing, cause we want to hide it from UI-Settings.
              { StartUpGameMode::Hellfire, N_("Hellfire") },
          })
    , shareware("Shareware", OptionEntryFlags::NeedDiabloMpq | OptionEntryFlags::RecreateUI, N_("Restrict to Shareware"), N_("Makes the game compatible with the demo. Enables multiplayer with friends who don't own a full copy of Diablo."), false)
    , diabloIntro("Diablo Intro", OptionEntryFlags::OnlyDiablo, N_("Intro"), N_("Shown Intro cinematic."), StartUpIntro::Once,
          {
              { StartUpIntro::Off, N_("OFF") },
              // Once is missing, cause we want to hide it from UI-Settings.
              { StartUpIntro::On, N_("ON") },
          })
    , hellfireIntro("Hellfire Intro", OptionEntryFlags::OnlyHellfire, N_("Intro"), N_("Shown Intro cinematic."), StartUpIntro::Once,
          {
              { StartUpIntro::Off, N_("OFF") },
              // Once is missing, cause we want to hide it from UI-Settings.
              { StartUpIntro::On, N_("ON") },
          })
    , splash("Splash", OptionEntryFlags::None, N_("Splash"), N_("Shown splash screen."), StartUpSplash::LogoAndTitleDialog,
          {
              { StartUpSplash::LogoAndTitleDialog, N_("Logo and Title Screen") },
              { StartUpSplash::TitleDialog, N_("Title Screen") },
              { StartUpSplash::None, N_("None") },
          })
{
	gameMode.SetValueChangedCallback(OptionGameModeChanged);
	shareware.SetValueChangedCallback(OptionSharewareChanged);
}
std::vector<OptionEntryBase *> StartUpOptions::GetEntries()
{
	return {
		&gameMode,
		&shareware,
		&diabloIntro,
		&hellfireIntro,
		&splash,
	};
}

DiabloOptions::DiabloOptions()
    : OptionCategoryBase("Diablo", N_("Diablo"), N_("Diablo specific Settings"))
{
}
std::vector<OptionEntryBase *> DiabloOptions::GetEntries()
{
	return {};
}

HellfireOptions::HellfireOptions()
    : OptionCategoryBase("Hellfire", N_("Hellfire"), N_("Hellfire specific Settings"))
{
}
std::vector<OptionEntryBase *> HellfireOptions::GetEntries()
{
	return {};
}

AudioOptions::AudioOptions()
    : OptionCategoryBase("Audio", N_("Audio"), N_("Audio Settings"))
    , walkingSound("Walking Sound", OptionEntryFlags::None, N_("Walking Sound"), N_("Player emits sound when walking."), true)
    , autoEquipSound("Auto Equip Sound", OptionEntryFlags::None, N_("Auto Equip Sound"), N_("Automatically equipping items on pickup emits the equipment sound."), false)
    , itemPickupSound("Item Pickup Sound", OptionEntryFlags::None, N_("Item Pickup Sound"), N_("Picking up items emits the items pickup sound."), false)
{
}
std::vector<OptionEntryBase *> AudioOptions::GetEntries()
{
	return {
		&walkingSound,
		&autoEquipSound,
		&itemPickupSound,
	};
}

OptionEntryResolution::OptionEntryResolution()
    : OptionEntryListBase("", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::RecreateUI, N_("Resolution"), N_("Affect the game's internal resolution and determine your view area. Note: This can differ from screen resolution, when Upscaling, Integer Scaling or Fit to Screen is used."))
{
}
void OptionEntryResolution::LoadFromIni(string_view category)
{
	size = { GetIniInt(category.data(), "Width", DEFAULT_WIDTH), GetIniInt(category.data(), "Height", DEFAULT_HEIGHT) };
}
void OptionEntryResolution::SaveToIni(string_view category) const
{
	SetIniValue(category.data(), "Width", size.width);
	SetIniValue(category.data(), "Height", size.height);
}

void OptionEntryResolution::CheckResolutionsAreInitialized() const
{
	if (!resolutions.empty())
		return;

	std::vector<Size> sizes;

	// Add resolutions
#ifdef USE_SDL1
	auto *modes = SDL_ListModes(nullptr, SDL_FULLSCREEN | SDL_HWPALETTE);
	// SDL_ListModes returns -1 if any resolution is allowed (for example returned on 3DS)
	if (modes != nullptr && modes != (SDL_Rect **)-1) {
		for (size_t i = 0; modes[i] != nullptr; i++) {
			sizes.emplace_back(Size { modes[i]->w, modes[i]->h });
		}
	}
#else
	int displayModeCount = SDL_GetNumDisplayModes(0);
	for (int i = 0; i < displayModeCount; i++) {
		SDL_DisplayMode mode;
		if (SDL_GetDisplayMode(0, i, &mode) != 0) {
			ErrSdl();
		}
		sizes.emplace_back(Size { mode.w, mode.h });
	}
#endif

	// Ensures that the ini specified resolution is present in resolution list even if it doesn't match a monitor resolution (for example if played in window mode)
	sizes.push_back(this->size);
	// Ensures that the vanilla/default resolution is always present
	sizes.emplace_back(Size { DEFAULT_WIDTH, DEFAULT_HEIGHT });

	// Sort by width then by height
	std::sort(sizes.begin(), sizes.end(),
	    [](const Size &x, const Size &y) -> bool {
		    if (x.width == y.width)
			    return x.height > y.height;
		    return x.width > y.width;
	    });
	// Remove duplicate entries
	sizes.erase(std::unique(sizes.begin(), sizes.end()), sizes.end());

	for (auto &size : sizes) {
		resolutions.emplace_back(size, fmt::format("{}x{}", size.width, size.height));
	}
}

size_t OptionEntryResolution::GetListSize() const
{
	CheckResolutionsAreInitialized();
	return resolutions.size();
}
string_view OptionEntryResolution::GetListDescription(size_t index) const
{
	CheckResolutionsAreInitialized();
	return resolutions[index].second;
}
size_t OptionEntryResolution::GetActiveListIndex() const
{
	CheckResolutionsAreInitialized();
	auto found = std::find_if(resolutions.begin(), resolutions.end(), [this](const auto &x) { return x.first == this->size; });
	if (found == resolutions.end())
		return 0;
	return std::distance(resolutions.begin(), found);
}
void OptionEntryResolution::SetActiveListIndex(size_t index)
{
	size = resolutions[index].first;
	NotifyValueChanged();
}

GraphicsOptions::GraphicsOptions()
    : OptionCategoryBase("Graphics", N_("Graphics"), N_("Graphics Settings"))
    , fullscreen("Fullscreen", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::RecreateUI, N_("Fullscreen"), N_("Display the game in windowed or fullscreen mode."), true)
#if !defined(USE_SDL1) || defined(__3DS__)
    , fitToScreen("Fit to Screen", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::RecreateUI, N_("Fit to Screen"), N_("Automatically adjust the game window to your current desktop screen aspect ratio and resolution."), true)
#endif
#ifndef USE_SDL1
    , upscale("Upscale", InvisibleOnAndroid | OptionEntryFlags::CantChangeInGame | OptionEntryFlags::RecreateUI, N_("Upscale"), N_("Enables image scaling from the game resolution to your monitor resolution. Prevents changing the monitor resolution and allows window resizing."), true)
    , scaleQuality("Scaling Quality", OptionEntryFlags::None, N_("Scaling Quality"), N_("Enables optional filters to the output image when upscaling."), ScalingQuality::AnisotropicFiltering,
          {
              { ScalingQuality::NearestPixel, N_("Nearest Pixel") },
              { ScalingQuality::BilinearFiltering, N_("Bilinear") },
              { ScalingQuality::AnisotropicFiltering, N_("Anisotropic") },
          })
    , integerScaling("Integer Scaling", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::RecreateUI, N_("Integer Scaling"), N_("Scales the image using whole number pixel ratio."), false)
    , vSync("Vertical Sync", OptionEntryFlags::RecreateUI, N_("Vertical Sync"), N_("Forces waiting for Vertical Sync. Prevents tearing effect when drawing a frame. Disabling it can help with mouse lag on some systems."), true)
#endif
    , blendedTransparancy("Blended Transparency", OptionEntryFlags::CantChangeInGame, N_("Blended Transparency"), N_("Enables uniform transparency mode. This setting affects the transparency on walls, game text menus, and boxes. If disabled will default to old checkerboard transparency."), true)
    , colorCycling("Color Cycling", OptionEntryFlags::None, N_("Color Cycling"), N_("Color cycling effect used for water, lava, and acid animation."), true)
    , limitFPS("FPS Limiter", OptionEntryFlags::None, N_("FPS Limiter"), N_("FPS is limited to avoid high CPU load. Limit considers refresh rate."), true)
    , showFPS("Show FPS", OptionEntryFlags::None, N_("Show FPS"), N_("Displays the FPS in the upper left corner of the screen."), true)
{
	resolution.SetValueChangedCallback(ResizeWindow);
	fullscreen.SetValueChangedCallback(ResizeWindow);
#if !defined(USE_SDL1) || defined(__3DS__)
	fitToScreen.SetValueChangedCallback(ResizeWindow);
#endif
#ifndef USE_SDL1
	upscale.SetValueChangedCallback(ResizeWindow);
	scaleQuality.SetValueChangedCallback(ReinitializeRenderer);
	integerScaling.SetValueChangedCallback(ResizeWindow);
	vSync.SetValueChangedCallback(ReinitializeRenderer);
#endif
	showFPS.SetValueChangedCallback(OptionShowFPSChanged);
}
std::vector<OptionEntryBase *> GraphicsOptions::GetEntries()
{
	// clang-format off
	return {
		&resolution,
#ifndef __vita__
		&fullscreen,
#endif
#if !defined(USE_SDL1) || defined(__3DS__)
		&fitToScreen,
#endif
#ifndef USE_SDL1
		&upscale,
		&scaleQuality,
		&integerScaling,
		&vSync,
#endif
		&blendedTransparancy,
		&colorCycling,
		&limitFPS,
		&showFPS,
	};
	// clang-format on
}

GameplayOptions::GameplayOptions()
    : OptionCategoryBase("Game", N_("Gameplay"), N_("Gameplay Settings"))
    , runInTown("Run in Town", OptionEntryFlags::CantChangeInMultiPlayer, N_("Run in Town"), N_("Enable jogging/fast walking in town for Diablo and Hellfire. This option was introduced in the expansion."), false)
    , grabInput("Grab Input", OptionEntryFlags::None, N_("Grab Input"), N_("When enabled mouse is locked to the game window."), false)
    , theoQuest("Theo Quest", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::OnlyHellfire, N_("Theo Quest"), N_("Enable Little Girl quest."), false)
    , cowQuest("Cow Quest", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::OnlyHellfire, N_("Cow Quest"), N_("Enable Jersey's quest. Lester the farmer is replaced by the Complete Nut."), false)
    , friendlyFire("Friendly Fire", OptionEntryFlags::CantChangeInMultiPlayer, N_("Friendly Fire"), N_("Allow arrow/spell damage between players in multiplayer even when the friendly mode is on."), true)
    , testBard("Test Bard", OptionEntryFlags::CantChangeInGame, N_("Test Bard"), N_("Force the Bard character type to appear in the hero selection menu."), false)
    , testBarbarian("Test Barbarian", OptionEntryFlags::CantChangeInGame, N_("Test Barbarian"), N_("Force the Barbarian character type to appear in the hero selection menu."), false)
    , experienceBar("Experience Bar", OptionEntryFlags::None, N_("Experience Bar"), N_("Experience Bar is added to the UI at the bottom of the screen."), false)
    , enemyHealthBar("Enemy Health Bar", OptionEntryFlags::None, N_("Enemy Health Bar"), N_("Enemy Health Bar is displayed at the top of the screen."), false)
    , autoGoldPickup("Auto Gold Pickup", OptionEntryFlags::None, N_("Auto Gold Pickup"), N_("Gold is automatically collected when in close proximity to the player."), false)
    , autoElixirPickup("Auto Elixir Pickup", OptionEntryFlags::None, N_("Auto Elixir Pickup"), N_("Elixirs are automatically collected when in close proximity to the player."), false)
    , autoPickupInTown("Auto Pickup in Town", OptionEntryFlags::None, N_("Auto Pickup in Town"), N_("Automatically pickup items in town."), false)
    , adriaRefillsMana("Adria Refills Mana", OptionEntryFlags::None, N_("Adria Refills Mana"), N_("Adria will refill your mana when you visit her shop."), false)
    , autoEquipWeapons("Auto Equip Weapons", OptionEntryFlags::None, N_("Auto Equip Weapons"), N_("Weapons will be automatically equipped on pickup or purchase if enabled."), true)
    , autoEquipArmor("Auto Equip Armor", OptionEntryFlags::None, N_("Auto Equip Armor"), N_("Armor will be automatically equipped on pickup or purchase if enabled."), false)
    , autoEquipHelms("Auto Equip Helms", OptionEntryFlags::None, N_("Auto Equip Helms"), N_("Helms will be automatically equipped on pickup or purchase if enabled."), false)
    , autoEquipShields("Auto Equip Shields", OptionEntryFlags::None, N_("Auto Equip Shields"), N_("Shields will be automatically equipped on pickup or purchase if enabled."), false)
    , autoEquipJewelry("Auto Equip Jewelry", OptionEntryFlags::None, N_("Auto Equip Jewelry"), N_("Jewelry will be automatically equipped on pickup or purchase if enabled."), false)
    , randomizeQuests("Randomize Quests", OptionEntryFlags::CantChangeInGame, N_("Randomize Quests"), N_("Randomly selecting available quests for new games."), true)
    , showMonsterType("Show Monster Type", OptionEntryFlags::None, N_("Show Monster Type"), N_("Hovering over a monster will display the type of monster in the description box in the UI."), false)
    , autoRefillBelt("Auto Refill Belt", OptionEntryFlags::None, N_("Auto Refill Belt"), N_("Refill belt from inventory when belt item is consumed."), false)
    , disableCripplingShrines("Disable Crippling Shrines", OptionEntryFlags::None, N_("Disable Crippling Shrines"), N_("When enabled Cauldrons, Fascinating Shrines, Goat Shrines, Ornate Shrines and Sacred Shrines are not able to be clicked on and labeled as disabled."), false)
    , quickCast("Quick Cast", OptionEntryFlags::None, N_("Quick Cast"), N_("Spell hotkeys instantly cast the spell, rather than switching the readied spell."), false)
    , numHealPotionPickup("Heal Potion Pickup", OptionEntryFlags::None, N_("Heal Potion Pickup"), N_("Number of Healing potions to pick up automatically."), 0, { 0, 1, 4, 8, 16 })
    , numFullHealPotionPickup("Full Heal Potion Pickup", OptionEntryFlags::None, N_("Full Heal Potion Pickup"), N_("Number of Full Healing potions to pick up automatically."), 0, { 0, 1, 4, 8, 16 })
    , numManaPotionPickup("Mana Potion Pickup", OptionEntryFlags::None, N_("Mana Potion Pickup"), N_("Number of Mana potions to pick up automatically."), 0, { 0, 1, 4, 8, 16 })
    , numFullManaPotionPickup("Full Mana Potion Pickup", OptionEntryFlags::None, N_("Full Mana Potion Pickup"), N_("Number of Mana potions to pick up automatically."), 0, { 0, 1, 4, 8, 16 })
    , numRejuPotionPickup("Rejuvenation Potion Pickup", OptionEntryFlags::None, N_("Heal Rejuvenation Pickup"), N_("Number of Rejuvenation potions to pick up automatically."), 0, { 0, 1, 4, 8, 16 })
    , numFullRejuPotionPickup("Full Rejuvenation Potion Pickup", OptionEntryFlags::None, N_("Full Rejuvenation Potion Pickup"), N_("Number of Rejuvenation potions to pick up automatically."), 0, { 0, 1, 4, 8, 16 })
{
	grabInput.SetValueChangedCallback(OptionGrabInputChanged);
	experienceBar.SetValueChangedCallback(OptionExperienceBarChanged);
	enemyHealthBar.SetValueChangedCallback(OptionEnemyHealthBarChanged);
}
std::vector<OptionEntryBase *> GameplayOptions::GetEntries()
{
	return {
		&runInTown,
		&grabInput,
		&theoQuest,
		&cowQuest,
		&friendlyFire,
		&testBard,
		&testBarbarian,
		&experienceBar,
		&enemyHealthBar,
		&autoGoldPickup,
		&autoElixirPickup,
		&autoPickupInTown,
		&adriaRefillsMana,
		&autoEquipWeapons,
		&autoEquipArmor,
		&autoEquipHelms,
		&autoEquipShields,
		&autoEquipJewelry,
		&randomizeQuests,
		&showMonsterType,
		&autoRefillBelt,
		&disableCripplingShrines,
		&quickCast,
		&numHealPotionPickup,
		&numFullHealPotionPickup,
		&numManaPotionPickup,
		&numFullManaPotionPickup,
		&numRejuPotionPickup,
		&numFullRejuPotionPickup,
	};
}

ControllerOptions::ControllerOptions()
    : OptionCategoryBase("Controller", N_("Controller"), N_("Controller Settings"))
{
}
std::vector<OptionEntryBase *> ControllerOptions::GetEntries()
{
	return {};
}

NetworkOptions::NetworkOptions()
    : OptionCategoryBase("Network", N_("Network"), N_("Network Settings"))
{
}
std::vector<OptionEntryBase *> NetworkOptions::GetEntries()
{
	return {};
}

ChatOptions::ChatOptions()
    : OptionCategoryBase("NetMsg", N_("Chat"), N_("Chat Settings"))
{
}
std::vector<OptionEntryBase *> ChatOptions::GetEntries()
{
	return {};
}

OptionEntryLanguageCode::OptionEntryLanguageCode()
    : OptionEntryListBase("Code", OptionEntryFlags::CantChangeInGame | OptionEntryFlags::RecreateUI, N_("Language"), N_("Define what language to use in game."))
{
}
void OptionEntryLanguageCode::LoadFromIni(string_view category)
{
#ifdef __ANDROID__
	JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
	jobject activity = (jobject)SDL_AndroidGetActivity();
	jclass clazz(env->GetObjectClass(activity));
	jmethodID method_id = env->GetMethodID(clazz, "getLocale", "()Ljava/lang/String;");
	jstring jLocale = (jstring)env->CallObjectMethod(activity, method_id);
	const char *cLocale = env->GetStringUTFChars(jLocale, nullptr);
	std::string locale = cLocale;
	env->ReleaseStringUTFChars(jLocale, cLocale);
	env->DeleteLocalRef(jLocale);
	env->DeleteLocalRef(activity);
	env->DeleteLocalRef(clazz);
#elif defined(__vita__)
	int32_t language = SCE_SYSTEM_PARAM_LANG_ENGLISH_US; // default to english
	const char *vita_locales[] = {
		"ja_JP",
		"en_US",
		"fr_FR",
		"es_ES",
		"de_DE",
		"it_IT",
		"nl_NL",
		"pt_PT",
		"ru_RU",
		"ko_KR",
		"zh_TW",
		"zh_CN",
		"fi_FI",
		"sv_SE",
		"da_DK",
		"no_NO",
		"pl_PL",
		"pt_BR",
		"en_GB",
		"tr_TR",
	};
	SceAppUtilInitParam initParam;
	SceAppUtilBootParam bootParam;
	memset(&initParam, 0, sizeof(SceAppUtilInitParam));
	memset(&bootParam, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&initParam, &bootParam);
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &language);
	if (language < 0 || language > SCE_SYSTEM_PARAM_LANG_TURKISH)
		language = SCE_SYSTEM_PARAM_LANG_ENGLISH_US; // default to english
	std::string locale = std::string(vita_locales[language]);
	sceAppUtilShutdown();
#elif defined(__3DS__)
	std::string locale = n3ds::GetLocale();
#elif defined(_WIN32)
	std::string locale;

#if WINVER >= 0x0600
	WCHAR localeBuffer[LOCALE_NAME_MAX_LENGTH];
	if (GetUserDefaultLocaleName(localeBuffer, LOCALE_NAME_MAX_LENGTH) != 0) {
		// The user default locale could be loaded, we need to convert from WIN32's default of UTF16 to UTF8
		char utf8Buffer[12] {};
		// We only handle 5 character locales (lang2-region2), so don't bother reading past that. This does leave the resulting string unterminated but the buffer was zero initialised anyway.
		WideCharToMultiByte(CP_UTF8, 0, localeBuffer, 5, utf8Buffer, 12, nullptr, nullptr);

		// GetUserDefaultLocaleName could return an ISO 639-2/T string (three letter language code) or even an arbitrary custom locale, however we only handle 639-1 (two letter language code) locale names when checking the fallback language.
		if (utf8Buffer[2] == '-') {
			// if a region is included in the locale do a simple transformation to the expected POSIX style.
			utf8Buffer[2] = '_';
		}

		locale.append(utf8Buffer);
	}
#else
	// Fallback method for older versions of windows, this is deprecated since Vista
	char localeBuffer[LOCALE_NAME_MAX_LENGTH];
	// Deliberately not using the unicode versions here as the information retrieved should be represented in ASCII/single byte UTF8 codepoints.
	if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, localeBuffer, LOCALE_NAME_MAX_LENGTH) != 0) {
		locale.append(localeBuffer);
		if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, localeBuffer, LOCALE_NAME_MAX_LENGTH) != 0) {
			locale.append("_");
			locale.append(localeBuffer);
		}
	}
#endif
#else
	std::string locale = std::locale("").name().substr(0, 5);
#endif

	locale = locale.substr(0, 5);
	LogVerbose("Preferred locale: {}", locale);
	if (!HasTranslation(locale)) {
		locale = locale.substr(0, 2);
		if (!HasTranslation(locale)) {
			locale = "en";
		}
	}
	LogVerbose("Best match locale: {}", locale);

	GetIniValue(category.data(), key.data(), szCode, sizeof(szCode), locale.c_str());
}
void OptionEntryLanguageCode::SaveToIni(string_view category) const
{
	SetIniValue(category.data(), key.data(), szCode);
}

void OptionEntryLanguageCode::CheckLanguagesAreInitialized() const
{
	if (!languages.empty())
		return;

	// Add well-known supported languages
	languages.emplace_back("bg", "Bulgarian");
	languages.emplace_back("cs", "Czech");
	languages.emplace_back("da", "Danish");
	languages.emplace_back("de", "German");
	languages.emplace_back("en", "English");
	languages.emplace_back("es", "Spanish");
	languages.emplace_back("fr", "French");
	languages.emplace_back("ja", "Japanese");
	languages.emplace_back("hr", "Croatian");
	languages.emplace_back("it", "Italian");
	languages.emplace_back("ko_KR", "Korean");
	languages.emplace_back("pl", "Polish");
	languages.emplace_back("pt_BR", "Portuguese (Brazil)");
	languages.emplace_back("ro_RO", "Romanian");
	languages.emplace_back("ru", "Russian");
	languages.emplace_back("sv", "Swedish");
	languages.emplace_back("uk", "Ukrainian");
	languages.emplace_back("zh_CN", "Simplified Chinese");
	languages.emplace_back("zh_TW", "Traditional Chinese");

	// Ensures that the ini specified language is present in languages list even if unknown (for example if someone starts to translate a new language)
	if (std::find_if(languages.begin(), languages.end(), [this](const auto &x) { return x.first == this->szCode; }) == languages.end()) {
		languages.emplace_back(szCode, szCode);
	}
}

size_t OptionEntryLanguageCode::GetListSize() const
{
	CheckLanguagesAreInitialized();
	return languages.size();
}
string_view OptionEntryLanguageCode::GetListDescription(size_t index) const
{
	CheckLanguagesAreInitialized();
	return languages[index].second;
}
size_t OptionEntryLanguageCode::GetActiveListIndex() const
{
	CheckLanguagesAreInitialized();
	auto found = std::find_if(languages.begin(), languages.end(), [this](const auto &x) { return x.first == this->szCode; });
	if (found == languages.end())
		return 0;
	return std::distance(languages.begin(), found);
}
void OptionEntryLanguageCode::SetActiveListIndex(size_t index)
{
	strcpy(szCode, languages[index].first.c_str());
	NotifyValueChanged();
}

LanguageOptions::LanguageOptions()
    : OptionCategoryBase("Language", N_("Language"), N_("Language Settings"))
{
	code.SetValueChangedCallback(OptionLanguageCodeChanged);
}
std::vector<OptionEntryBase *> LanguageOptions::GetEntries()
{
	return {
		&code,
	};
}

} // namespace devilution
