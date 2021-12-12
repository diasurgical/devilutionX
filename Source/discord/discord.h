#pragma once

#ifdef DISCORD

namespace devilution {
namespace discord_manager {
void UpdateGame();
void StartGame();
void UpdateMenu();
} // namespace discord_manager
} // namespace devilution

#else
namespace devilution {
namespace discord_manager {

constexpr void UpdateGame()
{
}

constexpr void StartGame()
{
}

constexpr void UpdateMenu()
{
}

} // namespace discord_manager
} // namespace devilution
#endif
