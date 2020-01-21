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

IntVec2 InputSystem::GetMouseDesktopRawPos() const
{
	return IntVec2(0,0);
}

Vec2 InputSystem::GetMouseNormalizedPos() const
{
	return m_mouseNormalizedClientPos;
}

const XboxController& InputSystem::GetXboxController( int controllerID )
{
	return m_controllers[controllerID];
}

const KeyButtonState& InputSystem::GetKeyStates( unsigned char keyCode )
{
	return m_keyStates[keyCode];
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
