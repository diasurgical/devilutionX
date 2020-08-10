namespace disp {

// Initializes the size of various graphical elements of the game. This is called within dvl::SpawnWindow.
// The full list of initialized elements is the screen resolution and the viewport height.
void InitDisplayElementSizes();

// Initializes the resolution of the screen.
// If possible, it will use the values of "screen width" and "screen height" in the configuration INI.
// Otherwise, it will use the native resolution of the primary/default monitor.
void InitDesiredScreenRes();

// Initializes the height of the viewport.
void InitViewportHeight();

// NOTE: For consistency, use SCREEN_WIDTH instead.
// Returns: The width of the screen in pixels.
// Causes undefined behavior if called before dvl::SpawnWindow is called.
const int GetScreenWidth();

// NOTE: For consistency, use SCREEN_HEIGHT instead.
// Returns: The height of the screen in pixels.
// Causes undefined behavior if called before dvl::SpawnWindow is called.
const int GetScreenHeight();

// NOTE: For consistency, use VIEWPORT_HEIGHT instead.
// Returns: The height of the viewport in pixels.
// Causes undefined behavior if called before dvl::SpawnWindow is called.
const int GetViewportHeight();

} // namespace disp
