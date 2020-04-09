#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/vec2.hpp"
#include <string>
#include <stack>
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
//#include "Engine/Core/EngineCommon.hpp"

extern const unsigned char ESC_KEY;
extern const unsigned char UP_KEY;
extern const unsigned char LEFT_KEY;
extern const unsigned char RIGHT_KEY;
extern const unsigned char DOWN_KEY;
extern const unsigned char SPACE_KEY;
extern const unsigned char DEL_KEY;
extern const unsigned char TILDE_KEY;
extern const unsigned char ENTER_KEY;
extern const unsigned char PGUP_KEY;
extern const unsigned char PGDOWN_KEY;
extern const unsigned char LCTRL_KEY;
extern const unsigned char RCTRL_KEY;
extern const unsigned char CTRL_KEY;
extern const unsigned char HOME_KEY;
extern const unsigned char END_KEY;
extern const unsigned char BACKSPACE_KEY;
extern const unsigned char SHIFT_KEY;
extern const unsigned char CTRL_C_KEY;
extern const unsigned char CTRL_V_KEY;
extern const unsigned char CTRL_X_KEY;
extern const unsigned char LBRACKET_KEY;
extern const unsigned char RBRACKET_KEY;
extern const unsigned char COMMA_KEY;
extern const unsigned char PERIOD_KEY;
extern const unsigned char BACKSLASH_KEY;
extern const unsigned char SINGLEQUOTE_KEY;
extern const unsigned char F5_KEY;
extern const unsigned char F6_KEY;
extern const unsigned char F7_KEY;
extern const unsigned char F8_KEY;
extern const unsigned char F11_KEY;
extern const unsigned char PLUS_KEY;
extern const unsigned char MINUS_KEY;
extern const unsigned char SEMICOLON_KEY;

class Window;
struct IntVec2;
constexpr int NUM_KEYCODES = 256;

constexpr int MAX_XBOX_CONTROLLERS = 4;

enum MOUSEBUTTON
{
	LeftMouseButton,
	MiddleMouseButton,
	RightMouseButton,
	Num_MouseButtons
};

enum eMousePositionMode
{
	MOUSE_MODE_ABSOLUTE,
	MOUSE_MODE_RELATIVE
};

struct MouseOptions
{
	eMousePositionMode m_mouseMode = MOUSE_MODE_ABSOLUTE;
	bool m_isMouseVisible = true;
	bool m_isMouseClipped = false;
};


class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void Startup( Window* window );
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void HandleKeyDown(unsigned char keyCode);
	void HandleKeyUp(unsigned char keyCode);
	void UpdateMouseButtonState(bool leftMouseButton, bool middleMouseButton, bool rightMouseButton );
	void AddMouseWheelScrollAmount( float scrollAmount );
	void HandleDevConsoleInput( unsigned char keyCode );

	static bool SetCursorModeToRelative( const EventArgs* args );
	static bool SetCursorModeToAbsolute( const EventArgs* args );

	IntVec2 GetMouseDesktopRawPos() const;
	Vec2 GetMouseNormalizedPos() const;
	Vec2 GetMouseDeltaPos() const;
	float GetDeltaMouseWheelScroll() const;

	const XboxController& GetXboxController(int controllerID);
	const KeyButtonState& GetKeyStates(unsigned char keyCode);
	const KeyButtonState& GetMouseButton(MOUSEBUTTON mouseButton);
	const KeyButtonState* GetAllKeyStates();
	void CopyStringToClipboard( std::string stringToCopy );
	std::string GetClipboardString();

	void SetCursorMode( eMousePositionMode mode );
	void HideSystemCursor();
	void ShowSystemCursor();
	void ClipSystemCursor();
	void UnclipSystemCursor();

	void SetScreenActive();
	void SetScreenInactive();

	void PushMouseOptions( eMousePositionMode mouseMode, bool isMouseVisible, bool isMouseClipped );
	void PopMouseOptions();
	void SetMouseOptionsFromTopOfStack();

private:

	void UpdateMouse();
	void UpdateRelativeMode();
private:
	Window* m_window = nullptr;
	Vec2 m_mouseNormalizedClientPos;
	Vec2 m_mousePreviousNormalizedClientPos;

	Vec2 m_relativeMovement;
	Vec2 m_positionLastFrame;


	float m_deltaScrollAmount = 0.f;
	eMousePositionMode m_currentMouseMode = MOUSE_MODE_ABSOLUTE;

	std::string m_consoleKeyBuffer;
	std::string m_consoleSpecialKeys;

	KeyButtonState m_keyStates[ NUM_KEYCODES ]; //extern the Keycodes and put them in .cpp
	KeyButtonState m_mouseButtonStates[ Num_MouseButtons ];
	XboxController m_controllers[ MAX_XBOX_CONTROLLERS ] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};

	std::stack<MouseOptions> m_mouseOptionStack;
};