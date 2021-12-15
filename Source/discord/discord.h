#pragma once

namespace devilution {
namespace discord_manager {

#ifdef DISCORD
void UpdateGame();
void StartGame();
void UpdateMenu(bool forced = false);
#else
constexpr void UpdateGame()
{
}

constexpr void StartGame()
{
}

constexpr void UpdateMenu(bool forced = false)
{
}
#endif

} // namespace discord_manager
} // namespace devilution
