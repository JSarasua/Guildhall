#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Math/MatrixUtils.hpp"



Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;
}

void Camera::SetPosition( const Vec3& position )
{
	m_transform.m_position = position;
	m_position = position;
}

void Camera::Translate( const Vec3& translation )
{
	m_transform.Translate( translation );
	m_position += translation;
}

void Camera::TranslateRelativeToView( Vec3 const& translation )
{
	Mat44 translationMatrix;
	translationMatrix.RotateZDegrees( m_transform.m_rotationPitchRollYawDegrees.z );
	translationMatrix.RotateYDegrees( m_transform.m_rotationPitchRollYawDegrees.y );
	translationMatrix.RotateXDegrees( m_transform.m_rotationPitchRollYawDegrees.x );

	Vec3 absoluteTranslation = translationMatrix.TransformPosition3D( translation );
	Translate( absoluteTranslation );
}

void Camera::RotatePitchRollYawDegrees( Vec3 const& rotator )
{
	m_transform.RotatePitchRollYawDegrees( rotator.x, rotator.y, rotator.z );
}

Vec3 Camera::GetPosition()
{
	return m_transform.m_position;
	//return m_position;
}

float Camera::GetAspectRatio() const
{
	//return m_outputSize.x/m_outputSize.y;
	return 16.f/9.f;
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	//bLeft = bottomLeft;
	//tRight = topRight;

	m_projection = Mat44::CreateOrthographicProjection( Vec3( bottomLeft, 0.f), Vec3(topRight, 1.f));
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return bLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return tRight;
}

Vec3 Camera::GetOrthoMin() const
{
// 	Vec4 ndc( -1.f, -1.f, 0.f, 1.f );
// 	Mat44 viewProjection = m_projection * m_view;
// 	Mat44 clipToWorld = viewProjection;
// 	MatrixInvert( clipToWorld );
// 
// 	Vec4 world = clipToWorld * ndc;
// 	world.x /= world.w;
// 	world.y /= world.w;
// 	world.z /= world.w;
// 	Vec3 world3D(world.x, world.y, world.z);
// 	return world3D;
// 	

	return Vec3();
}

Vec3 Camera::GetOrthoMax() const
{
	return Vec3();
}

Mat44 Camera::GetProjection() const
{
	return m_projection;
}

void Camera::SetProjectionOrthographic( Vec2 const& size, float nearZ, float farZ )
{
	m_outputSize = size;

	Vec2 halfDimensions = size * 0.5f;
	Vec3 center = m_transform.m_position;
	Vec3 mins = m_transform.m_position - halfDimensions; 
	mins.z = nearZ;
	Vec3 maxs = m_transform.m_position + halfDimensions;
	maxs.z = farZ;
	m_projection = Mat44::CreateOrthographicProjection( mins, maxs );
}

void Camera::SetProjectionPerspective( float fov, float nearZ, float farZ )
{
	m_projection = MakePerspectiveProjectMatrixD3D( fov, GetAspectRatio(), nearZ, farZ );
}

Mat44 Camera::GetViewMatrix() const
{
	return m_view;
}

Mat44 Camera::GetProjectionMatrix() const
{
	return m_projection;
}

Vec3 Camera::ClientToWorld( Vec2 client, float ndcZ )
{
	return Vec3( 0.f, 0.f, 0.f );
}

Vec3 Camera::WorldToClient( Vec3 worldPos )
{
	return Vec3( 0.f, 0.f, 0.f );
}

void Camera::SetColorTarget( Texture* texture )
{
	if( nullptr != texture )
	{
		m_texture = texture;
	}
}

Texture* Camera::GetColorTarget() const
{
	return m_texture;
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

void Camera::UpdateCameraUBO()
{
	Mat44 cameraModel = m_transform.ToMatrix();
	m_view = cameraModel;
	MatrixInvertOrthoNormal( m_view );


	//CameraData
	CameraData camData;
	camData.projection = GetProjection();
	camData.view = GetViewMatrix();
	//camData.view = Mat44::CreateTranslation3D( -GetPosition() );

	m_cameraUBO->Update( &camData, sizeof( camData ), sizeof( camData ) );
}
