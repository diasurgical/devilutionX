#include "options.h"

namespace dvl {

void BooleanOption::Load(const std::string& sectionName)
{
	m_value = getIniBool(sectionName.c_str(), this->m_displayName.c_str(), this->m_defaultValue);
}

void BooleanOption::Save(const std::string& sectionName)
{
	setIniInt(sectionName.c_str(), this->m_displayName.c_str(), this->m_value);
}

BooleanOption& BooleanOption::operator = (bool value)
{
	m_value = value;

	return *this;
}

OptionGroup::OptionGroup(const std::string& sectionName)
    : m_sectionName(sectionName)
{
}

void OptionGroup::Load()
{
	for (OptionBase& option : m_options) {
		option.Load(m_sectionName);
	}

	for (OptionGroup& optionGroup : m_optionGroups) {
		optionGroup.Load();
	}
}

void OptionGroup::Save()
{
	for (OptionBase& option : m_options) {
		option.Save(m_sectionName);
	}

	for (OptionGroup& optionGroup : m_optionGroups) {
		optionGroup.Save();
	}
}

void OptionGroup::AddOption(std::reference_wrapper<OptionBase> option)
{
	m_options.push_back(option);
}

void OptionGroup::AddOptionGroup(std::reference_wrapper<OptionGroup> optionGroup)
{
	m_optionGroups.push_back(optionGroup);
}

AudioOptions::AudioOptions()
    : OptionGroup("Audio")
{
	AddOption(bWalkingSound);
	AddOption(bAutoEquipSound);
}

GraphicsOptions::GraphicsOptions()
    : OptionGroup("Graphics")
{
	AddOption(bFullscreen);

#ifndef __vita__
	AddOption(bUpscale);
#endif

	AddOption(bFitToScreen);
	AddOption(bIntegerScaling);
	AddOption(bVSync);
	AddOption(bBlendedTransparancy);
	AddOption(bColorCycling);
	AddOption(bFPSLimit);
}

GameplayOptions::GameplayOptions()
    : OptionGroup("Game")
{
	AddOption(bJogInTown);
	AddOption(bGrabInput);
	AddOption(bTheoQuest);
	AddOption(bCowQuest);
	AddOption(bFriendlyFire);
	AddOption(bTestBard);
	AddOption(bTestBarbarian);
	AddOption(bExperienceBar);
}

Options::Options()
    : OptionGroup("")
{
	AddOptionGroup(Audio);
	AddOptionGroup(Graphics);
	AddOptionGroup(Gameplay);
}

}
