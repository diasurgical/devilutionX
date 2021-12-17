#pragma once

#include <functional>
#include <initializer_list>
#include <memory>

#include <SDL.h>

namespace devilution {

// A top-level loop handler that responds to events and handles rendering.
class MainLoopHandler {
public:
	virtual ~MainLoopHandler() = default;

	virtual void HandleEvent([[maybe_unused]] SDL_Event &event)
	{
	}

	virtual void Render()
	{
	}
};

// Replaces the current main loop handler with the given one.
void SetMainLoopHandler(std::unique_ptr<MainLoopHandler> handler);

// Pushes a main loop handler factory on stack.
void AddNextMainLoopHandler(std::function<std::unique_ptr<MainLoopHandler>()> factory);

// Adds a sequence of main loop handlers, and invokes them in order.
void AddMainLoopHandlers(std::initializer_list<std::function<std::unique_ptr<MainLoopHandler>()>> factories);

// Pops the last factory on stack and sets the handler to its result.
void NextMainLoopHandler();

// The function to call once the main loop has finished or on `MainLoopQuit`.
void SetMainLoopQuitFn(std::function<void(int status)> fn);

// Ends the main loop with the given status.
void MainLoopQuit(int status);

// Runs the main loop. This should only be called once by the executable
// and nothing should be called after it.
void RunMainLoop();

} // namespace devilution
