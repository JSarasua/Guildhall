#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/vec2.hpp"
//#include "Engine/Core/EngineCommon.hpp"

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
	void UpdateMouseButtonState(bool leftMouseButton, bool middleMouseButton, bool rightMouseButton );
	void AddMouseWheelScrollAmount( float scrollAmount );

	IntVec2 GetMouseDesktopRawPos() const;
	Vec2 GetMouseNormalizedPos() const;
	float GetDeltaMouseWheelScroll() const;

	const XboxController& GetXboxController(int controllerID);
	const KeyButtonState& GetKeyStates(unsigned char keyCode);
	const KeyButtonState& GetMouseButton(MOUSEBUTTON mouseButton);
	const KeyButtonState* GetAllKeyStates();
private:

	void UpdateMouse();
private:
	Vec2 m_mouseNormalizedClientPos;
	float m_deltaScrollAmount = 0.f;

	KeyButtonState m_keyStates[ NUM_KEYCODES ]; //extern the Keycodes and put them in .cpp
	KeyButtonState m_mouseButtonStates[ Num_MouseButtons ];
	XboxController m_controllers[ MAX_XBOX_CONTROLLERS ] =
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};


};