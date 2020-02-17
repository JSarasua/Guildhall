#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/vec2.hpp"
#include <string>
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

struct IntVec2;
constexpr int NUM_KEYCODES = 256;


constexpr int MAX_XBOX_CONTROLLERS = 4;

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void HandleKeyDown(unsigned char keyCode);
	void HandleKeyUp(unsigned char keyCode);
	void HandleDevConsoleInput( unsigned char keyCode );

	IntVec2 GetMouseDesktopRawPos() const;
	Vec2 GetMouseNormalizedPos() const;

	const XboxController& GetXboxController(int controllerID);
	const KeyButtonState& GetKeyStates(unsigned char keyCode);
	const KeyButtonState* GetAllKeyStates();

private:

	void UpdateMouse();
private:
	Vec2 m_mouseNormalizedClientPos;

	std::string m_consoleKeyBuffer;
	std::string m_consoleSpecialKeys;

	KeyButtonState m_keyStates[ NUM_KEYCODES ]; //extern the Keycodes and put them in .cpp
	XboxController m_controllers[ MAX_XBOX_CONTROLLERS ] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};


};