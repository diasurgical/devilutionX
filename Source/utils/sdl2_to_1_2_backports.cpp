#include "./sdl2_to_1_2_backports.h"

#include <cstddef>

#include "./console.h"

#define DEFAULT_PRIORITY SDL_LOG_PRIORITY_CRITICAL
#define DEFAULT_ASSERT_PRIORITY SDL_LOG_PRIORITY_WARN
#define DEFAULT_APPLICATION_PRIORITY SDL_LOG_PRIORITY_INFO
#define DEFAULT_TEST_PRIORITY SDL_LOG_PRIORITY_VERBOSE

namespace {

// We use the same names of these structs as the SDL2 implementation:
// NOLINTNEXTLINE(readability-identifier-naming)
struct SDL_LogLevel {
	int category;
	SDL_LogPriority priority;
	SDL_LogLevel *next;
};

SDL_LogLevel *SDL_loglevels;                                             // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_default_priority = DEFAULT_PRIORITY;                 // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_assert_priority = DEFAULT_ASSERT_PRIORITY;           // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_application_priority = DEFAULT_APPLICATION_PRIORITY; // NOLINT(readability-identifier-naming)
SDL_LogPriority SDL_test_priority = DEFAULT_TEST_PRIORITY;               // NOLINT(readability-identifier-naming)

// NOLINTNEXTLINE(readability-identifier-naming)
const char *const SDL_priority_prefixes[SDL_NUM_LOG_PRIORITIES] = {
	nullptr,
	"VERBOSE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"CRITICAL"
};

} // namespace

void SDL_LogSetAllPriority(SDL_LogPriority priority)
{
	for (SDL_LogLevel *entry = SDL_loglevels; entry != nullptr; entry = entry->next) {
		entry->priority = priority;
	}
	SDL_default_priority = priority;
	SDL_assert_priority = priority;
	SDL_application_priority = priority;
}

void SDL_LogSetPriority(int category, SDL_LogPriority priority)
{
	SDL_LogLevel *entry;
	for (entry = SDL_loglevels; entry != nullptr; entry = entry->next) {
		if (entry->category == category) {
			entry->priority = priority;
			return;
		}
	}

	entry = static_cast<SDL_LogLevel *>(SDL_malloc(sizeof(*entry)));
	if (entry != nullptr) {
		entry->category = category;
		entry->priority = priority;
		entry->next = SDL_loglevels;
		SDL_loglevels = entry;
	}
}

SDL_LogPriority SDL_LogGetPriority(int category)
{
	for (SDL_LogLevel *entry = SDL_loglevels; entry != nullptr; entry = entry->next) {
		if (entry->category == category) {
			return entry->priority;
		}
	}

	switch (category) {
	case SDL_LOG_CATEGORY_TEST:
		return SDL_test_priority;
	case SDL_LOG_CATEGORY_APPLICATION:
		return SDL_application_priority;
	case SDL_LOG_CATEGORY_ASSERT:
		return SDL_assert_priority;
	default:
		return SDL_default_priority;
	}
}

void SDL_Log(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, ap);
	va_end(ap);
}

void SDL_LogVerbose(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_VERBOSE, fmt, ap);
	va_end(ap);
}

void SDL_LogDebug(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_DEBUG, fmt, ap);
	va_end(ap);
}

void SDL_LogInfo(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_INFO, fmt, ap);
	va_end(ap);
}

void SDL_LogWarn(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_WARN, fmt, ap);
	va_end(ap);
}

void SDL_LogError(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_ERROR, fmt, ap);
	va_end(ap);
}

void SDL_LogCritical(int category, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, SDL_LOG_PRIORITY_CRITICAL, fmt, ap);
	va_end(ap);
}

void SDL_LogMessage(int category, SDL_LogPriority priority, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	SDL_LogMessageV(category, priority, fmt, ap);
	va_end(ap);
}

void SDL_LogMessageV(int category, SDL_LogPriority priority, const char *fmt, va_list ap)
{
	if (static_cast<int>(priority) < 0 || priority >= SDL_NUM_LOG_PRIORITIES || priority < SDL_LogGetPriority(category))
		return;

	::devilution::printInConsole("%s: ", SDL_priority_prefixes[priority]);
	::devilution::printInConsoleV(fmt, ap);
	::devilution::printInConsole("\n");
}

namespace {

#define DEFINE_COPY_ROW(name, type)                       \
	void name(type *src, int src_w, type *dst, int dst_w) \
	{                                                     \
		int i;                                            \
		int pos, inc;                                     \
		type pixel = 0;                                   \
                                                          \
		pos = 0x10000;                                    \
		inc = (src_w << 16) / dst_w;                      \
		for (i = dst_w; i > 0; --i) {                     \
			while (pos >= 0x10000L) {                     \
				pixel = *src++;                           \
				pos -= 0x10000L;                          \
			}                                             \
			*dst++ = pixel;                               \
			pos += inc;                                   \
		}                                                 \
	}
DEFINE_COPY_ROW(copy_row1, Uint8)
DEFINE_COPY_ROW(copy_row2, Uint16)
DEFINE_COPY_ROW(copy_row4, Uint32)

void copy_row3(Uint8 *src, int src_w, Uint8 *dst, int dst_w)
{
	int i;
	int pos, inc;
	Uint8 pixel[3] = { 0, 0, 0 };

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for (i = dst_w; i > 0; --i) {
		while (pos >= 0x10000L) {
			pixel[0] = *src++;
			pixel[1] = *src++;
			pixel[2] = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel[0];
		*dst++ = pixel[1];
		*dst++ = pixel[2];
		pos += inc;
	}
}

} // namespace

int SDL_SoftStretch(SDL_Surface *src, const SDL_Rect *srcrect,
    SDL_Surface *dst, const SDL_Rect *dstrect)
{
	// All the ASM support has been removed, as the platforms that the ASM
	// implementation exists for support SDL2 anyway.
	int src_locked;
	int dst_locked;
	int pos, inc;
	int dst_maxrow;
	int src_row, dst_row;
	Uint8 *srcp = NULL;
	Uint8 *dstp;
	SDL_Rect full_src;
	SDL_Rect full_dst;
	const int bpp = dst->format->BytesPerPixel;

	if (!SDLBackport_PixelFormatFormatEq(src->format, dst->format)) {
		SDL_SetError("Only works with same format surfaces");
		return -1;
	}

	/* Verify the blit rectangles */
	if (srcrect) {
		if ((srcrect->x < 0) || (srcrect->y < 0) || ((srcrect->x + srcrect->w) > src->w) || ((srcrect->y + srcrect->h) > src->h)) {
			SDL_SetError("Invalid source blit rectangle");
			return -1;
		}
	} else {
		full_src.x = 0;
		full_src.y = 0;
		full_src.w = src->w;
		full_src.h = src->h;
		srcrect = &full_src;
	}
	if (dstrect) {
		if ((dstrect->x < 0) || (dstrect->y < 0) || ((dstrect->x + dstrect->w) > dst->w) || ((dstrect->y + dstrect->h) > dst->h)) {
			SDL_SetError("Invalid destination blit rectangle");
			return -1;
		}
	} else {
		full_dst.x = 0;
		full_dst.y = 0;
		full_dst.w = dst->w;
		full_dst.h = dst->h;
		dstrect = &full_dst;
	}

	/* Lock the destination if it's in hardware */
	dst_locked = 0;
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			SDL_SetError("Unable to lock destination surface");
			return -1;
		}
		dst_locked = 1;
	}
	/* Lock the source if it's in hardware */
	src_locked = 0;
	if (SDL_MUSTLOCK(src)) {
		if (SDL_LockSurface(src) < 0) {
			if (dst_locked) {
				SDL_UnlockSurface(dst);
			}
			SDL_SetError("Unable to lock source surface");
			return -1;
		}
		src_locked = 1;
	}

	/* Set up the data... */
	pos = 0x10000;
	inc = (srcrect->h << 16) / dstrect->h;
	src_row = srcrect->y;
	dst_row = dstrect->y;

	/* Perform the stretch blit */
	for (dst_maxrow = dst_row + dstrect->h; dst_row < dst_maxrow; ++dst_row) {
		dstp = (Uint8 *)dst->pixels + (dst_row * dst->pitch)
		    + (dstrect->x * bpp);
		while (pos >= 0x10000L) {
			srcp = (Uint8 *)src->pixels + (src_row * src->pitch)
			    + (srcrect->x * bpp);
			++src_row;
			pos -= 0x10000L;
		}
		switch (bpp) {
		case 1:
			copy_row1(srcp, srcrect->w, dstp, dstrect->w);
			break;
		case 2:
			copy_row2((Uint16 *)srcp, srcrect->w,
			    (Uint16 *)dstp, dstrect->w);
			break;
		case 3:
			copy_row3(srcp, srcrect->w, dstp, dstrect->w);
			break;
		case 4:
			copy_row4((Uint32 *)srcp, srcrect->w,
			    (Uint32 *)dstp, dstrect->w);
			break;
		}
		pos += inc;
	}

	/* We need to unlock the surfaces if they're locked */
	if (dst_locked) {
		SDL_UnlockSurface(dst);
	}
	if (src_locked) {
		SDL_UnlockSurface(src);
	}
	return 0;
}

int SDL_BlitScaled(SDL_Surface *src, SDL_Rect *srcrect,
    SDL_Surface *dst, SDL_Rect *dstrect)
{
	if (!SDLBackport_PixelFormatFormatEq(src->format, dst->format) || SDLBackport_IsPixelFormatIndexed(src->format)) {
		return SDL_BlitSurface(src, srcrect, dst, dstrect);
	}

	double src_x0, src_y0, src_x1, src_y1;
	double dst_x0, dst_y0, dst_x1, dst_y1;
	SDL_Rect final_src, final_dst;
	double scaling_w, scaling_h;
	int src_w, src_h;
	int dst_w, dst_h;

	/* Make sure the surfaces aren't locked */
	if (!src || !dst) {
		SDL_SetError("SDL_UpperBlitScaled: passed a NULL surface");
		return -1;
	}
	if (src->locked || dst->locked) {
		SDL_SetError("Surfaces must not be locked during blit");
		return -1;
	}

	if (NULL == srcrect) {
		src_w = src->w;
		src_h = src->h;
	} else {
		src_w = srcrect->w;
		src_h = srcrect->h;
	}

	if (NULL == dstrect) {
		dst_w = dst->w;
		dst_h = dst->h;
	} else {
		dst_w = dstrect->w;
		dst_h = dstrect->h;
	}

	if (dst_w == src_w && dst_h == src_h) {
		/* No scaling, defer to regular blit */
		return SDL_BlitSurface(src, srcrect, dst, dstrect);
	}

	scaling_w = (double)dst_w / src_w;
	scaling_h = (double)dst_h / src_h;

	if (NULL == dstrect) {
		dst_x0 = 0;
		dst_y0 = 0;
		dst_x1 = dst_w - 1;
		dst_y1 = dst_h - 1;
	} else {
		dst_x0 = dstrect->x;
		dst_y0 = dstrect->y;
		dst_x1 = dst_x0 + dst_w - 1;
		dst_y1 = dst_y0 + dst_h - 1;
	}

	if (NULL == srcrect) {
		src_x0 = 0;
		src_y0 = 0;
		src_x1 = src_w - 1;
		src_y1 = src_h - 1;
	} else {
		src_x0 = srcrect->x;
		src_y0 = srcrect->y;
		src_x1 = src_x0 + src_w - 1;
		src_y1 = src_y0 + src_h - 1;

		/* Clip source rectangle to the source surface */

		if (src_x0 < 0) {
			dst_x0 -= src_x0 * scaling_w;
			src_x0 = 0;
		}

		if (src_x1 >= src->w) {
			dst_x1 -= (src_x1 - src->w + 1) * scaling_w;
			src_x1 = src->w - 1;
		}

		if (src_y0 < 0) {
			dst_y0 -= src_y0 * scaling_h;
			src_y0 = 0;
		}

		if (src_y1 >= src->h) {
			dst_y1 -= (src_y1 - src->h + 1) * scaling_h;
			src_y1 = src->h - 1;
		}
	}

	/* Clip destination rectangle to the clip rectangle */

	/* Translate to clip space for easier calculations */
	dst_x0 -= dst->clip_rect.x;
	dst_x1 -= dst->clip_rect.x;
	dst_y0 -= dst->clip_rect.y;
	dst_y1 -= dst->clip_rect.y;

	if (dst_x0 < 0) {
		src_x0 -= dst_x0 / scaling_w;
		dst_x0 = 0;
	}

	if (dst_x1 >= dst->clip_rect.w) {
		src_x1 -= (dst_x1 - dst->clip_rect.w + 1) / scaling_w;
		dst_x1 = dst->clip_rect.w - 1;
	}

	if (dst_y0 < 0) {
		src_y0 -= dst_y0 / scaling_h;
		dst_y0 = 0;
	}

	if (dst_y1 >= dst->clip_rect.h) {
		src_y1 -= (dst_y1 - dst->clip_rect.h + 1) / scaling_h;
		dst_y1 = dst->clip_rect.h - 1;
	}

	/* Translate back to surface coordinates */
	dst_x0 += dst->clip_rect.x;
	dst_x1 += dst->clip_rect.x;
	dst_y0 += dst->clip_rect.y;
	dst_y1 += dst->clip_rect.y;

	final_src.x = (Sint16)SDL_floor(src_x0 + 0.5);
	final_src.y = (Sint16)SDL_floor(src_y0 + 0.5);
	src_w = (int)SDL_floor(src_x1 + 1 + 0.5) - (int)SDL_floor(src_x0 + 0.5);
	src_h = (int)SDL_floor(src_y1 + 1 + 0.5) - (int)SDL_floor(src_y0 + 0.5);
	if (src_w < 0)
		src_w = 0;
	if (src_h < 0)
		src_h = 0;

	final_src.w = static_cast<Uint16>(src_w);
	final_src.h = static_cast<Uint16>(src_h);

	final_dst.x = (Sint16)SDL_floor(dst_x0 + 0.5);
	final_dst.y = (Sint16)SDL_floor(dst_y0 + 0.5);
	dst_w = (int)SDL_floor(dst_x1 - dst_x0 + 1.5);
	dst_h = (int)SDL_floor(dst_y1 - dst_y0 + 1.5);
	if (dst_w < 0)
		dst_w = 0;
	if (dst_h < 0)
		dst_h = 0;

	final_dst.w = static_cast<Uint16>(dst_w);
	final_dst.h = static_cast<Uint16>(dst_h);

	if (dstrect)
		*dstrect = final_dst;

	if (final_dst.w == 0 || final_dst.h == 0 || final_src.w == 0 || final_src.h == 0) {
		/* No-op. */
		return 0;
	}

	return SDL_SoftStretch(src, &final_src, dst, &final_dst);
}
