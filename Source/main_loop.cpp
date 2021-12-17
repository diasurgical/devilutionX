#include "main_loop.hpp"

#include <utility>
#include <vector>

#include "utils/log.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// #define DEBUG_MAIN_LOOP

namespace devilution {

namespace {
std::unique_ptr<MainLoopHandler> Handler;

std::vector<std::function<std::unique_ptr<MainLoopHandler>()>> HandlerFactories;
std::vector<std::function<std::unique_ptr<MainLoopHandler>()>> HandlersBeingCreated;

std::function<void(int status)> QuitFn;
int QuitStatus;

// Runs an iteration of the main loop.
// Return true if the loop should continue.
bool RunMainLoopIteration()
{
	if (Handler == nullptr) {
		QuitFn(QuitStatus);
		return false;
	}
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT)
			Handler = nullptr;
		if (Handler == nullptr) {
			QuitFn(QuitStatus);
			return false;
		}
		Handler->HandleEvent(event);
		if (Handler == nullptr) {
			QuitFn(QuitStatus);
			return false;
		}
	}
	Handler->Render();
	if (Handler == nullptr) {
		QuitFn(QuitStatus);
		return false;
	}
	return true;
}

#ifdef __EMSCRIPTEN__
extern "C" EM_BOOL RunMainLoopIterationEmscripten(void *userData)
{
	return RunMainLoopIteration() ? EM_TRUE : EM_FALSE;
}
#endif

} // namespace

void SetMainLoopQuitFn(std::function<void(int status)> fn)
{
	QuitFn = std::move(fn);
}

void MainLoopQuit(int status)
{
	QuitStatus = status;
	if (Handler == nullptr) {
		QuitFn(QuitStatus);
	} else {
		Handler = nullptr;
	}
}

void SetMainLoopHandler(std::unique_ptr<MainLoopHandler> handler)
{
#ifdef DEBUG_MAIN_LOOP
	if (Handler == nullptr) {
		Log("[MAIN_LOOP] Handler set to {}", handler != nullptr ? typeid(*handler).name() : "nullptr");
	} else {
		Log("[MAIN_LOOP] Handler changed from {} to {}", typeid(*Handler).name(), handler != nullptr ? typeid(*handler).name() : "nullptr");
	}
#endif

	Handler = nullptr;
	Handler = std::move(handler);

	if (!HandlersBeingCreated.empty()) {
#ifdef DEBUG_MAIN_LOOP
		Log("[MAIN_LOOP]  Skipped handler because its constructor called SetMainLoopHandler: {}", typeid(HandlersBeingCreated.back()).name());
#endif
		HandlersBeingCreated.pop_back();
	}
}

void AddNextMainLoopHandler(std::function<std::unique_ptr<MainLoopHandler>()> factory)
{
	HandlerFactories.push_back(std::move(factory));
}

void AddMainLoopHandlers(std::initializer_list<std::function<std::unique_ptr<MainLoopHandler>()>> factories)
{
	for (auto it = std::rbegin(factories); it != std::rend(factories); ++it) {
		HandlerFactories.push_back(*it);
	}
	NextMainLoopHandler();
}

void NextMainLoopHandler()
{
	if (!HandlersBeingCreated.empty()) {
#ifdef DEBUG_MAIN_LOOP
		Log("[MAIN_LOOP]  Skipped handler because its constructor called NextMainLoopHandler: {}", typeid(HandlersBeingCreated.back()).name());
#endif
		HandlersBeingCreated.pop_back();
	}
	if (HandlerFactories.empty()) {
		Handler = nullptr;
		return;
	}

	HandlersBeingCreated.push_back(std::move(HandlerFactories.back()));
	HandlerFactories.pop_back();

	// The handler's constructor is itself allowed to call `NextMainLoopHandler()`,
	// so we store the result to a global.
	// This way `handler` is always from the most recent call.
	Handler = nullptr;
	auto handler = HandlersBeingCreated.back()();
	if (!HandlersBeingCreated.empty()) {
		SetMainLoopHandler(std::move(handler));
	}
}

void RunMainLoop()
{
#ifdef __EMSCRIPTEN__
	void emscripten_set_main_loop(RunMainLoopIterationEmscripten, /*fps=*/0, /*simulate_infinite_loop=*/0);
#else
	while (RunMainLoopIteration()) { }
#endif
}

} // namespace devilution
