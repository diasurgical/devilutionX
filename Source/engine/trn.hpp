/**
 * @file trn.h
 *
 * Contains most of trn logic
 */
#pragma once

namespace devilution {

uint8_t *GetTRN(const char *path);

uint8_t *GetUniqueMonsterTRN(const char *path);

uint8_t *GetInfravisionTRN();

uint8_t *GetStoneTRN();

} // namespace devilution
