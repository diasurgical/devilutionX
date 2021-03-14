/**
 * @file automap.h
 *
 * Interface of the in-game map overlay.
 */
#ifndef __AUTOMAP_H__
#define __AUTOMAP_H__

#include "engine.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

/** Specifies whether the automap is enabled. */
extern bool automapflag;
/** Tracks the explored areas of the map. */
extern bool automapview[DMAXX][DMAXY];
/** Specifies the scale of the automap. */
extern Sint32 AutoMapScale;
extern Sint32 AutoMapXOfs;
extern Sint32 AutoMapYOfs;
extern Sint32 AmLine64;
extern Sint32 AmLine32;
extern Sint32 AmLine16;
extern Sint32 AmLine8;
extern Sint32 AmLine4;

/**
 * @brief Initializes the automap.
 */
void InitAutomapOnce();

/**
 * @brief Loads the mapping between tile IDs and automap shapes.
 */
void InitAutomap();

/**
 * @brief Displays the automap.
 */
void StartAutomap();

/**
 * @brief Scrolls the automap upwards.
 */
void AutomapUp();

/**
 * @brief Scrolls the automap downwards.
 */
void AutomapDown();

/**
 * @brief Scrolls the automap leftwards.
 */
void AutomapLeft();

/**
 * @brief Scrolls the automap rightwards.
 */
void AutomapRight();

/**
 * @brief Increases the zoom level of the automap.
 */
void AutomapZoomIn();

/**
 * @brief Decreases the zoom level of the automap.
 */
void AutomapZoomOut();

/**
 * @brief Renders the automap to the given buffer.
 */
void DrawAutomap(CelOutputBuffer out);

/**
 * @brief Marks the given coordinate as within view on the automap.
 */
void SetAutomapView(Sint32 x, Sint32 y);

/**
 * @brief Resets the zoom level of the automap.
 */
void AutomapZoomReset();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __AUTOMAP_H__ */
