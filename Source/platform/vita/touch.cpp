#include <cmath>

#include "options.h"
#include "touch.h"
#include "utils/display.h"
#include "utils/ui_fwd.h"

static int visible_width;
static int visible_height;
static int x_borderwidth;
static int y_borderwidth;

template <typename T>
inline T clip(T v, T amin, T amax)
{
	if (v < amin)
		return amin;
	if (v > amax)
		return amax;

	return v;
}

#define TOUCH_PORT_MAX_NUM 1
#define NO_TOUCH (-1) // finger id setting if finger is not touching the screen

static void InitTouch();
static void PreprocessEvents(SDL_Event *event);
static void PreprocessFingerDown(SDL_Event *event);
static void PreprocessFingerUp(SDL_Event *event);
static void preprocess_back_finger_down(SDL_Event *event);
static void preprocess_back_finger_up(SDL_Event *event);
static void PreprocessFingerMotion(SDL_Event *event);
static void SetMouseButtonEvent(SDL_Event *event, uint32_t type, uint8_t button, int32_t x, int32_t y);
static void SetMouseMotionEvent(SDL_Event *event, int32_t x, int32_t y, int32_t xrel, int32_t yrel);

static bool touch_initialized = false;
static unsigned int simulated_click_start_time[TOUCH_PORT_MAX_NUM][2]; // initiation time of last simulated left or right click (zero if no click)
static bool direct_touch = true;                                       // pointer jumps to finger
static int mouse_x = 0;                                                // always reflects current mouse position
static int mouse_y = 0;

enum {
	// clang-format off
	MaxNumFingers          =   3, // number of fingers to track per panel
	MaxTapTime             = 250, // taps longer than this will not result in mouse click events
	MaxTapMotionDistance   =  10, // max distance finger motion in Vita screen pixels to be considered a tap
	SimulatedClickDuration =  50, // time in ms how long simulated mouse clicks should be
	// clang-format on
};

struct Touch {
	int id; // -1: not touching
	uint32_t timeLastDown;
	int lastX;       // last known screen coordinates
	int lastY;       // last known screen coordinates
	float lastDownX; // SDL touch coordinates when last pressed down
	float lastDownY; // SDL touch coordinates when last pressed down
};

static Touch finger[TOUCH_PORT_MAX_NUM][MaxNumFingers]; // keep track of finger status

enum DraggingType {
	DragNone,
	DragTwoFinger,
	DragThreeFinger,
};

static DraggingType multi_finger_dragging[TOUCH_PORT_MAX_NUM]; // keep track whether we are currently drag-and-dropping

static void InitTouch()
{
	for (int port = 0; port < TOUCH_PORT_MAX_NUM; port++) {
		for (int i = 0; i < MaxNumFingers; i++) {
			finger[port][i].id = NO_TOUCH;
		}
		multi_finger_dragging[port] = DragNone;
	}

	for (auto &port : simulated_click_start_time) {
		for (unsigned int &time : port) {
			time = 0;
		}
	}

	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	visible_height = current.h;
	visible_width = (current.h * devilution::gnScreenWidth) / devilution::gnScreenHeight;
	x_borderwidth = (current.w - visible_width) / 2;
	y_borderwidth = (current.h - visible_height) / 2;
}

static void PreprocessEvents(SDL_Event *event)
{
	// Supported touch gestures:
	// left mouse click: single finger short tap
	// right mouse click: second finger short tap while first finger is still down
	// pointer motion: single finger drag
	// left button drag and drop: dual finger drag
	// right button drag and drop: triple finger drag
	if (event->type != SDL_FINGERDOWN && event->type != SDL_FINGERUP && event->type != SDL_FINGERMOTION) {
		return;
	}

	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	if (port != 0) {
		if (devilution::sgOptions.Controller.bRearTouch) {
			switch (event->type) {
			case SDL_FINGERDOWN:
				preprocess_back_finger_down(event);
				break;
			case SDL_FINGERUP:
				preprocess_back_finger_up(event);
				break;
			}
		}
		return;
	}

	switch (event->type) {
	case SDL_FINGERDOWN:
		PreprocessFingerDown(event);
		break;
	case SDL_FINGERUP:
		PreprocessFingerUp(event);
		break;
	case SDL_FINGERMOTION:
		PreprocessFingerMotion(event);
		break;
	}
}

static void PreprocessFingerDown(SDL_Event *event)
{
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	// id (for multitouch)
	SDL_FingerID id = event->tfinger.fingerId;

	int x = mouse_x;
	int y = mouse_y;

	if (direct_touch) {
		x = static_cast<int>(event->tfinger.x * visible_width) + x_borderwidth;
		y = static_cast<int>(event->tfinger.y * visible_height) + y_borderwidth;
		devilution::OutputToLogical(&x, &y);
	}

	// make sure each finger is not reported down multiple times
	for (int i = 0; i < MaxNumFingers; i++) {
		if (finger[port][i].id != id) {
			continue;
		}
		finger[port][i].id = NO_TOUCH;
	}

	// we need the timestamps to decide later if the user performed a short tap (click)
	// or a long tap (drag)
	// we also need the last coordinates for each finger to keep track of dragging
	for (int i = 0; i < MaxNumFingers; i++) {
		if (finger[port][i].id != NO_TOUCH) {
			continue;
		}
		finger[port][i].id = id;
		finger[port][i].timeLastDown = event->tfinger.timestamp;
		finger[port][i].lastDownX = event->tfinger.x;
		finger[port][i].lastDownY = event->tfinger.y;
		finger[port][i].lastX = x;
		finger[port][i].lastY = y;
		break;
	}
}

static void preprocess_back_finger_down(SDL_Event *event)
{
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;

	if (port != 1)
		return;

	event->type = SDL_CONTROLLERAXISMOTION;
	event->caxis.value = 32767;
	event->caxis.which = 0;
	if (event->tfinger.x <= 0.5) {
		;
		event->caxis.axis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	} else {
		event->caxis.axis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	}
}

static void preprocess_back_finger_up(SDL_Event *event)
{
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;

	if (port != 1)
		return;

	event->type = SDL_CONTROLLERAXISMOTION;
	event->caxis.value = 0;
	event->caxis.which = 0;
	if (event->tfinger.x <= 0.5) {
		;
		event->caxis.axis = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	} else {
		event->caxis.axis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	}
}

static void PreprocessFingerUp(SDL_Event *event)
{
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	// id (for multitouch)
	SDL_FingerID id = event->tfinger.fingerId;

	// find out how many fingers were down before this event
	int numFingersDown = 0;
	for (int i = 0; i < MaxNumFingers; i++) {
		if (finger[port][i].id >= 0) {
			numFingersDown++;
		}
	}

	int x = mouse_x;
	int y = mouse_y;

	for (int i = 0; i < MaxNumFingers; i++) {
		if (finger[port][i].id != id) {
			continue;
		}

		finger[port][i].id = NO_TOUCH;
		if (multi_finger_dragging[port] == DragNone) {
			if ((event->tfinger.timestamp - finger[port][i].timeLastDown) > MaxTapTime) {
				continue;
			}

			// short (<MAX_TAP_TIME ms) tap is interpreted as right/left mouse click depending on # fingers already down
			// but only if the finger hasn't moved since it was pressed down by more than MAX_TAP_MOTION_DISTANCE pixels
			float xrel = ((event->tfinger.x * devilution::GetOutputSurface()->w) - (finger[port][i].lastDownX * devilution::GetOutputSurface()->w));
			float yrel = ((event->tfinger.y * devilution::GetOutputSurface()->h) - (finger[port][i].lastDownY * devilution::GetOutputSurface()->h));
			auto maxRSquared = static_cast<float>(MaxTapMotionDistance * MaxTapMotionDistance);
			if ((xrel * xrel + yrel * yrel) >= maxRSquared) {
				continue;
			}

			if (numFingersDown != 2 && numFingersDown != 1) {
				continue;
			}

			Uint8 simulatedButton = 0;
			if (numFingersDown == 2) {
				simulatedButton = SDL_BUTTON_RIGHT;
				// need to raise the button later
				simulated_click_start_time[port][1] = event->tfinger.timestamp;
			} else if (numFingersDown == 1) {
				simulatedButton = SDL_BUTTON_LEFT;
				// need to raise the button later
				simulated_click_start_time[port][0] = event->tfinger.timestamp;
				if (direct_touch) {
					x = static_cast<int>(event->tfinger.x * visible_width) + x_borderwidth;
					y = static_cast<int>(event->tfinger.y * visible_height) + y_borderwidth;
					devilution::OutputToLogical(&x, &y);
				}
			}
			SetMouseButtonEvent(event, SDL_MOUSEBUTTONDOWN, simulatedButton, x, y);
		} else if (numFingersDown == 1) {
			// when dragging, and the last finger is lifted, the drag is over
			Uint8 simulatedButton = 0;
			if (multi_finger_dragging[port] == DragThreeFinger) {
				simulatedButton = SDL_BUTTON_RIGHT;
			} else {
				simulatedButton = SDL_BUTTON_LEFT;
			}
			SetMouseButtonEvent(event, SDL_MOUSEBUTTONUP, simulatedButton, x, y);
			multi_finger_dragging[port] = DragNone;
		}
	}
}

static void PreprocessFingerMotion(SDL_Event *event)
{
	// front (0) or back (1) panel
	SDL_TouchID port = event->tfinger.touchId;
	// id (for multitouch)
	SDL_FingerID id = event->tfinger.fingerId;

	// find out how many fingers were down before this event
	int numFingersDown = 0;
	for (int i = 0; i < MaxNumFingers; i++) {
		if (finger[port][i].id >= 0) {
			numFingersDown++;
		}
	}

	if (numFingersDown == 0) {
		return;
	}

	if (numFingersDown >= 1) {
		int x = mouse_x;
		int y = mouse_y;

		if (direct_touch) {
			x = static_cast<int>(event->tfinger.x * visible_width) + x_borderwidth;
			y = static_cast<int>(event->tfinger.y * visible_height) + y_borderwidth;
			devilution::OutputToLogical(&x, &y);
		} else {
			// for relative mode, use the pointer speed setting
			constexpr float SpeedFactor = 1.25F;

			// convert touch events to relative mouse pointer events
			// Whenever an SDL_event involving the mouse is processed,
			x = static_cast<int>(mouse_x + (event->tfinger.dx * SpeedFactor * devilution::GetOutputSurface()->w));
			y = static_cast<int>(mouse_y + (event->tfinger.dy * SpeedFactor * devilution::GetOutputSurface()->h));
		}

		x = clip(x, 0, devilution::GetOutputSurface()->w);
		y = clip(y, 0, devilution::GetOutputSurface()->h);
		int xrel = x - mouse_x;
		int yrel = y - mouse_y;

		// update the current finger's coordinates so we can track it later
		for (int i = 0; i < MaxNumFingers; i++) {
			if (finger[port][i].id != id)
				continue;
			finger[port][i].lastX = x;
			finger[port][i].lastY = y;
		}

		// If we are starting a multi-finger drag, start holding down the mouse button
		if (numFingersDown >= 2 && multi_finger_dragging[port] == DragNone) {
			// only start a multi-finger drag if at least two fingers have been down long enough
			int numFingersDownlong = 0;
			for (int i = 0; i < MaxNumFingers; i++) {
				if (finger[port][i].id == NO_TOUCH) {
					continue;
				}
				if (event->tfinger.timestamp - finger[port][i].timeLastDown > MaxTapTime) {
					numFingersDownlong++;
				}
			}
			if (numFingersDownlong >= 2) {
				int mouseDownX = mouse_x;
				int mouseDownY = mouse_y;
				if (direct_touch) {
					for (int i = 0; i < MaxNumFingers; i++) {
						if (finger[port][i].id == id) {
							uint32_t earliestTime = finger[port][i].timeLastDown;
							for (int j = 0; j < MaxNumFingers; j++) {
								if (finger[port][j].id >= 0 && (i != j)) {
									if (finger[port][j].timeLastDown < earliestTime) {
										mouseDownX = finger[port][j].lastX;
										mouseDownY = finger[port][j].lastY;
										earliestTime = finger[port][j].timeLastDown;
									}
								}
							}
							break;
						}
					}
				}

				Uint8 simulatedButton = 0;
				if (numFingersDownlong == 2) {
					simulatedButton = SDL_BUTTON_LEFT;
					multi_finger_dragging[port] = DragTwoFinger;
				} else {
					simulatedButton = SDL_BUTTON_RIGHT;
					multi_finger_dragging[port] = DragThreeFinger;
				}
				SDL_Event ev;
				SetMouseButtonEvent(&ev, SDL_MOUSEBUTTONDOWN, simulatedButton, mouseDownX, mouseDownY);
				SDL_PushEvent(&ev);
			}
		}

		if (xrel == 0 && yrel == 0) {
			return;
		}

		// check if this is the "oldest" finger down (or the only finger down)
		// otherwise it will not affect mouse motion
		bool updatePointer = true;
		if (numFingersDown > 1) {
			for (int i = 0; i < MaxNumFingers; i++) {
				if (finger[port][i].id != id) {
					continue;
				}
				for (int j = 0; j < MaxNumFingers; j++) {
					if (finger[port][j].id == NO_TOUCH || (j == i)) {
						continue;
					}
					if (finger[port][j].timeLastDown < finger[port][i].timeLastDown) {
						updatePointer = false;
					}
				}
			}
		}
		if (!updatePointer) {
			return;
		}
		SetMouseMotionEvent(event, x, y, xrel, yrel);
	}
}

namespace devilution {

void handle_touch(SDL_Event *event, int currentMouseX, int currentMouseY)
{
	mouse_x = currentMouseX;
	mouse_y = currentMouseY;

	if (!touch_initialized) {
		InitTouch();
		touch_initialized = true;
	}
	PreprocessEvents(event);
	if (event->type == SDL_FINGERDOWN || event->type == SDL_FINGERUP || event->type == SDL_FINGERMOTION) {
		event->type = SDL_USEREVENT;
		event->user.code = -1; // ensure this event is ignored;
	}
}

void finish_simulated_mouse_clicks(int currentMouseX, int currentMouseY)
{
	mouse_x = currentMouseX;
	mouse_y = currentMouseY;

	for (auto &port : simulated_click_start_time) {
		for (int i = 0; i < 2; i++) {
			if (port[i] == 0) {
				continue;
			}

			Uint32 currentTime = SDL_GetTicks();
			if (currentTime - port[i] < SimulatedClickDuration) {
				continue;
			}

			int simulatedButton;
			if (i == 0) {
				simulatedButton = SDL_BUTTON_LEFT;
			} else {
				simulatedButton = SDL_BUTTON_RIGHT;
			}
			SDL_Event ev;
			SetMouseButtonEvent(&ev, SDL_MOUSEBUTTONUP, simulatedButton, mouse_x, mouse_y);
			SDL_PushEvent(&ev);

			port[i] = 0;
		}
	}
}

} // namespace devilution

static void SetMouseButtonEvent(SDL_Event *event, uint32_t type, uint8_t button, int32_t x, int32_t y)
{
	event->type = type;
	event->button.button = button;
	if (type == SDL_MOUSEBUTTONDOWN) {
		event->button.state = SDL_PRESSED;
	} else {
		event->button.state = SDL_RELEASED;
	}
	event->button.x = x;
	event->button.y = y;
}

static void SetMouseMotionEvent(SDL_Event *event, int32_t x, int32_t y, int32_t xrel, int32_t yrel)
{
	event->type = SDL_MOUSEMOTION;
	event->motion.x = x;
	event->motion.y = y;
	event->motion.xrel = xrel;
	event->motion.yrel = yrel;
}
