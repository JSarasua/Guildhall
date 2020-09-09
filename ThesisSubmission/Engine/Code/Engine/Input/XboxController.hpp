#pragma once
#include "AnalogJoystick.hpp"
#include "KeyButtonState.hpp"



enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	//These are "simple" (literal/physcial) binary buttons
	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_DOWN,


	NUM_XBOX_BUTTONS
};

class XboxController
{
	friend class InputSystem;

public:
	explicit XboxController( int controllerID );
	~XboxController();
	bool					IsConnected() const			{ return m_isConnected; }
	int						GetControllerID() const		{ return m_isConnected; }
	const AnalogJoystick&	GetLeftJoystick() const		{return m_leftJoyStick; }
	const AnalogJoystick&	GetRightJoystick() const	{return m_rightJoyStick; }
	float					GetLeftTrigger() const		{return m_leftTriggerValue; }
	float					GetRightTrigger() const		{return m_rightTriggerValue; }
	const KeyButtonState&	GetButtonState( XboxButtonID buttonID ) const;


private:
	void Update();
	void Reset();
	void UpdateTrigger( float& triggerValue, unsigned char rawValue);
	void UpdateJoystick( AnalogJoystick&buttonID, short rawX, short rawY );
	void UpdateButton( XboxButtonID buttonID, short buttonFlags, unsigned short buttonFlag);


public:
	const int m_controllerID = -1;
	bool m_isConnected = false;
	KeyButtonState m_buttonStates[NUM_XBOX_BUTTONS];
	AnalogJoystick m_leftJoyStick = AnalogJoystick(0.3f, 0.95f);
	AnalogJoystick m_rightJoyStick = AnalogJoystick(0.3f, 0.95f);
	float m_leftTriggerValue = 0.f;
	float m_rightTriggerValue = 0.f;
};