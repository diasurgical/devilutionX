#include "DiabloUI/hub/friends.h"

#include "DiabloUI/hub/hub.h"
#include "engine/load_pcx.hpp"

namespace devilution {

namespace {

} // namespace

void HubLoadFriends()
{
	Layout = LoadPcx("ui_art\\bnselchn", /*transparentColor=*/0);
}

void HubInitFriends()
{
	const Point uiPosition = GetUIRectangle().position;
}

} // namespace devilution
