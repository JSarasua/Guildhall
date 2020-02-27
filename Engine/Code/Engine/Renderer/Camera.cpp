#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"



// void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
// {
// 	m_cameraView.mins = bottomLeft;
// 	m_cameraView.maxs = topRight;
// 
// }

void Camera::SetProjectionOrthographic( float height, float nearZ /*= -1.f*/, float farZ /*= 1.f */ )
{
	UNUSED(nearZ);
	UNUSED(farZ);

	float cameraWidth = height * GetAspectRatio();

	m_outputSize = Vec2(cameraWidth, height);

}

Vec2 Camera::GetOrthoBottomLeft() const
{
	Vec2 offset = Vec2( m_outputSize.x, m_outputSize.y );
	offset *= 0.5f;
	return m_position - offset;
}

Vec2 Camera::GetOrthoTopRight() const
{
	Vec2 offset = Vec2( m_outputSize.x, m_outputSize.y );
	offset *= 0.5f;
	return m_position + offset;
}

Vec2 Camera::GetClientToWorldPosition( Vec2 clientPos ) const
{
	float worldX = RangeMap( 0.f, 1.f, GetOrthoBottomLeft().x, GetOrthoTopRight().x, clientPos.x );
	float worldY = RangeMap( 0.f, 1.f, GetOrthoBottomLeft().y, GetOrthoTopRight().y, clientPos.y );
	Vec2 worldPos = Vec2(worldX, worldY);
	return worldPos;
}

float Camera::GetAspectRatio() const
{
	
	float aspectRatio = m_outputSize.x/m_outputSize.y;
	return aspectRatio;
}

void Camera::SetOutputSize( const Vec2& size )
{
	m_outputSize = size;
}

void Camera::SetPosition( const Vec3& position )
{
	Vec2 position2D(position.x, position.y);
	m_position = position2D;
}

void Camera::Translate2D( const Vec2& cameraDisplacement )
{
	m_position += cameraDisplacement;
}
