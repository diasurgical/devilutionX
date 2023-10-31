#ifdef _DEBUG
#include "panels/console.hpp"

#include <cstdint>
#include <string_view>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "DiabloUI/text_input.hpp"
#include "control.h"
#include "engine.h"
#include "engine/displacement.hpp"
#include "engine/dx.h"
#include "engine/palette.h"
#include "engine/rectangle.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "engine/surface.hpp"
#include "lua/repl.hpp"
#include "utils/algorithm/container.hpp"
#include "utils/sdl_geometry.h"
#include "utils/str_cat.hpp"
#include "utils/str_split.hpp"

namespace devilution {

namespace {

constexpr std::string_view Prompt = "> ";
bool IsConsoleVisible;
char ConsoleInputBuffer[4096];
TextInputCursorState ConsoleInputCursor;
TextInputState ConsoleInputState {
	TextInputState::Options {
	    .value = ConsoleInputBuffer,
	    .cursor = &ConsoleInputCursor,
	    .maxLength = sizeof(ConsoleInputBuffer) - 1,
	}
};
bool InputTextChanged = false;
std::string WrappedInputText { Prompt };

struct ConsoleLine {
	enum Type : uint8_t {
		Input,
		Output,
		Error
	};

	Type type;
	std::string text;
	std::string wrapped = {};
};

std::vector<ConsoleLine> ConsoleLines;

Rectangle OuterRect;
Rectangle InputRect;
int InputRectHeight;
constexpr int LineHeight = 20;
constexpr int TextPaddingYTop = 0;
constexpr int TextPaddingYBottom = 4;
constexpr int TextPaddingX = 4;
constexpr uint8_t BorderColor = PAL8_YELLOW;
bool FirstRender;

constexpr UiFlags TextUiFlags = UiFlags::FontSizeDialog;
constexpr UiFlags InputTextUiFlags = TextUiFlags | UiFlags::ColorDialogWhite;
constexpr UiFlags OutputTextUiFlags = TextUiFlags | UiFlags::ColorDialogWhite;
constexpr UiFlags ErrorTextUiFlags = TextUiFlags | UiFlags::ColorDialogRed;

constexpr int TextSpacing = 0;
constexpr GameFontTables TextFontSize = GetFontSizeFromUiFlags(InputTextUiFlags);

void CloseConsole()
{
	IsConsoleVisible = false;
	SDL_StopTextInput();
}

void SendInput()
{
	const std::string_view input = ConsoleInputState.value();
	tl::expected<std::string, std::string> result = RunLuaReplLine(input);

	ConsoleLines.emplace_back(ConsoleLine { .type = ConsoleLine::Input, .text = StrCat("> ", input) });

	if (result.has_value()) {
		if (!result->empty()) {
			ConsoleLines.emplace_back(ConsoleLine { .type = ConsoleLine::Output, .text = *std::move(result) });
		}
	} else {
		if (!result.error().empty()) {
			ConsoleLines.emplace_back(ConsoleLine { .type = ConsoleLine::Error, .text = std::move(result).error() });
		} else {
			ConsoleLines.emplace_back(ConsoleLine { .type = ConsoleLine::Error, .text = "Unknown error" });
		}
	}

	ConsoleInputState.clear();
}

void DrawInputText(const Surface &inputTextSurface, std::string_view originalInputText, std::string_view wrappedInputText)
{
	int lineY = 0;
	int numRendered = -static_cast<int>(Prompt.size());
	bool prevIsOriginalNewline = false;
	for (const std::string_view line : SplitByChar(wrappedInputText, '\n')) {
		const int lineCursorPosition = static_cast<int>(ConsoleInputCursor.position) - numRendered;
		const bool isCursorOnPrevLine = lineCursorPosition == 0 && !prevIsOriginalNewline && numRendered > 0;
		DrawString(
		    inputTextSurface, line, { 0, lineY },
		    TextRenderOptions {
		        .flags = InputTextUiFlags,
		        .spacing = TextSpacing,
		        .cursorPosition = isCursorOnPrevLine ? -1 : lineCursorPosition,
		        .highlightRange = { static_cast<int>(ConsoleInputCursor.selection.begin) - numRendered, static_cast<int>(ConsoleInputCursor.selection.end) - numRendered },
		    });
		lineY += LineHeight;
		numRendered += static_cast<int>(line.size());
		prevIsOriginalNewline = static_cast<size_t>(numRendered) < originalInputText.size()
		    && originalInputText[static_cast<size_t>(numRendered)] == '\n';
		if (prevIsOriginalNewline)
			++numRendered;
	}
}

void DrawConsoleLines(const Surface &out)
{
	int lineYEnd = out.h() - 4; // Extra space for letters like g.
	// NOLINTNEXTLINE(modernize-loop-convert)
	for (auto it = ConsoleLines.rbegin(), itEnd = ConsoleLines.rend(); it != itEnd; ++it) {
		ConsoleLine &consoleLine = *it;
		if (consoleLine.wrapped.empty()) {
			consoleLine.wrapped = WordWrapString(consoleLine.text, out.w(), TextFontSize, TextSpacing);
		}
		size_t end = consoleLine.wrapped.size();
		while (true) {
			const size_t begin = consoleLine.wrapped.rfind('\n', end - 1) + 1;
			const std::string_view line = std::string_view(consoleLine.wrapped.data() + begin, end - begin);
			lineYEnd -= LineHeight;
			switch (consoleLine.type) {
			case ConsoleLine::Input:
				DrawString(out, line, { 0, lineYEnd },
				    TextRenderOptions { .flags = InputTextUiFlags, .spacing = TextSpacing });
				break;
			case ConsoleLine::Output:
				DrawString(out, line, { 0, lineYEnd },
				    TextRenderOptions { .flags = OutputTextUiFlags, .spacing = TextSpacing });
				break;
			case ConsoleLine::Error:
				DrawString(out, line, { 0, lineYEnd },
				    TextRenderOptions { .flags = ErrorTextUiFlags, .spacing = TextSpacing });
				break;
			}
			if (lineYEnd < 0 || begin == 0)
				break;
			end = begin - 1;
		}
	}
}

} // namespace

void OpenConsole()
{
	IsConsoleVisible = true;
	FirstRender = true;
}

bool ConsoleHandleEvent(const SDL_Event &event)
{
	if (!IsConsoleVisible) {
		// Make console open on the top-left keyboard key even if it is not a backtick.
		if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_GRAVE) {
			OpenConsole();
			return true;
		}
		return false;
	}
	if (HandleTextInputEvent(event, ConsoleInputState)) {
		InputTextChanged = true;
		return true;
	}
	const auto modState = SDL_GetModState();
	const bool isShift = (modState & KMOD_SHIFT) != 0;
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			CloseConsole();
			return true;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			if (isShift) {
				ConsoleInputState.type("\n");
			} else {
				SendInput();
			}
			InputTextChanged = true;
			return true;
		default:
			return false;
		}
		break;
	default:
		return false;
	}
	return false;
}

void DrawConsole(const Surface &out)
{
	if (!IsConsoleVisible)
		return;

	OuterRect.position = { 0, 0 };
	OuterRect.size = { out.w(), out.h() - GetMainPanel().size.height - 2 };

	const std::string_view originalInputText = ConsoleInputState.value();
	if (InputTextChanged) {
		WrappedInputText = WordWrapString(StrCat(Prompt, originalInputText), OuterRect.size.width - 2 * TextPaddingX, TextFontSize, TextSpacing);
		InputTextChanged = false;
	}

	const int numLines = static_cast<int>(c_count(WrappedInputText, '\n')) + 1;
	const int inputTextHeight = numLines * LineHeight;
	InputRectHeight = inputTextHeight + TextPaddingYTop + TextPaddingYBottom;

	InputRect.position = { 0, OuterRect.size.height - InputRectHeight };
	InputRect.size = { OuterRect.size.width, InputRectHeight };
	const Rectangle inputTextRect {
		{ InputRect.position.x + TextPaddingX, InputRect.position.y + TextPaddingYTop },
		{ InputRect.size.width - 2 * TextPaddingX, inputTextHeight }
	};

	if (FirstRender) {
		const SDL_Rect sdlInputRect = MakeSdlRect(InputRect);
		SDL_SetTextInputRect(&sdlInputRect);
		SDL_StartTextInput();
		FirstRender = false;
	}

	const Rectangle bgRect = OuterRect;
	DrawHalfTransparentRectTo(out, bgRect.position.x, bgRect.position.y, bgRect.size.width, bgRect.size.height);

	DrawConsoleLines(
	    out.subregion(
	        TextPaddingX,
	        TextPaddingYTop,
	        OuterRect.size.width - 2 * TextPaddingX,
	        OuterRect.size.height - inputTextRect.size.height - 8));

	DrawHorizontalLine(out, InputRect.position - Displacement { 0, 1 }, InputRect.size.width, BorderColor);
	DrawInputText(
	    out.subregion(
	        inputTextRect.position.x,
	        inputTextRect.position.y,
	        // Extra space for the cursor on the right:
	        inputTextRect.size.width + TextPaddingX,
	        // Extra space for letters like g.
	        inputTextRect.size.height + TextPaddingYBottom),
	    originalInputText,
	    WrappedInputText);

	SDL_Rect sdlRect = MakeSdlRect(OuterRect);
	BltFast(&sdlRect, &sdlRect);
}

} // namespace devilution
#endif // _DEBUG
