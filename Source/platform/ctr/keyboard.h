#pragma once

#include <string_view>

#include <3ds.h>

/**
 * @brief Queues a request for user input for the next call to ctr_vkbdFlush()
 * @see ctr_vkdbFlush()
 * @param title Label for the input
 * @param inText Optional text to prefil the input field
 * @param outText Pointer to a buffer to receive user input
 * @param maxLength Size of the buffer
 */
void ctr_vkbdInput(std::string_view title, std::string_view inText, char *outText, size_t maxLength);

/**
 * @brief Processes pending requests for user input
 */
void ctr_vkbdFlush();
