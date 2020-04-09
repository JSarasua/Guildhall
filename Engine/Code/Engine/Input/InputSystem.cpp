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
const unsigned char LBRACKET_KEY	= VK_OEM_4;
const unsigned char RBRACKET_KEY	= VK_OEM_6;
const unsigned char COMMA_KEY		= VK_OEM_COMMA;
const unsigned char PERIOD_KEY		= VK_OEM_PERIOD;
const unsigned char BACKSLASH_KEY	= VK_OEM_5;
const unsigned char SINGLEQUOTE_KEY	= VK_OEM_7;
const unsigned char F5_KEY			= VK_F5;
const unsigned char F6_KEY			= VK_F6;
const unsigned char F7_KEY			= VK_F7;
const unsigned char F8_KEY			= VK_F8;
const unsigned char F11_KEY			= VK_F11;
const unsigned char PLUS_KEY		= VK_OEM_PLUS;
const unsigned char MINUS_KEY		= VK_OEM_MINUS;
const unsigned char SEMICOLON_KEY	= VK_OEM_1;

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{

}

void InputSystem::Startup( Window* window )
{
	m_window = window;
	g_theEventSystem->SubscribeToEvent( "relative mode", CONSOLECOMMAND, SetCursorModeToRelative );
	g_theEventSystem->SubscribeToEvent( "absolute mode", CONSOLECOMMAND, SetCursorModeToAbsolute );


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

	m_mouseButtonStates[LeftMouseButton].m_wasPressedLastFrame = m_mouseButtonStates[LeftMouseButton].m_isPressed;
	m_mouseButtonStates[MiddleMouseButton].m_wasPressedLastFrame = m_mouseButtonStates[MiddleMouseButton].m_isPressed;
	m_mouseButtonStates[RightMouseButton].m_wasPressedLastFrame = m_mouseButtonStates[RightMouseButton].m_isPressed;

	if( m_currentMouseMode == MOUSE_MODE_RELATIVE )
	{
		m_mousePreviousNormalizedClientPos = Vec2(0.5f, 0.5f);
	}
	else
	{
		m_mousePreviousNormalizedClientPos = m_mouseNormalizedClientPos;
	}

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

void InputSystem::HandleDevConsoleInput( unsigned char keyCode )
{
	m_consoleKeyBuffer.push_back(keyCode);
}



bool InputSystem::SetCursorModeToRelative( const EventArgs* args )
{
	UNUSED( args );
	g_theInput->SetCursorMode( MOUSE_MODE_RELATIVE );

	return true;
}

bool InputSystem::SetCursorModeToAbsolute( const EventArgs* args )
{
	UNUSED( args );
	g_theInput->SetCursorMode( MOUSE_MODE_ABSOLUTE );

	return true;
}

IntVec2 InputSystem::GetMouseDesktopRawPos() const
{
	return IntVec2(0,0);
}

Vec2 InputSystem::GetMouseNormalizedPos() const
{
	return m_mouseNormalizedClientPos;
}

Vec2 InputSystem::GetMouseDeltaPos() const
{
	if( m_currentMouseMode == MOUSE_MODE_RELATIVE )
	{
		return m_relativeMovement;
	}
	else
	{
		return m_mouseNormalizedClientPos - m_mousePreviousNormalizedClientPos;
	}
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
	ScreenToClient( (HWND)m_window->m_hwnd, &mousePos );
	Vec2 mouseClientPos( (float)mousePos.x, (float)mousePos.y);

	RECT clientRect;
	GetClientRect( (HWND)m_window->m_hwnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom);

	m_mouseNormalizedClientPos = clientBounds.GetUVForPoint(mouseClientPos);
	m_mouseNormalizedClientPos.y = 1.f - m_mouseNormalizedClientPos.y; //Flip y

	if( m_mousePreviousNormalizedClientPos == Vec2( 0.f, 0.f ) )
	{
		m_mousePreviousNormalizedClientPos = m_mouseNormalizedClientPos;
	}

	if( m_currentMouseMode == MOUSE_MODE_RELATIVE )
	{
		UpdateRelativeMode();
	}
}

void InputSystem::SetCursorMode( eMousePositionMode mode )
{
	m_currentMouseMode = mode;

	if( mode == MOUSE_MODE_RELATIVE )
	{
/*		HideSystemCursor();*/
		UpdateRelativeMode();
		m_relativeMovement = Vec2(0.f, 0.f);
	}
	else if( mode == MOUSE_MODE_ABSOLUTE )
	{
/*		ShowSystemCursor();*/
	}
}

void InputSystem::HideSystemCursor()
{
	while( ShowCursor( false ) >= 0 ) {}
}

void InputSystem::ShowSystemCursor()
{
	while( ShowCursor( true ) < 0 ) {}
}

void InputSystem::ClipSystemCursor()
{
	RECT clientRect;
	GetWindowRect( (HWND)m_window->m_hwnd, &clientRect );
	ClipCursor( &clientRect );
}

void InputSystem::UnclipSystemCursor()
{
	ClipCursor( nullptr );
}

void InputSystem::SetScreenActive()
{
	SetMouseOptionsFromTopOfStack();
}

void InputSystem::SetScreenInactive()
{
	SetCursorMode(MOUSE_MODE_ABSOLUTE);
	ShowSystemCursor();
	UnclipSystemCursor();
}

void InputSystem::PushMouseOptions( eMousePositionMode mouseMode, bool isMouseVisible, bool isMouseClipped )
{
	MouseOptions options;
	options.m_mouseMode = mouseMode;
	options.m_isMouseVisible = isMouseVisible;
	options.m_isMouseClipped = isMouseClipped;

	m_mouseOptionStack.push( options );

	SetMouseOptionsFromTopOfStack();
}

void InputSystem::PopMouseOptions()
{
	m_mouseOptionStack.pop();
	SetMouseOptionsFromTopOfStack();
	
}

void InputSystem::SetMouseOptionsFromTopOfStack()
{
	MouseOptions options;
	if( !m_mouseOptionStack.empty() )
	{
		options = m_mouseOptionStack.top();
	}


	bool isVisible = options.m_isMouseVisible;
	bool isClipped = options.m_isMouseClipped;

	SetCursorMode( options.m_mouseMode );

	if( isVisible )
	{
		ShowSystemCursor();
	}
	else
	{
		HideSystemCursor();
	}

	if( isClipped )
	{
		ClipSystemCursor();
	}
	else
	{
		UnclipSystemCursor();
	}
}

void InputSystem::UpdateRelativeMode()
{
// 	Vec2 currentMousePosition;
// 
// 
// 	RECT clientRect;
// 	GetClientRect( (HWND)m_window->m_hwnd, &clientRect );
// 	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom);
// 	Vec2 windowCenter = clientBounds.GetCenter();
// 
// 	SetCursorPos( (int)windowCenter.x, (int)windowCenter.y );
// 
// 
// 	if( m_mousePreviousNormalizedClientPos == Vec2( 0.f, 0.f ) )
// 	{
// 		m_mousePreviousNormalizedClientPos = Vec2(0.5f, 0.5f);
// 	}
	POINT currentPosition;
	GetCursorPos( &currentPosition );
	ScreenToClient( (HWND)m_window->m_hwnd, &currentPosition );
	Vec2 mouseClientPos( (float)currentPosition.x, (float)currentPosition.y );

	if( m_positionLastFrame == Vec2( 0.f, 0.f ) )
	{
		m_relativeMovement = Vec2( 0.f, 0.f );
	}
	else
	{
		m_relativeMovement = mouseClientPos - m_positionLastFrame;
	}

	RECT clientRect;
	GetClientRect( (HWND)m_window->m_hwnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.top, (float)clientRect.right, (float)clientRect.bottom );
	Vec2 windowCenter = clientBounds.GetCenter();
	SetCursorPos( (int)windowCenter.x, (int)windowCenter.y );

	GetCursorPos( &currentPosition );
	ScreenToClient( (HWND)m_window->m_hwnd, &currentPosition );
	windowCenter = Vec2( (float)currentPosition.x, (float)currentPosition.y );

	m_positionLastFrame = windowCenter;



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
