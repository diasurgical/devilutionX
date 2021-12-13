#pragma once

namespace devilution {
namespace discord_manager {

#ifdef DISCORD
void UpdateGame();
void StartGame();
void UpdateMenu();
#else
constexpr void UpdateGame()
{
}

constexpr void StartGame()
{
}

constexpr void UpdateMenu()
{
}
#endif

} // namespace discord_manager
} // namespace devilution
