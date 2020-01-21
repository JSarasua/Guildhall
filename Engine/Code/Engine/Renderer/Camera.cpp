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

void Camera::Translate2D( const Vec2& cameraDisplacement )
{
	bLeft += cameraDisplacement;
	tRight += cameraDisplacement;
}
