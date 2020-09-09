#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"




AnalogJoystick::AnalogJoystick( float innerDeadZoneFraction, float outerDeadZoneFraction ) :
		m_innerDeadZoneFraction(innerDeadZoneFraction),
		m_outerDeadZoneFraction(outerDeadZoneFraction)
{

}

void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2(0.f, 0.f);
	m_correctedPosition = Vec2(0.f, 0.f);
	m_correctedDegrees = 0.f;
	m_correctedMagnitude = 0.f;

}

void AnalogJoystick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	m_rawPosition.x = rawNormalizedX;
	m_rawPosition.y = rawNormalizedY;

	Vec2 raw = Vec2(rawNormalizedX,rawNormalizedY);
	m_correctedDegrees = raw.GetAngleDegrees();
	float rawMagnitude = raw.GetLength();
	float inBetweenMagnitude = RangeMap(m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f, rawMagnitude);
	m_correctedMagnitude = Clampf(inBetweenMagnitude, 0.f, 1.f);

	m_correctedPosition = Vec2::MakeFromPolarDegrees(m_correctedDegrees, m_correctedMagnitude);
	
}

