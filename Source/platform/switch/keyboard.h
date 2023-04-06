#pragma once

#include "utils/stdcompat/string_view.hpp"

/**
 * @brief Prompts the user for text input, pushes the user-provided text to the event queue, then returns
 * @param guide_text Hint text to display to the user if the input is empty
 * @param initial_text An optional prefilled value for the input
 * @param max_length How many bytes of input to accept from the user (certain characters will take multiple bytes)
 */
void switch_start_text_input(devilution::string_view guide_text, devilution::string_view initial_text, unsigned max_length);
