#include <Windows.h>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"

extern HWND g_hWnd;

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{

}

void InputSystem::Startup()
{

}

void InputSystem::BeginFrame()
{
	//UpdateMouse
	UpdateMouse();
	//UpdateController

	for( int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; controllerIndex++ )
	{
		m_controllers[controllerIndex].Update();
	}
}

void InputSystem::EndFrame()
{
	
	for( int keyboardIndex = 0; keyboardIndex < NUM_KEYCODES; keyboardIndex++ )
	{
		m_keyStates[keyboardIndex].m_wasPressedLastFrame = m_keyStates[keyboardIndex].m_isPressed;
	}

	m_mouseButtonStates[LeftMouseButton].m_wasPressedLastFrame = m_mouseButtonStates[LeftMouseButton].m_isPressed;
	m_mouseButtonStates[MiddleMouseButton].m_wasPressedLastFrame = m_mouseButtonStates[MiddleMouseButton].m_isPressed;
	m_mouseButtonStates[RightMouseButton].m_wasPressedLastFrame = m_mouseButtonStates[RightMouseButton].m_isPressed;

	m_deltaScrollAmount = 0.f;
}

void InputSystem::Shutdown()
{

}

void InputSystem::HandleKeyDown( unsigned char keyCode )
{
	m_keyStates[keyCode].m_isPressed = true;
}

void InputSystem::HandleKeyUp( unsigned char keyCode )
{
	m_keyStates[keyCode].m_isPressed = false;

}

void InputSystem::UpdateMouseButtonState( bool leftMouseButton, bool middleMouseButton, bool rightMouseButton )
{
	m_mouseButtonStates[LeftMouseButton].m_isPressed = leftMouseButton;
	m_mouseButtonStates[MiddleMouseButton].m_isPressed = middleMouseButton;
	m_mouseButtonStates[RightMouseButton].m_isPressed = rightMouseButton;
}

void InputSystem::AddMouseWheelScrollAmount( float scrollAmount )
{
	m_deltaScrollAmount += scrollAmount;
}

IntVec2 InputSystem::GetMouseDesktopRawPos() const
{
	return IntVec2(0,0);
}

Vec2 InputSystem::GetMouseNormalizedPos() const
{
	return m_mouseNormalizedClientPos;
}

float InputSystem::GetDeltaMouseWheelScroll() const
{
	return m_deltaScrollAmount;
}

const XboxController& InputSystem::GetXboxController( int controllerID )
{
	return m_controllers[controllerID];
}

const KeyButtonState& InputSystem::GetKeyStates( unsigned char keyCode )
{
	return m_keyStates[keyCode];
}

const KeyButtonState& InputSystem::GetMouseButton( MOUSEBUTTON mouseButton )
{
	return m_mouseButtonStates[mouseButton];
}

const KeyButtonState* InputSystem::GetAllKeyStates()
{
	return m_keyStates;
}

void InputSystem::UpdateMouse()
{
	POINT mousePos;
	GetCursorPos( &mousePos );
	ScreenToClient( g_hWnd, &mousePos );
	Vec2 mouseClientPos( (float)mousePos.x, (float)mousePos.y);

	RECT clientRect;
	GetClientRect( g_hWnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom);

	m_mouseNormalizedClientPos = clientBounds.GetUVForPoint(mouseClientPos);
	m_mouseNormalizedClientPos.y = 1.f - m_mouseNormalizedClientPos.y; //Flip y
}
