#ifdef _DEBUG
#include "panels/console.hpp"

#include <cstdint>
#include <string_view>

#include <SDL.h>
#include <function_ref.hpp>

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
constexpr std::string_view HelpText =
    // Displayed as the first console message
    "Lua console\n"
    "Shift+Enter to insert a newline, PageUp/Down to scroll,"
    " Up/Down to fill the input from history,"
    " Shift+Up/Down to fill the input from output history,"
    " Ctrl+L to clear history, Esc to close.";

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
		Help,
		Input,
		Output,
		Error
	};

	Type type;
	std::string text;
	std::string wrapped = {};
	int numLines = 0;

	[[nodiscard]] std::string_view textWithoutPrompt() const
	{
		std::string_view result = text;
		if (type == ConsoleLine::Input) {
			result.remove_prefix(Prompt.size());
		}
		return result;
	}
};

std::vector<ConsoleLine> ConsoleLines;
int ConsoleLinesTotalHeight;

// Index of the currently filled input/output, counting from end.
int HistoryIndex = -1;

// Draft input, saved when navigating history.
std::string DraftInput;

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

// Scroll offset from the bottom (in pages), to be applied on next render.
int PendingScrollPages;
// Scroll offset from the bottom in pixels.
int ScrollOffset;
constexpr int ScrollStep = LineHeight * 3;

void CloseConsole()
{
	IsConsoleVisible = false;
	SDL_StopTextInput();
}

int GetConsoleLinesInnerWidth()
{
	return OuterRect.size.width - 2 * TextPaddingX;
}

void AddConsoleLine(ConsoleLine &&consoleLine)
{
	consoleLine.wrapped = WordWrapString(consoleLine.text, GetConsoleLinesInnerWidth(), TextFontSize, TextSpacing);
	consoleLine.numLines += static_cast<int>(c_count(consoleLine.wrapped, '\n')) + 1;
	ConsoleLinesTotalHeight += consoleLine.numLines * LineHeight;
	ConsoleLines.emplace_back(std::move(consoleLine));
	ScrollOffset = 0;
}

void SendInput()
{
	const std::string_view input = ConsoleInputState.value();
	tl::expected<std::string, std::string> result = RunLuaReplLine(input);

	AddConsoleLine(ConsoleLine { .type = ConsoleLine::Input, .text = StrCat(Prompt, input) });

	if (result.has_value()) {
		if (!result->empty()) {
			AddConsoleLine(ConsoleLine { .type = ConsoleLine::Output, .text = *std::move(result) });
		}
	} else {
		if (!result.error().empty()) {
			AddConsoleLine(ConsoleLine { .type = ConsoleLine::Error, .text = std::move(result).error() });
		} else {
			AddConsoleLine(ConsoleLine { .type = ConsoleLine::Error, .text = "Unknown error" });
		}
	}

	ConsoleInputState.clear();
	DraftInput.clear();
	HistoryIndex = -1;
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
	const int innerHeight = out.h() - 4; // Extra space for letters like g.
	if (PendingScrollPages) {
		ScrollOffset += innerHeight * PendingScrollPages;
		PendingScrollPages = 0;
	}
	ScrollOffset = std::clamp(ScrollOffset, 0, std::max(0, ConsoleLinesTotalHeight - innerHeight));

	int lineYEnd = innerHeight + ScrollOffset;
	// NOLINTNEXTLINE(modernize-loop-convert)
	for (auto it = ConsoleLines.rbegin(), itEnd = ConsoleLines.rend(); it != itEnd; ++it) {
		ConsoleLine &consoleLine = *it;
		const int linesYBegin = lineYEnd - LineHeight * consoleLine.numLines;
		if (linesYBegin > innerHeight) {
			lineYEnd = linesYBegin;
			continue;
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
			case ConsoleLine::Help:
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

const ConsoleLine &GetConsoleLineFromEnd(int index)
{
	return *(ConsoleLines.rbegin() + index);
}

void SetHistoryIndex(int index)
{
	InputTextChanged = true;
	HistoryIndex = static_cast<int>(ConsoleLines.size()) - (index + 1);
	if (HistoryIndex == -1) {
		ConsoleInputState.assign(DraftInput);
		return;
	}
	const ConsoleLine &line = ConsoleLines[index];
	ConsoleInputState.assign(line.textWithoutPrompt());
}

void PrevHistoryItem(tl::function_ref<bool(const ConsoleLine &line)> filter)
{
	if (HistoryIndex == -1) {
		DraftInput = ConsoleInputState.value();
	}
	for (int i = HistoryIndex + 1; i < ConsoleLines.size(); ++i) {
		const int index = static_cast<int>(ConsoleLines.size()) - (i + 1);
		if (filter(ConsoleLines[index])) {
			SetHistoryIndex(index);
			return;
		}
	}
}

void NextHistoryItem(tl::function_ref<bool(const ConsoleLine &line)> filter)
{
	for (int i = static_cast<int>(ConsoleLines.size()) - HistoryIndex; i < ConsoleLines.size(); ++i) {
		if (filter(ConsoleLines[i])) {
			SetHistoryIndex(i);
			return;
		}
	}
	if (HistoryIndex != -1) {
		SetHistoryIndex(ConsoleLines.size());
	}
}

bool IsHistoryInputLine(const ConsoleLine &line)
{
	if (line.type != ConsoleLine::Input)
		return false;
	std::string_view text = line.text;
	text.remove_prefix(Prompt.size());
	if (text.empty())
		return false;
	return HistoryIndex == -1 || GetConsoleLineFromEnd(HistoryIndex).textWithoutPrompt() != text;
}

void PrevInput()
{
	PrevHistoryItem(IsHistoryInputLine);
}

void NextInput()
{
	NextHistoryItem(IsHistoryInputLine);
}

bool IsHistoryOutputLine(const ConsoleLine &line)
{
	return !line.text.empty()
	    && (line.type == ConsoleLine::Output || line.type == ConsoleLine::Error)
	    && (HistoryIndex == -1
	        || GetConsoleLineFromEnd(HistoryIndex).textWithoutPrompt() != line.text);
}

void PrevOutput()
{
	PrevHistoryItem(IsHistoryOutputLine);
}

void NextOutput()
{
	NextHistoryItem(IsHistoryOutputLine);
}

void ClearConsole()
{
	ConsoleLines.clear();
	HistoryIndex = -1;
	ScrollOffset = 0;
	AddConsoleLine(ConsoleLine { .type = ConsoleLine::Help, .text = std::string(HelpText) });
}

} // namespace

bool IsConsoleOpen()
{
	return IsConsoleVisible;
}

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
		case SDLK_UP:
			isShift ? PrevOutput() : PrevInput();
			return true;
		case SDLK_DOWN:
			isShift ? NextOutput() : NextInput();
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
		case SDLK_PAGEUP:
			++PendingScrollPages;
			return true;
		case SDLK_PAGEDOWN:
			--PendingScrollPages;
			return true;
		case SDLK_l:
			ClearConsole();
			return true;
		default:
			return false;
		}
		break;
#if SDL_VERSION_ATLEAST(2, 0, 0)
	case SDL_MOUSEWHEEL:
		if (event.wheel.y > 0) {
			ScrollOffset += ScrollStep;
		} else if (event.wheel.y < 0) {
			ScrollOffset -= ScrollStep;
		}
		return true;
#else
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_WHEELUP) {
			ScrollOffset += ScrollStep;
			return true;
		}
		if (event.button.button == SDL_BUTTON_WHEELDOWN) {
			ScrollOffset -= ScrollStep;
			return true;
		}
		return false;
#endif
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
		if (ConsoleLines.empty()) {
			AddConsoleLine(ConsoleLine { .type = ConsoleLine::Help, .text = std::string(HelpText) });
		}
	}

	const Rectangle bgRect = OuterRect;
	DrawHalfTransparentRectTo(out, bgRect.position.x, bgRect.position.y, bgRect.size.width, bgRect.size.height);

	DrawConsoleLines(
	    out.subregion(
	        TextPaddingX,
	        TextPaddingYTop,
	        GetConsoleLinesInnerWidth(),
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
