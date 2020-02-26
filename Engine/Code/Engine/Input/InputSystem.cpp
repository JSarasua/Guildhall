#include <Windows.h>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Platform/Window.hpp"


const unsigned char ESC_KEY			= VK_ESCAPE;
const unsigned char UP_KEY			= VK_UP;
const unsigned char LEFT_KEY		= VK_LEFT;
const unsigned char RIGHT_KEY		= VK_RIGHT;
const unsigned char DOWN_KEY		= VK_DOWN;
const unsigned char SPACE_KEY		= VK_SPACE;
const unsigned char DEL_KEY			= VK_DELETE;
const unsigned char TILDE_KEY		= VK_OEM_3;
const unsigned char ENTER_KEY		= VK_RETURN;
const unsigned char PGUP_KEY		= VK_PRIOR;
const unsigned char PGDOWN_KEY		= VK_NEXT;
const unsigned char LCTRL_KEY		= VK_LCONTROL;
const unsigned char RCTRL_KEY		= VK_RCONTROL;
const unsigned char CTRL_KEY		= VK_CONTROL;
const unsigned char HOME_KEY		= VK_HOME;
const unsigned char END_KEY			= VK_END;
const unsigned char BACKSPACE_KEY	= VK_BACK;
const unsigned char SHIFT_KEY		= VK_SHIFT;
const unsigned char CTRL_C_KEY		= 0x03;
const unsigned char CTRL_V_KEY		= 0x16;
const unsigned char CTRL_X_KEY		= 0x18;

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{

}

void InputSystem::Startup( Window* window )
{
	m_window = window;
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
		if( m_keyStates[keyboardIndex].m_wasPressedLastFrame != m_keyStates[keyboardIndex].m_isPressed )
		{
			m_keyStates[keyboardIndex].m_wasPressedLastFrame = m_keyStates[keyboardIndex].m_isPressed;
		}
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

void InputSystem::HandleDevConsoleInput( unsigned char keyCode )
{
	m_consoleKeyBuffer.push_back(keyCode);
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
	ScreenToClient( (HWND)m_window->m_hwnd, &mousePos );
	Vec2 mouseClientPos( (float)mousePos.x, (float)mousePos.y);

	RECT clientRect;
	GetClientRect( (HWND)m_window->m_hwnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom);

	m_mouseNormalizedClientPos = clientBounds.GetUVForPoint(mouseClientPos);
	m_mouseNormalizedClientPos.y = 1.f - m_mouseNormalizedClientPos.y; //Flip y
}

void InputSystem::CopyStringToClipboard( std::string stringToCopy )
{
	const char* copyString = stringToCopy.c_str();
	const size_t len = stringToCopy.length() + 1;
	HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE, len ); //change to gmem_fixed
	memcpy( GlobalLock(hMem), copyString, len );
	GlobalUnlock(hMem);
	OpenClipboard(nullptr);
	EmptyClipboard();
	if( NULL == SetClipboardData( CF_TEXT, hMem ) )
	{
		GlobalFree( hMem );
	}
	CloseClipboard();
}

std::string InputSystem::GetClipboardString()
{
	HANDLE handle;
	if( !OpenClipboard( nullptr ) )
	{
		return std::string();
	}

	handle = GetClipboardData( CF_TEXT );
	std::string copiedText = std::string((char*)handle);

	CloseClipboard();
	return copiedText;
}
