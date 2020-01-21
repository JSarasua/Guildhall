#include <Windows.h>
#include <Xinput.h>
#include "Engine/Input/XboxController.hpp"
#pragma comment( lib, "xinput9_1_0")
#include "Engine/Math/MathUtils.hpp"




XboxController::XboxController( int controllerID ) : m_controllerID(controllerID)
{

}

XboxController::~XboxController()
{

}

const KeyButtonState& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_controllerID, &xboxControllerState);
	if( errorStatus == ERROR_SUCCESS )
	{
		m_isConnected = true;
		UpdateJoystick(m_leftJoyStick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateJoystick(m_rightJoyStick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
		UpdateButton(XBOX_BUTTON_ID_A,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_ID_B,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_ID_X,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_ID_Y,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_Y);
		UpdateButton(XBOX_BUTTON_ID_BACK,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_ID_START,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_START);
		UpdateButton(XBOX_BUTTON_ID_LSHOULDER,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_RSHOULDER,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_LTHUMB,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_RTHUMB,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_DPAD_RIGHT,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_LEFT,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_UP,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_ID_DPAD_DOWN,xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		UpdateTrigger(m_leftTriggerValue, xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTriggerValue, xboxControllerState.Gamepad.bRightTrigger);
	}
	else
	{
		m_isConnected = false;
	}
}

void XboxController::Reset()
{

}

void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	triggerValue = (float)rawValue * 1.f/255.f;
}

void XboxController::UpdateJoystick( AnalogJoystick& buttonID, short rawX, short rawY )
{
	float scaledX = RangeMap(-32768.f, 32767.f, -1.f, 1.f, rawX);
	float scaledY = RangeMap(-32768.f, 32767.f, -1.f, 1.f, rawY);

	buttonID.UpdatePosition(scaledX,scaledY);
}

void XboxController::UpdateButton( XboxButtonID buttonID, short buttonFlags, unsigned short buttonFlag )
{
	bool isPressed = buttonFlags & buttonFlag;
	m_buttonStates[buttonID].UpdateStatus(isPressed);
}
