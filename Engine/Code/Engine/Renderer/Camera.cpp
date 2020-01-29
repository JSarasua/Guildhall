#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"



void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_cameraView.mins = bottomLeft;
	m_cameraView.maxs = topRight;

}

void Camera::SetProjectionOrthographic( float height, float nearZ /*= -1.f*/, float farZ /*= 1.f */ )
{
	UNUSED(nearZ);
	UNUSED(farZ);

	float cameraWidth = height * GetAspectRatio();
	Vec2 center = m_cameraView.GetCenter();
	Vec2 offset = Vec2(cameraWidth, height);

	m_cameraView.maxs = center + offset;
	m_cameraView.mins = center - offset;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_cameraView.mins;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_cameraView.maxs;
}

Vec2 Camera::GetClientToWorldPosition( Vec2 clientPos ) const
{
	Vec2 worldPos = m_cameraView.GetPointAtUV( clientPos );
	return worldPos;
}

float Camera::GetAspectRatio() const
{
	const Vec2& cameraDimensions = m_cameraView.GetDimensions();
	float aspectRatio = cameraDimensions.x/cameraDimensions.y;
	return aspectRatio;
}

void Camera::SetOutputSize( const Vec2& size )
{
	m_outputSize = size;
}

void Camera::SetPosition( const Vec3& position )
{
	Vec2 position2D(position.x, position.y);
	m_cameraView.SetCenter(position2D);
}

void Camera::Translate2D( const Vec2& cameraDisplacement )
{
	m_cameraView.mins += cameraDisplacement;
	m_cameraView.maxs += cameraDisplacement;
}
