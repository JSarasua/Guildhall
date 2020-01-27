#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"




void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	bLeft = bottomLeft;
	tRight = topRight;

}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return bLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return tRight;
}

void Camera::SetClearMode( unsigned int clearFlags, Rgba8 color, float depth /*= 0.f*/, unsigned int stencil /*= 0 */ )
{
	UNUSED(depth);
	UNUSED(stencil);

	m_clearMode = clearFlags;
	m_clearColor = color;
}

void Camera::Translate2D( const Vec2& cameraDisplacement )
{
	bLeft += cameraDisplacement;
	tRight += cameraDisplacement;
}
