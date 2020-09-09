#include "Engine/Input/KeyButtonState.hpp"


void KeyButtonState::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = isNowPressed;
}

bool KeyButtonState::WasJustPressed() const
{
	if( m_isPressed && !m_wasPressedLastFrame )
	{
		return true;
	}

	return false;
}

bool KeyButtonState::WasJustReleased() const
{
	if( !m_isPressed && m_wasPressedLastFrame )
	{
		return true;
	}

		return false;

}

void KeyButtonState::Reset()
{
	m_isPressed = false;
	m_wasPressedLastFrame = false;
}

