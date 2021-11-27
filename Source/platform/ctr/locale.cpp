#include <3ds.h>

#include "platform/ctr/cfgu_service.hpp"
#include "platform/ctr/locale.hpp"

namespace devilution {
namespace n3ds {

std::string GetLocale()
{
	CFGUService cfguService;
	if (!cfguService.IsInitialized())
		return "";

	u8 language;
	Result res = CFGU_GetSystemLanguage(&language);
	if (!R_SUCCEEDED(res))
		return "";

	switch (language) {
	case CFG_LANGUAGE_JP:
		return "ja";
	case CFG_LANGUAGE_EN:
		return "en";
	case CFG_LANGUAGE_FR:
		return "fr";
	case CFG_LANGUAGE_DE:
		return "de";
	case CFG_LANGUAGE_IT:
		return "it";
	case CFG_LANGUAGE_ES:
		return "es";
	case CFG_LANGUAGE_ZH:
		return "zh_CN";
	case CFG_LANGUAGE_KO:
		return "ko_KR";
	case CFG_LANGUAGE_NL:
		return "nl";
	case CFG_LANGUAGE_PT:
		return "pt_BR";
	case CFG_LANGUAGE_RU:
		return "ru";
	case CFG_LANGUAGE_TW:
		return "zh_TW";
	default:
		return "";
	}
}

} // namespace n3ds
} // namespace devilution
