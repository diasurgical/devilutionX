#pragma once

#include <tinygettext/tinygettext.hpp>

extern std::unique_ptr<tinygettext::DictionaryManager> gDictionaryManager;

static inline std::string _(const std::string& message)
{
	if (gDictionaryManager) {
		return gDictionaryManager->get_dictionary().translate(message);
	}
	return message;
}

static inline std::string __(const std::string& message, const std::string& message_plural, int num)
{
	if (gDictionaryManager) {
		return gDictionaryManager->get_dictionary().translate_plural(message, message_plural, num);
	}

	if (num == 1) {
		return message;
	}

	return message_plural;
}
